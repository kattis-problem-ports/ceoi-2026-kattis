// Judge implementation of the "Treasure Hunt" library. It is compiled together
// with the submission on the evaluation server and is NOT given to contestants;
// they get attachments/treasurehuntlib-public.cpp instead.
//
// Unlike most interactive ports this one answers Query in-process rather than
// over a pipe to the output validator. A single test case can contain 100 000
// hunts of up to 1000 queries each, so even a microsecond of round-trip cost per
// query would dominate the 8 s limit by two orders of magnitude.
//
// The judge input is read from standard input:
//     H
//     N K x_1 y_1 ... x_K y_K      (H times, one line per hunt)
//
// At exit the library writes one line to standard output for the output
// validator, which does the scoring:
//     TOKEN P  aI aN aK aQ aF  bI bN bK bQ bF
// where P is the number of hunts the submission actually started, hunt a is the
// one minimising F/K (fewest treasures found, relative to K) and hunt b the one
// maximising Q/ceil(log2 N) (most queries spent, relative to the budget). Those
// two hunts are the only ones the score depends on. If P = 0 only "TOKEN 0" is
// written. TOKEN keeps a submission from printing a summary of its own; see
// output_validators/validator/validate.cpp.
//
// Protocol violations (too many queries, out-of-range coordinates, a call
// before the first NextHunt or after the terminating one) print a reason to
// standard error and exit non-zero, which the statement promises as an RTE.
#include "treasurehuntlib.h"

#include <cstdio>
#include <cstdlib>

namespace {

const char *const TOKEN = "bT7qKz3mWx9pR2vN5sJd";

const int MAX_QUERIES = 1000;
const int MAX_K = 3;

// ---------------------------------------------------------------- input ----

char buf[1 << 16];
size_t bufLen = 0, bufPos = 0;

int getChar() {
	if (bufPos == bufLen) {
		bufLen = fread(buf, 1, sizeof(buf), stdin);
		bufPos = 0;
		if (bufLen == 0) return -1;
	}
	return (unsigned char)buf[bufPos++];
}

[[noreturn]] void die(const char *why) {
	fflush(stdout);
	fprintf(stderr, "treasurehunt: %s\n", why);
	fflush(stderr);
	// _Exit skips the atexit handler below, so no summary is written.
	_Exit(1);
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

long long hunts = -1;      // total number of hunts in the file
long long processed = 0;    // hunts handed out by NextHunt so far
bool started = false;       // NextHunt has been called at least once
bool ended = false;         // NextHunt has reported the terminator

int curN, curK;
int tx[MAX_K], ty[MAX_K];
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
	long long q = curQueries, f = 0;
	for (int i = 0; i < curK; i++)
		if (curFound >> i & 1) f++;
	long long l = ceilLog2(curN);

	// Strict comparisons keep the earliest hunt on ties, which makes the
	// summary a deterministic function of the run.
	if (aI == 0 || f * aK < aF * curK) {
		aI = processed; aN = curN; aK = curK; aQ = q; aF = f;
	}
	if (bI == 0 || q * bL > bQ * l) {
		bI = processed; bN = curN; bK = curK; bQ = q; bF = f; bL = l;
	}
}

void finish() {
	closeHunt();
	if (processed == 0) {
		printf("%s 0\n", TOKEN);
	} else {
		printf("%s %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld\n",
		       TOKEN, processed, aI, aN, aK, aQ, aF, bI, bN, bK, bQ, bF);
	}
	fflush(stdout);
}

struct Registrar {
	Registrar() { atexit(&finish); }
};
Registrar registrar;

}  // namespace

void NextHunt(int &N, int &K) {
	if (ended) die("NextHunt was called after it had already reported that there are no hunts left");

	if (!started) {
		started = true;
		hunts = readInt();
	} else {
		closeHunt();
	}

	if (processed == hunts) {
		ended = true;
		N = -1;
		K = -1;
		return;
	}

	curN = (int)readInt();
	curK = (int)readInt();
	for (int i = 0; i < curK; i++) {
		tx[i] = (int)readInt();
		ty[i] = (int)readInt();
	}
	processed++;
	curQueries = 0;
	curFound = 0;
	curClosed = false;
	N = curN;
	K = curK;
}

int Query(int x, int y) {
	if (!started) die("Query was called before the first call to NextHunt");
	if (ended) die("Query was called after NextHunt reported that there are no hunts left");
	if (x < 0 || x >= curN || y < 0 || y >= curN)
		die("Query was called with coordinates outside the grid");
	if (++curQueries > MAX_QUERIES)
		die("more than 1000 queries were made within a single treasure hunt");

	int best = 0, res = 0;
	for (int i = 0; i < curK; i++) {
		int dx = tx[i] - x, dy = ty[i] - y;
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
