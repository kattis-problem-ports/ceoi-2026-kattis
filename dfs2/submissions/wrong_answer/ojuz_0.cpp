// Downloaded from https://oj.uz/submission/1415420 (scored 0/100 on oj.uz).
// Scores 0 on this package.
#include <bits/stdc++.h>

using namespace std;

#define ll long long
const ll mmod = 1e9+7;  
#define vl vector<long long>
#define vll vector<vector<long long>>
#define pl pair<long long, long long>
#define vb vector<bool>

vl tree;
vl maxima;

ll pow(ll x){
    if (x == 0){
        return 1;
    }
    if (x % 2 == 1){
        ll y = pow(x/2)%mmod;
        return (2*y)%mmod;
    }
    return pow(x/2)%mmod;
}

void update(ll l, ll r, ll p, ll c, ll i){
    if (l == p && r == p){
        tree[i] += c;
        return;
    }

    if (r < p || l > p){
        return;
    }

    ll mid = (l+r)/2;
    update(l, mid, p, c, 2*i);
    update(mid+1, r, p, c, 2*i+1);
    tree[i] = tree[2*i] + tree[2*i+1];
    return;
}

ll query(ll l, ll r, ll L, ll R, ll i){
    if (L <= l && r <= R){
        return tree[i];
    }
    if (r < L || R < l){
        return 0;
    }
    ll mid = (l+r)/2;
    ll a = query(l, mid, L, R, 2*i);
    ll b = query(mid+1, r, L, R, 2*i+1);
    return a + b;
}


int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ll n = 2e5+5;
    ll depth = 0;
    ll suma = 0;
    ll k = 1;
    while (k <= n+4){
        k *= 2;
    }
    tree.resize(2*k+4, 0);
    maxima.resize(n);

    vl z;

    char c;
    ll d, v;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }

        std::stringstream ss(line);
        ll d, v;
        char c;

        if (ss >> d >> c >> v && c == '/') {
            while (d < z.size()) {
                update(0, k - 1, z.back(), -1, 1);
                z.pop_back();
            }
            if (d != 0){
                if (v < maxima[z.size()-1]){
                    cout << "0\n";
                    return 0;
                }
                maxima[z.back()] = v;
            }
            z.push_back(v);
            suma += query(0, v, 0, k - 1, 1);
            if (d != 0) {
                update(0, k - 1, v, 1, 1);
            }
        }
    }

    cout << max(pow(suma),1ll) << "\n";

    


    return 0;
}
