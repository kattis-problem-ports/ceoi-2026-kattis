#include "validator.h"
#include <bitset>

const long long MAX_N = 1000;
const long long MAX_M = 100000; // 10^5

const int NB = 1001; // bitset width, vertices are 1..n

void run() {
    long long n = Int(1, Arg("maxn", MAX_N));
    Space();
    long long m = Int(1, Arg("maxm", MAX_M));
    Endl();

    long long maxdeg = Arg("maxdeg", MAX_N);   // subtask 3: degree <= 7
    bool complete = bool(Arg("complete", 0));  // subtask 2: m = n(n-1)/2

    vector<bitset<NB>> adj(n + 1);
    vector<long long> deg(n + 1, 0);

    for (long long e = 0; e < m; e++) {
        long long a = Int(1, n);
        Space();
        long long b = Int(1, n);
        Endl();
        if (a == b) die("flower " + to_string(a) + " is connected to itself");
        if (adj[a][b]) die("the pair (" + to_string(a) + ", " + to_string(b) +
                           ") is listed more than once");
        adj[a][b] = adj[b][a] = 1;
        deg[a]++; deg[b]++;
    }

    for (long long v = 1; v <= n; v++)
        if (deg[v] > maxdeg)
            die("flower " + to_string(v) + " has degree " + to_string(deg[v]) +
                ", above the allowed " + to_string(maxdeg));

    if (complete && m != n * (n - 1) / 2)
        die("complete: expected m = n(n-1)/2 = " + to_string(n * (n - 1) / 2) +
            ", got " + to_string(m));

    // The statement guarantees the garden is already closed under the growth
    // rule: two disconnected flowers may not share two or more neighbours.
    for (long long a = 1; a <= n; a++)
        for (long long b = a + 1; b <= n; b++) {
            if (adj[a][b]) continue;
            if ((adj[a] & adj[b]).count() >= 2)
                die("flowers " + to_string(a) + " and " + to_string(b) +
                    " are disconnected but share at least two neighbours, so the "
                    "input is not closed under the growth rule");
        }
}
