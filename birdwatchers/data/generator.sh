#!/usr/bin/env bash

ulimit -s unlimited # the reference solutions recurse over the mentorship tree

. ../../testdata_tools/gen.sh

use_solution janez.cpp opt # n up to 10^6; a debug build is far too slow here

samplegroup
limits maxn=100
sample 1

group group1 15
limits maxn=100
include_group sample
tg_manual ../data_source/group1

group group2 10
limits maxn=1000
include_group group1
tg_manual ../data_source/group2

group group3 50
limits maxn=300000
include_group group2
tg_manual ../data_source/group3

group group4 25
limits maxn=1000000
include_group group3
tg_manual ../data_source/group4
