// Downloaded from https://oj.uz/submission/1414259 (scored 42/100 on oj.uz).
// Scores 42 on this package.
// @EXPECTED_GRADES@ AC AC AC RTE RTE
#include <iostream>
#include <algorithm>
#include <cassert>
#include <vector>
#ifdef LOCAL
#define dbg(...) (cerr<<__VA_ARGS__)
#else
#define dbg(...) (void(0))
#endif
using namespace std;
using ll=long long;
using pii=pair<int, int>;

const int MN=5013;
const ll inf64=4e18;
int N, M; ll F;
ll A[MN], B[MN], C[MN];
ll lft[MN], rgt[MN];
ll dp[MN][MN];

void driver()
{
    cin>>N>>M>>F;
    for (int i=1; i<=N; i++) cin>>A[i];
    for (int j=1; j<=M; j++) cin>>B[j];
    sort(A+1, A+1+N);
    sort(B+1, B+1+M);
    int ptr=0;
    for (int i=1; i<=N; i++)
    {
        while (ptr<M && B[ptr+1]<A[i]) ptr++;
        C[i]=ptr;
        dbg(i<<": "<<C[i]<<endl);
    }
    for (int i=1; i<=N; i++)
    {
        lft[i]=0;
        for (int j=1; j<=C[i]; j++)
        {
            lft[i]=max(lft[i], -(A[i]-B[j])*2+F*(C[i]-j+1));
        }
        lft[i]-=-A[i]+F*C[i];
        dbg("LFT "<<i<<": "<<lft[i]<<endl);
    }
    for (int i=1; i<=N; i++)
    {
        rgt[i]=0;
        for (int j=C[i]+1; j<=M; j++)
        {
            rgt[i]=max(rgt[i], -(B[j]-A[i])*2+F*(j-C[i]));
        }
        rgt[i]+=-A[i]+F*C[i];
        dbg("RGT "<<i<<": "<<rgt[i]<<endl);
    }
    dp[0][0]=0;
    for (int i=1; i<=N; i++)
    {
        for (int j=0; j<=min(i, N-i); j++)
        {
            dp[i][j]=-inf64;
            if (j) dp[i][j]=max(dp[i][j], dp[i-1][j-1]+lft[i]);
            if (j<min(i-1, N-i+1)) dp[i][j]=max(dp[i][j], dp[i-1][j+1]+rgt[i]);
        }
    }
    cout<<dp[N][0]<<endl;
}

int main()
{
    int T; cin>>T;
    while (T--) driver();
    return 0;
}
