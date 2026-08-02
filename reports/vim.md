# Vim (CEOI 2026, Day 2)

## State

Good. I consider this port ready. It is the first of the three CEOI 2026 problems with
non-binary scoring.

* Statement transcribed from `CEOI26-day2-tasks.pdf` (pages 7-8), including the ten-row
  step-by-step table for the sample. No illustrations.
* Test data is the original data: 25 secret cases + 1 sample, imported verbatim.
* Three reference solutions in `submissions/accepted` (`koci`, `tomaz`, `janez`) and one
  `partially_accepted` (`count_only`). All three accepted solutions produce the same
  command counts as the shipped `.out` files on all 25 secret cases and the sample.
* `check_subtask_inclusion.py`: 0 misses.
* `kattistools`: clean. `verifyproblem`: 0 errors; the three full solutions score 100 and
  `count_only` scores exactly 50, as the statement's half-credit rule requires.

## Executive decisions

**The answer is not unique, so this needs a custom output validator, and the half-credit
rule makes it a scoring one.** The minimum command *count* is unique, but the example
sequence is not: on the sample the three reference solutions produce `YPPPPPPYPP`,
`YPPYPhYPPP` and `YPYPPhYPPP`, and the official answer file has a fourth,
`YPYPhPYPPP`. The validator therefore checks the count against the judge's answer and
*simulates* the contestant's sequence rather than comparing it.

**Simulation only needs three integers.** Every character in the editor is `-`, so the
state is (text length $L$, cursor position $p$, clipboard length $C$). `P` inserts $C$
characters before $p$ and moves the cursor to $p + C - 1$. I verified this model against
the statement's ten-step table for $n = 21$ row by row, and then against all 8714 answer
lines the generator produced — every one simulates to exactly its target $n$. That
cross-check validates the model solution and my validator semantics simultaneously.

**Half credit is implemented with the multiplicative accept-score pattern.** `data/gen.sh`
is a local copy of `testdata_tools/gen.sh` patched to emit `grading: custom` and
`subtask_score=<score>`, with `graders/grader.py` taking `min` over the per-case fractions
and multiplying by the group score. The validator calls `accept_with_score(1.0)` when every
sequence is valid and `accept_with_score(0.5)` when the counts are all right but at least
one example is missing or invalid. A wrong count is a plain `wrong_answer`. The `min`
aggregation matches the statement, which grants half the points "for that subtask" rather
than per test case.

**The sample uses `sample_manual`, deliberately.** The statement's explanation table walks
through `YPYPhPYPPP` step by step. If the sample `.ans` were regenerated from the model
solution it would show a different (equally valid) sequence and the table would no longer
match what the contestant sees. The official `.pubout` is preserved as `data/sample/1.ans`.

**$n = 1$ is handled even though no test case uses it.** For $n = 1$ the answer is zero
commands, and the correct example is the empty sequence, which cannot be printed as a
token. The validator special-cases `jcount == 0` so that a bare `0` earns full credit
rather than half. The smallest $n$ anywhere in the data is 2, so this never fires, but all
three reference solutions do print a bare `0` for $n = 1$ and it would be wrong to penalise
that.

**`janez_125_vim2.cpp` prints a diagnostic line** (`N cases (K required BFS), maxN = ...`)
**to stderr, not stdout.** I checked this explicitly before shipping it, since on stdout it
would have corrupted every answer. It is harmless and I left it alone. `koci.cpp` is the
model because it is the fastest and prints nothing extra.

**`count_only.cpp` is `koci.cpp` with exactly one line deleted** — the loop that prints the
sequence. Keeping it a one-line derivation makes it obvious that its counts are the correct
ones and that only the example is missing, which is precisely the case the half-credit rule
describes.

**`validate.h` comes from `testdata_tools`, not the skill's assets.** `kattistools` flags
the asset copy as outdated; the `testdata_tools` version adds
`__attribute__((format(printf, ...)))` to the message functions. The validator compiles
clean under `-Wall -Wformat` against it.

**`time_multiplier: 3`, giving the original 1 s limit.** The slowest accepted solution is
0.16 s, so anything up to 6 lands on 1 s.

## Top 3 suggestions for improving the `port-package` skill

1. **Ship the multiplicative accept-score setup as an asset, not just a URL.** The skill
   links to `Matistjati/Multiplicative_accept_score_Kattis` but the porter still has to
   work out that it means: patch two lines of `gen.sh` into `data/gen.sh`, add
   `graders/grader.py`, set `validation: custom score`, and call `accept_with_score` with a
   *fraction*. I reconstructed it by reading the existing `ceoi-2021/wells` port. A
   `assets/multiplicative_score/` directory with those three files would remove an entire
   research step, and would also make the `subtask_score=0` line for the sample group
   discoverable.

2. **Say that the `.ans` used for a sample must match the statement's worked example.** The
   skill explains `sample_manual` in terms of "multiple possible answers", which sounds
   optional. The sharper rule is: if the statement narrates a specific answer, that answer
   is part of the statement and must be preserved, or the explanation silently becomes
   wrong. This is easy to miss because everything still verifies.

3. **Recommend replaying the generated `.ans` files through the output validator's own
   model.** Writing twenty lines of Python to simulate all 8714 answer lines caught nothing
   here, but it is the only way to be confident that a simulating validator and the model
   solution agree on the rules — and if they had disagreed, the port would have shipped a
   validator that rejects correct submissions. The skill asks the porter to check the
   output validator's correctness but suggests no concrete technique.
