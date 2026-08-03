// Downloaded from https://oj.uz/submission/1417146 (scored 37/100 on oj.uz).
// Scores 28 on this package, NOT 37 -- this package's test data differs from oj.uz's.
// @EXPECTED_GRADES@ AC AC WA WA WA WA
#include <bits/stdc++.h>

using i64 = long long;

#ifdef DEBUG
    #include "../debug.h"
#else
    #define debug(...) void(23)
#endif

constexpr int inf = int(1E9);

template<typename T>
bool chmin(T& a, T b) {
    if (a > b) {
        a = b;
        return true;
    }
    return false;
}

i64 calc(i64 x, i64 y) {
    i64 a = y / x;
    i64 b = y % x;
    return 1 + (y * (y + 1) - a * (a - 1) * (x - b) - a * (a + 1) * b) / 2;
}

constexpr int max_N = int(1E4) + 1;
constexpr int max_M = int(1E2) + 1;

int f[max_N][max_M];
int g[max_N][max_M];

void solve() {
    int N;
    std::cin >> N;

    if (N < max_N) {
        int res = inf;
        for (int i = 1; i <= std::min(N, max_M - 1); ++i) {
            chmin(res, f[N][i]);
        }
        std::cout << res << ' ';
        for (int i = 1; i < max_M; ++i) {
            if (f[N][i] == res) {
                std::string s = "";
                int x = N;
                int y = i;
                while (x != 1) {
                    if (g[x][y] == 1) {
                        s += 'h';
                        y -= 1;
                    } else if (g[x][y] == 2) {
                        s += 'l';
                        y += 1;
                    } else {
                        int t = -g[x][y];
                        int ny = y - t;
                        int nx = x - ny * t;
                        for (int k = 0; k < t; ++k) {
                            s += 'P';
                        }
                        s += 'Y';
                        x = nx;
                        y = ny;
                    }
                }
                std::reverse(s.begin(), s.end());
                std::cout << s << '\n';
                break;
            }
        }
        return;
    }

    int ans = N;

    for (int i = 1; i <= N && i <= 2 * std::sqrt(N); ++i) {
        int lo = i, hi = N;
        while (lo < hi) {
            int mid = (lo + hi) >> 1;
            if (calc(i, mid) < N) {
                lo = mid + 1;
            } else {
                hi = mid;
            }
        }
        ans = std::min(ans, i + lo);
    }

    std::cout << ans << '\n';
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::fill_n(&f[0][0], max_N * max_M, inf);

    f[1][1] = 0;
    for (int i = 1; i < max_N; ++i) {
        for (int j = 1; j + 1 <= std::min(i, max_M - 1); ++j) {
            if (chmin(f[i][j + 1], f[i][j] + 1)) {
                g[i][j + 1] = 1;
            }
        }
        for (int j = std::min(max_M - 1, i); j >= 2; --j) {
            if (chmin(f[i][j - 1], f[i][j] + 1)) {
                g[i][j - 1] = 2;
            }
        }
        for (int j = 1; j <= std::min(i, max_M - 1); ++j) {
            for (int k = 1; i + j * k < max_N && j + k < max_M; ++k) {
                if (chmin(f[i + j * k][j + k], f[i][j] + 1 + k)) {
                    g[i + j * k][j + k] = -k;
                }
            }
        }
    }

    int TT;
    std::cin >> TT;

    while (TT--) {
        solve();
    }

    return 0;
}