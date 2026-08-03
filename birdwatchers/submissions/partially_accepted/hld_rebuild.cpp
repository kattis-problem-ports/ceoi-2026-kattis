// Heavy-light decomposition + lazy segment tree over the HLD positions, holding the
// influence of every node.  The decomposition is rebuilt only every B = 1000 updates;
// in between, the positions are stale, and the set 'st' records the position of every
// node that has been detached since the last rebuild.  Walking up from a node then
// follows the stale chains but jumps at those cut points to the node's current parent,
// so an ancestor path is traversed as O(1 + c) contiguous stale segments of O(log n)
// chains each, where c is the number of cut points it crosses (c <= B).  The Treasurer
// is the smallest value >= ceil(M/2), found by descending into the segment tree nodes
// that straddle that threshold.
//
// Per update that is O((1 + c) log^2 n), plus a Theta(n) rebuild every B updates:
//     O(n + q*n/B + q*B*log^2 n),
// which for B = sqrt(n)/log n would be O(n + q*sqrt(n)*log n) -- the same sqrt class as
// submissions/accepted/sqrt.cpp, one log worse.
//
// It loses group 4 on time, not on the asymptotics: the rebuild is Theta(n) but with a
// very large constant (a 4n-node/16-byte segment tree rebuilt from scratch, ST::clear
// taking its vector by value, n vector<int> reallocations for g[], two std::function
// recursions), and with B = 1000 those 31 rebuilds are ~88% of the runtime.  Measured
// on the reference machine at a 10 s limit: heaviest group-3 test 2.2 s, but 11.4 s on
// secret/group4/053-g4-15 -- about 15% over.  Raising B to ~5000 gets that to ~8.5 s.
//
// Note also that dfs() and hld() recurse to depth n, which needs more than the usual
// 8 MB of stack at n = 10^6.

#include <bits/stdc++.h>
using namespace std;
using ll = long long;
using pii = pair<int, int>;
#define pb push_back
#define ff first
#define ss second

struct ST{
    struct node{
        int mn, mx, in, ix;
    };
    vector<node> t;
    vector<int> p, m;
    int n, W;
    ST(int ns, int Ws){
        n = ns; W = Ws;
        t.resize(4 * n);
    }
    void build(int v, int tl, int tr){
        if (tl == tr){
            t[v].mn = t[v].mx = m[tl];
            t[v].in = t[v].ix = tl;
            return;
        }
        int tm = (tl + tr) / 2, vv = 2 * v;
        build(vv, tl, tm);
        build(vv + 1, tm + 1, tr);

        if (t[vv].mx > t[vv + 1].mx){
            t[v].mx = t[vv].mx;
            t[v].ix = t[vv].ix;
        }
        else {
            t[v].mx = t[vv + 1].mx;
            t[v].ix = t[vv + 1].ix;
        }

        if (t[vv].mn > t[vv + 1].mn){
            t[v].mn = t[vv + 1].mn;
            t[v].in = t[vv + 1].in;
        }
        else {
            t[v].mn = t[vv].mn;
            t[v].in = t[vv].in;
        }
    }
    void clear(vector<int> M){
        m = M;
        p.assign(4 * n, 0);
        build(1, 1, n);
    }
    void push(int v){
        int vv = 2 * v;
        t[vv].mx += p[v]; t[vv].mn += p[v]; p[vv] += p[v];
        t[vv + 1].mx += p[v]; t[vv + 1].mn += p[v]; p[vv + 1] += p[v];
        p[v] = 0;
    }
    void add(int v, int tl, int tr, int l, int r, int x){
        if (l > tr || r < tl) return;
        if (l <= tl && tr <= r){
            t[v].mx += x; t[v].mn += x; p[v] += x;
            return;
        }
        int tm = (tl + tr) / 2, vv = 2 * v;
        push(v);
        add(vv, tl, tm, l, r, x);
        add(vv + 1, tm + 1, tr, l, r, x);

        if (t[vv].mx > t[vv + 1].mx){
            t[v].mx = t[vv].mx;
            t[v].ix = t[vv].ix;
        }
        else {
            t[v].mx = t[vv + 1].mx;
            t[v].ix = t[vv + 1].ix;
        }

        if (t[vv].mn > t[vv + 1].mn){
            t[v].mn = t[vv + 1].mn;
            t[v].in = t[vv + 1].in;
        }
        else {
            t[v].mn = t[vv].mn;
            t[v].in = t[vv].in;
        }
    }
    void add(int l, int r, int x){
        add(1, 1, n, l, r, x);
    }
    int get(int v, int tl, int tr, int p){
        if (tl == tr){
            return t[v].mx;
        }
        int tm = (tl + tr) / 2, vv = 2 * v;
        push(v);
        if (p <= tm){
            return get(vv, tl, tm, p);
        }
        return get(vv + 1, tm + 1, tr, p);
    }
    int get(int p){
        return get(1, 1, n, p);
    }
    pii find(int v, int tl, int tr){
        if (t[v].mx < W) return {0, 0};
        if (t[v].mn >= W) return {t[v].mn, t[v].in};
        int tm = (tl + tr) / 2, vv = 2 * v;
        push(v);

        auto [k1, p1] = find(vv, tl, tm);
        auto [k2, p2] = find(vv + 1, tm + 1, tr);

        if (!k1) return {k2, p2};
        if (!k2) return {k1, p1};

        if (k1 > k2) return {k2, p2};
        return {k1, p1};
    }
    int find(){
        return find(1, 1, n).ss;
    }
};

int main(){
   ios_base::sync_with_stdio(0);
   cin.tie(0);
   cout.tie(0);

   int n, q; cin>>n>>q;
   vector<int> m(n + 1), p(n + 1);
   int R, M = 0;
   for (int i = 1; i <= n; i++){
       cin>>p[i]>>m[i];
       M += m[i];
       if (!p[i]){
           R = i;
       }
   }

   int W = (M + 1) / 2;

   const int B = 1000;

   vector<int> g[n + 1], nt(n + 1), pos(n + 1), sz(n + 1), f(n + 1);
   function<void(int)> dfs = [&](int v){
       sz[v] = 1; nt[v] = 0; f[v] = m[v];
       for (int i: g[v]){
           dfs(i);
           sz[v] += sz[i];
           f[v] += f[i];
           if (sz[nt[v]] < sz[i]){
               nt[v] = i;
           }
       }
   };

   vector<int> head(n + 1);
   int timer = 0;
   function<void(int, int)> hld = [&](int v, int k){
       head[v] = k;
       pos[v] = ++timer;
       if (!nt[v]) return;
       hld(nt[v], k);
       for (int i: g[v]){
           if (i == p[v] || i == nt[v]) continue;
           hld(i, i);
       }
   };

   ST T(n, W);

   vector<int> inv(n + 1);

   set<int> st;
   auto rebuild = [&](){
       st.clear();
       for (int i = 1; i <= n; i++) g[i].clear();
       for (int i = 1; i <= n; i++) g[p[i]].pb(i);

       timer = 0;
       dfs(R);
       hld(R, R);

       vector<int> F(n + 1);
       for (int i = 1; i <= n; i++){
           F[pos[i]] = f[i];
           inv[pos[i]] = i;
       }

       T.clear(F);
   };

   auto comp = [&](){
       return inv[T.find()];
   };

   auto add = [&](int v, int x){
       // v, p[v], p[p[v]], ... add x to their values
       while (v > 0){
           // cout<<"yo "<<v<<" "<<x<<"\n";
           int l = pos[head[v]], r = pos[v];

           auto it = st.lower_bound(r + 1);
           if (it != st.begin() && *prev(it) >= l){
               it--;
               int k = (*it);
               T.add(k, r, x);
               v = p[inv[k]];
           }
           else {
               T.add(l, r, x);
               v = p[head[v]];
           }
       }
   };

   auto ch = [&](int x, int z){
       int s = T.get(pos[x]);
       add(p[x], -s);
       add(z, s);
       p[x] = z;
       st.insert(pos[x]);
   };

   rebuild();
   int t = comp();
   cout<<t<<"\n";

   mt19937 rng((int) time(0));

   for (int tt = 1; tt <= q; tt++){
       if (tt % B == 0){
           rebuild();
       }

       int x1, z1; cin>>x1>>z1;
       int x = 1 + (t + x1) % n, z = 1 + (t + z1) % n;

       ch(x, z);

       t = comp();

       cout<<t<<"\n";
    }
}
