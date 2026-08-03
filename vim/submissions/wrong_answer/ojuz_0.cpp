// Downloaded from https://oj.uz/submission/1418658 (scored 0/100 on oj.uz).
// Scores 0 on this package.
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

const int INF = 1e9;

void solve() {
    int n;
    if (!(cin >> n)) return;

    int limit = min(n + 1, (int)sqrt(n) + 5);
    vector<vector<vector<int>>> dp(n + 1, vector<vector<int>>(limit, vector<int>(limit, INF)));

    dp[1][0][0] = 0;

    for (int i = 1; i <= n; i++) {
        bool changed = true;
        while (changed) {
            changed = false;
            for (int j = 0; j < limit; j++) {
                for (int k = 0; k < limit; k++) {
                    if (dp[i][j][k] == INF) continue;

                    if (k + 1 < limit && dp[i][j][k + 1] > dp[i][j][k] + 1) {
                        dp[i][j][k + 1] = dp[i][j][k] + 1;
                        changed = true;
                    }
                    if (k > 0 && dp[i][j][k - 1] > dp[i][j][k] + 1) {
                        dp[i][j][k - 1] = dp[i][j][k] + 1;
                        changed = true;
                    }
                    int new_j = k + 1;
                    if (new_j < limit && dp[i][new_j][k] > dp[i][j][k] + 1) {
                        dp[i][new_j][k] = dp[i][j][k] + 1;
                        changed = true;
                    }
                }
            }
        }

        for (int j = 1; j < limit; j++) {
            for (int k = 0; k < limit; k++) {
                if (dp[i][j][k] == INF) continue;
                if (i + j <= n && k + 1 < limit) {
                    dp[i + j][j][k + 1] = min(dp[i + j][j][k + 1], dp[i][j][k] + 1);
                }
            }
        }
    }

    int ans = INF;
    for (int j = 0; j < limit; j++) {
        for (int k = 0; k < limit; k++) {
            ans = min(ans, dp[n][j][k]);
        }
    }

    cout << ans << "\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    if (cin >> t) {
        while (t--) {
            solve();
        }
    }
    return 0;
}