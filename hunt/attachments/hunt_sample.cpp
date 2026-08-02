// Example solution for "Treasure Hunt" (CEOI 2026, Day 1).
//
// It solves subtask 1 only (K = 1): two binary searches, one for the column and
// one for the row, so about 2*ceil(log2 N) queries per hunt -- comfortably inside
// the 11*ceil(log2 N) needed for full credit on that subtask. On hunts with more
// than one chest it gives up immediately and moves on, which is allowed but
// scores nothing on subtasks 2 and 3.
//
// Build and run it against the local library:
//
//     g++ -O2 hunt_sample.cpp treasurehuntlib-public.cpp
//     HUNT_K=1 HUNT_COUNT=1000 ./a.out
#include "treasurehuntlib.h"

int main() {
	int N, K;
	while (true) {
		NextHunt(N, K);
		if (N < 0) break;
		if (K != 1) continue;  // this example only handles a single chest

		// With one chest the compass points RIGHT exactly when the chest is in a
		// column to the right of the query and LEFT when it is to the left, so a
		// binary search on the column works from any fixed row.
		int lo = 0, hi = N - 1, col = -1;
		bool found = false;
		while (!found && col < 0) {
			int mid = lo + (hi - lo) / 2;
			int r = Query(mid, 0);
			if (r == TREASURE) found = true;
			else if (r & DIR_RIGHT) lo = mid + 1;
			else if (r & DIR_LEFT) hi = mid - 1;
			else col = mid;  // neither: the chest is in this very column
		}
		if (found) continue;

		// The chest is in column col and, since row 0 was not it, strictly below
		// row 0. Binary search the row the same way.
		lo = 1;
		hi = N - 1;
		while (true) {
			int mid = lo + (hi - lo) / 2;
			int r = Query(col, mid);
			if (r == TREASURE) break;
			if (r & DIR_DOWN) lo = mid + 1;
			else hi = mid - 1;
		}
	}
	return 0;
}
