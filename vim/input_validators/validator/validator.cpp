#include "validator.h"

const long long MAX_T = 100;
const long long MAX_N = 10000000; // 10^7

void run() {
    long long t = Int(1, Arg("maxt", MAX_T));
    Endl();
    long long maxn = Arg("maxn", MAX_N);
    for (long long i = 0; i < t; i++) {
        Int(1, maxn);
        Endl();
    }
}
