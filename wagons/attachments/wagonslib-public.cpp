// A simple local implementation of the wagon-sorting library, for developing and
// testing your solution on your own machine. Compile it together with your
// program:
//
//     g++ foo.cpp wagonslib-public.cpp
//
// On the evaluation server a different implementation is used, so do not rely on
// anything about how this one works.
//
// By default it creates a random permutation of 1000 wagons. Your program owns
// main(), so the count and the seed are taken from the environment instead of
// the command line:
//
//     ./a.out                                  # 1000 wagons, seed 12345
//     WAGON_COUNT=8000 ./a.out                 # 8000 wagons, seed 12345
//     WAGON_COUNT=8000 WAGON_SEED=7 ./a.out    # 8000 wagons, seed 7
//
// When your program calls DoneSorting, this implementation checks the result and
// prints a short report (to stderr) with the number of operations you used and
// the value of f_n(m) that the real judge would award.
#include "wagonslib.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <numeric>
#include <random>
#include <vector>

namespace {

std::deque<int> q1, q2;
long long ops = 0;
int n_wagons = 0;
bool begun = false, done = false;

[[noreturn]] void die(const char* msg) {
    fprintf(stderr, "wagonslib: %s\n", msg);
    exit(1); // the real library reports a run-time error here
}

std::deque<int>& queue_of(int q) {
    if (q != 1 && q != 2) die("queue index must be 1 or 2");
    return q == 1 ? q1 : q2;
}

void ensure_started() {
    if (!begun) die("a library function was called before BeginSorting");
    if (done) die("a library function was called after DoneSorting");
}

} // namespace

void BeginSorting() {
    if (begun) die("BeginSorting was called more than once");
    begun = true;

    int count = 1000;
    unsigned seed = 12345;
    if (const char* a = getenv("WAGON_COUNT")) count = atoi(a);
    if (const char* a = getenv("WAGON_SEED")) seed = (unsigned)atoi(a);
    if (count < 1) count = 1;

    std::vector<int> v(count);
    std::iota(v.begin(), v.end(), 1);
    std::shuffle(v.begin(), v.end(), std::mt19937(seed));
    for (int x : v) q1.push_back(x);
    n_wagons = count;
    fprintf(stderr, "wagonslib: %d wagons\n", n_wagons);
}

bool IsEmpty(int q) {
    ensure_started();
    ops++;
    return queue_of(q).empty();
}

void Move(int q, int r) {
    ensure_started();
    std::deque<int>& from = queue_of(q);
    std::deque<int>& to = queue_of(r);
    if (from.empty()) die("Move was called on an empty queue");
    ops++;
    int w = from.front();
    from.pop_front();
    to.push_back(w);
}

int Compare() {
    ensure_started();
    if (q1.empty() || q2.empty()) die("Compare was called while a queue was empty");
    ops++;
    return q1.front() < q2.front() ? 1 : 2;
}

void DoneSorting() {
    ensure_started();
    done = true;

    bool ok = q2.empty() && (int)q1.size() == n_wagons;
    for (size_t i = 1; ok && i < q1.size(); i++)
        if (q1[i - 1] > q1[i]) ok = false;

    if (!ok) {
        fprintf(stderr, "wagonslib: WRONG - the wagons are not sorted (%lld operations)\n", ops);
        return;
    }

    long long n = n_wagons, lg = 0;
    while ((1LL << lg) < n) lg++;
    long long L1 = 2 * n * lg, L2 = 3 * n * lg, L3 = n * n;
    double f;
    if (ops <= L1) f = 1.0;
    else if (ops <= L2 && L2 > L1) f = 1.0 - 0.2 * double(ops - L1) / double(L2 - L1);
    else if (ops <= L3 && L3 > L2) f = 0.8 - 0.5 * double(ops - L2) / double(L3 - L2);
    else f = 0.3;

    fprintf(stderr,
            "wagonslib: sorted correctly, %lld operations "
            "(thresholds %lld / %lld / %lld), f_n(m) = %.4f\n",
            ops, L1, L2, L3, f);
}
