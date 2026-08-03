// Downloaded from https://oj.uz/submission/1419476 (scored 50/100 on oj.uz).
// Scores 50 on this package.
// @EXPECTED_GRADES@ AC AC AC TLE TLE TLE
#include <bits/stdc++.h>
#pragma GCC optimize("O3", "unroll-loops")
using namespace std;

using ll = long long;

const int N = 200'000 + 10;
const int MOD = 1'000'000'000 + 7;

int n;
vector<int> g[N];

int pw[N];

pair<int, int> get_order_node(const string& s) {
	int order = 0;
	int node = 0;
	bool delimeter = false;
	for (const auto& c: s) {
		if (c == '/') {
			delimeter = true;
			continue;
		}
		if (!delimeter) order = order * 10 + (c - '0');
		else node = node * 10 + (c - '0');
	}
	return make_pair(order, node);
}

int dfs(int u, int max) {
	int cnt = 0;
	for (const auto& v: g[u]) {
		cnt += (v > max);
		// cout << "DFS " << v << ' ' << cnt << '\n';
		cnt += dfs(v, max);
		// cout << "DFS done " << v << ' ' << cnt << '\n';
	}
	return cnt;
}

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(nullptr); cout.tie(nullptr);
	string s;
	stack<pair<int, int>> order_node;
	while (cin >> s) {
		auto [order, node] = get_order_node(s);
		if (order == 0) {
			n = node;
		} else {
			while (order_node.top().first + 1 != order) order_node.pop();
			g[order_node.top().second].push_back(node);
			// cout << order_node.top().second << ' ' << node << '\n';
		}
		order_node.push(make_pair(order, node));
	}

	pw[0] = 1;
	for (int i = 1; i <= n; ++i) pw[i] = 1ll * pw[i - 1] * 2 % MOD;
	// cout << '\n';
	int res = 1;
	for (int u = 0; u <= n; ++u) {
		int cnt = 0;
		for (const auto& v: g[u]) {
			// cout << u << ' ' << v << '\n';
			cnt += dfs(v, v);
		}
		// cout << u << ' ' << cnt << ' ' << pw[cnt] << '\n';
		res = 1ll * res * pw[cnt] % MOD;
	}
	cout << res;
	return (0 ^ 0);
}

// Code by vuavisao