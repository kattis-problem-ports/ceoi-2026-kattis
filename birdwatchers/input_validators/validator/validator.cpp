#include "validator.h"

const long long MAX_N = 1000000; // 10^6
const long long MAX_Q = 30000;   // 3 * 10^4
const long long MAX_M_SUM = 1000000000; // 10^9

void run() {
    long long n = Int(1, Arg("maxn", MAX_N));
    Space();
    long long q = Int(1, Arg("maxq", MAX_Q));
    Endl();

    vector<int> mentor(n + 1, 0);
    long long roots = 0, total_members = 0;

    for (long long i = 1; i <= n; i++) {
        long long s = Int(0, n);
        if (s == 0) roots++;
        else if (s == i) die("officer " + to_string(i) + " is their own mentor");
        Space();
        long long m = Int(1, MAX_M_SUM);
        Endl();
        mentor[i] = (int)s;
        total_members += m;
        if (total_members > MAX_M_SUM)
            die("the total number of members exceeds " + to_string(MAX_M_SUM));
    }

    if (roots != 1)
        die("expected exactly one President (s_i = 0), found " + to_string(roots));

    // No officer is, directly or indirectly, a mentor of themselves: following
    // mentors from any officer must reach the President.
    vector<char> state(n + 1, 0); // 0 = unvisited, 1 = on current walk, 2 = reaches root
    for (long long s = 1; s <= n; s++) {
        if (state[s]) continue;
        vector<int> path;
        long long u = s;
        while (u != 0 && state[u] == 0) {
            state[u] = 1;
            path.push_back((int)u);
            u = mentor[u];
        }
        if (u != 0 && state[u] == 1)
            die("the mentorship relation contains a cycle");
        for (int v : path) state[v] = 2;
    }

    // The changes of allegiance. Note that the statement's guarantee that z_j is
    // not a disciple of x_j cannot be checked here: x_j and z_j are encoded
    // against t_{j-1}, the answer after the previous change, so verifying it
    // would mean solving the problem inside the validator.
    for (long long j = 0; j < q; j++) {
        Int(1, n);
        Space();
        Int(1, n);
        Endl();
    }
}
