#!/usr/bin/env bash

# Local, patched copy of testdata_tools/gen.sh; see the note at its top. It makes
# accept_with_score(x) from the output validator multiply the group score by x,
# which is how the statement's f_n(m) is applied.
. ./gen.sh

# Interactive problem: the .ans files are unused, so they are produced by a
# submission that prints nothing.
SOLUTION_BASE=$PROBLEM_PATH/submissions
use_solution empty.cpp

# The .in files hold the secret wagon order, so they must not be shown to teams.
# The only sample is the protocol transcript in data/sample/1.interaction, which
# has no .in of its own.

# The three subtasks pin n to disjoint ranges, so there are no inclusions.
group group1 30
limits minn=30 maxn=100
tg_manual ../data_source/group1

group group2 35
limits minn=800 maxn=1000
tg_manual ../data_source/group2

group group3 35
limits minn=8000 maxn=10000
tg_manual ../data_source/group3
