// A local implementation of the "Treasure Hunt" library, for developing and
// testing your solution on your own machine. Compile it together with your
// program:
//
//     g++ -O2 foo.cpp treasurehuntlib-public.cpp
//
// The implementation on the evaluation server is a different one, so do not rely
// on anything beyond what treasurehuntlib.h promises.
//
// ---------------------------------------------------------------------------
// Where the treasure hunts come from
// ---------------------------------------------------------------------------
//
// By default this library invents its own random hunts. Since your program owns
// main(), there is nowhere to pass parameters, so they are read from the
// environment:
//
//     HUNT_COUNT   number of hunts               (default 10)
//     HUNT_MINN    smallest grid size            (default 2)
//     HUNT_MAXN    largest grid size             (default 1000000)
//     HUNT_K       number of chests, 1..3        (default 3)
//     HUNT_SEED    random seed                   (default 12345)
//
//     HUNT_COUNT=1000 HUNT_K=2 ./a.out
//
// Alternatively, call
//
//     void InitFromFile(const char *fileName);
//
// before your first NextHunt to play a fixed list of hunts from a file. The file
// format is one line with the number of hunts H, then H lines of
//
//     N K  x_1 y_1  ...  x_K y_K
//
// for example
//
//     2
//     4 1  3 2
//     10 2  0 0  9 9
//
// Declare it yourself where you use it:
//
//     void InitFromFile(const char *fileName);   // only in the local library
//
// ---------------------------------------------------------------------------
// What it reports
// ---------------------------------------------------------------------------
//
// Everything goes to standard error, so it will not interfere with the rules
// (your program must leave standard input and standard output alone). At exit
// the library prints, over all hunts played, the number of treasures missed and
// the score fraction the statement's formula would award: 1/2 for finding
// everything plus 1/2 * min_i f(Q_i / ceil(log2 N_i)). Protocol violations --
// more than 1000 queries in a hunt, coordinates outside the grid, a call before
// the first NextHunt or after the terminating one -- are reported and abort the
// program, exactly as they do on the evaluation server.
#include "treasurehuntlib.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>
#include <set>
#include <string>
#include <utility>
#include <vector>

void InitFromFile(const char *fileName);

namespace {

const int MAX_QUERIES = 1000;

struct Hunt {
	int n, k;
	int x[3], y[3];
};

std::vector<Hunt> hunts;
bool haveHunts = false;
size_t processed = 0;
bool started = false, ended = false;

int curQueries = 0;
unsigned curFound = 0;
bool curClosed = true;   // already folded into the statistics below

// Statistics over the hunts played so far.
long long missed = 0, totalQueries = 0;
double minF = 1.0;
int worstI = -1, worstQ = 0, worstN = 0;

[[noreturn]] void die(const char *why) {
	fflush(stdout);
	fprintf(stderr, "\n*** treasurehunt: %s\n", why);
	fprintf(stderr, "*** On the evaluation server this is a run-time error and the whole "
	                "subtask scores 0.\n");
	fflush(stderr);
	exit(1);
}

int envInt(const char *name, int def) {
	const char *v = getenv(name);
	if (v == nullptr || *v == 0) return def;
	return atoi(v);
}

int ceilLog2(int n) {
	int l = 0;
	while ((1 << l) < n) l++;
	return l;
}

double fOfT(double t) {
	if (t <= 11.0) return 1.0;
	if (t >= 20.0) return 0.0;
	return 1.0 - (t - 11.0) / 9.0;
}

void generateRandomHunts() {
	int count = envInt("HUNT_COUNT", 10);
	int minN = envInt("HUNT_MINN", 2);
	int maxN = envInt("HUNT_MAXN", 1000000);
	int k = envInt("HUNT_K", 3);
	unsigned seed = (unsigned)envInt("HUNT_SEED", 12345);
	if (count < 1) die("HUNT_COUNT must be at least 1");
	if (minN < 2 || maxN < minN || maxN > 1000000) die("HUNT_MINN / HUNT_MAXN out of range");
	if (k < 1 || k > 3) die("HUNT_K must be 1, 2 or 3");

	std::mt19937 rng(seed);
	for (int i = 0; i < count; i++) {
		Hunt h;
		h.n = (int)(std::uniform_int_distribution<int>(minN, maxN)(rng));
		// A grid of n^2 cells must hold k distinct chests.
		h.k = k;
		while ((long long)h.n * h.n < h.k) h.k--;
		std::set<std::pair<int, int>> used;
		for (int j = 0; j < h.k; j++) {
			int x, y;
			do {
				x = std::uniform_int_distribution<int>(0, h.n - 1)(rng);
				y = std::uniform_int_distribution<int>(0, h.n - 1)(rng);
			} while (!used.insert(std::make_pair(x, y)).second);
			h.x[j] = x;
			h.y[j] = y;
		}
		hunts.push_back(h);
	}
	fprintf(stderr, "treasurehunt: generated %d random hunts, N in [%d, %d], K = %d, seed %u\n",
	        count, minN, maxN, k, seed);
}

// Called both when the next hunt starts and once more at exit, so that a hunt
// abandoned in the middle is still counted; the guard keeps the last hunt from
// being folded in twice.
void closeHunt() {
	if (curClosed) return;
	curClosed = true;
	const Hunt &h = hunts[processed - 1];
	int found = 0;
	for (int i = 0; i < h.k; i++)
		if (curFound >> i & 1) found++;
	missed += h.k - found;
	totalQueries += curQueries;
	double t = double(curQueries) / double(ceilLog2(h.n));
	if (fOfT(t) < minF || worstI < 0) {
		minF = fOfT(t);
		worstI = (int)processed;
		worstQ = curQueries;
		worstN = h.n;
	}
}

void report() {
	closeHunt();
	fflush(stdout);
	fprintf(stderr, "\ntreasurehunt: %zu of %zu hunts played, %lld queries in total\n",
	        processed, hunts.size(), totalQueries);
	if (processed < hunts.size())
		fprintf(stderr, "treasurehunt: you stopped early -- keep calling NextHunt until it "
		                "reports -1, or the remaining hunts count as unsolved\n");
	if (missed > 0 || processed < hunts.size()) {
		fprintf(stderr, "treasurehunt: %lld treasures were never queried, so you would get at "
		                "most half of the subtask's points\n", missed);
	} else {
		fprintf(stderr, "treasurehunt: all treasures found; worst hunt %d has Q = %d, N = %d, "
		                "t = %.3f, f(t) = %.4f\n",
		        worstI, worstQ, worstN, double(worstQ) / double(ceilLog2(worstN)), minF);
		fprintf(stderr, "treasurehunt: score fraction %.4f of the subtask\n", 0.5 + 0.5 * minF);
	}
	fflush(stderr);
}

struct Registrar {
	Registrar() { atexit(&report); }
};
Registrar registrar;

}  // namespace

void InitFromFile(const char *fileName) {
	if (started) die("InitFromFile was called after the first NextHunt");
	FILE *f = fopen(fileName, "rt");
	if (f == nullptr) {
		fprintf(stderr, "treasurehunt: cannot open %s\n", fileName);
		exit(1);
	}
	long long count = 0;
	if (fscanf(f, "%lld", &count) != 1 || count < 1) die("bad hunt count in the file");
	for (long long i = 0; i < count; i++) {
		Hunt h;
		if (fscanf(f, "%d %d", &h.n, &h.k) != 2) die("bad hunt in the file");
		if (h.n < 2 || h.n > 1000000 || h.k < 1 || h.k > 3) die("bad N or K in the file");
		for (int j = 0; j < h.k; j++)
			if (fscanf(f, "%d %d", &h.x[j], &h.y[j]) != 2 ||
			    h.x[j] < 0 || h.x[j] >= h.n || h.y[j] < 0 || h.y[j] >= h.n)
				die("bad treasure coordinates in the file");
		hunts.push_back(h);
	}
	fclose(f);
	haveHunts = true;
	fprintf(stderr, "treasurehunt: read %lld hunts from %s\n", count, fileName);
}

void NextHunt(int &N, int &K) {
	if (ended) die("NextHunt was called after it had already reported that there are no hunts left");

	if (!started) {
		started = true;
		if (!haveHunts) generateRandomHunts();
	} else {
		closeHunt();
	}

	if (processed == hunts.size()) {
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
