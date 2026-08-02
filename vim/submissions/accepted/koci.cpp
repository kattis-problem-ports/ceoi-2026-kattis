#include <iostream>
#include <iomanip>
#include <climits>
#include <stack>
#include <fstream>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>
#include <vector>
#include <deque>
#include <queue>
#include <set>
#include <map>
#include <cassert>
#include <stdexcept>

#define FOR(i,n) for(int i=0,_n=n;i<_n;i++)
#define FORR(i,s,n) for(int i=s,_n=n;i<_n;i++)
#define mp make_pair
#define pb push_back
#define pii pair<int,int>
#define pli pair<ll,int>
#define vi vector<int>
#define fs first
#define sec second

#define maxn 600
#define maxs 50

using namespace std;
typedef long long ll;

const ll MOD = 1000000007LL;

int memo[maxn+1][maxs+1][maxs+1]; // len, kurzor, clip
char command[maxn+1][maxs+1][maxs+1];
int previous[maxn+1][maxs+1][maxs+1][3];

void precompute(){
    memset(memo,-1,sizeof(memo));
    memo[1][1][0]=0;
    queue<pair<int,pii>> q;
    q.push(mp(1,mp(1,0)));
    while(!q.empty()){
        int len, kurzor, clip;
        len=q.front().fs;
        kurzor=q.front().sec.fs;
        clip=q.front().sec.sec;
        q.pop();
        int moves = memo[len][kurzor][clip];
        if(clip>0 && len+clip<=maxn && kurzor<maxs && memo[len+clip][kurzor+1][clip]==-1){
            memo[len+clip][kurzor+1][clip] = moves+1;
            command[len+clip][kurzor+1][clip]='P';
            previous[len+clip][kurzor+1][clip][0]=len;
            previous[len+clip][kurzor+1][clip][1]=kurzor;
            previous[len+clip][kurzor+1][clip][2]=clip;
            q.push(mp(len+clip,pii(kurzor+1,clip)));
        }
        if(memo[len][kurzor][kurzor]==-1){
            memo[len][kurzor][kurzor] = moves+1;
            command[len][kurzor][kurzor]='Y';
            previous[len][kurzor][kurzor][0]=len;
            previous[len][kurzor][kurzor][1]=kurzor;
            previous[len][kurzor][kurzor][2]=clip;
            q.push(mp(len,pii(kurzor,kurzor)));
        }
        if(kurzor<min(len,maxs) && memo[len][kurzor+1][clip]==-1){
            memo[len][kurzor+1][clip] = moves+1;
            command[len][kurzor+1][clip]='h';
            previous[len][kurzor+1][clip][0]=len;
            previous[len][kurzor+1][clip][1]=kurzor;
            previous[len][kurzor+1][clip][2]=clip;
            q.push(mp(len,pii(kurzor+1,clip)));
        }
        if(kurzor>1 && memo[len][kurzor-1][clip]==-1){
            memo[len][kurzor-1][clip] = moves+1;
            command[len][kurzor-1][clip]='l';
            previous[len][kurzor-1][clip][0]=len;
            previous[len][kurzor-1][clip][1]=kurzor;
            previous[len][kurzor-1][clip][2]=clip;
            q.push(mp(len,pii(kurzor-1,clip)));
        }
    }
}


ll len_from_buckets(int s, int k){
    ll avg_bucket= (s-k)/k;
    ll overflow = (s-k)%k;
    ll sq_sum = (k-overflow)*avg_bucket*avg_bucket+overflow*(avg_bucket+1)*(avg_bucket+1);
    return ((s-k)*(s-k)-sq_sum)/2+s-k+1;
}

pii find_n_buckets(int s){
    int max_len=0;
    int best_k=0;
    // This for loop could prob be a binary search, but nah
    FORR(k,1,s/2){
        ll total = len_from_buckets(s,k);
        if (total>max_len){
            max_len=total;
            best_k=k;
        }
    }
    return mp(max_len, best_k);
}

void buckets_check(int n, int s, vi buckets){
    int sum_l=0;
    FOR(i,buckets.size())sum_l+=buckets[i];
    if(sum_l+buckets.size()!=s){
        printf("Bucket len is off n %d s %d buckets",n,s);
        FOR(i,buckets.size())printf(" %d",buckets[i]);
        printf("\n");
    }
    int seq_l = 1;
    int buff=1;
    FOR(i, buckets.size()){
        seq_l+=buff*buckets[i];
        buff+=buckets[i];
    }
    if(seq_l!=n){
        printf("Sequence len is off n %d s %d seq_l %d buckets",n,s, seq_l);
        FOR(i,buckets.size())printf(" %d",buckets[i]);
        printf("\n");
    }
}


int solve_greedy(int n){
    int l=10,d=(int)sqrt(n)*2; // Should be < sqrt(n) but just in case
    while(l<d){
        int s=(l+d)/2;
        pii n_buckets = find_n_buckets(s);
        if(n_buckets.fs>=n)d=s;
        else l=s+1;
    }
    pii n_buckets = find_n_buckets(l);
    int total = n_buckets.fs;
    int k = n_buckets.sec;
    // Decrease through more buckets
    while(len_from_buckets(l,k+1)>=n)k++;
    total = len_from_buckets(l,k);
    vi buckets;
    ll avg_bucket= (l-k)/k;
    ll overflow = (l-k)%k;
    FOR(i,k-overflow)buckets.pb(avg_bucket);
    FOR(i,overflow)buckets.pb(avg_bucket+1);
    int sink=k-1;
    while(total>n){
        //printf("Total %d sink %d Buckets ",total, sink);
        //FOR(i,k)printf(" %d",buckets[i]);printf("\n");
        if (sink==0){
            printf("Got stuck\n");
            return -1;
        }
        int largest=0;
        FOR(i,sink)
            if(buckets[i]>buckets[largest])largest=i;
        int change = buckets[sink]-buckets[largest] + 1;
        if(total-change<n)sink--;
        else{
            total-=change;
            buckets[largest]--;
            buckets[sink]++;
        }
    }
    buckets_check(n,l,buckets);
    printf("%d ",l);
    FOR(i, k){
        printf("Y");
        FOR(j,buckets[i])printf("P");
    }
    printf("\n");
    return l;
}

void solve(){
    int n;
    scanf("%d", &n);
    int greedy_ans=-1;
    if(n>600){
        greedy_ans=solve_greedy(n);
        return;
    }
    //if(n>maxn)return;
    int best_kurzor=0, best_clip=0;
    int ans=n+10;
    FOR(i,maxs)FOR(j,maxs){
        if (memo[n][i][j]==-1)continue;
        if (memo[n][i][j]<ans){
            ans = memo[n][i][j];
            best_kurzor=i;
            best_clip=j;
        }
    }
    //if (n>300){
    //    if (ans==greedy_ans)printf("Greedy ok\n");
    //    else printf("ALERT n=%d, greedy_ans=%d, ans=%d\n",n,greedy_ans,ans);
    //    return;
    //}
    vector<char> commands;
    printf("%d ",memo[n][best_kurzor][best_clip]);
    int len=n, kurzor=best_kurzor, clip=best_clip;
    while(memo[len][kurzor][clip]!=0){
        commands.pb(command[len][kurzor][clip]);
        int new_len = previous[len][kurzor][clip][0];
        int new_kurzor = previous[len][kurzor][clip][1];
        int new_clip = previous[len][kurzor][clip][2];
        len=new_len;
        kurzor=new_kurzor;
        clip=new_clip;
    }
    for(int i=commands.size()-1;i>=0;i--)printf("%c",commands[i]);
    printf("\n");
}

int main(){
    precompute();
    int t;
    scanf("%d", &t);
    while(t--){
        solve();
    }
	return 0;
}
