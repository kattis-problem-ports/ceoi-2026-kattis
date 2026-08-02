#include <bits/stdc++.h>
using namespace std;

#define DEBUG(x) { cerr << #x << " = "; cerr << x << endl; }
#define DEBUG_ITER(x) { cerr << #x << " = ["; for (auto _ : x) cerr << _ << "| "; cerr << "]" << endl; }

typedef long long int64;

// optimal right extensions for each computer
vector<int64> compute_ext_right(vector<int64> a, vector<int64> b, int64 f) {
    int n = a.size(), m = b.size();
    vector<int64> ext(n);
    int j=m-1;
    for (int i=n-1; i>=0; i--) {
        // towers between a[i] and a[i+1]
        int j2=j;
        while (j2>=0 && b[j2]>a[i]) j2--;
        int k=j2+1;
        int64 cnt=0, score=0;
        while (k<=j) {
			cnt++;
			score = max(score, cnt*f - 2*(b[k]-a[i]));
			k++;
        }
        j=j2;
        ext[i] = score;
        // extend further using next computer
        if (i<n-1) ext[i] = max(ext[i], cnt*f - 2*(a[i+1]-a[i]) + ext[i+1]);
    }
    return ext;
}


// optimal left extensions for each computer
vector<int64> compute_ext_left(vector<int64> a, vector<int64> b, int64 f) {
	for (auto &x : a) x*=-1;
	reverse(a.begin(), a.end());
	for (auto &x : b) x*=-1;
    reverse(b.begin(), b.end());
    auto ext = compute_ext_right(a, b, f);
    reverse(ext.begin(), ext.end());
    return ext;
}

// cost of cable extending all the way to the left towards 0
vector<int64> compute_cable_left(vector<int64> a, vector<int64> b, int64 f) {
    int n = a.size(), m = b.size();
    vector<int64> res(n);
    int j = 0;
    for (int i = 0; i < n; i++) {
        while (j < m && b[j] <= a[i]) j++;
        res[i] = j * f - a[i];
    }
    return res;
}

int main() {
	//freopen("test.in","r",stdin);
    int T;
    cin >> T;
    while (T--) {
        int n, m;
        int64 f;
        cin >> n >> m >> f;
        vector<int64> a(n), b(m);
        for (auto &x : a) cin >> x;
        for (auto &x : b) cin >> x;
        sort(a.begin(), a.end());
        sort(b.begin(), b.end());
        // precomputations
        auto right_ext = compute_ext_right(a, b, f);
        auto left_ext  = compute_ext_left(a, b, f);
        auto left_cab  = compute_cable_left(a, b, f);
        // baseline: all computers act as right endpoints
        int64 score = 0;
        for (int i = 0; i < n; i++) score += right_ext[i] + left_cab[i];
        // benefit of switching to left endpoint
        vector<int64> delta(n);
        for (int i = 0; i < n; i++) delta[i] = left_ext[i] - right_ext[i] - 2*left_cab[i];
        // greedy with heap
        priority_queue<int64, vector<int64>, greater<int64>> pq;
        for (int i = 0; i < n; i++) {
			int64 d=delta[n-1-i];
            pq.push(d);
            score += d;
            int lim = (i+1) / 2;
            if ((int)pq.size() > lim) {
                score -= pq.top();
                pq.pop();
            }
        }
        cout << score << endl;
    }
    return 0;
}
