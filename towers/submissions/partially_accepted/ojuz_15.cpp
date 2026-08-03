// Downloaded from https://oj.uz/submission/1413284 (scored 15/100 on oj.uz).
// Scores 15 on this package.
// @EXPECTED_GRADES@ AC AC WA WA WA
#include <bits/stdc++.h>

using namespace std;
typedef long long ll;

constexpr ll INF = 1e18;

vector<ll> solve_side(ll f, vector<ll> &a, vector<ll> &b) {
    vector<pair<ll, int>> pos = {{-INF, 0}};
    for (ll x : a) pos.emplace_back(x, 0);
    for (ll x : b) pos.emplace_back(x, 1);

    sort(pos.begin(), pos.end());
    ll csum = 0, cmin = 0;
    vector<ll> ret;
    for (int i = 1; i < pos.size(); i++) {
        auto [last_x, last_t] = pos[i-1];
        auto [cur_x, cur_t] = pos[i];

        csum += last_t*f - 2*(cur_x - last_x);
        cmin = min(cmin, csum);
        if (cur_t == 0) ret.push_back(csum - cmin);
    }
    return ret;
}

ll solve(int n, int m, ll f, vector<ll> &a, vector<ll> &b) {
    vector<ll> left_gain = solve_side(f, a, b), right_gain;

    {
        vector<ll> ta, tb;
        for (int i = 0; i < n; i++) ta.push_back(-a[i]);
        for (int i = 0; i < m; i++) tb.push_back(-b[i]);
        reverse(ta.begin(), ta.end());
        reverse(tb.begin(), tb.end());
        right_gain = solve_side(f, ta, tb);
        reverse(right_gain.begin(), right_gain.end());
    }

    // for (ll j : left_gain) cout << j << " ";
    // cout << "\n";
    // for (ll j : right_gain) cout << j << " ";
    // cout << "\n\n";

    int bnd = 2000;
    vector<ll> dp(bnd+1, -INF);
    dp[1] = left_gain[0];

    for (int i = 1; i < n; i++) {
        vector<ll> ndp(bnd+1, -INF);
        ll da = a[i] - a[i-1], lg = left_gain[i], rg = right_gain[i];
        int cnt = lower_bound(b.begin(), b.end(), a[i])
            - lower_bound(b.begin(), b.end(), a[i-1]);
        ll ext = f*cnt;

        for (int j = 1; j < bnd; j++) {
            ll cst = dp[j] + j*(ext - da);
            ndp[j-1] = max(ndp[j-1], cst + rg);
            ndp[j+1] = max(ndp[j+1], cst + lg);
        }
        ndp[1] = max(ndp[1], dp[0] + lg);
        dp = ndp;

        // for (ll j : dp) cout << (j <= -1e6 ? "-INF" : to_string(j)) << " ";
        // cout << "\n";
    }

    return dp[0];
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int T;
    cin >> T;
    while (T--) {
        int n, m; ll f;
        cin >> n >> m >> f;

        vector<ll> a(n), b(m);
        for (int i = 0; i < n; i++) cin >> a[i];
        for (int i = 0; i < m; i++) cin >> b[i];

        sort(a.begin(), a.end());
        sort(b.begin(), b.end());
        cout << solve(n, m, f, a, b) << "\n";
    }

    return 0;
}

/*
1
6 3 10
2 4 6 8 10 13
1 5 9
*/