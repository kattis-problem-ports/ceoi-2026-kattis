#!/usr/bin/env bash

ulimit -s unlimited # both accepted solutions recurse to depth n

. ../../testdata_tools/gen.sh

use_solution model.cpp

samplegroup
limits maxn=5000000
sample 1
sample 2

group group1 25
limits maxn=20
tc 1
tg_manual ../data_source/group1

group group2 25
limits maxn=1000
include_group group1
tg_manual ../data_source/group2

group group3 30
limits maxn=200000
include_group group2
tg_manual ../data_source/group3

group group4 20
limits maxn=5000000
include_group group3
tc 2
tg_manual ../data_source/group4
