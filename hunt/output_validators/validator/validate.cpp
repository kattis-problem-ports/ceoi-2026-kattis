// Interactive scoring output validator for "Treasure Hunt" (CEOI 2026, Day 1).
//
// The submission is compiled together with include/cpp/treasurehuntlib.cpp,
// which answers Query in-process. This validator speaks to that library twice:
//
//     validator -> library    REQUEST
//     library   -> validator  RESPONSE          (else the run is not scored)
//     validator -> library    the judge input, verbatim
//     library   -> validator  ACCEPTED, then    P  aI aN aK aQ aF  bI bN bK bQ bF
//
// P is the number of hunts the submission started, hunt a is the one minimising
// F/K and hunt b the one maximising Q/ceil(log2 N). Those are the only two
// hunts the statement's score depends on, since both branches of the formula are
// a minimum over all hunts. See treasurehuntlib.cpp for details.
//
// Handing the test data over only after RESPONSE is what keeps a submission from
// working around the library: the treasure coordinates never reach a program that
// does not answer the handshake, and the library takes standard input and standard
// output away from the submission as soon as it has read them. Forging a summary
// therefore requires the contents of treasurehuntlib.cpp, which contestants do
// not have -- attachments/treasurehuntlib-public.cpp is the organizers' separate
// public implementation, with no handshake and no test data in it. N_a/K_a and
// N_b/K_b are additionally checked against the judge input, so even a forgery
// built from that source has to agree with the secret test data.
//
// This validator turns the summary into the fraction of the subtask's points:
//
//     all treasures found everywhere:  1/2 + 1/2 * f(Q_b / ceil(log2 N_b))
//     otherwise:                       1/2 * F_a / K_a
//
// with f(t) = 1 for t <= 11, 1 - (t-11)/9 for 11 <= t <= 20, and 0 for t >= 20.
//
// Flags:
//   subtask_score=<S>   points the group is worth. When given, the emitted
//                       fraction is pre-rounded so that the group score comes
//                       out as round(fraction * S), which is what the statement
//                       asks for; the group grader would otherwise floor it.
//                       Some problemtools versions drop output_validator_flags
//                       for interactive validators, so the same key=value is
//                       also read from the judge answer file, which the
//                       generator fills with nothing else. A flag on the command
//                       line wins.
#include "validate.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>
using namespace std;

typedef long long ll;

// Shared secrets with include/cpp/treasurehuntlib.cpp.
static const char *const REQUEST = "must_use_the_provided_cpp_library";
static const char *const RESPONSE = "VvvbSvoVRZPPStmeuE5h";
static const char *const ACCEPTED = "OK:Ro6oOWvHpy5EUC7P4Ha6";

static const ll MAX_QUERIES = 1000;

// The submission never spoke for the library, so it was not using it. Never echo
// what it sent.
[[noreturn]] static void no_handshake() {
    judge_message("Handshake failed: the submission did not answer with the library's response\n");
    author_message("Your program did not talk to the judge's implementation of the library. "
                   "Solve this task in C++ and use the provided library: include "
                   "\"treasurehuntlib.h\" and leave standard input and standard output alone.\n");
    wrong_answer("No handshake\n");
}

// A summary that does not fit the library's output means the submission bypassed
// the library, or died in a way that skipped its bookkeeping.
[[noreturn]] static void bad_summary(const char *why) {
    judge_message("Bad library summary: %s\n", why);
    author_message("Your program did not produce the summary the library writes at exit. "
                   "Make sure you do not call exit() through a path that skips the "
                   "library's bookkeeping.\n");
    wrong_answer("Bad library summary\n");
}

static ll ceil_log2(ll n) {
    ll l = 0;
    while ((ll)1 << l < n) l++;
    return l;
}

static double f_of_t(double t) {
    if (t <= 11.0) return 1.0;
    if (t >= 20.0) return 0.0;
    return 1.0 - (t - 11.0) / 9.0;
}

// accept_with_score writes the score with "%.9le" and the group grader floors
// score * subtask_score, so nudge a fraction below 1 up to keep e.g. 46/60 from
// landing on 45.
[[noreturn]] static void accept_fraction(double s) {
    if (s > 0.0 && s < 1.0) s = min(1.0, s * (1 + 1e-6) + 1e-9);
    accept_with_score(s);
}

int main(int argc, char **argv) {
    init_io(argc, argv);

    ll subtask_score = 0;
    {
        string line;
        while (getline(judge_ans, line))
            if (line.rfind("subtask_score=", 0) == 0) subtask_score = atoll(line.c_str() + 14);
    }
    for (int i = 4; i < argc; i++)
        if (strncmp(argv[i], "subtask_score=", 14) == 0) subtask_score = atoll(argv[i] + 14);

    // ---- handshake ----
    cout << REQUEST << endl;
    string reply;
    if (!getline(author_out, reply) || reply != RESPONSE) no_handshake();

    // ---- hand over the test data, keeping a copy for the cross-check ----
    // The submission is blocked on its first read until this is through, so read
    // in megabyte blocks rather than through an istreambuf_iterator.
    string data;
    {
        static char buf[1 << 20];
        while (judge_in) {
            judge_in.read(buf, sizeof(buf));
            data.append(buf, (size_t)judge_in.gcount());
        }
    }
    if (data.empty()) judge_error("the judge input is empty");
    cout.write(data.data(), (streamsize)data.size());
    cout.flush();

    // ---- the summary the library writes at exit ----
    string ok;
    if (!getline(author_out, ok)) bad_summary("no summary at all");
    if (ok != ACCEPTED) bad_summary("wrong or missing prefix");

    ll processed;
    if (!(author_out >> processed) || processed < 0) bad_summary("missing hunt count");

    ll aI = 0, aN = 0, aK = 0, aQ = 0, aF = 0;
    ll bI = 0, bN = 0, bK = 0, bQ = 0, bF = 0;
    if (processed > 0) {
        if (!(author_out >> aI >> aN >> aK >> aQ >> aF >> bI >> bN >> bK >> bQ >> bF))
            bad_summary("truncated summary");
        if (aI < 1 || aI > processed || bI < 1 || bI > processed)
            bad_summary("worst-hunt index out of range");
        if (aF < 0 || aF > aK || bF < 0 || bF > bK) bad_summary("treasure count out of range");
        if (aQ < 0 || aQ > MAX_QUERIES || bQ < 0 || bQ > MAX_QUERIES)
            bad_summary("query count out of range");
    }

    // ---- the judge input, which is the authority on H, N_i and K_i ----
    istringstream in(data);
    ll hunts;
    if (!(in >> hunts)) judge_error("could not read the number of hunts");
    for (ll i = 1; i <= hunts; i++) {
        ll n, k;
        if (!(in >> n >> k)) judge_error("could not read hunt %lld", i);
        for (ll j = 0; j < 2 * k; j++) {
            ll dummy;
            if (!(in >> dummy)) judge_error("could not read hunt %lld", i);
        }
        if (i == aI && (n != aN || k != aK)) bad_summary("hunt a does not match the test data");
        if (i == bI && (n != bN || k != bK)) bad_summary("hunt b does not match the test data");
    }
    if (processed > hunts) bad_summary("more hunts reported than the test data contains");

    // ---- score ----
    double frac;
    if (processed < hunts) {
        // Every hunt the program never started has F_i = 0 < K_i, so the
        // minimum of F_i/K_i is 0 and only the first half of the points is in
        // play -- times zero.
        frac = 0.0;
        judge_message("Only %lld of %lld hunts were started.\n", processed, hunts);
        author_message("Your program stopped after %lld of %lld treasure hunts. Keep calling "
                       "NextHunt until it reports -1, even on hunts you give up on.\n",
                       processed, hunts);
    } else if (aF < aK) {
        frac = 0.5 * double(aF) / double(aK);
        judge_message("Worst hunt %lld (N = %lld, K = %lld): only %lld treasures found.\n",
                      aI, aN, aK, aF);
        author_message("At least one treasure hunt was left unsolved: %lld of %lld treasures "
                       "found. No credit for query efficiency.\n", aF, aK);
    } else {
        ll l = ceil_log2(bN);
        double t = double(bQ) / double(l);
        frac = 0.5 + 0.5 * f_of_t(t);
        judge_message("All treasures found. Worst hunt %lld: N = %lld, K = %lld, Q = %lld, "
                      "ceil(log2 N) = %lld, t = %.6f, f(t) = %.6f\n",
                      bI, bN, bK, bQ, l, t, f_of_t(t));
    }

    // Pre-round so the group grader's floor reproduces round(frac * S).
    if (subtask_score > 0) {
        ll pts = (ll)llround(frac * double(subtask_score));
        judge_message("Fraction %.6f of %lld points -> %lld\n", frac, subtask_score, pts);
        frac = double(pts) / double(subtask_score);
    }
    accept_fraction(frac);
}
