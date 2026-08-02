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

int64 modPow(int64 a, int64 k, int64 m) {
	if (k==0) return 1;
	else if (k%2==0) {
		int64 b=modPow(a,k/2,m);
		return (b*b)%m;
	} else {
		int64 b=modPow(a,k-1,m);
		return (a*b)%m;
	}
}

#define N 200'000
#define MOD 1'000'000'007

int bit[N+1];

int get(int i) {
	int r=0;
	for (int x=i;x>0;x-=x&-x) r+=bit[x];
	return r;
}

void put(int i, int d) {
	for (int x=i;x<=N;x+=x&-x) bit[x]+=d;
}

stack<PII> s;

int main() {
	//freopen("test.in","r",stdin);
	int d,x,n;
	char ch;
	cin >> d >> ch >> x;
	n=x+1;
	assert(n<=N);
	s.push({d,x});
	int64 k=0;
	REP (i,n-1) {
		cin >> d >> ch >> x;
		while (d <= s.top().first) {
			put(s.top().second+1, -1);
			s.pop();
		}
		k+=get(x+1);
		s.push({d,x});
		put(x+1, +1);
	}
	cout << modPow(2,k,MOD) << endl;
	return 0;
}
