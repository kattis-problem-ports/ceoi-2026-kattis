// Solves K = 1 correctly but deliberately spends exactly 16*ceil(log2 N) queries
// on every hunt, which puts t = 16 in the middle of the scoring ramp:
// f(16) = 1 - (16-11)/9 = 4/9, so subtask 1 is worth 1/2 + 1/2 * 4/9 = 13/18 of
// its 10 points, i.e. 7. Subtasks 2 and 3 get nothing, since it gives up as soon
// as K > 1.
//
// Its only job is to pin down the interior of the piecewise-linear part of the
// score; the two reference solutions both land at f = 1.
#include "treasurehuntlib.h"

int main() {
	int N, K;
	while (true) {
		NextHunt(N, K);
		if (N < 0) break;
		if (K != 1) continue;

		int lg = 0;
		while ((1 << lg) < N) lg++;
		int budget = 16 * lg;
		int used = 0;

		int lo = 0, hi = N - 1, col = -1;
		bool found = false;
		while (!found && col < 0) {
			int mid = lo + (hi - lo) / 2;
			int r = Query(mid, 0);
			used++;
			if (r == TREASURE) found = true;
			else if (r & DIR_RIGHT) lo = mid + 1;
			else if (r & DIR_LEFT) hi = mid - 1;
			else col = mid;
		}
		if (!found) {
			lo = 1;
			hi = N - 1;
			while (true) {
				int mid = lo + (hi - lo) / 2;
				int r = Query(col, mid);
				used++;
				if (r == TREASURE) break;
				if (r & DIR_DOWN) lo = mid + 1;
				else hi = mid - 1;
			}
		}

		// Burn the rest of the budget. 2*ceil(log2 N) + 2 <= 16*ceil(log2 N) for
		// every N >= 2, so this loop always has work left to do and never
		// overshoots the 1000-query cap (16*20 = 320).
		while (used < budget) {
			Query(0, 0);
			used++;
		}
	}
	return 0;
}
