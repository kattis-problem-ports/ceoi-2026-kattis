#include <cstdio>
#include <cstring>
#include <vector>

#define FOR(i,n) for(int i=0,_n=n;i<_n;i++)
#define FORR(i,s,n) for(int i=s,_n=n;i<_n;i++)
#define pb push_back
#define vi vector<int>

#define maxn 1001
#define maxm 100001

using namespace std;

bool e[maxn][maxn];
int el[maxm][2];

int main(){
    int n,m;
    scanf("%d%d", &n,&m);
    memset(e,0,sizeof(e));
    FOR(i,m){
        scanf("%d%d", &el[i][0],&el[i][1]);
        e[el[i][0]][el[i][1]]=true;
        e[el[i][1]][el[i][0]]=true;
    }
    int min_e=0;
    FOR(i,m){
        int a=el[i][0], b=el[i][1];
        if(!e[a][b])continue;
        vi clique({a,b});
        FORR(j,1,n+1){
            if(j==a || j==b)continue;
            if(e[a][j] && e[b][j])clique.pb(j);
        }
        if(clique.size()%2)min_e+=3*(clique.size()-1)/2;
        else min_e+=3*clique.size()/2-2;
        FOR(j,clique.size())FOR(k,clique.size())e[clique[j]][clique[k]]=false;
    }
    printf("%d\n",m-min_e);
	return 0;
}
