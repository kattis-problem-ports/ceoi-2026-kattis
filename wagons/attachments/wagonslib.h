// Library for the task "Wagon Sorting" (CEOI 2026, practice session).
// Do not modify this file. Your program must not read from standard input or
// write to standard output; the library uses them to talk to the judge.
#ifndef WAGONSLIB_H
#define WAGONSLIB_H

// Must be called exactly once, before any other function from this library.
void BeginSorting();

// Returns whether queue q (which must be 1 or 2) is empty.
bool IsEmpty(int q);

// Removes the first wagon from the start of queue q and transfers it to the end
// of queue r. Both q and r must be 1 or 2, and may be equal. Queue q must not
// be empty.
void Move(int q, int r);

// Compares the wagons at the start of both queues and returns the number of the
// queue holding the smaller value. Neither queue may be empty.
int Compare();

// Must be called exactly once, after the wagons have been sorted. No further
// calls to this library are allowed afterwards.
void DoneSorting();

#endif
