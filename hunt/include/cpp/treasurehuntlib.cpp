// Judge implementation of the "Treasure Hunt" library. It is compiled together
// with the submission on the evaluation server and is NOT given to contestants;
// they get attachments/treasurehuntlib-public.cpp, which is the organizers'
// public implementation and shares nothing with this file but the header.
//
// Unlike most interactive ports this one answers Query in-process rather than
// over a pipe to the output validator. A single test case can contain 100 000
// hunts of up to 1000 queries each, so even a microsecond of round-trip cost per
// query would dominate the 8 s limit by two orders of magnitude. The pipe to the
// validator is therefore used exactly twice: once to fetch the test data, once
// to report the result.
//
// The exchange with output_validators/validator/validate.cpp is:
//
//     validator -> library    REQUEST
//     library   -> validator  RESPONSE
//     validator -> library    H, then H hunts "N K x_1 y_1 ... x_K y_K"
//     library   -> validator  ACCEPTED, then the summary line
//
// The validator sends no test data before it has seen RESPONSE, so a submission
// that ignores the library gets nothing to work with. Once the data has arrived
// this code takes the descriptors away from the program it is linked into:
// standard input becomes /dev/null, so the treasure coordinates cannot be read a
// second time, and standard output is pointed at standard error, so nothing the
// submission prints can reach the validator. The pipe survives on a private
// descriptor. Forging a result therefore takes the contents of this file --
// RESPONSE, ACCEPTED and the descriptor -- and not merely the observation that
// something is being piped around.
//
// The summary reports only the two hunts the score depends on:
//
//     P  aI aN aK aQ aF  bI bN bK bQ bF
//
// where P is the number of hunts the submission actually started, hunt a is the
// one minimising F/K (fewest treasures found, relative to K) and hunt b the one
// maximising Q/ceil(log2 N) (most queries spent, relative to the budget). Both
// branches of the statement's score are a minimum over all hunts, so those two
// determine it. If P = 0 only "0" is written.
//
// Protocol violations (too many queries, out-of-range coordinates, a call
// before the first NextHunt or after the terminating one) print a reason to
// standard error and exit non-zero, which the statement promises as an RTE.
//
// Everything below is deliberately free of globals with constructors and of
// iostreams: the setup routine runs as a constructor with priority 101, that is
// before every dynamic initializer in the program -- including this file's own,
// had there been any -- so that no submission can get at standard input first.
#include "treasurehuntlib.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

namespace {

// Shared secrets with output_validators/validator/validate.cpp.
const char *const REQUEST = "must_use_the_provided_cpp_library";
const char *const RESPONSE = "VvvbSvoVRZPPStmeuE5h";
const char *const ACCEPTED = "OK:Ro6oOWvHpy5EUC7P4Ha6";

const int MAX_QUERIES = 1000;
const int MAX_K = 3;

// --------------------------------------------------------------- output ----

// The pipe to the validator, moved to a descriptor out of main()'s way.
int realStdout = 1;

void writeAll(int fd, const char *buf, size_t len) {
	while (len > 0) {
		ssize_t w = write(fd, buf, len);
		if (w < 0 && errno == EINTR) continue;
		if (w <= 0) return;  // EPIPE: the validator is gone, nothing to salvage
		len -= (size_t)w;
		buf += w;
	}
}

[[noreturn]] void die(const char *why) {
	char msg[256];
	int n = snprintf(msg, sizeof(msg), "treasurehunt: %s\n", why);
	if (n > 0) writeAll(2, msg, (size_t)n);
	// _Exit skips the atexit handler below, so no summary is written; the
	// non-zero exit status is what the statement promises as an RTE.
	_Exit(1);
}

// ---------------------------------------------------------------- input ----

char inbuf[1 << 16];
size_t inLen = 0, inPos = 0;
bool inEof = false;

int getChar() {
	if (inPos == inLen) {
		if (inEof) return -1;
		ssize_t r;
		do {
			r = read(0, inbuf, sizeof(inbuf));
		} while (r < 0 && errno == EINTR);
		if (r <= 0) {
			inEof = true;
			return -1;
		}
		inLen = (size_t)r;
		inPos = 0;
	}
	return (unsigned char)inbuf[inPos++];
}

long long readInt() {
	int c = getChar();
	while (c == ' ' || c == '\n' || c == '\r' || c == '\t') c = getChar();
	if (c < '0' || c > '9') die("malformed judge input");
	long long v = 0;
	while (c >= '0' && c <= '9') {
		v = v * 10 + (c - '0');
		c = getChar();
	}
	return v;
}

// ---------------------------------------------------------------- state ----

struct Hunt {
	int n, k;
	int x[MAX_K], y[MAX_K];
};

Hunt *hunts = nullptr;      // malloc'd, so no dynamic initializer to run
long long huntCount = 0;    // total number of hunts in the test case
long long processed = 0;    // hunts handed out by NextHunt so far
bool started = false;       // NextHunt has been called at least once
bool ended = false;         // NextHunt has reported the terminator

int curQueries;
unsigned curFound;          // bitmask of treasures queried in the current hunt
bool curClosed = true;      // the current hunt has already been folded into the stats

// Worst hunt by F/K, and worst hunt by Q/ceil(log2 N). Indices are 1-based.
long long aI, aN, aK, aQ, aF;
long long bI, bN, bK, bQ, bF, bL;

int ceilLog2(int n) {
	int l = 0;
	while ((1 << l) < n) l++;
	return l;
}

// Called both when the next hunt starts and once more at exit, so that a program
// that stops in the middle of a hunt still has it counted; the guard keeps the
// last hunt from being folded in twice.
void closeHunt() {
	if (curClosed) return;
	curClosed = true;
	const Hunt &h = hunts[processed - 1];
	long long q = curQueries, f = 0;
	for (int i = 0; i < h.k; i++)
		if (curFound >> i & 1) f++;
	long long l = ceilLog2(h.n);

	// Strict comparisons keep the earliest hunt on ties, which makes the
	// summary a deterministic function of the run.
	if (aI == 0 || f * aK < aF * h.k) {
		aI = processed; aN = h.n; aK = h.k; aQ = q; aF = f;
	}
	if (bI == 0 || q * bL > bQ * l) {
		bI = processed; bN = h.n; bK = h.k; bQ = q; bF = f; bL = l;
	}
}

void finish() {
	closeHunt();
	char buf[512];
	int n;
	if (processed == 0) {
		n = snprintf(buf, sizeof(buf), "%s\n0\n", ACCEPTED);
	} else {
		n = snprintf(buf, sizeof(buf),
		             "%s\n%lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld\n",
		             ACCEPTED, processed, aI, aN, aK, aQ, aF, bI, bN, bK, bQ, bF);
	}
	if (n > 0) writeAll(realStdout, buf, (size_t)n);
}

// ---------------------------------------------------------------- setup ----

// Priority 101 puts this ahead of every dynamic initializer in the program, so
// the handshake and the test data are consumed before any submission code runs.
__attribute__((constructor(101))) void setup() {
	// A validator that has already given up must not turn into a signal.
	signal(SIGPIPE, SIG_IGN);

	char line[128];
	size_t len = 0;
	int c;
	while ((c = getChar()) >= 0 && c != '\n')
		if (len + 1 < sizeof(line)) line[len++] = (char)c;
	line[len] = 0;
	if (strcmp(line, REQUEST) != 0)
		die("this task must be solved in C++, using the provided library");

	char resp[64];
	int n = snprintf(resp, sizeof(resp), "%s\n", RESPONSE);
	writeAll(1, resp, (size_t)n);

	huntCount = readInt();
	if (huntCount < 1) die("malformed judge input");
	hunts = (Hunt *)calloc((size_t)huntCount, sizeof(Hunt));
	if (hunts == nullptr) die("out of memory while reading the judge input");
	for (long long i = 0; i < huntCount; i++) {
		Hunt &h = hunts[i];
		h.n = (int)readInt();
		h.k = (int)readInt();
		if (h.n < 2 || h.k < 1 || h.k > MAX_K) die("malformed judge input");
		for (int j = 0; j < h.k; j++) {
			h.x[j] = (int)readInt();
			h.y[j] = (int)readInt();
		}
	}

	// The test data is the answer, so put it out of the submission's reach.
	realStdout = fcntl(1, F_DUPFD, 100);
	if (realStdout < 0) die("could not duplicate standard output");
	if (dup2(2, 1) < 0) die("could not redirect standard output");
	if (close(0) < 0) die("could not close standard input");
	if (open("/dev/null", O_RDONLY) != 0) die("could not replace standard input");
	memset(inbuf, 0, sizeof(inbuf));
	inLen = inPos = 0;
	inEof = true;

	atexit(&finish);
}

}  // namespace

void NextHunt(int &N, int &K) {
	if (ended) die("NextHunt was called after it had already reported that there are no hunts left");

	if (!started) started = true;
	else closeHunt();

	if (processed == huntCount) {
		ended = true;
		N = -1;
		K = -1;
		return;
	}

	processed++;
	curQueries = 0;
	curFound = 0;
	curClosed = false;
	N = hunts[processed - 1].n;
	K = hunts[processed - 1].k;
}

int Query(int x, int y) {
	if (!started) die("Query was called before the first call to NextHunt");
	if (ended) die("Query was called after NextHunt reported that there are no hunts left");
	const Hunt &h = hunts[processed - 1];
	if (x < 0 || x >= h.n || y < 0 || y >= h.n)
		die("Query was called with coordinates outside the grid");
	if (++curQueries > MAX_QUERIES)
		die("more than 1000 queries were made within a single treasure hunt");

	int best = 0, res = 0;
	for (int i = 0; i < h.k; i++) {
		int dx = h.x[i] - x, dy = h.y[i] - y;
		int d = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
		if (d == 0) {
			curFound |= 1u << i;
			return TREASURE;
		}
		if (res == 0 || d < best) { best = d; res = 0; }
		if (d == best) {
			if (dx > 0) res |= DIR_RIGHT; else if (dx < 0) res |= DIR_LEFT;
			if (dy > 0) res |= DIR_DOWN; else if (dy < 0) res |= DIR_UP;
		}
	}
	return res;
}
