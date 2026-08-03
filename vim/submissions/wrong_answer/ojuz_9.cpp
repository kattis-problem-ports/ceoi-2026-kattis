// Downloaded from https://oj.uz/submission/1417118 (scored 9/100 on oj.uz).
// Scores 0 on this package, NOT 9 -- this package's test data differs from oj.uz's.
#include <bits/stdc++.h>

using i64 = long long;

#ifdef DEBUG
    #include "../debug.h"
#else
    #define debug(...) void(23)
#endif

i64 calc(i64 x, i64 y) {
    i64 a = y / x;
    i64 b = y % x;
    return 1 + (y * (y + 1) - a * (a - 1) * (x - b) - a * (a + 1) * b) / 2;
}

void solve() {
    int N;
    std::cin >> N;

    if (N == 1) {
        std::cout << 0 << '\n';
        return;
    }

    int ans = N;
    for (int i = 1; i < N; ++i) {
        int x = (N - i - 1) / (i + 1);
        if ((N - i - 1) % (i + 1) == 0) {
            ans = std::min(ans, 2 + i + x);
        }
    }

    for (int i = 3; i <= N && i <= 2 * std::sqrt(N); ++i) {
        int lo = i, hi = N;
        while (lo < hi) {
            int mid = (lo + hi) >> 1;
            if (calc(i, mid) < N) {
                lo = mid + 1;
            } else {
                hi = mid;
            }
        }
        debug(i, lo, calc(i, lo));
        ans = std::min(ans, i + lo);
    }

    std::cout << ans << '\n';
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int TT;
    std::cin >> TT;

    while (TT--) {
        solve();
    }

    return 0;
}