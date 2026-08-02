// Library for the task "Treasure Hunt" (CEOI 2026, Day 1).
//
// Do not modify this file. Your program must not read from standard input or
// write to standard output; the library uses them to talk to the judge.
//
// The library declares nothing besides the two functions and the five constants
// below, so it will not collide with your own names.
#ifndef TREASUREHUNTLIB_H
#define TREASUREHUNTLIB_H

// Starts the next treasure hunt: sets N to the grid size and K to the number of
// treasure chests. When there are no hunts left it sets both to -1, and your
// program must then terminate with exit code 0 without calling NextHunt or
// Query again. It is allowed to call NextHunt before all treasures of the
// current hunt have been found, but that costs points.
void NextHunt(int &N, int &K);

// Return values of Query.
enum { TREASURE = 0, DIR_RIGHT = 1, DIR_UP = 2, DIR_LEFT = 4, DIR_DOWN = 8 };

// Places the compass on the cell (x, y), where 0 <= x, y <= N-1 and
// y-coordinates increase from top to bottom. Returns TREASURE if that cell
// holds a chest, and otherwise the sum of the constants for the directions the
// compass points to. At most 1000 calls are allowed per treasure hunt.
int Query(int x, int y);

#endif
