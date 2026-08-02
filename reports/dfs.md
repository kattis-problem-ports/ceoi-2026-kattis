# DFS (CEOI 2026, Day 1)

## State

Good. I consider this port ready.

* Statement transcribed from `CEOI26-day1-tasks.pdf` (pages 7-8). The illustration of the
  two 3-vertex graphs was extracted from the PDF as `dfs-graphs.png`.
* Test data is the original data: 60 secret cases + 1 sample, imported verbatim.
* **Four** organizer solutions are shipped, all in `submissions/accepted`
  (`janez`, `tomaz`, `jakobzorz`, `jan`), alongside `joshua.cpp`. All four organizer
  solutions reproduce the shipped `.out` file for all 60 secret cases and the sample.
* `check_subtask_inclusion.py`: 0 misses.
* `kattistools`: clean. `verifyproblem`: 0 errors, all five submissions score 100.

## Executive decisions

**The input format defeats `validator.h`'s tokenizer, and this is worth knowing before you
write the validator.** Each line is `d/v` with no spaces. `Word()` only stops at whitespace,
so it returns the whole string `"0/2"` and `Int()` then fails to parse it. My first
validator used `Int(); Char('/'); Int();` and rejected *every* test case, including the
ones in their own group — which `check_subtask_inclusion.py` reports as 157 `BAD` cells, not
as a validator error, so it reads like a catastrophic data problem rather than a two-line
bug. The shipped validator reads each line with `Line()` and parses the two fields by hand
against the grammar `0 | [1-9][0-9]*`, which keeps the strictness (no spaces, no leading
zeros, no trailing characters) that `validator.h` would normally give for free.

**$n$ is not in the input; it is recovered from the first line.** The statement guarantees
the first line is `0/n-1`, so the validator reads that line, sets $n = v + 1$, and then
requires exactly $n-1$ further lines. This also means a truncated or over-long file is
caught automatically.

**Subtasks 4 and 5 are structural, not size bounds, so they get their own validator flags.**
`chain=1` enforces "line $i$ is exactly `i-1/i-2`" (subtask 4) and `incdepth=1` enforces
"line $i$ has depth $i-1$" (subtask 5). I checked both properties directly against the
original data before writing them: every `test.04.*` case satisfies `chain`, every
`test.05.*` satisfies `incdepth` but not `chain`, and every `test.06.*` violates `incdepth`
within the first ten lines.

**Groups 4 and 5 are not supersets of groups 1-3, so three cases needed `tc`.** Subtasks 1-3
bound $n$; subtasks 4-5 constrain the shape. They are incomparable, so `include_group`
cannot express the overlap. Three of the tiny group1 cases happen to satisfy the structural
constraints anyway — `g1-01` ($n=2$, `0/1` then `1/0`) is trivially a chain, and `g1-02`,
`g1-03` have depths increasing by one per line — and the inclusion checker flagged all four
resulting misses. They are pulled in individually with `tc`. The sample is likewise a chain
and is pulled into group4 with `tc 1`.

**`time_multiplier: 3`, giving the original 1 s limit.** The slowest accepted solution is
0.25 s, so anything up to 4 lands on 1 s; the choice is not load-bearing. The four
solutions span 0.06 s to 0.25 s on group6, which is a comfortable spread well inside the
limit.

**`ulimit -s unlimited` in the generator.** All four solutions recurse once per vertex and
the data contains near-path inputs of depth ~200 000.

**All four reference solutions were kept as accepted rather than picking one.** They are
independent implementations and agreeing on all 61 cases is the strongest correctness
evidence available for this port — there is no oj.uz submission set for CEOI 2026 yet.

## Top 3 suggestions for improving the `port-package` skill

1. **Warn that `validator.h` is whitespace-tokenised, and say what to do about
   punctuation-separated formats.** `d/v`, `a,b`, `x:y` are common enough that the skill
   should show the `Line()`-plus-manual-parse pattern once, including the "no leading
   zeros, no trailing characters" checks you lose by not using `Int()`. Right now the
   asset validator only demonstrates space-separated integers.

2. **Tell the porter that `check_subtask_inclusion.py` reporting `BAD` in a case's *own*
   group means the validator is broken, not the data.** The distinction between `BAD`
   (validator rejects a case that is in the group) and `MISS` (validator accepts a case
   that is not in the group) is the single most useful debugging signal the tool gives, and
   it is not explained anywhere. A one-line gloss would turn a confusing 157-cell wall of
   red into an immediate diagnosis.

3. **Add guidance for subtasks that are incomparable rather than nested.** The skill's
   inclusion advice is entirely about the nested case ("if $N \le 1000$ in group 1 and
   $N \le 10^5$ in group 2, then group 1 should be included in group 2"). Structural
   subtasks like these cut across the size-bounded ones, so the right move is
   `include_group` for the nested chains plus `tc` for the individual cross-over cases the
   checker finds. Saying so explicitly would save a round trip.
