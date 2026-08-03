// Downloaded from https://oj.uz/submission/1418470 (scored 5/100 on oj.uz).
// Scores 5 on this package.
// Also WA on the samples (oj.uz does not score the sample group).
// @EXPECTED_GRADES@ AC WA WA WA WA
#include <bits/stdc++.h>
using namespace std;

#define ll long long
#define vl vector<long long>
#define vll vector<vector<long long>>
#define vb vector<bool>
#define pl pair<long long, long long>



int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ll t;
    cin >> t;
    while (t--){

        ll n, m, f;
        cin >> n >> m >> f;

        vl comps(n), towers(m);
        for (ll i = 0; i < n; i++){
            cin >> comps[i];
        }
        for (ll i = 0; i < m; i++){
            cin >> towers[i];
        }
        m += 2;
        towers.push_back(-1e18);
        towers.push_back(1e18);

        sort(comps.begin(), comps.end()); sort(towers.begin(), towers.end());

        ll s = 0;

        for (ll i = 0; i < n; i+=2){
            ll a, b;
            a = comps[i];
            b = comps[i+1];
            ll c1 = 0;
            ll c2 = 0;
            ll o1 = 0;
            ll o2 = 0;

            for (ll j = m-1; j > 0; j--){
                if (towers[j] < a){
                    c1 += f + 2*(towers[j] - min(towers[j+1], a));
                    o1 = max(o1, c1);
                }
            }

            for (ll j = 1; j < m-1; j++){
                if (towers[j] > b){
                    c2 += f - 2*(towers[j] - max(towers[j-1], b));
                    o2 = max(o2, c2);
                }
            }


            for (ll j = 1; j < m-1; j++){
                if (towers[j] >= a && towers[j] <= b){
                    s += f;
                }
            }
            s -= (b-a);
            s += o1 + o2;
        }

        cout << s << "\n";
    }

    return 0;
}