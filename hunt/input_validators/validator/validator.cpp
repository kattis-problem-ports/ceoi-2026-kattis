// Input validator for "Treasure Hunt" (CEOI 2026, Day 1).
//
// A test case is a list of treasure hunts, one per line:
//
//     H
//     N K  x_1 y_1  x_2 y_2  ...  x_K y_K      (H times)
//
// Fields are separated by one or more spaces. The original data uses two
// conventions -- the hand-written cases pad the coordinate pairs into columns,
// the generated ones use single spaces -- and it is shipped verbatim, so the
// validator allows runs of spaces. Everything else stays strict: no tabs, no
// leading or trailing whitespace, no blank lines, no leading zeroes.
//
// Flags:
//   k=<v>   require K = v in every hunt (used to pin the three subtasks)
#include "validator.h"

// validator.h invites extension; this is the only addition.
static void Spaces() {
	Space();
	while (_peek1() == ' ') Space();
}

void run() {
	const int MAX_HUNTS = 100000;
	const int MAX_N = 1000000;

	// K is the whole difference between the three subtasks, so every group pins
	// it exactly; 0 means "any value allowed by the statement".
	int fixedK = Arg("k", 0);
	assert(fixedK >= 0 && fixedK <= 3);

	long long hunts = Int(1, MAX_HUNTS);
	Endl();

	for (long long h = 0; h < hunts; h++) {
		long long n = Int(2, MAX_N);
		Spaces();
		long long k = Int(1, 3);
		if (fixedK != 0 && k != fixedK)
			die_line("K is " + to_string(k) + ", but this test group requires K = " + to_string(fixedK));

		vector<pair<long long, long long>> treasures;
		for (long long i = 0; i < k; i++) {
			Spaces();
			long long x = Int(0, n - 1);
			Spaces();
			long long y = Int(0, n - 1);
			treasures.emplace_back(x, y);
		}
		Endl();

		// "K <= 3 treasure chests hidden at various distinct cells".
		sort(treasures.begin(), treasures.end());
		for (size_t i = 1; i < treasures.size(); i++)
			if (treasures[i] == treasures[i - 1])
				die_line("two treasure chests share the cell (" +
				         to_string(treasures[i].first) + ", " +
				         to_string(treasures[i].second) + ")");
	}
}
