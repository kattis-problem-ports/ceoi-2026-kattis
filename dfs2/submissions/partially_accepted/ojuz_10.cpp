// Downloaded from https://oj.uz/submission/1419856 (scored 10/100 on oj.uz).
// Scores 10 on this package.
// @EXPECTED_GRADES@ WA WA WA AC WA WA
#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define ld long double
#define vi vector<ll>
#define vvi vector<vector<ll>>
#define vs vector<string>
#define vc vector<char>
#define vb vector<bool>
#define vp vector<pair<ll, ll>>
#define vpp vector<pair<ll, pair<ll, ll>>>
#define pp pair<ll, ll>
#define qi queue<ll>
#define qp queue<pp>
#define pqi priority_queue<ll>
#define pqp priority_queue<pp>
#define mi map<ll, ll>
#define mpi map<pp, ll>
#define mip map<ll, pp>
#define mp map<pp, pp>
#define mb map<ll, bool>
#define si set<ll>
#define sp set<pp>
#define sc set<char>
#define mod 1000000007
#define inf 1000000000000000000
#define all(x) (x).begin(), (x).end()
#define rall(x) (x).rbegin(), (x).rend()

ll binpow(ll a, ll b) {
    ll ans = 1;
    while(b > 0) {
        if(b & 1) {
            ans = (ans * a) % mod;
            b--;
        }
        else {
            a = (a * a) % mod;
            b /= 2;
        }
    }
    return ans;
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    ll ans = 1;
    string line;
    while(getline(cin, line)) {
        istringstream ss(line);
        string num;
        ll d;
        while(getline(ss, num, '/')) {
            d = stoi(num);
            break;
        }
        ll pw = binpow(2, d - 1);
        ans = (ans * pw) % mod;
    }
    cout << ans << '\n';
    return 0;
}