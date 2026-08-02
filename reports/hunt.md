# Treasure Hunt (CEOI 2026, Day 1)

## State

Good. I consider this port ready.

* Statement transcribed from `CEOI26-day1-tasks.pdf` (pages 9-11), including the scoring
  formula and the worked call/return example. No illustrations.
* Test data is the original data: 53 secret cases, imported verbatim. One upstream file
  (`treasurehunt.01.67`) is byte-identical to `treasurehunt.01.64` and was dropped, so 53
  rather than 54.
* `submissions/accepted/janez.cpp` scores **100**; `partially_accepted/koci.cpp` scores
  **85**, `first_only.cpp` **28** and `k1_padded.cpp` **7**; the three
  `run_time_error/` submissions score 0.
* `check_subtask_inclusion.py`: 0 misses, no `BAD` cells.
* `kattistools`: clean. `verifyproblem`: **0 errors, 2 warnings** (`License is 'unknown'`,
  shared with every CEOI 2026 port, and `Sample testcase group ... does not contain any
  testcases`, which is intended — see below).
* Slowest AC 0.948 s uncontended on the 256-core box; `time_multiplier: 8` reproduces the
  original 8 s limit.

## Executive decisions

**The library answers queries in-process instead of over a pipe, and that is the single
decision the port hangs on.** One test case holds up to 100 000 treasure hunts of up to
1000 queries each; the reference solution makes about 2.2 · 10^7 `Query` calls on the
largest case. At the ~10 µs per round-trip that `wagons` measured for an interactive
output validator, that is 220 seconds against an 8 s limit. So `include/cpp/treasurehuntlib.cpp`
reads the hunt file from standard input itself, answers `Query` with three subtractions,
and writes a one-line summary of the run to standard output at exit; a plain
(non-interactive) `custom score` output validator turns that line into a score. Measured
cost: 0.95 s for the whole 100 000-hunt case, i.e. ~40 ns per query. The layout is
otherwise `avian`'s (NOI 2017), which is the only other `include/cpp` problem in reach.

**The summary reports only the two hunts that matter, not all 100 000.** Both branches of
the statement's score are a *minimum over all hunts*, so the score is determined by the
hunt minimising `F_i/K_i` and the hunt maximising `Q_i/ceil(log2 N_i)`. The library tracks
those two and prints `TOKEN P aI aN aK aQ aF bI bN bK bQ bF`. That keeps the output at one
line instead of 800 KB, and it lets the validator cross-check `N` and `K` for both hunts
against the judge input.

**Scoring is split between library and validator on purpose, with a shared secret.** The
library is compiled together with the submission, so a submission could in principle print
a summary of its own. `TOKEN` (a random string in `include/cpp/treasurehuntlib.cpp`, which
is *not* an attachment — contestants get the tokenless
`attachments/treasurehuntlib-public.cpp`) is what prevents that, and the `N`/`K`
cross-check means a forged summary would also have to agree with the secret test data.
This is best-effort rather than airtight: it assumes Kattis does not publish `include/`.
The alternative — interactive validation, which *is* airtight — costs the 220 seconds
above. Worth knowing if `include/` ever becomes visible.

**`min` over test cases is exactly the statement's "all hunts of a subtask are evaluated
together".** The statement pools every hunt of a subtask regardless of how they were split
into test cases. Per-hunt, define `g_i = 1/2 + 1/2·f(t_i)` when `F_i = K_i` and
`g_i = 1/2·F_i/K_i` otherwise; then the subtask score is `S · min_i g_i`, because the
incomplete hunts all have `g_i < 1/2` and the complete ones `g_i >= 1/2`. Minimum
commutes with splitting, so `grader_flags: min` over test cases reproduces the pooled
formula exactly, with no restructuring of the data.

**The validator pre-rounds so the group grader's floor becomes round-to-nearest.** The
statement says a non-integer subtask score is "rounded to the nearest integer", but
`grader.py` does `int(score * subtask_score)`. Each group therefore passes its point value
to the validator as well (`output_validator_flags: subtask_score=N`), and the validator
emits `round(fraction · S)/S` nudged up by 1e-6. Rounding is monotone, so doing it
per-test-case before the `min` gives the same answer as after. It is not cosmetic: `koci`'s
worst group 3 fraction is 0.74444, and 0.74444 · 60 = 44.67, so flooring would score it 44
where the statement says 45. Keep `subtask_score` in `output_validator_flags` equal to the
`group` score if you touch `generator.sh`.

**`janez.cpp` scoring 100 is the evidence that the reconstructed library is right.** The
organizers' `treasurehuntlib.h` and `treasurehuntlib-public.cpp` are linked from the
statement but are in neither the test archive nor the contest site, so the interface is a
reconstruction from the statement and everything behind it is mine. The check that it is
faithful: `janez` is commented "11 log n poizvedb" and is tuned to that budget, and against
this library its worst hunt over all 5.3 million hunts in the data uses
219 queries at N ≈ 10^6, i.e. `t = 10.95` — just inside the 11 needed for full credit, on
every one of the 53 cases. Any error in the direction semantics would have pushed some hunt
over. The scores were also computed twice by independent implementations (a throwaway
Python scorer reading the summaries, and `validate.cpp`) and agree on all four submissions.

**The judge library and the public one were differential-tested, not just eyeballed.** They
are separate implementations of the same `Query` (the public one keeps all hunts in memory
and generates its own; the judge one streams and reports a token-signed summary), and a
divergence would silently mislead every contestant. A throwaway probe queries 40
deterministic pseudorandom cells per hunt and checksums the answers; over seven test cases
spanning all three subtasks -- about 4.2 million queries -- both libraries produce
bit-identical checksums. `attachments/treasurehuntlib.h` is byte-identical to
`include/cpp/treasurehuntlib.h`.

**`koci.cpp` is `partially_accepted`, not `accepted`.** It is a full second reference
solution and reproduces `janez` on subtasks 1 and 2, but spends up to `15.6·log2 N` queries
on K = 3 and so scores 45/60 there. The digits in the upstream filenames
(`koci_41_...`, `janez_40_...`) are file ids, not scores — the `dfs` package numbers its
four 100-point solutions 14 through 17 the same way.

**Two synthetic partial submissions pin the two score branches.** `first_only.cpp` does the
binary search for one chest and stops, giving 10 + 8 + 10 = 28 and exercising
`F_i < K_i` (28 is exactly `10 + round(30/4) + round(60/6)`). `k1_padded.cpp` solves K = 1
but burns exactly `16·ceil(log2 N)` queries, landing on `f(16) = 4/9` and 7 points, which
pins the interior of the ramp — both reference solutions sit at `f = 1`, so nothing else in
the package tests it. The three `run_time_error/` submissions cover the library's three
abort paths (over 1000 queries, out-of-range coordinates, a call after the terminator).

**No sample test case, deliberately.** The `.in` files are the treasure coordinates, so
they cannot be shown to teams, and the statement's example is a call/return table rather
than an input/output pair. `data/sample/` therefore holds only its `testdata.yaml`, exactly
as `avian` does, and `attachments/sample.in` carries the example hunt for use with
`InitFromFile`. This is the source of verifyproblem's second warning; the alternative would
be shipping a sample whose `.ans` is a library summary, which would leak `TOKEN`.

**The `.ans` files hold the reference solution's summary rather than being empty.**
The output validator ignores `judge_ans`, so empty files would do — but verifyproblem checks
that the judge answer is itself accepted, and 53 empty files produced 53 warnings. The
generator therefore builds `janez.cpp` against the judge library and uses that as the model
(`data/model.sh`), which also leaves a per-case record of the reference query counts.

**The input validator allows runs of spaces, because the original data uses two
conventions.** The hand-written cases (`*.01`, `*.51`) pad coordinate pairs into columns
with double spaces; the generated ones use single spaces. Rather than rewrite the data, the
validator accepts one or more spaces between fields via a two-line `Spaces()` helper —
`validator.h` invites extension — and stays strict about everything else (no tabs, no
leading or trailing whitespace, no leading zeroes, exact line count). The public library
uses `fscanf`, so "whitespace-separated" is also what a contestant writing their own hunt
file will observe.

**Subtasks 1-3 pin `K` to 1, 2 and 3, so they are disjoint.** No `include_group`, no `tc`
cross-inclusions, and `check_subtask_inclusion.py` confirms there is nothing to pull in.

**`time_multiplier: 8` is not load-bearing.** The slowest accepted run is 0.948 s and the
limit it reproduces is the contest's 8 s. Nothing in the package is within 8x of the limit,
and unlike `wagons` this port pays no overhead the original did not, so the runtimes are
directly comparable to the contest's.

## Repo-wide changes made in the same pass

Both from reading the BOI 2026 git history, which is two days newer than these ports:

* `source:` is now `Central European Olympiad in Informatics (CEOI) 2026, Day 1` /
  `Day 2` / `Practice session` on all nine ports, matching BOI's `abfa212`. `kattistools`
  reports the per-day strings as a "Consistent source" error; BOI 2026 reports the identical
  error, so it is an accepted deviation rather than something to fix.
* BOI's `fc112df` additionally dropped every `memory:` limit. **Not applied here** — the
  CEOI ports keep the original contest limits by explicit decision.
* BOI's `abfa212` renamed ports to Kattis-unique shortnames (`blocks`→`blocks2`,
  `journey`→`touristsjourney`, ...). **Not done here.** Several CEOI 2026 shortnames
  (`dfs`, `flowers`, `towers`, `cave`, `subsets`, `hunt`) look likely to collide on Kattis
  and still need checking before upload.
* BOI's `1d98010` refreshed `validator.h` / `validate.h` and dropped `PPATH` from the
  generators. Already current here — verified byte-identical to `testdata_tools`, and no
  port uses `PPATH`.

## Top 3 suggestions for improving the `port-package` skill

1. **Say when a library-based interactive task should *not* be ported as an interactive
   problem.** The skill's only recipe for these is the pipe-per-call one that `wagons` uses,
   and `wagons` itself is documented as paying ~10 µs per call. The decision rule is a
   single multiplication — expected library calls times 10 µs, against the time limit — and
   when it fails the answer is the `avian` shape: an in-process library in `include/cpp/`,
   a summary line on stdout, a non-interactive `custom score` validator, and a secret token
   to stop submissions forging the summary. That token is the non-obvious part and is worth
   spelling out, including the caveat that it depends on `include/` being non-public.

2. **Warn that a `custom score` problem with unused `.ans` files produces one warning per
   test case, and give the fix.** verifyproblem checks that the judge answer is accepted by
   the output validator, so the "the validator ignores `judge_ans`, generate it with
   `empty.cpp`" pattern that works for interactive problems produces N warnings for a
   non-interactive one. Building the reference solution against the judge library and using
   *that* as the model solution costs three lines in `generator.sh` and turns N warnings
   into 0 — but it needs a `compile`-able wrapper, because `use_solution` cannot link two
   sources.

3. **Point out that `grader.py` floors and that scoring statements usually say "round".**
   Every score-band problem hits this, and the fix — pass the group's point value to the
   output validator as well and pre-round there, relying on rounding being monotone so it
   commutes with the `min` — is not something a porter will find on their own. A worked
   example would also be the natural place to warn that the `subtask_score` in
   `grader_flags` and in `output_validator_flags` must be kept equal by hand.
