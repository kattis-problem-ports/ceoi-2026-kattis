// Downloaded from https://oj.uz/submission/1414163 (scored 14/100 on oj.uz).
// Scores 14 on this package.
// @EXPECTED_GRADES@ AC AC WA WA WA WA
#include <bits/stdc++.h>
using namespace std;

const int N = 1010;

int main() {
	ios_base::sync_with_stdio(0); cin.tie(0); cout.tie(0);
	vector<vector<pair<int, int>>> adjlist(N * N, vector<pair<int, int>>());
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (i != 0 && j < N - 1) adjlist[i * N + j].push_back({(i - 1) * N + j + 1, 1});
			if (j != 0 && i < N - 1) adjlist[i * N + j].push_back({(i + 1) * N + j - 1, 1});
			int cj = j + i, ci = i + 1, cnt = 2;
			while (cj < N && ci < N) {
				adjlist[i * N + j].push_back({ci * N + cj, cnt});
				cj += i, ci++, cnt++;
			}
		}
	}
	vector<int> dist(N * N, N * N);
	dist[0] = 0;
	priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
	pq.push({0, 0});
	while (pq.size()) {
		auto i = pq.top(); pq.pop();
		if (dist[i.second] < i.first) continue;
//		cout << i.first << ' ' << i.second << endl;
		for (auto j : adjlist[i.second]) {
			if (dist[j.first] > i.first + j.second) {
				dist[j.first] = i.first + j.second;
				pq.push({dist[j.first], j.first});
			}
		}
	}
//	for (int i = 0; i < N; i++) {
//		for (int j = 0; j < N; j++) cout << dist[i * N + j] << ' ';
//		cout << '\n';
//	}
	int t; cin >> t;
	while (t--) {
		int n, ans = N * N; cin >> n;
		for (int i = 0; i < n; i++) ans = min(ans, dist[(n - i - 1) * N + i]);
		cout << ans << '\n';
	}
}
