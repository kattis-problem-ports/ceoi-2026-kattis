// Downloaded from https://oj.uz/submission/1419848 (scored 25/100 on oj.uz).
// Scores 25 on this package.
// @EXPECTED_GRADES@ AC AC TLE TLE
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

vector<si> g;
vi cur, sum, par;
ll root, M = 0;
ll ans = -1;

void dfs(ll v, ll p) {
    sum[v] = cur[v];
    for(auto x : g[v]) {
        if(x == p) continue;
        dfs(x, v);
        sum[v] += sum[x];
    }

    if(sum[v] >= M && (ans == -1 || sum[v] < sum[ans])) {
        ans = v;
    }
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    ll n, q;
    cin >> n >> q;
    g = vector<si>(n);
    cur = vi(n), sum = vi(n, 0), par = vi(n, 0);
    for(int i = 0; i < n; i++) {
        ll p, val;
        cin >> p >> val;
        p--;
        par[i] = p;
        M += val;
        cur[i] = val;
        if(p == -1) {
            root = i;
        }
        else {
            g[p].insert(i);
        }
    }
    M = (M + 1) / 2;
    dfs(root, -1);
    ans++;
    cout << ans << '\n';
    while(q--) {
        ll a, b;
        cin >> a >> b;
        a = (ans + a) % n;
        b = (ans + b) % n;
        g[par[a]].erase(a);
        par[a] = b;
        g[b].insert(a);
        ans = -1;
        dfs(root, -1);
        ans++;
        cout << ans << '\n';
    }
    return 0;
}