# Flower Cutting (CEOI 2026, Day 2)

## State

Good. I consider this port ready.

* Statement transcribed from `CEOI26-day2-tasks.pdf` (pages 3-4). Both illustrations
  (before/after cutting) were extracted from the PDF as `flowers-1.png` and
  `flowers-2.png` and kept with their original captions.
* Test data is the original data: 48 secret cases + 1 sample, imported verbatim.
* **Four** reference solutions are shipped, all in `submissions/accepted`
  (`janez`, `tomaz`, `jschr`, `koci`). All four reproduce the shipped `.out` file for all
  48 secret cases and the sample.
* `check_subtask_inclusion.py`: 0 misses (21 before the fixes described below).
* `kattistools`: clean. `verifyproblem`: 0 errors, all four submissions score 100.

## Executive decisions

**The subtasks are largely incomparable, and this drove the whole data layout.** Subtasks 1
and 4 bound $n$ and $m$; subtask 2 requires a complete graph; subtask 3 bounds the degree.
Only $1 \subseteq 4 \subseteq 5$ is a genuine nesting. As a result 21 cases satisfied the
constraints of groups they were not shipped in, and `include_group` could express almost
none of it.

**Two sets of files were moved between `data_source` directories** because `gen.sh` only
lets a group reuse cases defined in an *earlier* group:

* `g2-05` ($n=2$, $m=1$) and `g2-06` ($n=3$, $m=3$) are subtask-2 cases that are small
  enough for subtasks 1, 3 and 4. They now live in `data_source/group1` and group2 pulls
  them back with `tc`.
* `g4-07`, `g4-08`, `g4-09` are subtask-4 cases that happen to be complete graphs. They now
  live in `data_source/group2` and group4 pulls them back with `tc`.

Filenames keep their original subtask prefix so the provenance stays readable.

**Group3 needs twelve explicit `tc` lines rather than `include_group group1`.** Every
subtask-1 case except `g1-09` has maximum degree $\leq 7$, and so does the sample.
`include_group group1` would drag in `g1-09` (maximum degree 8), which the validator would
then correctly reject under `maxdeg=7`. The generator uses a bash `for` loop over the ten
qualifying names plus the two relocated `g2-*` cases.

**The input validator checks the closure property, which is the problem's real
precondition.** The statement guarantees the garden is already saturated: no two
disconnected flowers share two or more neighbours. That is not a size bound and it would be
easy to skip, but it is exactly the assumption every solution relies on. It is checked with
a `bitset<1001>` per vertex and an all-pairs intersection, which is about $5 \cdot 10^5$
pairs times 16 words — negligible at $n \leq 1000$.

**Subtask flags are expressed as properties, not as subtask numbers:** `maxdeg=7` for
subtask 3 and `complete=1` for subtask 2 (checked as $m = n(n-1)/2$, which combined with
the no-duplicate-edge and no-self-loop checks does force the complete graph).

**`time_multiplier: 3`, giving Kattis' 1 s floor.** The slowest accepted solution is
0.036 s, so any multiplier up to 27 produces the same limit. This is far below the original
4 s limit, but with a 27x margin over the slowest reference solution there is nothing near
the boundary. All four solutions land within 0.02-0.04 s of each other, so there is no
signal to calibrate against.

## Top 3 suggestions for improving the `port-package` skill

1. **Document the "a group can only borrow from an earlier group" constraint and its
   workaround up front.** The skill explains `include_group` and `tc`, but not that
   `gen.sh` requires groups in lexicographic order and that reuse is therefore one
   directional. When subtasks are incomparable this is the single biggest structural
   decision in the port, and the only fix — physically moving files into an earlier
   group's `data_source` directory and pulling them back with `tc` — is not obvious.

2. **Say that non-numeric guarantees in the statement ("the input is guaranteed to follow
   the rule described above") are validator obligations too.** The skill's emphasis is on
   `maxn=`-style bounds, and it would be easy to read "validate all guarantees given in the
   statement" as covering only the bulleted constraints. Here the closure property is the
   most important thing about the input and appears only in prose.

3. **Suggest computing the subtask-membership matrix from the data before writing
   `generator.sh`.** A ten-line `awk` pass over the original cases (n, m, max degree,
   completeness) predicted all 21 misses in advance. Doing that first turns the inclusion
   checker into a confirmation step instead of two or three regenerate-and-recheck rounds.
