#!/usr/bin/env bash

# Local, patched copy of testdata_tools/gen.sh: it additionally emits
# "grading: custom" and "subtask_score=" so that accept_with_score(x) in the
# output validator multiplies the group score by x, which is how the statement's
# piecewise score is applied.

# The .in files hold the treasure coordinates, so they must never be shown to
# teams and there is no sample test case. The statement's call/return table is the
# example, and attachments/sample.in replays it against the local library. With no
# sample/*.in there is nothing for gen.sh's sample-reuse check to complain about.

. ./gen.sh

# The scoring output validator reads the run's score out of the submission's own
# output and ignores the .ans file, so the .ans files could be empty -- but
# verifyproblem checks that the judge answer is itself accepted, and an empty file
# is not. They therefore hold the library summary of the reference solution, which
# doubles as a record of its query counts per test case.
echo "Building the reference solution against the judge library..."
g++ -O2 -std=gnu++20 -I../include/cpp -o model_ref \
    ../submissions/accepted/janez.cpp ../include/cpp/treasurehuntlib.cpp
add_cleanup model_ref
SOLUTION_BASE=$PROBLEM_PATH/data
use_solution model.sh

# The three subtasks pin K to 1, 2 and 3, so they are disjoint and nothing is
# included from one group into another. The output validator needs the group's
# point value to round the way the statement asks for, so every group repeats it
# in output_validator_flags; it must stay equal to the group's score.
group group1 10
output_validator_flags subtask_score=10
limits k=1
tg_manual ../data_source/group1

group group2 30
output_validator_flags subtask_score=30
limits k=2
tg_manual ../data_source/group2

group group3 60
output_validator_flags subtask_score=60
limits k=3
tg_manual ../data_source/group3
