#!/usr/bin/env bash

ulimit -s unlimited # the reference solutions recurse to depth n

. ../../testdata_tools/gen.sh

use_solution janez.cpp

samplegroup
limits maxn=6 chain=1 incdepth=1
sample 1

group group1 10
limits maxn=6
include_group sample
tg_manual ../data_source/group1

group group2 20
limits maxn=500
include_group group1
tg_manual ../data_source/group2

group group3 20
limits maxn=10000
include_group group2
tg_manual ../data_source/group3

group group4 10
limits maxn=200000 chain=1 incdepth=1
tc 1
tc g1-01 # a group1 case that happens to be a chain
tg_manual ../data_source/group4

group group5 20
limits maxn=200000 incdepth=1
include_group group4
tc g1-02 # group1 cases whose depths happen to increase by one per line
tc g1-03
tg_manual ../data_source/group5

group group6 20
limits maxn=200000
include_group group3 group5
tg_manual ../data_source/group6
