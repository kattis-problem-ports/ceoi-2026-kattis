#include "validator.h"

const long long MAX_N = 10000;

void run() {
    // The subtasks each pin n to a range, so both bounds are parameters.
    long long n = Int(Arg("minn", 1), Arg("maxn", MAX_N));
    Endl();
    // The wagon values are a permutation of 1..n: they are distinct, which is
    // all the statement requires, and the original data uses exactly 1..n.
    vector<long long> v;
    v.reserve(n);
    for (long long i = 0; i < n; i++) {
        if (i) Space();
        v.push_back(Int(1, n));
    }
    Endl();
    AssertUnique(v);
}
