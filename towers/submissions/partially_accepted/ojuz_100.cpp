// Downloaded from https://oj.uz/submission/1419397 (scored 100/100 on oj.uz).
// Scores 63 on this package: it accumulates the answer in int64, which wraps
// mod 2^64 on three group-5 cases whose true optimum exceeds LLONG_MAX. The
// official answer files (and oj.uz) contain the same wrapped values, which is
// why it "passed" there; this package ships corrected answers.
// @EXPECTED_GRADES@ AC AC AC AC WA
#include<bits/stdc++.h>
using namespace std;

void solve() {
    long long n,m,f,a;
    cin >> n >> m >> f;
    vector<pair<long long,long long>> haha(0);
    for(long long i = 0; i < n; i++) {
        cin >> a;
        haha.push_back({a,1});
    }
    for(long long i = 0; i < m; i++) {
        cin >> a;
        haha.push_back({a,0});
    }
    sort(haha.begin(),haha.end());
    long long big = -1000000000000000000LL,br1 = 0,br0 = 0;
    vector<long long> wow(n);
    vector<long long> wut(n);
    for(long long i = 0; i < haha.size(); i++) {
        long long x = haha[i].first;
        if(haha[i].second) {
            wow[br1]+=max(0LL,big-2*x+f*br0);
            wow[br1]+=x-f*br0;
            wut[br1]+=f*br0-x;
            br1++;
        }
        else {
            big = max(big,2*x-f*br0);
            br0++;
        }
    }
    big = -1000000000000000000LL;
    br1 = n;
    br0 = m;
    for(long long i = haha.size()-1; i >= 0; i--) {
        long long x = haha[i].first;
        if(haha[i].second) {
            br1--;
            wut[br1]+=max(0LL,big+2*x-f*br0);
        }
        else {
            big = max(big,f*br0-2*x);
            br0--;
        }
    }
    long long ans = 0;
    priority_queue<long long> idk;
    for(long long i = 0; i < n; i++) {
        ans+=wow[i];
    }
    for(long long i = 0; i < n; i++) {
        idk.push(wow[i]-wut[i]);
        if(idk.size() > (i+1)/2) {
            idk.pop();
        }
    }
    while(!idk.empty()) {
        ans-=idk.top();
        idk.pop();
    }
    cout << ans << "\n";
}

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    int t;
    cin >> t;
    while(t--) {
        solve();
    }
    return 0;
}