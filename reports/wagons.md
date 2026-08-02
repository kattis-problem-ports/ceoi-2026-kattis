# Wagon Sorting (CEOI 2026, Practice session)

## State

Good, with one caveat that is inherent to the original material: **no shipped submission
scores 100**, because neither organizer solution stays under the full-credit operation
budget. See below.

* Statement transcribed from `CEOI26-practice-tasks.pdf` (pages 9-11), including the
  scoring table and the worked call/return example. The queue illustration was extracted
  from the PDF as `wagons-queues.png`.
* Test data is the original data: 60 secret cases (20 per group), imported verbatim. Every
  input is a permutation of $1..n$ and the $n$ ranges match the subtasks exactly
  ($[30,100]$, $[800,1000]$, $[8000,10000]$).
* `submissions/accepted/janez.cpp` scores 96, `partially_accepted/tomaz.cpp` scores 84,
  and two hand-written adversarial submissions in `wrong_answer` score 0.
* `check_subtask_inclusion.py`: 0 misses. `kattistools`: clean.
  `verifyproblem`: 0 errors, 2 warnings (license, and the "AC submission did not attain
  full score" note).

## Executive decisions

**The organizers' library is not downloadable, so I reconstructed it.** The statement links
`wagonslib.h` and `wagonslib-public.cpp`, but both URLs 404 and the files are in neither the
test archive nor the contest site. The statement specifies the five functions completely,
so `include/cpp/wagonslib.h` is a faithful reconstruction of the interface, and everything
behind it is mine:

* `include/cpp/grader.cpp` — compiled with the submission on Kattis, turns the five calls
  into a line protocol (`B`, `E q`, `M q r`, `C`, `D`).
* `output_validators/validator/validate.cpp` — the interactive judge: maintains both
  queues, enforces the protocol, counts operations and applies $f_n(m)$.
* `attachments/wagonslib-public.cpp` — a local implementation for contestants that
  generates a random permutation (size and seed from `WAGON_COUNT`/`WAGON_SEED`, since the
  contestant owns `main`) and prints the operation count and the $f_n(m)$ it would earn.
* `attachments/wagons_sample.cpp` — a selection-sort example, verified to sort correctly
  and to land in the 0.3 band as the editorial says the naive method should.

**Only `Move` is buffered; `IsEmpty` and `Compare` flush.** A merge sort on $n = 10\,000$
makes about 290 000 calls, and flushing every one of them would triple the pipe traffic.
Buffering the calls that need no reply is safe: the judge only ever writes in response to
`E` or `C`, and those are flushed before the submission blocks on a read, so there is no
deadlock.

**No submission reaches 100, and this is a property of the original solutions, not the
port.** Full credit needs $m \le 2n\lceil\log_2 n\rceil$; `janez` (merge sort) uses 288 648
operations at $n = 10\,000$ against a threshold of 280 000, i.e. 3% too many, giving
$f \approx 0.988$. Measured against the reconstructed library before any Kattis wiring
existed, `janez` scores 0.968/0.976/0.988 and `tomaz` 0.846/0.864/0.878 at
$n = 100/1000/10\,000$ — and `verifyproblem` then reproduced exactly 96 and 84. That
agreement between two independent implementations of the scoring function is the main
evidence that the port's scoring is right. `janez` stays in `accepted` despite the warning,
because the skill prefers that to having no accepted submission at all (which would leave
the time limit at the 300 s default).

**`time_multiplier: 3`, giving 9 s rather than the original 5 s.** The slowest accepted run
is 2.69 s at $n = 10\,000$, and essentially all of it is pipe round-trips: about 9.5 µs per
operation for 290 000 operations. The original library was linked into the submission and
paid none of that. Holding to 5 s would mean a limit whose headroom over the reference
solution is 1.8x, where the cost being measured is an artifact of the port rather than of
the problem. 9 s restores a 3.3x margin.

**Protocol violations are reported as WA, not RTE.** The statement says an illegal call
makes the library kill the program with a run-time error. A Kattis output validator cannot
produce RTE, and the distinction is invisible in the score — both are 0 for the whole
subtask under `on_reject: break` with `min` aggregation — so the judge rejects with
`wrong_answer` instead. The statement was reworded accordingly ("the run is rejected and
you score 0 points for the current test case") rather than promising a verdict the port
cannot produce. `submissions/wrong_answer/bad_protocol.cpp` exercises this path and
`no_sort.cpp` exercises the not-sorted path; both score 0 as intended.

**The sample is a protocol transcript with no `.in`**, following the CEOI 2023 interactive
ports in this repo. It mirrors the statement's call table exactly, and the statement says
in so many words that the contestant never writes it — the library does.

## Top 3 suggestions for improving the `port-package` skill

1. **Add a section on porting library-based interactive tasks.** This is the standard IOI/CEOI
   shape and the port needs five coordinated pieces (`include/cpp/` header + grader,
   interactive output validator, `attachments/` public library, `empty.cpp`, the sample
   `.interaction` with no `.in`). None of that is in the skill; I reconstructed it by
   reading `ceoi-2023/light`. Even a pointer saying "copy the layout from an existing
   interactive port" would help, and a worked asset would help more.

2. **Say what to do when the original grader library is not published.** It is common for
   the header to be linked from a contest site that no longer serves it. The recipe —
   reconstruct the interface from the statement, then validate the reconstruction by
   checking that the reference solutions' scores come out where the editorial says they
   should — is worth stating, because it turns an apparent blocker into a routine step.

3. **Warn that pipe overhead can dominate an interactive port's runtime, and that the
   original time limit is therefore not directly transferable.** The skill says to set the
   limit from the slowest accepted run but says nothing about interactive problems paying a
   per-call cost the original never had. A note to compare the measured time against the
   operation count (here 9.5 µs/op, i.e. essentially all overhead) would make it obvious
   when the original limit needs loosening.
