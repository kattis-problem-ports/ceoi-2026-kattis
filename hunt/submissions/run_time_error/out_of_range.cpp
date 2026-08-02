// Queries the column just past the right edge. Coordinates must be in [0, N-1],
// so the library must stop the program with a run-time error.
#include "treasurehuntlib.h"

int main() {
	int N, K;
	NextHunt(N, K);
	if (N < 0) return 0;
	Query(N, 0);
	return 0;
}
