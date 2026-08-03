#ifndef __TREASUREHUNTLIB_H_INCLUDED__
#define __TREASUREHUNTLIB_H_INCLUDED__

// Call this function to finish the current treasure hunt (if any)
// and begin the next one.  The function returns the grid size in 'n'
// and the number of treasures in 'k'.  If there is no next hunt
// (because you've finished all the hunts in the current test case),
// the function returns n = k = -1.
extern void NextHunt(int &n, int &k);

// Constants used in the return values of the Query() function.
enum { TREASURE = 0, DIR_RIGHT = 1, DIR_UP = 2, DIR_LEFT = 4, DIR_DOWN = 8 };

// x and y must be from the range {0, 1, ..., n - 1}.
// If cell (x, y) contains a treasure, this function returns TREASURE.
// Otherwise it returns the sum of one or more of the DIR_xxxx constants
// indicating which directions the compass points to when you stand in that cell.
// (Note on directions: "left" means in the direction of decreasing x,
// "right" is the direction of increasing x, "up" is the direction of decreasing y,
// "down" is the direction of increasing y.)
extern int Query(int x, int y);

#endif // __TREASUREHUNTLIB_H_INCLUDED__
