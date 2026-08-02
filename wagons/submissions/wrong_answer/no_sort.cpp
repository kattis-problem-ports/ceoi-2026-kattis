// Uses the library correctly but never actually sorts: it just counts the wagons
// and puts them back in their original order. Exercises the validator's
// "not sorted" path. Expected score: 0.
#include "wagonslib.h"

int main() {
    BeginSorting();
    int n = 0;
    while (!IsEmpty(1)) { Move(1, 2); n++; }
    for (int i = 0; i < n; i++) Move(2, 1);
    DoneSorting();
    return 0;
}
