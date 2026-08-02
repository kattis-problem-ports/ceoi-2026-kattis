# Cave (CEOI 2026, Practice session)

## State

Good. I consider this port ready.

* Statement transcribed from `CEOI26-practice-tasks.pdf` (pages 7-8). No illustrations.
* Test data is the original data: 10 secret cases + 1 sample, imported verbatim.
* `submissions/accepted/model.cpp` is the organizers' `pastirc_43_kapniki.cpp`. It
  reproduces the shipped `.out` file for **all 10 secret cases and the sample** byte for
  byte.
* `check_subtask_inclusion.py`: 0 misses — every case that satisfies a group's constraints
  is present in that group.
* `kattistools`: clean. `verifyproblem`: 0 errors, 1 warning (`License is 'unknown'`, which
  is the same as every other CEOI port in this repo).

## Executive decisions

**The organizers' second reference solution, `tomaz_53_cave.cpp`, is wrong, and is not
shipped.** It builds its list of breakpoints starting at $y = 0$ rather than $y = 1$, so
when the optimum is attained on the very first interval it counts the (invalid) height
$y = 0$ as a solution and reports one height too many. Minimal reproducer:

```
10 1
T
9
```

Only $y = 1$ leaves the railway unobstructed (a stalactite of size 9 in a cave of height 10
blocks every $y \ge 2$), so the answer is `0 1`; `tomaz_53_cave.cpp` prints `0 2`.
`pastirc_43_kapniki.cpp` prints `0 1` and is the one used as the model. Both agree on all
10 official test cases, so the bug was invisible to the original data — **the shipped test
data has no case whose optimal interval starts at $y = 1$.** I did not add one, because the
skill says not to generate test data beyond what is given; but this is the obvious gap if
you ever want to strengthen the set, and it is why `tomaz_53_cave.cpp` is not in
`submissions/accepted` even though it would score 100 on the current data.

**`time_multiplier: 3`, which lands on Kattis' 1 s floor.** The only accepted submission
runs in 0.024 s, so *any* multiplier up to 41 produces the same 1 s limit; the multiplier is
not load-bearing here and I used the skill's default. Note this is much tighter than the
original contest limit of 4 s, but with 40x headroom over the model solution, and with the
subtask-2 sweep being $O(n \log n + v)$ at $v \le 10^6$, nothing legitimate is close.

**Statement constraint `1 <= v` is not achievable.** The statement gives $1 \le v \le
10^{18}$ and $1 \le k_i < v$, but since $n \ge 1$ there is always at least one formation,
so $v \ge 2$ in every valid input. I kept the statement faithful to the original and let
the input validator enforce `Int(1, v-1)` on each $k_i$, which implies $v \ge 2$ without
stating a bound the original does not have.

**Source encoding.** `pastirc_43_kapniki.cpp` shipped as CP-1250 with CRLF line endings
(its comments are in Slovenian). It is stored as UTF-8 with LF; only the encoding changed,
the code is untouched. This was not cosmetic — `kattistools` crashes outright on the
non-UTF-8 bytes.

**`validator.h` is the copy from `testdata_tools`, not the one in the skill's assets.**
`kattistools` flags the asset copy as outdated, and the `testdata_tools` version is
genuinely newer (it rejects `-0` as an integer, which the older one accepts).

## Top 3 suggestions for improving the `port-package` skill

1. **Tell the porter to check the reference solutions against each other, not just against
   the shipped answer files.** Both solutions here match the official `.out` files on all
   10 cases, so "reproduces the official output" would have certified a solution with a
   real off-by-one. Two minutes of differential testing on hand-made edge inputs found it.
   The skill should say: when the package ships more than one reference solution, diff them
   against each other on boundary inputs before picking one as the model.

2. **The skill's `validator.h` asset is stale and conflicts with `kattistools`.** The skill
   says "Do not change `validator.h`" and ships a copy in `assets/`, but `kattistools`
   immediately warns that it is outdated and tells you to take the one from
   `testdata_tools`. Either update the asset or drop it and point at `testdata_tools`
   directly, so the porter is not made to choose between two of the skill's own tools.

3. **Warn that source files from non-English contests are often not UTF-8.** `kattistools`
   does not warn on CP-1250 input, it dies with a `UnicodeDecodeError` traceback that looks
   like a tool bug rather than a data problem. A one-line note to run
   `file submissions/*/*` and `iconv` anything that is not UTF-8 would save the porter from
   debugging the wrong thing. The same applies to CRLF.
