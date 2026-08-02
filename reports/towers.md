# Towers (CEOI 2026, Day 2)

## State

Good. I consider this port ready.

* Statement transcribed from `CEOI26-day2-tasks.pdf` (pages 5-6). No illustrations.
* Test data is the original data: 82 secret cases + 1 sample, imported verbatim.
* **Three** reference solutions are shipped, all in `submissions/accepted`
  (`janez`, `tomaz`, `jakobzorz`). All three reproduce the shipped `.out` file for all
  82 secret cases and the sample.
* `check_subtask_inclusion.py`: 0 misses.
* `kattistools`: clean. `verifyproblem`: 0 errors, all three submissions score 100.

## Executive decisions

**All three reference solutions have signed-integer overflow, and it is not something I
introduced.** `testdata_tools` compiles the model with debug flags including UBSan, and the
generator immediately reported e.g.
`signed integer overflow: -176401371088486 + -9223344519337322198`. The second operand is a
"negative infinity" sentinel just above `LLONG_MIN`, and adding a real (negative) score to
it wraps. Counts on group5: `janez` 3, `jakobzorz` 8, `tomaz` 12. This is genuine UB, but:

* the three solutions are independent implementations and all three agree with the official
  answer files on all 83 cases, both at `-O0 -fsanitize=undefined` and at `-O2`;
* the overflow is confined to sentinel arithmetic, not to the answer.

I set `use_solution janez.cpp opt` so the generator compiles at `-O2` rather than with the
sanitizer. That silences the diagnostics rather than fixing them, which is the right call
here — patching three contest solutions' sentinel handling would be a bigger change than
the port warrants, and `verifyproblem` and Kattis both build submissions at `-O2` anyway.
**Worth knowing if these solutions are ever reused as a base for anything.**

**The answers come uncomfortably close to overflowing `long long` legitimately.** The
largest official answer is $6\,816\,546\,732\,160\,238\,079$, about $0.74 \times$
`LLONG_MAX`. With $n = m = 2 \cdot 10^5$ in a single test case and $f \le 10^9$, the
theoretical maximum of $\frac{n}{2} \cdot f \cdot m$ is $2 \cdot 10^{19}$, which does *not*
fit in a signed 64-bit integer; the shipped data stays inside the range only because
visiting towers costs length and the tested $f$ values do not push it further. I did not
add a case to probe this, per the instruction not to generate test data beyond what is
given, but it is the obvious hazard for anyone writing a new solution.

**Subtask constraints are a mix of per-test-case and summed bounds**, so the validator has
five separate flags: `maxt` (number of test cases), `maxsumn`/`maxsumm` (the summed $N$ and
$M$), and `maxn`/`maxm` (per test case). Subtask 1 is `maxsumn=5000 maxm=1`, subtask 2 is
`maxt=20 maxn=10 maxm=100`. Getting this wrong in either direction would silently mis-scope
the groups, since $N$ and $n$ are different quantities in the statement.

**The inclusion structure is a clean chain despite five subtasks:** subtasks 1 and 2 both
fit inside subtask 3 ($N, M \le 5000$), which fits inside 4, which fits inside 5. I checked
before writing the generator that no group1 case satisfies subtask 2's bounds
(their $T$ or $n$ is always too large) and vice versa, so no `tc` cross-references were
needed. The inclusion checker confirmed 0 misses on the first run.

**`time_multiplier: 3`, giving the original 1 s limit.** The slowest accepted solution is
0.146 s, so anything up to 6 lands on 1 s. The three solutions span 0.07-0.15 s on group5.

## Top 3 suggestions for improving the `port-package` skill

1. **Tell the porter what to do when the *organizers'* solutions trip the sanitizer.** The
   skill says to use `use_solution model.cpp` and nothing about the debug build's UBSan.
   When the reference code has benign-looking UB, the porter has to decide between patching
   contest code, silencing with `opt`, and treating it as a blocker. A short rule — verify
   the answers agree at `-O0+UBSan` and `-O2`, then use `opt` and document — would settle
   it consistently.

2. **Add a check for "does the answer fit in 64 bits" to the statement-review guidance.**
   Deriving the theoretical maximum from the constraints (here $2 \cdot 10^{19}$, above
   `LLONG_MAX`) is quick and is exactly the kind of latent problem defect the skill asks
   the porter to look for. It also tells you whether the shipped data is merely lucky.

3. **Warn about statements that use both per-test-case and summed limits.** This problem
   writes $n, m$ for one test case and $N, M$ for the sums, and the subtask table mixes
   them freely ($N \le 5000, m = 1$). The skill's advice to name validator flags after the
   quantity (`maxn=1000`) needs a corollary for this case: use distinct names for the
   summed and per-case bounds, or the flags become ambiguous to the next reader.
