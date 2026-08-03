// Downloaded from https://oj.uz/submission/1419868 (scored 100/100 on oj.uz).
// Scores 100 on this package.
#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define ld long double
#define vi vector<ll>
#define vvi vector<vector<ll>>
#define vs vector<string>
#define vc vector<char>
#define vb vector<bool>
#define vp vector<pair<ll, ll>>
#define vpp vector<pair<ll, pair<ll, ll>>>
#define pp pair<ll, ll>
#define qi queue<ll>
#define qp queue<pp>
#define pqi priority_queue<ll>
#define pqp priority_queue<pp>
#define mi map<ll, ll>
#define mpi map<pp, ll>
#define mip map<ll, pp>
#define mp map<pp, pp>
#define mb map<ll, bool>
#define si set<ll>
#define sp set<pp>
#define sc set<char>
#define mod 1000000007
#define inf 1000000000000000000
#define all(x) (x).begin(), (x).end()
#define rall(x) (x).rbegin(), (x).rend()

ll binpow(ll a, ll b) {
    ll ans = 1;
    while(b > 0) {
        if(b & 1) {
            ans = (ans * a) % mod;
            b--;
        }
        else {
            a = (a * a) % mod;
            b /= 2;
        }
    }
    return ans;
}

struct SegTree {
    ll n, size;
    vi tree;

    SegTree(ll n) : n(n) {
        size = pow(2, ceil(log2(n)));
        tree.resize(size * 2, 0);
    }

    void update(ll k, ll v) {
        k += size;
        tree[k] = v;
        k /= 2;
        while(k > 0) {
            tree[k] = tree[k * 2] + tree[k * 2 + 1];
            k /= 2;
        }
    }

    ll sum(ll l, ll r, ll k, ll a, ll b) {
        if(r < a || b < l) return 0;
        if(a <= l && r <= b) return tree[k];
        ll d = (l + r) / 2;
        return sum(l, d, 2 * k, a, b) + sum(d + 1, r, 2 * k + 1, a, b);
    }

    ll get_smaller(ll v) {
        return sum(0, size - 1, 1, 0, v - 1);
    }
};

SegTree tree(0);
ll ans = 1;
vvi g;

void dfs(ll v, ll p) {
    ll pos = tree.get_smaller(v);
    ll pw = binpow(2, pos);
    ans = (ans * pw) % mod;
    for(auto x : g[v]) {
        if(x == p) continue;
        tree.update(x, 1);
        dfs(x, v);
        tree.update(x, 0);
    }
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    string line;
    vp inp;
    while(getline(cin, line)) {
        istringstream ss(line);
        string num;
        ll d = -1, v;
        while(getline(ss, num, '/')) {
            if(d == -1) d = stoi(num);
            else v = stoi(num);
        }
        inp.push_back({d, v});
    }
    ll n = inp.size();
    ll root = inp[0].second;
    stack<ll> st;
    st.push(0);
    g = vvi(n);
    for(int i = 1; i < n; i++) {
        auto[d, v] = inp[i];
        while(inp[st.top()].first + 1 != d) {
            st.pop();
        }
        ll p = inp[st.top()].second;
        g[p].push_back(v);
        g[v].push_back(p);
        st.push(i);
    }

    tree = SegTree(n);
    dfs(root, -1);
    cout << ans << '\n';
    return 0;
}