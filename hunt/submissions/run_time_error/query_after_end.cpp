// Keeps querying after NextHunt has reported that no hunts are left. The
// statement forbids both NextHunt and Query after that point, so the library must
// stop the program with a run-time error.
#include "treasurehuntlib.h"

int main() {
	int N, K;
	while (true) {
		NextHunt(N, K);
		if (N < 0) break;
	}
	Query(0, 0);
	return 0;
}
