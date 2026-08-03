// Downloaded from https://oj.uz/submission/1420459 (scored 14/100 on oj.uz).
// Scores 14 on this package.
// Also WA on the samples (oj.uz does not score the sample group).
// @EXPECTED_GRADES@ WA AC WA WA WA
#include <iostream>
#include <set>
#include <vector>
#ifdef LOCAL
#define dbg(...) (cerr<<__VA_ARGS__)
#else
#define dbg(...) (void(0))
#endif
using namespace std;
using pii=pair<int, int>;

const int MN=1013;
int N, M;
int dp[MN];
vector<pii> E;
set<int> G[MN], target[MN];

bool check()
{
    dbg("CHECK"<<endl);
    while (1)
    {
        bool f=0;
        for (int u=1; u<=N && !f; u++)
        {
            for (int v=1; v<=N; v++)
            {
                if (G[u].find(v)!=G[u].end() || u==v) continue;
                int cnt=0;
                if (G[u].size()<G[v].size())
                {
                    for (int i:G[u])
                    {
                        if (G[v].find(i)!=G[v].end())
                        {
                            cnt++;
                            if (cnt==2) break;
                        }
                    }
                }
                else
                {
                    for (int i:G[v])
                    {
                        if (G[u].find(i)!=G[u].end())
                        {
                            cnt++;
                            if (cnt==2) break;
                        }
                    }
                }
                if (cnt==2)
                {
                    G[u].insert(v);
                    G[v].insert(u);
                    dbg(u<<" TO "<<v<<endl);
                    f=1;
                    break;
                }
            }
        }
        if (!f) break;
    }
    for (int i=1; i<=N; i++) if (G[i]!=target[i]) return 0;
    return 1;
}

int main()
{
    /*cin>>N>>M;
    for (int i=0; i<M; i++)
    {
        int U, V; cin>>U>>V;
        E.push_back({U, V});
        target[U].insert(V);
        target[V].insert(U);
    }*/
    cin>>N; M=N*(N-1)/2;
    dp[4]=2;
    for (int i=5; i<=N; i++) for (int j=2; j<=i; j++) dp[i]=max(dp[i], dp[j]+dp[i+2-j]+(j-2)*(i-j));
    cout<<dp[N]<<endl;
    return 0;
    dbg("HERE"<<endl);
    int ans=M;
    for (int st=0; st<(1<<M); st++)
    {
        for (int i=1; i<=N; i++) G[i].clear();
        for (int i=0; i<M; i++)
        {
            if ((st>>i)&1)
            {
                G[E[i].first].insert(E[i].second);
                G[E[i].second].insert(E[i].first);
            }
        }
        if (check()) ans=min(ans, __builtin_popcount(st));
    }
    cout<<M-ans<<endl;
    return 0;
}
