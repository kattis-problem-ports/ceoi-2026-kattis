#!/usr/bin/env bash

. ../../testdata_tools/gen.sh

use_solution janez.cpp

compile gen_blocktree.py

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
for t in g1-00 g1-01 g1-02 g1-03 g1-04 g1-05 g1-06 g1-07 g1-08 g1-10 g2-05 g2-06; do
  tc $t
done
tg_manual ../data_source/group3
tc g3-blocktree-deg7 gen_blocktree n=1000 hub=4 spoke=3 mode=consume maxdeg=7

group group4 15
limits maxn=50 maxm=1000
include_group group1
tc g4-07
tc g4-08
tc g4-09
tg_manual ../data_source/group4
tc g4-blocktree-small gen_blocktree n=50 hub=4 spoke=4 mode=consume

group group5 36
limits maxn=1000 maxm=100000
include_group group2 group3 group4
tg_manual ../data_source/group5
tc g5-blocktree-wide gen_blocktree n=1000 hub=8 spoke=5 mode=consume iso=5
