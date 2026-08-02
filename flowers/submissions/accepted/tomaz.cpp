#include <bits/stdc++.h>
using namespace std;

#define ALL(c) (c).begin(),(c).end()
#define PB push_back
#define IN(x,c) (find(c.begin(),c.end(),x) != (c).end())
#define REP(i,n) for (int i=0;i<(int)(n);i++)
#define FOR(i,a,b) for (int i=(a);i<=(b);i++)
#define INIT(a,v) memset(a,v,sizeof(a))
template<class A, class B> A cvt(B x) { stringstream ss; ss<<x; A y; ss>>y; return y; }

#define SPC << " " <<
#define DEBUG(x) { cerr << #x << " = "; cerr << x << endl; }
#define DEBUG_ITER(x) { cerr << #x << " = ["; for (auto _ : x) cerr << _ << "| "; cerr << "]" << endl; }
template<class A, class B> ostream& operator<<(ostream& os, pair<A,B> &p) {
	os << "(" << p.first << ", " << p.second << ")";
	return os;
}

typedef long long int64;
typedef pair<int,int> PII;
typedef pair<int64,int64> PLL;
typedef vector<int> VI;
typedef vector<PII> VII;
typedef array<int,2> II;
typedef array<int,3> III;
typedef array<int64,3> LLL;
typedef vector<string> VS;

#define N 1000

int n,m;
vector<int> neigh[N];
vector<PII> edges;

int adj[N][N];
int vis[N][N];

int keep(int c) {
	if (c%2==0) return 3*c/2-2;
	else return 3*(c-1)/2;
}

int main() {
	//freopen("test.in","r",stdin);
	cin >> n >> m;
	REP (i,m) {
		int a,b;
		cin >> a >> b; a--; b--;
		edges.push_back({a,b});
		adj[a][b]=adj[b][a]=1;
		neigh[a].push_back(b);
		neigh[b].push_back(a);
	}
	int prune=0;
	for (auto [a,b] : edges) if (!vis[a][b]) {
		vis[a][b]=1;
		vector<int> clique={a,b};
		for (int c : neigh[a]) if (adj[b][c]) {
			clique.push_back(c);
		}
		int k=clique.size();
		prune+=k*(k-1)/2-keep(k);
		for (int a : clique) for (int b : clique) vis[a][b]=1;
	}
	cout << prune << endl;
	return 0;
}
