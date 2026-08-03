#!/usr/bin/python3

# The "special" n: every n in [1, 10^7] for which the O(log n) greedy of
# submissions/accepted/janez.cpp (SolveGreedyFast) cannot hit n exactly and the
# solution has to fall back on an O(n^2) BFS.  The list was produced by an
# exhaustive scan of the whole input range (data_generation/scan_janez.cpp), so
# it is complete -- there is no special n above 547.
#
# Three disjoint reasons make these n hard, all verified against an independent
# BFS (data_generation/brute.cpp):
#   * the f(S) bound is not tight, i.e. the true optimum is one command MORE
#     than the smallest S whose longest reachable string is >= n:
#         11, 17, 41, 59, 89, 137, 201, 277
#     A solution that binary-searches for that S and prints it outputs a count
#     that is too small -> wrong answer.
#   * the optimum genuinely needs cursor moves ('h'), not just Y/P blocks:
#         17, 27, 85
#     A solution restricted to YP...P blocks needs 17 / 12 / 19 commands here
#     instead of 10 / 11 / 18.
#   * the remaining values are exactly representable with the minimum number of
#     Y/P commands, but only by a group decomposition that the natural greedy
#     ("inflate the largest group, take from the second largest") never reaches.
SPECIAL = [5, 11, 17, 21, 27, 35, 41, 45, 49, 59, 85, 89, 95,
           137, 150, 190, 201, 217, 277, 289, 408, 547]

import sys

def cmdlinearg(name, default=None):
    for arg in sys.argv:
        if arg.startswith(name + "="):
            return arg.split("=")[1]
    if default is None:
        print("missing parameter", name)
        sys.exit(1)
    return default

maxn = int(cmdlinearg('maxn'))
t = int(cmdlinearg('t'))

# Only the special n that the group's constraint admits.
ns = [n for n in SPECIAL if n <= maxn]
if not ns:
    print("no special n fits maxn =", maxn)
    sys.exit(1)

# n = 1 is the one length needing zero commands, so its example sequence is the
# empty string -- the one answer that cannot be printed as a token.
ns = [1] + ns

# Spend the whole t budget on the largest special n. A solution that rebuilds
# its fallback BFS per test case (as janez.cpp does) pays ~4n^2 every time.
if len(ns) > t:
    print("t =", t, "is too small for", len(ns), "values")
    sys.exit(1)
ns += [ns[-1]] * (t - len(ns))

print(len(ns))
print('\n'.join(map(str, ns)))
