#include "validator.h"

const long long MAX_V = 1000000000000000000LL; // 10^18
const long long MAX_N = 100000;                // 10^5

void run() {
    long long v = Int(1, Arg("maxv", MAX_V));
    Space();
    long long n = Int(1, Arg("maxn", MAX_N));
    Endl();

    string types = Word();
    if ((long long)types.size() != n)
        die("type string has length " + to_string(types.size()) +
            ", expected " + to_string(n));
    for (char c : types)
        if (c != 'M' && c != 'T') die("type character must be 'M' or 'T'");
    Endl();

    // 1 <= k_i < v. Since n >= 1 this also forces v >= 2.
    for (long long i = 0; i < n; i++) {
        if (i) Space();
        Int(1, v - 1);
    }
    Endl();
}
