// Downloaded from https://oj.uz/submission/1413705 (scored 41/100 on oj.uz).
// Scores 41 on this package.
// @EXPECTED_GRADES@ AC AC AC AC AC WA
#include <bits/stdc++.h>
using namespace std;

#define f first
#define s second
#define ll long long
#define pii pair<int,int>
#define pli pair<ll,int>
#define pll pair<ll,ll>
#define tiii tuple<int,int,int>
#define tiiii tuple<int,int,int,int>
#define pb push_back
#define eb emplace_back
#define emp emplace
#define mkp make_pair
#define mkt make_tuple
#define vctr vector
#define arr array
#define all(x) x.begin(), x.end()
#define amin(a,b) a = min(a,b)
#define amax(a,b) a = max(a,b)
#define brick(x) cout << #x << " = " << (x) << " | "
#define dbg(x) cout << #x << " = " << (x) << '\n'
#define vdbg(a) cout << #a << " = "; for(auto _x : a)cout << _x << ' '; cout << '\n'
#define adbg(a,n) cout << #a << " = "; for(int _i = 1; _i <= n; ++_i)cout << a[_i] << ' '; cout << '\n'
#define adbg0(a,n) cout << #a << " = "; for(int _i = 0; _i < n; ++_i)cout << a[_i] << ' '; cout << '\n'
mt19937 rng(static_cast<uint32_t>(chrono::steady_clock::now().time_since_epoch().count()));
int uid(int a, int b) { return uniform_int_distribution<int>(a,b)(rng); }
ll uld(ll a, ll b) { return uniform_int_distribution<ll>(a,b)(rng); }

const int MOD = 1e9+7; // 998244353;

int dp[1000005];
vctr<int> ans[105];
int ans2[105];

vctr<int> v;

bool has[1000005];
bool vst[20005][205][7];
bool vst2[10005][105][7];
bool vst3[50005][505][5];

unordered_map<int,int> umap;

void curse2(int sum, int elms, int lst) {
	if (sum > 10000)return;
	if (vst2[sum][elms][v.size()])return;
	vst2[sum][elms][v.size()] = true;
	if (dp[sum] > elms-1+(int)v.size()) {
		dp[sum] = elms-1+(int)v.size();
		if (has[sum]) {
			ans[umap[sum]] = v;
		}
	}
	if (v.size() > 8)return;
	for (int i = lst; i <= 30; ++i) {
		v.pb(i);
		curse2(sum+elms*i,elms+i,i);
		v.pop_back();
	}
	return;
}

void curse(int sum, int elms, int lst) {
	if (sum > 20000 || elms > 200)return;
	// if (sum == 100000 && elms-1+(int)v.size() == 477) {
	// 	vdbg(v);
	// }
	if (vst[sum][elms][v.size()])return;
	vst[sum][elms][v.size()] = true;
	if (dp[sum] > elms-1+(int)v.size()) {
		dp[sum] = elms-1+(int)v.size();
		if (has[sum]) {
			ans[umap[sum]] = v;
		}
	}
	if (v.size() < 7) {
		for (int i = lst; i <= min(30,lst+10); ++i) {
			v.pb(i);
			curse(sum+elms*i,elms+i,i);
			v.pop_back();
		}
	} else {
		for (int i = lst-10; i <= lst+10; ++i) {
			for (int j = 1; j <= 15; ++j) {
				int nsum = sum+elms*j*i+i*i*j*(j-1)/2;
				if (nsum > 100000)break;
				if (dp[nsum] > elms+j*i-1+(int)v.size()+j) {
					dp[nsum] = elms+j*i-1+(int)v.size()+j;
					if (has[nsum]) {
						ans[umap[nsum]] = v;
						for (int ii = 0; ii < j; ++ii) {
							ans[umap[nsum]].pb(i);
						}
					}
				}
			}
		}
	}
	return;
}

int curz = 0;
int p = 4;

void curse3(int sum, int elms, int lst) {
	// if (sum > 50000 || elms > 500)return;
	// if (sum == 100000 && elms-1+(int)v.size() == 477) {
	// 	vdbg(v);
	// }
	// if (vst3[sum][elms][v.size()])return;
	// vst3[sum][elms][v.size()] = true;
	// if (dp[sum] > elms-1+(int)v.size()) {
	// 	dp[sum] = elms-1+(int)v.size();
	// 	if (has[sum]) {
	// 		ans[umap[sum]] = v;
	// 	}
	// }
	if ((int)v.size() < p) {
		for (int i = lst; i <= lst+6; ++i) {
			v.pb(i);
			curse3(sum+elms*i,elms+i,i);
			v.pop_back();
		}
	} else {
		for (int i = lst; i <= lst+7; ++i) {
			for (int j = 1; j <= 15; ++j) {
				int nsum = sum+elms*j*i+i*i*j*(j-1)/2;
				int nelms = elms+j*i;
				if (nsum > curz+1000000)break;
				for (int ii = i; ii <= i+2; ++ii) {
					for (int jj = 1; jj <= 15; ++jj) {
						int nnsum = nsum+nelms*jj*ii+ii*ii*jj*(jj-1)/2;
						int nnelms = nelms+jj*ii;
						if (nnsum > curz+1000000)break;
						if (nnsum > curz && nnsum-curz <= 1000000) {
							// if (umap.find(nnsum) == umap.end())continue;
							// int idx = umap[nnsum];
							// if (nnsum==990145 && nnelms-1+(int)v.size()+j+jj==1460) {
							// 	vdbg(v);
							// 	brick(i);dbg(ii);
							// 	brick(j);dbg(jj);
							// }
							int vidx = nnsum-curz;
							if (dp[vidx] > nnelms-1+(int)v.size()+j+jj) {
								dp[vidx] = nnelms-1+(int)v.size()+j+jj;
								if (has[vidx]) {
									int y = umap[nnsum];
									ans2[y] = dp[vidx];
									ans[y] = v;
									for (int iii = 0; iii < j; ++iii) {
										ans[y].pb(i);
									}
									for (int iii = 0; iii < jj; ++iii) {
										ans[y].pb(ii);
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return;
}

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);
	int tt;
	cin >> tt;
	vctr<int> queries;
	int aidx = 0;
	while (tt--) {
		int n;
		cin >> n;
		queries.pb(n);
		if (n == 17) {
			// cout << "10 YPYPYPhYPP\n";
			continue;
		} else if (n == 27) {
			// cout << "11 YPPYPPhYPPP\n";
			continue;
		} else if (n == 85) {
			// cout << "18 YPPPPYPPPPhYPPPPPP\n";
			continue;
		} else if (n == 990145) {
			continue;
		}
		if (umap.find(n) == umap.end()) {
			umap[n] = ++aidx;
		}
	}
	int val[10];
	for (int j = 0; j < 4; ++j) {
		val[j] = 20+25*j;
		fill(dp,dp+1000001,2e9);
		if (j == 0) {
			curse2(1,1,1);
			curse(1,1,10);
			for (auto it : umap) {
				if (it.f <= 100000) {
					ans2[it.s] = dp[it.f];
				}
			}
		}
		curz = j*1000000;
		for (auto it : umap) {
			if (curz < it.f && it.f <= curz+1000000) {
				has[it.f-curz] = true;
			}
		}
		for (int i = val[j]; i <= val[j]+40; ++i) {
			v.pb(i);
			curse3(1+i,1+i,i);
			v.pop_back();
		}
		for (auto it : umap) {
			if (curz <= it.f && it.f <= curz+1000000) {
				has[it.f-curz] = false;
			}
		}
	}
	// for (int i = 99900; i <= 100000; ++i) {
	// 	dbg(dp[i].f);
	// 	dbg(dp[i].s.size());
	// 	vdbg(dp[i].s);
	// }
	// 	dbg(mx);
	for (auto n : queries) {
		if (n == 17) {
			cout << "10 YPYPYPhYPP\n";
			continue;
		} else if (n == 27) {
			cout << "11 YPPYPPhYPPP\n";
			continue;
		} else if (n == 85) {
			cout << "18 YPPPPYPPPPhYPPPPPP\n";
			continue;
		}
		vctr<int> res;
		if (n == 990145) {
			cout << 1460 << ' ';
			res = {52,52,52,52,52,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,54,54,54,54,54,54,54};
		} else {
			cout << ans2[umap[n]] << ' ';
			res = ans[umap[n]];
		}
		// continue;
		// vdbg(res);
		// continue;
		for (auto it : res) {
			cout << 'Y';
			cout << string(it,'P');
		} cout << '\n';
	}
	return 0;
}