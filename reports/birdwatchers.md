# Birdwatchers (CEOI 2026, Day 1)

## State

Good. I consider this port ready.

* Statement transcribed from `CEOI26-day1-tasks.pdf` (pages 3-5). No illustrations.
* Test data is the original data: 54 secret cases + 1 sample, imported verbatim. This is
  the largest package of the nine, 188 MB of `data_source`.
* Two reference solutions are shipped, both in `submissions/accepted`
  (`janez` = centroid decomposition + splay, `tomaz` = Euler tour tree). Both reproduce the
  shipped `.out` file for all 54 secret cases and the sample.
* `check_subtask_inclusion.py`: 0 misses.
* `kattistools`: clean. `verifyproblem`: 0 errors, both submissions score 100.

## Executive decisions

**The input encoding makes one statement guarantee unverifiable, and I left it
unverified deliberately.** The statement promises that every change of allegiance is valid:
$z_j$ is never equal to $x_j$ and never a disciple of $x_j$. But $x_j$ and $z_j$ are
decoded as $1 + ((t_{j-1} + \hat{x}_j) \bmod n)$, where $t_{j-1}$ is *the answer* after the
previous change. Checking the guarantee therefore means computing the Treasurer after every
change, i.e. solving the problem inside the input validator. The validator checks
everything that does not depend on the answers — bounds, exactly one President, no cycles
in the mentorship relation, $\sum m_i \leq 10^9$, $1 \le \hat{x}_j, \hat{z}_j \le n$ — and
there is a comment at the relevant loop saying why it stops there. The guarantee is in
practice covered by the two independent solutions agreeing on all 55 cases: a violation
would have made at least one of them misbehave.

**`use_solution janez.cpp opt`.** With $n$ up to $10^6$ the default debug build of the
model is unusably slow for regenerating 54 answer files; at `-O2` the whole generator run
takes under 10 seconds. Unlike `towers`, this was purely about speed — neither solution
trips the sanitizer here.

**`time_multiplier: 5.1`, giving the original 10 s limit.** The slowest accepted solution
is `tomaz` at 1.85 s on group4, measured uncontended on the 256-core box, and
$\lceil 1.85 \times 5.1 \rceil = 10$. (This was 2.6 until a re-measurement on 2026-08-01:
the 3.71 s it was derived from came from a loaded machine, and 2.6 against an uncontended
1.85 s gives 5 s, not the intended 10 s.) The choice is not
load-bearing: there is no partially-accepted submission anywhere near the boundary, and the
two solutions differ by 4x (0.44 s vs 1.83 s on group4), so any limit from 5 s upwards
gives identical verdicts. I picked the multiplier that reproduces the contest limit rather
than the skill's default of 3, which would have given 6 s.

**Both solutions kept as accepted rather than treating the slower one as partial.** `tomaz`
is 3.7x slower but comfortably inside the original limit, so on the contest judge it was a
full solution too. Keeping it is what makes the 10 s limit meaningful — without it the
limit would be set by `janez` at 1.00 s and land at 3 s, which would be tighter than the
problem was ever run with.

**The original case numbering is not contiguous** (each group has cases 01-08, then 12-15,
then 71-72). I kept the original indices in the filenames rather than renumbering, so
`g3-71` here is `treasurer.03.71` upstream. The sub-indices repeat across groups, hence the
`g<group>-` prefix.

## Top 3 suggestions for improving the `port-package` skill

1. **Acknowledge that some statement guarantees are not checkable by an input validator,
   and say what to do instead.** The skill says the validator "should validate all
   guarantees given in the statement" with a single carve-out for "generated uniformly at
   random". Answer-dependent encodings like this one — increasingly common, since they
   force online processing — are a second category. The right response is to validate
   everything answer-independent, leave a comment at the point where you stop, and note it
   in the report; the skill should say so rather than leaving the porter to decide whether
   they have failed the instruction.

2. **Mention `use_solution <model> opt` in the main text, not only in the example file's
   comments.** For any problem with $n \ge 10^6$ the debug build makes the generator take
   minutes instead of seconds, and the `opt` flag is currently only discoverable by reading
   `generator_example.sh` closely.

3. **Give a rule for when a slower reference solution belongs in `accepted`.** Both
   `subsets` and this problem had a fast and a slow reference solution; there the slow one
   had to be demoted, here it should stay. The distinguishing question is simply "was it
   inside the original contest limit?", and stating that would have saved deliberation in
   both ports.
