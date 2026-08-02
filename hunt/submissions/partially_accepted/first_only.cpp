// Finds one chest per hunt and then moves on, whatever K is. This is the
// "first treasure" step that every full solution starts with: a binary search
// for a column that holds a chest, then a binary search for the row inside it.
// It works for any K because a query that reports neither LEFT nor RIGHT is in
// the same column as some nearest chest.
//
// Expected score: subtask 1 full (K = 1 means one chest is all of them, in about
// 2*ceil(log2 N) queries), and half credit scaled by 1/K on the others -- so it
// exercises the F < K branch of the scoring formula.
#include "treasurehuntlib.h"

int main() {
	int N, K;
	while (true) {
		NextHunt(N, K);
		if (N < 0) break;

		// Column of some nearest chest, searched along the top row.
		int lo = 0, hi = N - 1;
		bool found = false;
		while (lo < hi) {
			int mid = lo + (hi - lo) / 2;
			int r = Query(mid, 0);
			if (r == TREASURE) { found = true; break; }
			bool left = r & DIR_LEFT, right = r & DIR_RIGHT;
			if (!left && !right) { lo = hi = mid; break; }
			if (left) hi = mid - 1;
			else lo = mid + 1;
		}
		if (found) continue;
		int col = lo;

		// Row inside that column. Only a bare DOWN means every nearest chest is
		// strictly below; anything else lets us cut the lower half away.
		lo = 0;
		hi = N - 1;
		while (lo < hi) {
			int mid = lo + (hi - lo) / 2;
			int r = Query(col, mid);
			if (r == TREASURE) { found = true; break; }
			if (r == DIR_DOWN) lo = mid + 1;
			else hi = mid - 1;
		}
		if (!found) Query(col, lo);
	}
	return 0;
}
