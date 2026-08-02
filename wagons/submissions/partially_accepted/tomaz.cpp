#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <math.h>
#include <queue>
#include <list>
#include <algorithm>
#include <map>
#include <set>
#include <stack>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <random>
#include <numeric>
using namespace std;

#include "wagonslib.h"

#define ALL(c) (c).begin(),(c).end()
#define PB push_back
#define IN(x,c) (find(c.begin(),c.end(),x) != (c).end())
#define REP(i,n) for (int i=0;i<(int)(n);i++)
#define FOR(i,a,b) for (int i=(a);i<=(b);i++)
#define INIT(a,v) memset(a,v,sizeof(a))
template<class A, class B> A cvt(B x) { stringstream ss; ss<<x; A y; ss>>y; return y; }
template<class A> ostream& operator<<(ostream &os, vector<A> &v) { REP (i,v.size()) { if (i!=0) os << " "; os << v[i]; } return os; }

#define SPC << " " <<
#define DEBUG(x) cerr << #x << " = "; cerr << x << endl;
#define DEBUG_ITER(x) cerr << #x << " = "; for (auto _ : x) cerr << _ << ' '; cerr << endl;

typedef pair<int,int> PII;
typedef long long int64;
typedef vector<int> VI;
typedef vector<PII> VII;
typedef vector<double> VD;

int main() {
	BeginSorting();
	int n=0;
	while (!IsEmpty(1)) { Move(1,2); n++; }
	REP (i,n) Move(2,1);
	for (int l=1;l<n;l*=2) {
		int m=ceil(1.0*n/l);
		int k=m/2;
		int n1=n, n2=0;
		REP (i,k) {
			REP (j,l) { Move(1,2); n1--; n2++; }
		}
		REP (i,k) {
			int c1=min(l,n1), c2=min(l,n2);
			while (c1>0 && c2>0) {
				if (Compare()==1) { Move(1,1); c1--; n1--; }
				else { Move(2,1); c2--; n2--; }
			}
			while (c1>0) { Move(1,1); c1--; n1--; }
			while (c2>0) { Move(2,1); c2--; n2--; }
		}
		REP (i,n1) Move(1,1);
	}
	DoneSorting();
	return 0;
}
