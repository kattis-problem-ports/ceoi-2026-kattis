#!/usr/bin/env bash

. ../../testdata_tools/gen.sh

use_solution janez.cpp

# The five subtasks are largely incomparable: 1 and 4 bound n and m, 2 requires a
# complete graph, 3 bounds the degree. Many cases therefore satisfy the constraints of
# groups they were not shipped in, and since a group can only reuse cases from an
# *earlier* group, two sets of files live in a data_source directory other than the one
# named after their original subtask:
#   data_source/group1/g2-05, g2-06   (subtask-2 cases small enough for subtasks 1, 3, 4)
#   data_source/group2/g4-07..g4-09   (subtask-4 cases that are complete graphs)
# The filenames keep their original subtask prefix for traceability.

samplegroup
limits maxn=1000 maxm=100000
sample 1

group group1 20
limits maxn=10 maxm=20
include_group sample
tg_manual ../data_source/group1

group group2 14
limits maxn=1000 maxm=100000 complete=1
tc g1-05 # the one subtask-1 case that is a complete graph (K6)
tc g2-05
tc g2-06
tg_manual ../data_source/group2

group group3 15
limits maxn=1000 maxm=100000 maxdeg=7
tc 1
# every subtask-1 case except g1-09 has maximum degree <= 7
for t in g1-00 g1-01 g1-02 g1-03 g1-04 g1-05 g1-06 g1-07 g1-08 g1-10 g2-05 g2-06; do
  tc $t
done
tg_manual ../data_source/group3

group group4 15
limits maxn=50 maxm=1000
include_group group1
tc g4-07
tc g4-08
tc g4-09
tg_manual ../data_source/group4

group group5 36
limits maxn=1000 maxm=100000
include_group group2 group3 group4
tg_manual ../data_source/group5
