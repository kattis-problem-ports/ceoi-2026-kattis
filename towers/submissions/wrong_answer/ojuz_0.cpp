// Downloaded from https://oj.uz/submission/1415401 (scored 0/100 on oj.uz).
// Scores 0 on this package.
#include<bits/stdc++.h>
using namespace std;

int64_t solve(int n, int m, int64_t f, vector<int64_t> &c, vector<int64_t> &t) {
    vector<int64_t> dp(n+1, -1e18);

    for (int i = 0; i < n; i++) {
        if (i > 0) {
            dp[i+1] = dp[i-1] - (c[i] - c[i-1]);
        }
        dp[i+1] = max(dp[i+1], dp[i] + (i%2 ? 0 : f) - abs(c[i] - t[0]));
    }
    return dp.back();
}

int main() {
    int T;
    cin >> T;
    while (T--) {
        int64_t n, m, f;
        cin >> n >> m >> f;
        vector<int64_t> c(n), t(m);
        for (int64_t &i : c) cin >> i;
        for (int64_t &i : t) cin >> i;
        sort(c.begin(), c.end());
        sort(t.begin(), t.end());
        cout << solve(n, m, f, c, t) << endl;
    }
    return 0;
}
