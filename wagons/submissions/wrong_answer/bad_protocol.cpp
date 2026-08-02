// Calls Compare() before checking that both queues are non-empty, which the
// statement forbids. Exercises the validator's protocol-violation path.
// Expected score: 0.
#include "wagonslib.h"

int main() {
    BeginSorting();
    int n = 0;
    while (!IsEmpty(1)) { Move(1, 2); n++; }
    // Queue 1 is empty here, so this call is illegal.
    Compare();
    for (int i = 0; i < n; i++) Move(2, 1);
    DoneSorting();
    return 0;
}
