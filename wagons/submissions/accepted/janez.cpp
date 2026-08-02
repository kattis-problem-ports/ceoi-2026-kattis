#include <algorithm>
#include "wagonslib.h"
using namespace std;

void Merge(int d1, int d2, int dest)
{
    // Keep merging until we reach the end of both runs.
    while (d1 > 0 || d2 > 0) {
        // If only one run is still nonempty, move from it;
        // otherwise, use Compare to determine which run to move from.
        int src = (d2 == 0) ? 1 : (d1 == 0) ? 2 : Compare();
        if (src == 1) --d1; else --d2;
        Move(src, dest); }
}

int main()
{
    BeginSorting();
    // Count the wagons by moving them all from queue 1 to 2.
    int n1 = 0, n2 = 0; 
    while (! IsEmpty(1)) { Move(1, 2); ++n2; }
    // Move half the wagons back to queue 1 again.
    while (n1 < n2) { Move(2, 1); ++n1; --n2; }
    // Use several passes of merge sorting to sort them.
    int d = 1; // d represents the run length.
    while (n2 > 0)
    {
        // During the merging, both queues will contain some wagons still
        // waiting to be merged in the current pass, and some wagons that have
        // already been merged and are waiting for the next pass. 
        // n1, n2 will count the former; N1, N2 the latter.
        int N1 = 0, N2 = 0, dest = 1;
        while (n1 > 0 || n2 > 0)
        {
            int d1 = min(n1, d), d2 = min(n2, d);
            // Merge a run of d1 wagons from queue 1 and a run of d2 wagons from queue 2 
            // to the destination queue 'dest'.
            Merge(d1, d2, dest);
            // Update the wagon counts.
            n1 -= d1; n2 -= d2; (dest == 1 ? N1 : N2) += d1 + d2;
            // Prepare for the next merging.
            dest = 3 - dest;
        }
        // Get ready for the next pass, when the runs will be twice as long.
        n1 = N1; n2 = N2; d *= 2;
    }
    DoneSorting();
    return 0;
}