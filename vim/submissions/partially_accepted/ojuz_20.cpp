// Downloaded from https://oj.uz/submission/1418664 (scored 20/100 on oj.uz).
// Scores 20 on this package.
// @EXPECTED_GRADES@ AC TLE TLE TLE TLE TLE
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>

using namespace std;

const int INF = 1e9;

struct Parent {
    int i, j, k;
    char op;
};

void solve() {
    int n;
    if (!(cin >> n)) return;

    if (n == 1) {
        cout << "0 \n";
        return;
    }

    int limit = min(n + 1, (int)(3.0 * sqrt(n)) + 20);
    vector<vector<vector<int>>> dp(n + 1, vector<vector<int>>(limit, vector<int>(limit, INF)));
    vector<vector<vector<Parent>>> parent(n + 1, vector<vector<Parent>>(limit, vector<Parent>(limit)));

    dp[1][0][0] = 0;

    for (int i = 1; i <= n; i++) {
        bool changed = true;
        while (changed) {
            changed = false;
            for (int j = 0; j < limit; j++) {
                for (int k = 0; k < limit; k++) {
                    if (dp[i][j][k] == INF) continue;

                    if (k < i - 1 && k + 1 < limit) {
                        if (dp[i][j][k + 1] > dp[i][j][k] + 1) {
                            dp[i][j][k + 1] = dp[i][j][k] + 1;
                            parent[i][j][k + 1] = {i, j, k, 'h'};
                            changed = true;
                        }
                    }
                    if (k > 0) {
                        if (dp[i][j][k - 1] > dp[i][j][k] + 1) {
                            dp[i][j][k - 1] = dp[i][j][k] + 1;
                            parent[i][j][k - 1] = {i, j, k, 'l'};
                            changed = true;
                        }
                    }
                    int new_j = k + 1;
                    if (new_j < limit) {
                        if (dp[i][new_j][k] > dp[i][j][k] + 1) {
                            dp[i][new_j][k] = dp[i][j][k] + 1;
                            parent[i][new_j][k] = {i, j, k, 'Y'};
                            changed = true;
                        }
                    }
                }
            }
        }

        for (int j = 1; j < limit; j++) {
            for (int k = 0; k < limit; k++) {
                if (dp[i][j][k] == INF) continue;
                if (i + j <= n && k + 1 < limit) {
                    if (dp[i + j][j][k + 1] > dp[i][j][k] + 1) {
                        dp[i + j][j][k + 1] = dp[i][j][k] + 1;
                        parent[i + j][j][k + 1] = {i, j, k, 'P'};
                    }
                }
            }
        }
    }

    int best_ans = INF;
    int best_j = -1, best_k = -1;
    for (int j = 0; j < limit; j++) {
        for (int k = 0; k < limit; k++) {
            if (dp[n][j][k] < best_ans) {
                best_ans = dp[n][j][k];
                best_j = j;
                best_k = k;
            }
        }
    }

    string path = "";
    int ci = n, cj = best_j, ck = best_k;
    while (!(ci == 1 && cj == 0 && ck == 0)) {
        Parent p = parent[ci][cj][ck];
        path += p.op;
        ci = p.i;
        cj = p.j;
        ck = p.k;
    }
    reverse(path.begin(), path.end());

    cout << best_ans << " " << path << "\n";
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