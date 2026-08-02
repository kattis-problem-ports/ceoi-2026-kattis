// Grader for "Wagon Sorting". It is compiled together with your submission and
// takes care of all communication with the judge; you only call the functions
// declared in wagonslib.h.
//
// Protocol (one command per line, submission -> judge):
//   B        BeginSorting
//   E q      IsEmpty(q)   -> judge replies 0 or 1
//   M q r    Move(q, r)   -> no reply
//   C        Compare()    -> judge replies 1 or 2
//   D        DoneSorting
// Only the commands that need an answer are flushed, so a long run of Move
// calls costs one write instead of one per call.
#include "wagonslib.h"

#include <cstdio>
#include <cstdlib>

static int read_reply() {
    int x;
    // A closed pipe means the judge has already decided; exit quietly rather
    // than crashing, so the judge's verdict is the one that is reported.
    if (scanf("%d", &x) != 1) exit(0);
    return x;
}

void BeginSorting() {
    printf("B\n");
}

bool IsEmpty(int q) {
    printf("E %d\n", q);
    fflush(stdout);
    return read_reply() != 0;
}

void Move(int q, int r) {
    printf("M %d %d\n", q, r);
}

int Compare() {
    printf("C\n");
    fflush(stdout);
    return read_reply();
}

void DoneSorting() {
    printf("D\n");
    fflush(stdout);
}
