#!/usr/bin/env bash

. ../../testdata_tools/gen.sh

# claude.cpp is the overflow-safe (__int128) model solution; the original
# reference solutions accumulated the answer in int64 and wrapped mod 2^64 on
# three group-5 cases, which is what the official .out files contain. All four
# accepted solutions (references patched to __int128 totals) now agree on all
# cases and are UBSan/ASan-clean, so the default sanitizer build is used.
use_solution claude.cpp

samplegroup
limits maxt=20 maxn=10 maxm=100
sample 1

group group1 5
limits maxsumn=5000 maxm=1
tg_manual ../data_source/group1

group group2 10
limits maxt=20 maxn=10 maxm=100
include_group sample
tg_manual ../data_source/group2

group group3 27
limits maxsumn=5000 maxsumm=5000
include_group group1 group2
tg_manual ../data_source/group3

group group4 21
limits maxsumn=5000
include_group group3
tg_manual ../data_source/group4

group group5 37
include_group group4
tg_manual ../data_source/group5
