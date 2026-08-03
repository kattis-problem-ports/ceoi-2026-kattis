// Overflow-safe model solution, written independently for the Kattis port
// (Claude, 2026-08-03). The official reference solutions accumulate the answer
// in int64 and wrap mod 2^64 on three group-5 cases whose true optimum
// exceeds LLONG_MAX; the official .out files contain those wrapped values.
// This solution is the generator's reference and defines the corrected .ans.
//
// Derivation:
//   A cable pairing computers l < r that extends its reach to [x, y] (x <= l,
//   y >= r, endpoints at towers) has minimal length (r-l) + 2(l-x) + 2(y-r)
//   and can visit every tower in [x, y].  Since f >= 0 it visits all of them.
//   Towers strictly inside (l, r) are free; extensions are independent per side.
//
//   With W(c) = f * (#towers < c) - c, the base score of a pair is
//   W(r) - W(l), so   score(pair) = (W(r) + GR(r)) - (W(l) - GL(l))
//   where GL(c) = max(0, max over towers t < c of f*(#towers in [t,c)) - 2(c-t))
//   and GR symmetric.  Every computer is either a left or a right endpoint of
//   its pair; a left/right labeling (sorted by position) is realizable iff it
//   is a balanced bracket sequence (L = '(' , R = ')').
//
//   So: maximize sum(vL over L) + sum(vR over R),  vL = GL - W, vR = GR + W,
//   subject to: exactly n/2 R's, and among the first k computers at most
//   floor(k/2) R's.  Prefix bounds on a nested family + cardinality cap form a
//   matroid; the max-weight basis greedy (weights sorted descending, add if
//   independent) is exact.  Independence is checked with a min-segment-tree
//   over slack(k) = floor(k/2) - (#selected among first k), range add.
//
// Overflow safety:
//   Per-element values: |f*cnt| <= 1e9 * 2e5 = 2e14, coords <= 1e9, so every
//   per-element quantity is < 1e15 and fits comfortably in int64.
//   Sums over up to 2e5 elements can reach ~2e20 and the true answer can
//   theoretically exceed int64 (n/2 * f * m up to 2e19), so ALL accumulation
//   is done in __int128 and printed with a custom writer.
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef __int128 lll;

static void print_i128(lll v) {
    if (v < 0) { putchar('-'); v = -v; }
    char buf[48]; int p = 0;
    do { buf[p++] = char('0' + int(v % 10)); v /= 10; } while (v > 0);
    while (p > 0) putchar(buf[--p]);
    putchar('\n');
}

struct SegTree {
    int n;
    vector<ll> mn, lz;
    void init(int n_) { n = n_; mn.assign(2 * n, 0); lz.assign(2 * n, 0); build(1, 1, n); }
    void build(int node, int lo, int hi) {
        lz[node] = 0;
        if (lo == hi) { mn[node] = lo / 2; return; }
        int mid = (lo + hi) / 2, l = node + 1, r = node + 2 * (mid - lo + 1);
        build(l, lo, mid); build(r, mid + 1, hi);
        mn[node] = min(mn[l], mn[r]);
    }
    void add(int node, int lo, int hi, int a, int b, ll v) {
        if (b < lo || hi < a) return;
        if (a <= lo && hi <= b) { mn[node] += v; lz[node] += v; return; }
        int mid = (lo + hi) / 2, l = node + 1, r = node + 2 * (mid - lo + 1);
        add(l, lo, mid, a, b, v); add(r, mid + 1, hi, a, b, v);
        mn[node] = min(mn[l], mn[r]) + lz[node];
    }
    ll query(int node, int lo, int hi, int a, int b) {
        if (b < lo || hi < a) return LLONG_MAX;
        if (a <= lo && hi <= b) return mn[node];
        int mid = (lo + hi) / 2, l = node + 1, r = node + 2 * (mid - lo + 1);
        return min(query(l, lo, mid, a, b), query(r, mid + 1, hi, a, b)) + lz[node];
    }
};

int main() {
    int T;
    if (scanf("%d", &T) != 1) return 1;
    long long sumN = 0, sumM = 0;
    while (T--) {
        ll n, m, f;
        if (scanf("%lld %lld %lld", &n, &m, &f) != 3) return 1;
        assert(n >= 2 && n % 2 == 0 && m >= 1 && f >= 0 && f <= 1000000000);
        sumN += n; sumM += m;
        assert(sumN <= 200000 && sumM <= 200000);
        vector<ll> a(n), b(m);
        for (auto &x : a) { if (scanf("%lld", &x) != 1) return 1; assert(1 <= x && x <= 1000000000); }
        for (auto &x : b) { if (scanf("%lld", &x) != 1) return 1; assert(1 <= x && x <= 1000000000); }
        sort(a.begin(), a.end());
        sort(b.begin(), b.end());

        // prefmaxP[j] = max over towers 0..j-1 of (2*b[t] - f*t)   ("stop at tower t going left")
        // sufmaxQ[j] = max over towers j..m-1 of (f*(t+1) - 2*b[t]) ("stop at tower t going right")
        vector<ll> prefmaxP(m + 1, LLONG_MIN), sufmaxQ(m + 1, LLONG_MIN);
        for (int j = 0; j < m; j++)
            prefmaxP[j + 1] = max(prefmaxP[j], 2 * b[j] - f * (ll)j);
        for (int j = m - 1; j >= 0; j--)
            sufmaxQ[j] = max(sufmaxQ[j + 1], f * (ll)(j + 1) - 2 * b[j]);

        vector<ll> vL(n), vR(n), d(n);
        lll base = 0;
        for (int i = 0; i < n; i++) {
            ll cnt = lower_bound(b.begin(), b.end(), a[i]) - b.begin();
            ll W = f * cnt - a[i];
            ll GL = 0, GR = 0;
            if (cnt > 0) GL = max(GL, f * cnt - 2 * a[i] + prefmaxP[cnt]);
            if (cnt < m) GR = max(GR, 2 * a[i] - f * cnt + sufmaxQ[cnt]);
            vL[i] = GL - W;
            vR[i] = GR + W;
            d[i] = vR[i] - vL[i];
            base += vL[i];
        }

        // Matroid greedy: pick exactly n/2 indices maximizing sum d, subject to
        // at most floor(k/2) picked among the first k (1-based positions).
        vector<int> ord(n);
        iota(ord.begin(), ord.end(), 0);
        sort(ord.begin(), ord.end(), [&](int x, int y) { return d[x] > d[y]; });
        SegTree st;
        st.init((int)n);
        lll picked = 0;
        ll cntPicked = 0;
        for (int idx : ord) {
            if (cntPicked == n / 2) break;
            int pos = idx + 1;
            if (st.query(1, 1, (int)n, pos, (int)n) >= 1) {
                st.add(1, 1, (int)n, pos, (int)n, -1);
                picked += d[idx];
                cntPicked++;
            }
        }
        assert(cntPicked == n / 2);
        print_i128(base + picked);
    }
    return 0;
}
