// Downloaded from https://oj.uz/submission/1417184 (scored 28/100 on oj.uz).
// Scores 10 on this package, NOT 28 -- this package's test data differs from oj.uz's.
// @EXPECTED_GRADES@ AC WA WA WA WA WA
#include <bits/stdc++.h>
using namespace std;

constexpr int MAXN = 1023;
constexpr int LIM = 23 << 1;
constexpr int MAXNN = 5023;

unsigned short dp[MAXN][LIM];
unsigned short minj[MAXN];
unsigned dp2[MAXNN];

int main() {
#ifdef LOCAL
	const auto start = chrono::high_resolution_clock::now();
#endif
	cin.tie(0)->sync_with_stdio(0);

	memset(dp, 0xFF, MAXN * LIM * sizeof(unsigned short));
	dp[1][1] = 0;

	for(int i = 1; i < MAXN; i ++) {
		for(int j = 1; j <= min(LIM - 1, i); j ++) {
			if(dp[i][j] == UINT16_MAX) continue;
			if(j + 1 < min(LIM, i + 1) && dp[i][j + 1] > dp[i][j] + 1) {
				dp[i][j + 1] = dp[i][j] + 1;
			}
			for(int k = j + 1; k < LIM; k ++) {
				int dff = (k - j) * j;
				if(i + dff >= MAXN) break;
				if(dp[i + dff][k] > dp[i][j] + (k - j) + 1) {
					dp[i + dff][k] = dp[i][j] + (k - j) + 1;
				}
			}

			if(dp[i][j] < dp[i][minj[i]]) minj[i] = j;
		}
	}

	for(int i = 1; i < MAXNN; i ++) {
		int maxi = 1;

		for(int j = 1; j <= (i / 2); j ++) {
			int cur = 2 + (i - j) * (i - j) + (i - j) * 2;

			int sing = (i - j) / j;
			int extr = i - j - sing * j;
			int norm = j - extr;
			cur -= sing * sing * norm;
			cur -= (sing + 1) * (sing + 1) * extr;
			maxi = max(maxi, cur / 2);
		}

		dp2[i] = maxi;
	}

#ifdef LOCAL
	const auto end = chrono::high_resolution_clock::now();
	const chrono::duration<double> diff = end - start;
	const chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds>(diff);
	cout << ms.count() << "ms\n";
#endif

	int t;
	cin >> t;

	while(t--) {
		int n;
		cin >> n;
		if(n < MAXN) {
			cout << dp[n][minj[n]] << '\n';
			continue;
		}
		else {
			int i = lower_bound(dp2, dp2 + MAXNN, n) - dp2;
			cout << i << '\n';
		}
	}
}
