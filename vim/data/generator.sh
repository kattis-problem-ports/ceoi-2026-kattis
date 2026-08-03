#!/usr/bin/env bash

# Local, patched copy of testdata_tools/gen.sh: it additionally emits
# "grading: custom" and "subtask_score=" so that accept_with_score(x) in the
# output validator multiplies the group score by x. Used for the 50% partial
# credit awarded when only the command count is correct.
. ./gen.sh

use_solution koci.cpp

compile special_n.py

samplegroup
limits maxn=100
# The statement walks through the official sample sequence step by step, so the
# shipped .ans must be preserved rather than regenerated (any minimal sequence
# is accepted, and the three reference solutions all produce different ones).
sample_manual 1

group group1 20
limits maxn=100
tc 1
tg_manual ../data_source/group1

group group2 8
limits maxn=1000
include_group group1
tg_manual ../data_source/group2
# Every n whose optimum the natural greedy misses. The original data covers 17
# of the 22 (150, 190, 201, 217 and 277 were untested), and n = 1 -- the only
# length answered by the empty command sequence -- was untested too. The rest of
# the t budget goes on the largest of them, which is what a solution that
# rebuilds an O(n^2) fallback per test case has to survive.
tc special-n special_n maxn=1000 t=100

group group3 18
limits maxn=10000
include_group group2
tg_manual ../data_source/group3

group group4 18
limits maxn=100000
include_group group3
tg_manual ../data_source/group4

group group5 18
limits maxn=1000000
include_group group4
tg_manual ../data_source/group5

group group6 18
limits maxn=10000000
include_group group5
tg_manual ../data_source/group6
