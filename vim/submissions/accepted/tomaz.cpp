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

int sqr(int x) { return x*x; }

/* Solve for all small cases (n<=1000) in O(n^2) */

#define N 1001
#define L 101

//State = string length, cursor distance from end (last=1), clipboard size
int key(III s) { return s[0]*L*L+s[1]*L+s[2]; }

int moves[N*L*L], prv[N*L*L];
char press[N*L*L];
int state[N], sol[N];
string seq[N];

void solve_small() {
	INIT(moves,-1);
	INIT(state,-1);
	III start = {1,1,0};
	moves[key(start)] = 0;
	queue<III> q;
	q.push(start);
	int done=1;
	while (done<N) {
		III s=q.front(); q.pop();
		int m=moves[key(s)];
		auto [n,d,c]=s;
		assert(d<L && c<L);
		if (state[n]==-1) { state[n]=key(s); done++; }
		if (d<L) {
			III sY={n,d,d}; int k=key(sY);
			if (moves[k]==-1) { moves[k]=m+1; prv[k]=key(s); press[k]='Y'; q.push(sY); }
		}
		if (c>0 && n+c<N && d+1<L) {
			III sP={n+c,d+1,c}; int k=key(sP);
			if (moves[k]==-1) { moves[k]=m+1; prv[k]=key(s); press[k]='P'; q.push(sP); }
		}
		if (d<n && d+1<L) {
			III sh={n,d+1,c}; int k=key(sh);
			if (moves[k]==-1) { moves[k]=m+1; prv[k]=key(s); press[k]='h'; q.push(sh); }
		}
		if (d>1) {
			III sl={n,d-1,c}; int k=key(sl);
			if (moves[k]==-1) { moves[k]=m+1; prv[k]=key(s); press[k]='l'; q.push(sl); }
		}
	}
	for (int n=2;n<N;n++) {
		int k=state[n];
		assert(moves[k]<L);
		sol[n]=moves[k];
		int m=moves[k];
		string p;
		while (k!=key(start)) {
			p+=press[k];
			k=prv[k];
		}
		reverse(ALL(p));
		seq[n]=p;
	}
}

/* Precompute longest strings with m moves*/

#define M 5001  // m=5000 exceeds 10^7

int lon[M];

int length(int m, int k) {
	int p=m-k;
	int a=p/k;
	int k1=p%k, k0=k-k1;
	int sq=k0*sqr(a)+k1*sqr(a+1);
	int l=sqr(p)-sq;
	assert(l%2==0);
	return 1+p+l/2;
}

void solve_longest() {
	lon[0]=1;
	lon[1]=1;
	for (int m=2;m<M;m++) {
		for (int k=1;k<=m/2;k++) {  //k = number of (YP...P) groups
			lon[m]=max(lon[m], length(m,k));
		}
	}
}

int length(VI groups) {
	int l=1, s=1;
	for (int g : groups) {
		l+=s*g;
		s+=g;
	}
	return l;
}

VI modify(VI groups, int n) {
	sort(groups.begin(),groups.end()); reverse(groups.begin(),groups.end());
	REP (i,groups.size()-1) {
		// inflate the largest group as much as possible (decrease length)
		// take from the second largest to keep the rest of the same size
		while (length(groups)>n) {
			sort(groups.begin()+i,groups.end()); reverse(groups.begin()+i,groups.end());
			groups[i]++; groups[i+1]--;
			if (length(groups)<n) { groups[i]--; groups[i+1]++; break; }
		}
	}
	return groups;
}

int main() {
	//freopen("vim25.in","r",stdin);
	//freopen("test.out","w",stdout);
	solve_small();
	solve_longest();
	int tests,n;
	cin >> tests;
	REP (test,tests) {
		cin >> n;
		if (n<N) {
			cout << sol[n] << " " << seq[n] << '\n';
		} else {
			int m=1;
			while (lon[m]<n) m++;
			cout << m << ' ';
			// construct sequence
			int k=m/2;
			while (length(m,k)<n) k--;
			VI groups;
			while (k>0) {
				int g=(m-k)/k;
				groups.push_back(g);
				m-=1+g; k--;
			}
			groups = modify(groups,n);
			assert(n==length(groups));
			for (int g : groups) cout << 'Y' << string(g,'P');
			cout << '\n';
		}
	}
	return 0;
}
