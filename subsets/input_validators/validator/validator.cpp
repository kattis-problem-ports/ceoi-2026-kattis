#include "validator.h"

const long long MAX_N = 5000000;      // 5 * 10^6
const long long SMALL_N = 200000;     // input format switches above this
const long long MAX_PARAM = 1000000000; // 10^9, for the generated-tree parameters

void run() {
    long long n = Int(1, Arg("maxn", MAX_N));
    Space();
    long long b = Int(1, n); // 1 <= b <= n
    Endl();

    if (n <= SMALL_N) {
        // Explicit tree: n lines of "p_i c_i".
        vector<int> par(n + 1, 0);
        long long roots = 0;
        for (long long i = 1; i <= n; i++) {
            long long p = Int(0, n);
            if (p == 0) roots++;
            else if (p == i) die("vertex " + to_string(i) + " is its own parent");
            Space();
            Int(1, b); // colour c_i
            Endl();
            par[i] = (int)p;
        }
        if (roots != 1)
            die("expected exactly one root (p_i = 0), found " + to_string(roots));

        // The parent edges must form a tree: with exactly one root and n-1
        // non-root edges it is enough to rule out cycles, i.e. every vertex
        // must reach the root by following parents.
        vector<char> state(n + 1, 0); // 0 = unvisited, 1 = on current walk, 2 = reaches root
        for (long long s = 1; s <= n; s++) {
            if (state[s]) continue;
            vector<int> stack;
            long long u = s;
            while (u != 0 && state[u] == 0) {
                state[u] = 1;
                stack.push_back((int)u);
                u = par[u];
            }
            if (u != 0 && state[u] == 1)
                die("the parent pointers contain a cycle, so they do not form a tree");
            for (int v : stack) state[v] = 2;
        }
    } else {
        // Generated tree: a single line "A B M K A' B' M'".
        for (int i = 0; i < 7; i++) {
            if (i) Space();
            Int(1, MAX_PARAM);
        }
        Endl();
    }
}
