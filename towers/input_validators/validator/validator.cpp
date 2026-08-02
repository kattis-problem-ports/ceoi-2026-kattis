#include "validator.h"

const long long MAX_T = 10000;    // 10^4
const long long MAX_SUM = 200000; // 2 * 10^5, on both sum(n) and sum(m)
const long long MAX_POS = 1000000000; // 10^9, positions and f

void run() {
    long long maxt    = Arg("maxt", MAX_T);
    long long maxsumn = Arg("maxsumn", MAX_SUM);
    long long maxsumm = Arg("maxsumm", MAX_SUM);
    long long maxn    = Arg("maxn", MAX_SUM); // per test case
    long long maxm    = Arg("maxm", MAX_SUM); // per test case

    long long T = Int(1, maxt);
    Endl();

    long long sumn = 0, summ = 0;
    for (long long tc = 0; tc < T; tc++) {
        long long n = Int(1, min(maxn, maxsumn));
        if (n % 2 != 0) die("n must be even, got " + to_string(n));
        Space();
        long long m = Int(1, min(maxm, maxsumm));
        Space();
        Int(0, MAX_POS); // f
        Endl();

        sumn += n; summ += m;
        if (sumn > maxsumn) die("sum of n exceeds " + to_string(maxsumn));
        if (summ > maxsumm) die("sum of m exceeds " + to_string(maxsumm));

        // All computer and tower positions are distinct within a test case.
        vector<long long> pos;
        pos.reserve(n + m);
        for (long long i = 0; i < n; i++) {
            if (i) Space();
            pos.push_back(Int(1, MAX_POS));
        }
        Endl();
        for (long long i = 0; i < m; i++) {
            if (i) Space();
            pos.push_back(Int(1, MAX_POS));
        }
        Endl();

        sort(pos.begin(), pos.end());
        for (size_t i = 1; i < pos.size(); i++)
            if (pos[i] == pos[i - 1])
                die("position " + to_string(pos[i]) +
                    " is used more than once in test case " + to_string(tc + 1));
    }
}
