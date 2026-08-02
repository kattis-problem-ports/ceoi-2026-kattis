#include<iostream>
#include<algorithm>
#include<vector>
#include<cassert>
#include<set>
using namespace std;
typedef long long ll;

vector<ll>get_lc(vector<int>computers,vector<int>towers,int K){
    int n=(int)computers.size();
    reverse(towers.begin(),towers.end());
    vector<ll>lc(n);
    for(int i=0;i<n;i++){
        vector<int>tw;
        while(towers.size()&&towers.back()<computers[i]){
            tw.push_back(towers.back());
            towers.pop_back();
        }
        for(int j=0;j<(int)tw.size();j++){
            lc[i]=max(lc[i],-2LL*(computers[i]-tw[j])+1LL*K*((int)tw.size()-j));
        }
        if(i)
            lc[i]=max(lc[i],lc[i-1]-2LL*(computers[i]-computers[i-1])+1LL*K*int(tw.size()));
    }
    return lc;
}

void solve(){
    int n,m,K;
    cin>>n>>m>>K;
    assert(n%2==0);
    assert(n<=200000);
    assert(m<=200000);
    vector<int>computers(n),towers(m);
    for(int&i:computers)
        cin>>i;
    for(int&i:towers)
        cin>>i;
    sort(computers.begin(),computers.end());
    sort(towers.begin(),towers.end());
    vector<ll>lc=get_lc(computers,towers,K);
    for(int&i:computers)
        i=1e9+1-i;
    reverse(computers.begin(),computers.end());
    for(int&i:towers)
        i=1e9+1-i;
    reverse(towers.begin(),towers.end());
    vector<ll>rc=get_lc(computers,towers,K);
    for(int&i:computers)
        i=1e9+1-i;
    reverse(computers.begin(),computers.end());
    for(int&i:towers)
        i=1e9+1-i;
    reverse(towers.begin(),towers.end());
    reverse(rc.begin(),rc.end());

    ll res=0;
    vector<ll>arr;
    reverse(towers.begin(),towers.end());
    int last=max(towers[0],computers.back());
    for(int i=0;i<n;i++){
        while(towers.size()&&towers.back()<computers[i])
            towers.pop_back();
        ll cr=lc[i]-(last-computers[i])+1LL*K*int(towers.size());
        res+=cr;
        arr.push_back(rc[i]+(last-computers[i])-1LL*K*int(towers.size())-cr);
    }
    multiset<ll>s;
    for(int i=0;i<n;i++){
        int max_num=(i+1)/2;
        s.insert(arr[i]);
        if(s.size()>max_num)
            s.erase(s.begin());
    }
    for(ll i:s)
        res+=i;
    cout<<res<<"\n";
}

int main(){
    ios::sync_with_stdio(false);cout.tie(0);cin.tie(0);
    int t;
    cin>>t;
    while(t--)
        solve();
}