// Finds one chest per hunt -- the binary search from partially_accepted/
// first_only.cpp -- and then spends the whole query budget, exactly 1000 queries
// per hunt, on pseudorandom probes.
//
// It pins two things nothing else in the package does: the 1000-query cap from
// just below (1001 would be an RTE, and too_many_queries.cpp only covers that
// side), and the f(t) = 0 floor of the scoring ramp with all treasures found.
// The three subtasks land as follows:
//
//   subtask 1  K = 1, so one chest is all of them: F_i = K_i everywhere, and the
//              score is the all-found branch with t_i = 1000/ceil(log2 N_i) >= 50,
//              i.e. f = 0 and half of the 10 points -> 5
//   subtask 2  min F_i/K_i = 1/2 -> 1/2 * 30 * 1/2 = 7.5, rounded to 8
//   subtask 3  min F_i/K_i = 1/3 -> 1/2 * 60 * 1/3 = 10
//
// so 23 in total, against first_only.cpp's 28: the padding costs subtask 1 the
// second half of its points and leaves the other two alone.
//
// The padding is deterministic (a fixed-seed xorshift, no random_device) so the
// score is reproducible. On the hunts with a small N the probes do sweep the grid
// and find every chest, but the score is a minimum over all hunts and the cases
// with N ~ 10^6 keep F_i = 1 there, so min F_i/K_i is 1/K regardless.
#include "treasurehuntlib.h"

#include <cstdint>

namespace {

const int MAX_QUERIES = 1000;

uint64_t rngState = 0x9E3779B97F4A7C15ull;

uint64_t NextRandom() {
	rngState ^= rngState << 13;
	rngState ^= rngState >> 7;
	rngState ^= rngState << 17;
	return rngState;
}

// A coordinate in [0, N). Multiply-shift rather than a modulo: this runs 1000
// times per hunt over nearly five million hunts, and two 64-bit divisions per
// query would cost more than the library call itself.
int RandomCoord(int N) {
	return (int)(((NextRandom() >> 32) * (uint64_t)N) >> 32);
}

}  // namespace

int main() {
	int N, K;
	while (true) {
		NextHunt(N, K);
		if (N < 0) break;

		int used = 0;
		bool found = false;

		// Column of some nearest chest, searched along the top row. A query that
		// reports neither LEFT nor RIGHT shares its column with a nearest chest.
		int lo = 0, hi = N - 1;
		while (lo < hi) {
			int mid = lo + (hi - lo) / 2;
			int r = Query(mid, 0);
			used++;
			if (r == TREASURE) { found = true; break; }
			bool left = r & DIR_LEFT, right = r & DIR_RIGHT;
			if (!left && !right) { lo = hi = mid; break; }
			if (left) hi = mid - 1;
			else lo = mid + 1;
		}

		if (!found) {
			int col = lo;

			// Row inside that column. Only a bare DOWN means every nearest chest
			// is strictly below; anything else lets us cut the lower half away.
			lo = 0;
			hi = N - 1;
			while (lo < hi) {
				int mid = lo + (hi - lo) / 2;
				int r = Query(col, mid);
				used++;
				if (r == TREASURE) { found = true; break; }
				if (r == DIR_DOWN) lo = mid + 1;
				else hi = mid - 1;
			}
			if (!found) {
				Query(col, lo);
				used++;
			}
		}

		// Burn the rest of the budget. Both searches take at most
		// ceil(log2 N) + 1 queries, so used <= 2*ceil(log2 N) + 3 <= 43 for every
		// N <= 10^6 and the loop always has work left without ever reaching 1001.
		while (used < MAX_QUERIES) {
			int x = RandomCoord(N), y = RandomCoord(N);
			Query(x, y);
			used++;
		}
	}
	return 0;
}
