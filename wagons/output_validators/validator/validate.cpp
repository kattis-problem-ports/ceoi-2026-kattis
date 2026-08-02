// Interactive output validator for "Wagon Sorting" (CEOI 2026, practice).
//
// Judge input file: n, then the n wagon values in queue 1 from the start of the
// queue to its end.
//
// It speaks the line protocol described in include/cpp/grader.cpp and scores the
// run with the statement's piecewise-linear function f_n(m), where m counts the
// calls to IsEmpty, Move and Compare (BeginSorting and DoneSorting are free).
#include "validate.h"

#include <algorithm>
#include <cstdio>
#include <deque>
#include <string>
#include <vector>
using namespace std;

typedef long long ll;

// Never echo anything the submission printed; only fixed reasons.
[[noreturn]] static void protocol_error(const char* why) {
    judge_message("Protocol violation: %s\n", why);
    author_message("Protocol violation\n");
    wrong_answer("Protocol violation\n");
}

[[noreturn]] static void not_sorted(const char* why) {
    judge_message("Not sorted: %s\n", why);
    author_message("The wagons are not sorted correctly\n");
    wrong_answer("Not sorted\n");
}

// accept_with_score writes the score with "%.9le" and the group grader floors
// score * subtask_score, so nudge the fraction up to keep e.g. 0.8 * 35 from
// landing on 27 instead of 28.
[[noreturn]] static void accept_fraction(double s) {
    if (s < 1.0) s = min(1.0, s * (1 + 1e-6) + 1e-9);
    accept_with_score(s);
}

int main(int argc, char** argv) {
    init_io(argc, argv);

    ll n;
    if (!(judge_in >> n)) judge_error("could not read n from the judge input");
    deque<ll> q[3]; // q[1], q[2]
    for (ll i = 0; i < n; i++) {
        ll v;
        if (!(judge_in >> v)) judge_error("could not read wagon %lld", i + 1);
        q[1].push_back(v);
    }

    bool begun = false, done = false;
    ll m = 0; // calls to IsEmpty, Move and Compare

    string cmd;
    while (author_out >> cmd) {
        if (done) protocol_error("a call was made after DoneSorting");

        if (cmd == "B") {
            if (begun) protocol_error("BeginSorting was called more than once");
            begun = true;
            continue;
        }
        if (!begun) protocol_error("a call was made before BeginSorting");

        if (cmd == "D") {
            done = true;
            continue;
        }

        if (cmd == "E") {
            ll a;
            if (!(author_out >> a)) protocol_error("IsEmpty is missing its argument");
            if (a != 1 && a != 2) protocol_error("IsEmpty was given a queue other than 1 or 2");
            m++;
            printf("%d\n", q[a].empty() ? 1 : 0);
            fflush(stdout);
        } else if (cmd == "M") {
            ll a, b;
            if (!(author_out >> a >> b)) protocol_error("Move is missing an argument");
            if (a != 1 && a != 2) protocol_error("Move was given a source queue other than 1 or 2");
            if (b != 1 && b != 2) protocol_error("Move was given a target queue other than 1 or 2");
            if (q[a].empty()) protocol_error("Move was called on an empty source queue");
            m++;
            ll w = q[a].front();
            q[a].pop_front();
            q[b].push_back(w);
        } else if (cmd == "C") {
            if (q[1].empty() || q[2].empty())
                protocol_error("Compare was called while a queue was empty");
            m++;
            printf("%d\n", q[1].front() < q[2].front() ? 1 : 2);
            fflush(stdout);
        } else {
            protocol_error("unrecognised command");
        }
    }

    if (!begun) protocol_error("BeginSorting was never called");
    if (!done) protocol_error("DoneSorting was never called");

    // All wagons back in queue 1, in increasing order of value.
    if (!q[2].empty()) not_sorted("queue 2 is not empty");
    if ((ll)q[1].size() != n) judge_error("wagons went missing: %zu of %lld", q[1].size(), n);
    for (size_t i = 1; i < q[1].size(); i++)
        if (q[1][i - 1] > q[1][i]) not_sorted("queue 1 is not in increasing order");

    // f_n(m): 1 up to 2*n*ceil(log2 n), 0.8 at 3*n*ceil(log2 n), 0.3 at n^2,
    // linear in between, and 0.3 from n^2 on.
    ll lg = 0;
    while ((ll)1 << lg < n) lg++;
    ll L1 = 2 * n * lg, L2 = 3 * n * lg, L3 = n * n;

    double score;
    if (m <= L1) score = 1.0;
    else if (m <= L2 && L2 > L1) score = 1.0 - 0.2 * double(m - L1) / double(L2 - L1);
    else if (m <= L3 && L3 > L2) score = 0.8 - 0.5 * double(m - L2) / double(L3 - L2);
    else score = 0.3;
    if (score > 1.0) score = 1.0;
    if (score < 0.3) score = 0.3;

    judge_message("n = %lld, operations = %lld, thresholds %lld/%lld/%lld, score %.6f\n",
                  n, m, L1, L2, L3, score);
    accept_fraction(score);
}
