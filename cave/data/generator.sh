#!/usr/bin/env bash

. ../../testdata_tools/gen.sh

use_solution model.cpp

samplegroup
limits maxn=1000 maxv=1000
sample 1

group group1 20
limits maxn=1000 maxv=1000
include_group sample
tg_manual ../data_source/group1

group group2 40
limits maxn=100000 maxv=1000000
include_group group1
tg_manual ../data_source/group2

group group3 40
limits maxn=100000 maxv=1000000000000000000
include_group group2
tg_manual ../data_source/group3
