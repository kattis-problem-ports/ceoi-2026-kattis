// Downloaded from https://oj.uz/submission/1418616 (scored 85/100 on oj.uz).
// Scores 34 on this package, NOT 85 -- this package's test data differs from oj.uz's.
// The regenerated group 4/5 data in this package is stronger than oj.uz's
// and exposes this solution as wrong rather than merely incomplete.
// @EXPECTED_GRADES@ AC AC WA WA WA
#include <bits/stdc++.h>
using namespace std;

#define ll long long
#define vl vector<long long>
#define vll vector<vector<long long>>
#define vb vector<bool>
#define pl pair<long long, long long>

vll graf;

ll full(ll n){
    if (n < 4){
        return 0;
    }
    ll y = n*(n-1)/2 - 6 - (n-5)/2*3;
    if (n%2 == 0){
        y = n*(n-1)/2 - 4 - (n-4)/2*3;
    }
    return y;
}


int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    ll n, m;
    cin >> n >> m;

    graf.resize(n, vl (n, 0));

    for (ll i = 0; i < m; i++){
        ll a, b;
        cin >> a >> b;
        a--; b--;
        graf[a][b]++;
        graf[b][a]++;
    }
    ll s = 0;
    set<ll> vis;
    for (ll i = 0; i < n; i++){
        for (ll j = i+1; j < n; j++){
            if (graf[i][j] == 0){
                break;
            }
            ll last = -1;
            ll c = 0;
            for (ll k = 0; k < n; k++){
                if (graf[i][k] == graf[j][k] && graf[i][k] == 1){
                    if (last == -1){
                        last = k;
                        c = 3;
                        continue;
                    }
                    c++;
                    vis.insert(k);
                    vis.insert(last);
                    vis.insert(i);
                    vis.insert(j);

                    
                }
            }
            s += full(c);
            for (auto x : vis){
                for (auto y : vis){
                    graf[x][y] = 0;
                    graf[y][x] = 0;
                }
            }
        }
    }

    cout << s << "\n";
    
    return 0;
}