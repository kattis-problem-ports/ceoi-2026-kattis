// Downloaded from https://oj.uz/submission/1412314 (scored 30/100 on oj.uz).
// Scores 30 on this package.
// @EXPECTED_GRADES@ WA WA WA AC AC WA
#include <bits/stdc++.h>
#include <ext/pb_ds/tree_policy.hpp>
#include <ext/pb_ds/assoc_container.hpp>
#define all(a) (a).begin(), (a).end()
#define popcount __builtin_popcountll
using namespace std;
//using namespace __gnu_pbds;
//template<typename T> using ordered_set = tree<T, null_type, less<>, rb_tree_tag, tree_order_statistics_node_update>;

const int N = 200'000, MOD = 1e9+7, offset = 1 << 18;
vector<int> adj[N];
int tree[2*offset];
long long ans = 0;

void update(int i, int x) {
    tree[i += offset] = x;
    while (i >>= 1) tree[i] = tree[2*i] + tree[2*i + 1];
}

int query(int k, int v = 1, int l = 0, int r = offset - 1) {
    if (r < k) return tree[v];
    if (k <= l) return 0;
    int mid = (l+r)/2;
    return query(k, 2*v, l, mid) + query(k, 2*v+1, mid+1, r);
}

void dfs(int u) {
    update(u, 1);
    for (int v : adj[u]) {
        ans += query(v);
        dfs(v);
    }
    update(u, 1);
}

long long pow2(long long n) {
    if (n == 0) return 1;
    auto r = pow2(n/2);
    r *= r;
    if (n % 2) r = r*2;
    return r % MOD;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<pair<int,int>> st;
    int n;
    for (string t; cin >> t; ) {
        int p = t.find('/');
        int dep = stoi(t.substr(0, p));
        int node = stoi(t.substr(p+1));
        if (!st.empty()) {
            while (st.back().second >= dep)
                st.pop_back();
            int par = st.back().first;
            adj[par].push_back(node);
        } else {
            n = node + 1;
        }
        st.emplace_back(node, dep);
        //if (node == 1) break; // retarded ahh input
    }
    dfs(n-1);
    cout << pow2(ans);
}