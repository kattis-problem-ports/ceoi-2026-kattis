// A minimal (and deliberately unambitious) example solution for "Wagon Sorting".
// It performs a selection sort, which always sorts correctly but uses about n^2
// operations, so it only earns the lowest score band. Use it to check that your
// build works:
//
//     g++ wagons_sample.cpp wagonslib-public.cpp && ./a.out
#include "wagonslib.h"

int main() {
    BeginSorting();

    // Count the wagons by shifting them all into queue 2 and back.
    int n = 0;
    while (!IsEmpty(1)) { Move(1, 2); n++; }
    for (int i = 0; i < n; i++) Move(2, 1);

    // Invariant: queue 1 holds the not-yet-sorted wagons at the front, followed
    // by the `sorted` smallest wagons in increasing order; queue 2 is empty.
    for (int sorted = 0; sorted < n; sorted++) {
        int k = n - sorted; // wagons still to be placed

        Move(1, 2); // the first candidate for the minimum
        for (int i = 1; i < k; i++) {
            if (Compare() == 1) {
                // Queue 1's front is smaller: it becomes the new candidate.
                Move(2, 1);
                Move(1, 2);
            } else {
                Move(1, 1);
            }
        }

        // Queue 1 is now [sorted block, remaining wagons]; rotate the sorted
        // block to the back so the new minimum can be appended after it.
        for (int i = 0; i < sorted; i++) Move(1, 1);
        Move(2, 1);
    }

    DoneSorting();
    return 0;
}
