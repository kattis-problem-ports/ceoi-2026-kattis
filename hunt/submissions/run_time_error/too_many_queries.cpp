// Makes 1001 queries in the first hunt. The library must stop the program with a
// run-time error, which the statement promises for exceeding the 1000-query cap.
#include "treasurehuntlib.h"

int main() {
	int N, K;
	NextHunt(N, K);
	if (N < 0) return 0;
	for (int i = 0; i <= 1000; i++) Query(i % N, 0);
	return 0;
}
