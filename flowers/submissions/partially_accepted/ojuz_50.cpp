// Downloaded from https://oj.uz/submission/1418372 (scored 50/100 on oj.uz).
// Scores 50 on this package.
// @EXPECTED_GRADES@ AC TLE AC AC TLE
#include <bits/stdc++.h>
#define int long long
#define ppi pair <pair <int, int>, int>
#define pppi pair <int, pair <pair <int, int>, int> >
#define mp make_pair

using namespace std;

const int INF = 1e15;

vector <vector <pair <int, int> > > adj;
vector <vector <bool> > isconec;
map <pair <int, int> , int> pos;
vector <int> par, sz;
map <int, int> ton;

int find (int v) {
    if (v==par[v]) return v;
    return par[v] = find(par[v]);
}

void unite (int a, int b) {
    int p1 = find(a), p2 = find(b);
    if (p1==p2) return;
    if (sz[p1]<sz[p2]) {
        swap(p1, p2);
    }
    sz[p1]+=sz[p2];
    par[p2]=p1;
}

void solve () {
    int n, m;
    cin >> n >> m;
    adj = vector <vector <pair <int, int> > > (n);
    isconec= vector <vector <bool> > (n, vector <bool> (n));
    vector <pair <int, int> > edges;
    for (int i = 0; i< m; i++) {
        int x, y;
        cin >> x >> y;
        x--; y--;
        isconec[x][y]=true;
        isconec[y][x]=true;
        pos[mp(x,y)]=i;
        pos[mp(y,x)]=i;
        adj[x].push_back(mp(y, i));
        adj[y].push_back(mp(x, i));
        edges.push_back(mp(x, y));
    }
    par = vector <int> (m);
    sz = vector <int> (m);
    for (int i = 0; i< m; i++) {
        par[i]=i;
        sz[i]=1;
    }
    for (auto u : edges) {
        int a = u.first, b = u.second;
        for (auto g: adj[a]) {
            int v = g.first;
            if (v== b) continue;
            if (find(pos[mp(a,b)])==find(pos[mp(v,a)])) continue;
            if (isconec[v][b]) {
                unite(pos[mp(v,b)], pos[mp(v,a)]);
                unite(pos[mp(v,b)], pos[mp(a,b)]);
                //cout << "uniting the following three edges " << v << " to " << b << ", " << v << " to " << a << ", " << a << " to " << b << endl;
            }
        }
    }
    map <int, int> num;
    for (int i = 0; i< m; i++) {
        num[find(i)]++;
    }
    int ans = 0;
    for (auto u : num) {
        int x = u.second;
        //cout << "we have " << u.first << " " << u.second << endl;
        //cout << ton[u.second] << endl;
        if (ton[x]>=4) {
            ans+=(3*ton[x]-4+1)/2;
        } else {
            if (ton[x]==2) {
                ans+=1;
            } else {
                ans+=ton[x];
            }
        }
    }
    cout << m-ans << endl;
}

signed main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    int t=1;
    //cin >> t;
    int cur = 0;
    for (int i = 1; i <= 1000; i++) {
        cur+=i;
        ton[cur]=i+1;
    }
    while (t--) {
        solve();
    }
}
