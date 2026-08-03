#!/usr/bin/env bash

# Local, patched copy of testdata_tools/gen.sh: it additionally emits
# "grading: custom" and "subtask_score=" so that accept_with_score(x) in the
# output validator multiplies the group score by x, which is how the statement's
# piecewise score is applied.

# The .in files hold the treasure coordinates, so they must never be shown to
# teams and there is no sample test case. The example is sample/1.interaction,
# which Kattis renders in the statement, and attachments/sample.in is the same hunt
# in the format the public library's InitFromFile reads. gen.sh only wipes secret/,
# so 1.interaction survives regeneration; with no sample/*.in there is nothing for
# its sample-reuse check to complain about either.

. ./gen.sh

# The problem is interactive, so there is no judge answer to compute: the output
# validator scores the live run and verifyproblem does not check .ans files for
# interactive problems. The model "solution" therefore writes nothing and the .ans
# files are filled afterwards with the group's point value, which the validator
# needs for its rounding and which some problemtools versions will not pass on as
# an output_validator_flag for an interactive validator.
SOLUTION_BASE=$PROBLEM_PATH/data
use_solution model.sh

# Fill the current group's .ans files with the point value. Must come after the
# group's test cases; the wait is for gen.sh's parallel generation.
ans_subtask_score () {
  wait
  local f
  for f in "$CURGROUP_DIR"/*.ans; do
    echo "subtask_score=$1" > "$f"
  done
}

# The three subtasks pin K to 1, 2 and 3, so they are disjoint and nothing is
# included from one group into another. The output validator needs the group's
# point value to round the way the statement asks for, so every group repeats it
# in output_validator_flags; it must stay equal to the group's score.
group group1 10
output_validator_flags subtask_score=10
limits k=1
tg_manual ../data_source/group1
ans_subtask_score 10

group group2 30
output_validator_flags subtask_score=30
limits k=2
tg_manual ../data_source/group2
ans_subtask_score 30

group group3 60
output_validator_flags subtask_score=60
limits k=3
tg_manual ../data_source/group3
ans_subtask_score 60
