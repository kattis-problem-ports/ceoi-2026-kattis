# Beautiful Subsets (CEOI 2026, Practice session)

## State

Good. I consider this port ready.

* Statement transcribed from `CEOI26-practice-tasks.pdf` (pages 3-5). The tree illustration
  was extracted from the PDF as `subsets-tree.png` and checked against sample 1 vertex by
  vertex: the figure's pink/green/blue classes are exactly colors 1/2/3 of the sample input,
  and the root drawn at the top is vertex 4, as the statement says.
* Test data is the original data: 20 secret cases + 2 samples, imported verbatim.
* `submissions/accepted/model.cpp` is the organizers'
  `janez_42_lepepodmnozice-lin-pruned-ok8b.cpp`. It reproduces the shipped `.out` file for
  **all 20 secret cases and both samples** byte for byte.
* `check_subtask_inclusion.py`: 0 misses.
* `kattistools`: clean. `verifyproblem`: 0 errors.

## Executive decisions

**The original test data numbers its groups 01, 02, 03, 08 — there is no 04-07.** Group 08
is subtask 4 (its four cases are the only ones using the compressed input format, with
$n$ between 4.5M and 5M). I renamed them group1..group4 because Kattis orders test groups
lexicographically and the statement's subtasks are numbered 1-4.

**`ulimit -s unlimited` in the generator is load-bearing, not a precaution.** Both reference
solutions recurse once per tree vertex. At the default 8 MB stack they segfault on
`g3-03`, `g3-06` and sample 2 — all three are near-path-shaped trees of depth ~200 000.
This initially looked like two independent solutions disagreeing with the official answer
files on the same two cases; it is in fact just stack exhaustion, and with an unlimited
stack both reproduce the official output everywhere. Kattis and `verifyproblem` both run
submissions with an unlimited stack, so only the generator needed the change.

**`time_multiplier: 2.8`, giving the original 8 s limit.** The model solution runs 2.84 s on
group4 uncontended, and $\lceil 2.84 \times 2.8 \rceil = 8$. Timings were taken with
`-j 1`; a `-j 3` run measured the same submission at 3.32 s and the slow one at 9.01 s
instead of 9.89 s, which is enough to move the rounding, so do not re-derive the multiplier
from a parallel run.

**`tomaz_52_podmnozice.cpp` is demoted to `partially_accepted`, and it is borderline.** It
is correct on every case but takes 9.89 s on group4, above the original 8 s limit, so it
scores 80. If it were left in `accepted` it would set the limit itself and drag it to 50 s,
which would make the limit meaningless. The catch: at the 8 s limit its slowest case
measures 8.14 s — under 2% over — and `verifyproblem` flags it as sensitive to the time
limit. Its recorded expected score of 80 is therefore not robust; on a Kattis machine ~10%
faster than mine it will score 100. I still chose the original 8 s over an artificially
tight 7 s, because the limit's job is to be right for the *accepted* solution, and 8 s
gives the model a comfortable 2.8x margin. Treat the 80 as "expected, but do not be alarmed
if it comes out 100".

**The two solutions differ in kind, not just speed**, which is why keeping both is worth the
borderline verdict: `janez` is the pruned linear-time solution (its filename says
`lin-pruned-ok8b`, i.e. it is the one written to clear subtask 8), while `tomaz` is the
straightforward small-to-large solution. They are genuinely independent implementations and
agreeing on all 22 cases is meaningful cross-validation.

**Sample 2 cannot be reused in groups 1-3.** It has $n = 200\,001$, so it only satisfies
subtask 4's constraints; `include_group sample` would have made groups 1-3 invalid.
Sample 1 is pulled into group1 with `tc 1` and sample 2 into group4 with `tc 2`, which keeps
`REQUIRE_SAMPLE_REUSE` satisfied without over-including.

**The input validator branches on $n$, not on the subtask.** The input format switch at
$n \leq 200\,000$ is a global rule in the statement, not a subtask constraint, so the
validator reads whichever format $n$ implies and only uses `Arg("maxn", ...)` for the
subtask bound. It also verifies the parent pointers actually form a tree (exactly one root,
no cycles), which the statement states as a guarantee.

## Top 3 suggestions for improving the `port-package` skill

1. **Tell the porter to run reference solutions with an unlimited stack before concluding
   anything about correctness.** Two solutions crashing identically on the same cases looks
   exactly like "the official answer files are wrong", which is an alarming and
   time-consuming thing to chase. The skill mentions `ulimit -s unlimited` only as a
   commented-out line in `generator.sh`; it should say up front that a segfault on deep
   inputs is almost always stack, and that Kattis runs with unlimited stack anyway.

2. **Say explicitly that time limits must be measured with `-j 1`.** The skill tells you to
   use 3 threads as a baseline for `verifyproblem`, and separately tells you to set
   `time_multiplier` from the slowest accepted runtime. Those two instructions conflict:
   here the parallel run understated the model by 15% and the slow submission by 9%, which
   is more than enough to change which second the limit rounds to.

3. **Give guidance on what to do with a correct-but-too-slow reference solution.** Leaving
   it in `accepted` silently inflates the time limit (50 s here); moving it to
   `partially_accepted` can leave it one percent from the boundary. The skill should say to
   check whether the original contest limit already excluded it, and to record the expected
   score together with an explicit note when `verifyproblem` calls it sensitive.
