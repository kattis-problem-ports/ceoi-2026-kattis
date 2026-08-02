// Organizers' reference solution `tomaz_52_podmnozice.cpp`.
// Correct on every test case, but too slow for group4 (n up to 5*10^6):
// 9.89 s uncontended, against the original contest limit of 8 s.
// Expected score: 80 (group1+group2+group3).
// @EXPECTED_GRADES@ AC AC AC TLE
#include <bits/stdc++.h>
using namespace std;

const int MAX_N = 5e6 + 1;
const int MAX_B = 5e6 + 1;

int n, b;
int par[MAX_N], c[MAX_N];  // parent, color of node
vector<int> adj[MAX_N];
int root = 0;

int vp[MAX_N];

vector<int> path, seq;
vector<int> by_color[MAX_B];

void dfs(int v) {
	int u;
	if (by_color[c[v]].empty()) {
		if (root!=v) vp[v] = root;
	} else {
		int ci = by_color[c[v]].back();
		if (ci+1==path.size()) vp[v]=path[ci];
		else vp[v]=path[ci+1];
	}

	by_color[c[v]].push_back(path.size());
	path.push_back(v);
	seq.push_back(v);
    for (int x : adj[v]) dfs(x);
    path.pop_back();
    by_color[c[v]].pop_back();
}

int seen[MAX_N];
vector<int> vnodes;
vector<int> vadj[MAX_N];

void build(int col) {
	for (int x : by_color[col]) {
		int p=vp[x];
		vadj[p].push_back(x);
		if (!seen[p]) {
			vnodes.push_back(p);
			seen[p]=1;
			if (c[p]!=col && p!=root) {
				vadj[par[p]].push_back(p);
				if (!seen[par[p]]) {
					vnodes.push_back(par[p]);
					seen[par[p]]=1;
				}
			}
		}
	}
}

void clean() {
	for (int x : vnodes) {
		vadj[x].clear();
		seen[x]=0;
	}
	vnodes.clear();
}

int ans[MAX_B]; // answer for each color

// solve on virtual tree
int solve(int v, int color) {
    int best1 = 0, best2 = 0; // top two largest child paths
    for (int u : vadj[v]) {
        int child_len = solve(u, color);
        if (child_len > best1) {
            best2 = best1;
            best1 = child_len;
        } else if (child_len > best2) {
            best2 = child_len;
        }
    }
    // Merge two paths with this node as the junction
    ans[color] = max(ans[color], (c[v] == color ? 1 : 0) + best1 + best2);
    return best1 + (c[v] == color);
}

int main() {
    //freopen("test.out", "w", stdout);
    //freopen("test.in", "r", stdin);
    scanf("%d %d", &n, &b);
    if (n<=2e5) {
		for (int i = 1; i <= n; i++) {
			scanf("%d %d", &par[i], &c[i]);
		}
    } else {
    	int A,B,M,K,A1,B1,M1;
    	scanf("%d %d %d %d %d %d %d",&A,&B,&M,&K,&A1,&B1,&M1);
    	long long R=0, R1=0;
		for (int i = 1; i <= n; i++) {
			R=(A*R+B)%M; R1=(A1*R1+B1)%M1;
			par[i] = (i==1)?0:i-1-(R%min(K,i-1));
			c[i] = 1+R1%b;
		}
    }
    for (int i = 1; i <= n; i++) {
    	if (par[i] == 0) root = i;
		else adj[par[i]].push_back(i);
    }
    dfs(root);
    for (int i=1;i<=n;i++) by_color[c[i]].push_back(i);
    long long sum=0;
    for (int col = 1; col <= b; col++) {
		build(col);
		solve(root, col);
		clean();
		if (n<=2e5) printf("%d\n", ans[col]);
		sum+=ans[col];
    }
    if (n>2e5) printf("%lld\n",sum);
    return 0;
}
