// Birdwatchers (CEOI 2026, day 1) -- the O((n+q) sqrt n) solution of the editorial,
// section 5 ("Subtask 3: sqrt n per update with tree regions").
//
// Maintained state:
//   * the tree as parent + doubly linked child lists,
//   * a partition of the tree into connected regions of height <= B, one aggregate
//     T[R] = influence of the region's topmost node per region, plus the list of the
//     regions attached directly below each region,
//   * localSub[u] = total weight of the nodes of u's own region inside u's subtree,
//   * tin/tout of a build-time DFS, used only to test "is a an descendant of v"
//     for two nodes of the same region (such tests stay valid: regions only ever
//     lose nodes or move rigidly).
//
// One update (move the subtree of x below z):
//   W = inf(x); make x a region root (splitting its region if needed, which costs
//   O(B) because a non-root node of a region carries < B region nodes below it);
//   subtract W from T of every region whose root is an ancestor of the old parent y
//   and add W to T of every region whose root is an ancestor of z (the regions above
//   lca(y,z) get -W and +W, so no lca is needed); then the new Treasurer is the
//   smallest-influence senior node among { old Treasurer, climb(y), climb(z) }.
//
// Every B updates the decomposition is rebuilt from scratch in O(n), which keeps the
// number of regions at O(sqrt n).

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <vector>
#include <algorithm>

using namespace std;

// ---------------------------------------------------------------- fast input
static char ibuf[1 << 16];
static size_t ipos = 0, ilen = 0;
static inline int gc() {
    if (ipos == ilen) { ilen = fread(ibuf, 1, sizeof(ibuf), stdin); ipos = 0; if (!ilen) return -1; }
    return ibuf[ipos++];
}
static inline int readInt() {
    int c = gc();
    while (c != '-' && (c < '0' || c > '9')) c = gc();
    int sgn = 1; if (c == '-') { sgn = -1; c = gc(); }
    int x = 0;
    while (c >= '0' && c <= '9') { x = x * 10 + (c - '0'); c = gc(); }
    return x * sgn;
}

// ---------------------------------------------------------------- the tree
static int n, q;
static long long M;              // total weight
static int root;                 // 0-based id of the President
static vector<int> par, firstChild, nextSib, prevSib, weight;

static inline void linkChild(int c, int p) {      // make c the first child of p
    par[c] = p;
    prevSib[c] = -1;
    nextSib[c] = firstChild[p];
    if (firstChild[p] != -1) prevSib[firstChild[p]] = c;
    firstChild[p] = c;
}
static inline void unlinkChild(int c) {
    int p = par[c];
    if (prevSib[c] != -1) nextSib[prevSib[c]] = nextSib[c]; else firstChild[p] = nextSib[c];
    if (nextSib[c] != -1) prevSib[nextSib[c]] = prevSib[c];
    prevSib[c] = nextSib[c] = -1;
}

// ---------------------------------------------------------------- the regions
static int B;                    // region size threshold ~ sqrt(n)
static int regCount;             // regions are numbered 0 .. regCount-1, none ever dies
static vector<int> region;       // region of each node
static vector<int> rroot;        // topmost node of each region
static vector<long long> T;      // T[R] = influence of rroot[R]
static vector<int> localSub;     // weight of region(u)-nodes inside subtree(u)
static vector<int> tin, tout;    // build-time DFS intervals (intra-region use only)

// the regions hanging directly below a region, as intrusive doubly linked lists:
// parReg[g] is the region containing par[rroot[g]]
static vector<int> parReg, childHead, childNext, childPrev;

static inline void addChildReg(int g, int R) {
    parReg[g] = R;
    childPrev[g] = -1;
    childNext[g] = childHead[R];
    if (childHead[R] != -1) childPrev[childHead[R]] = g;
    childHead[R] = g;
}
static inline void removeChildReg(int g) {
    int R = parReg[g];
    if (childPrev[g] != -1) childNext[childPrev[g]] = childNext[g]; else childHead[R] = childNext[g];
    if (childNext[g] != -1) childPrev[childNext[g]] = childPrev[g];
    childPrev[g] = childNext[g] = -1;
}

// Scratch space for rebuilds.  Everything except the DFS itself is done in "index
// space": position i in the DFS preorder instead of the node id ord[i].  Then the
// parent of i is pidx[i] < i, usually just a few slots away, so the passes below are
// cache friendly -- which matters, since a rebuild touches all n nodes.
static vector<int> ord, pidx, cntI, regI, locI, subWI, posOfRoot;
static vector<char> rootI;
static vector<int> dfsStk;

static void rebuild() {
    B = max(1, (int) sqrt((double) n));

    // DFS preorder, iteratively (the tree can be a path of 10^6 nodes)
    ord.clear();
    dfsStk.clear();
    dfsStk.push_back(root);
    while (!dfsStk.empty()) {
        int u = dfsStk.back(); dfsStk.pop_back();
        tin[u] = (int) ord.size();
        ord.push_back(u);
        for (int c = firstChild[u]; c != -1; c = nextSib[c]) dfsStk.push_back(c);
    }
    pidx[0] = -1;
    for (int i = 1; i < n; ++i) pidx[i] = tin[par[ord[i]]];
    for (int i = 0; i < n; ++i) locI[i] = weight[ord[i]];

    // grow regions from the leaves upwards: a node closes its region as soon as the
    // region reaches B nodes, otherwise it is merged into its parent's region
    for (int i = 0; i < n; ++i) cntI[i] = 1;
    for (int i = n - 1; i >= 1; --i) {
        if (cntI[i] >= B) rootI[i] = 1;
        else { rootI[i] = 0; cntI[pidx[i]] += cntI[i]; }
    }
    rootI[0] = 1;                                    // the President always starts a region

    regCount = 0;
    for (int i = 0; i < n; ++i) {
        if (rootI[i]) { regI[i] = regCount; rroot[regCount] = ord[i]; posOfRoot[regCount] = i; ++regCount; }
        else regI[i] = regI[pidx[i]];
        region[ord[i]] = regI[i];
    }

    // subtree weights, region-local subtree weights (cntI is reused for subtree sizes)
    for (int i = 0; i < n; ++i) { subWI[i] = locI[i]; cntI[i] = 1; }
    for (int i = n - 1; i >= 1; --i) {
        int p = pidx[i];
        subWI[p] += subWI[i];
        cntI[p] += cntI[i];
        if (regI[i] == regI[p]) locI[p] += locI[i];
    }
    // ord is a DFS preorder, so every subtree is a contiguous block of it
    for (int i = 0; i < n; ++i) { int u = ord[i]; localSub[u] = locI[i]; tout[u] = i + cntI[i] - 1; }

    for (int R = 0; R < regCount; ++R) { T[R] = subWI[posOfRoot[R]]; childHead[R] = -1; }
    for (int R = 0; R < regCount; ++R) {
        int a = par[rroot[R]];
        if (a < 0) { parReg[R] = -1; childPrev[R] = childNext[R] = -1; }
        else addChildReg(R, region[a]);
    }
}

// is a a descendant-or-self of v?  Only valid when region[a] == region[v].
static inline bool insideSubtree(int v, int a) {
    return tin[v] <= tin[a] && tout[a] <= tout[v];
}

// influence of v, i.e. the total weight of its subtree: the part inside v's own
// region plus the aggregate of every region hanging directly below it
static long long infOf(int v) {
    int R = region[v];
    if (rroot[R] == v) return T[R];
    long long s = localSub[v];
    for (int R2 = childHead[R]; R2 != -1; R2 = childNext[R2])
        if (insideSubtree(v, par[rroot[R2]])) s += T[R2];
    return s;
}

// the lowest senior node on the path from entry up to the root of its region
static vector<int> pathBuf;
static vector<long long> addBuf;
static int climbInRegion(int entry, int R) {
    pathBuf.clear();
    for (int u = entry; ; u = par[u]) { pathBuf.push_back(u); if (u == rroot[R]) break; }
    int k = (int) pathBuf.size();

    addBuf.assign(k, 0);
    for (int R2 = childHead[R]; R2 != -1; R2 = childNext[R2]) {
        int a = par[rroot[R2]];
        // the path nodes containing a form a suffix; find its lowest element
        int lo = 0, hi = k - 1;
        while (lo < hi) {
            int mid = (lo + hi) >> 1;
            if (insideSubtree(pathBuf[mid], a)) hi = mid; else lo = mid + 1;
        }
        addBuf[lo] += T[R2];
    }

    long long run = 0;
    for (int i = 0; i < k; ++i) {
        run += addBuf[i];
        long long f = (long long) localSub[pathBuf[i]] + run;
        if (2 * f >= M) return pathBuf[i];
    }
    return rroot[R];                       // unreachable: rroot[R] is senior here
}

// the lowest senior ancestor-or-self of v.  The hops from region to region only touch
// the (small) per-region arrays; the node arrays are read once, at the very end.
static int climb(int v) {
    int R = region[v];
    if (2 * T[R] >= M) return climbInRegion(v, R);
    while (true) {
        int prev = R;
        R = parReg[R];                     // the topmost region has T = M, so R stays valid
        if (2 * T[R] >= M) return climbInRegion(par[rroot[prev]], R);
    }
}

// make x the root of its own region; costs O(B) because a non-root node of a region
// has fewer than B region nodes below it
static void splitAt(int x, long long infX) {
    int R = region[x];
    int g = regCount++;
    rroot[g] = x;
    T[g] = infX;

    int S = localSub[x];
    static vector<int> stk;
    stk.clear();
    stk.push_back(x);
    while (!stk.empty()) {
        int u = stk.back(); stk.pop_back();
        region[u] = g;
        for (int c = firstChild[u]; c != -1; c = nextSib[c])
            if (region[c] == R) stk.push_back(c);
    }
    for (int u = par[x]; ; u = par[u]) {   // x's old region loses S weight above x
        localSub[u] -= S;
        if (u == rroot[R]) break;
    }

    // the new region hangs below par[x], which is still in R; and the regions that
    // were attached below the nodes now in g have to be handed over to g
    childHead[g] = -1;
    addChildReg(g, R);
    static vector<int> moved;
    moved.clear();
    for (int g2 = childHead[R]; g2 != -1; g2 = childNext[g2])
        if (g2 != g && region[par[rroot[g2]]] == g) moved.push_back(g2);
    for (size_t i = 0; i < moved.size(); ++i) { removeChildReg(moved[i]); addChildReg(moved[i], g); }
}

int main() {
    n = readInt(); q = readInt();
    par.assign(n, -1); firstChild.assign(n, -1); nextSib.assign(n, -1); prevSib.assign(n, -1);
    weight.assign(n, 0);
    region.assign(n, 0); localSub.assign(n, 0); tin.assign(n, 0); tout.assign(n, 0);
    ord.reserve(n); dfsStk.reserve(n + 1);
    pidx.assign(n, -1); cntI.assign(n, 0); regI.assign(n, 0); locI.assign(n, 0); subWI.assign(n, 0);
    rootI.assign(n, 0); posOfRoot.assign(n + 2, 0);
    // at most n/B + 1 regions after a rebuild, plus one per update until the next one
    rroot.assign(n + 2, -1); T.assign(n + 2, 0);
    parReg.assign(n + 2, -1); childHead.assign(n + 2, -1);
    childNext.assign(n + 2, -1); childPrev.assign(n + 2, -1);

    root = -1;
    M = 0;
    {
        vector<int> s(n);
        for (int i = 0; i < n; ++i) { s[i] = readInt() - 1; weight[i] = readInt(); M += weight[i]; }
        for (int i = 0; i < n; ++i) {
            if (s[i] < 0) root = i;
            else linkChild(i, s[i]);
        }
    }

    rebuild();

    // the initial Treasurer: walk down from the root into the senior child
    int t = root;
    while (true) {
        int nxt = -1;
        for (int c = firstChild[t]; c != -1; c = nextSib[c])
            if (2LL * subWI[tin[c]] >= M) { nxt = c; break; }
        if (nxt < 0) break;
        t = nxt;
    }
    printf("%d\n", t + 1);

    int sinceRebuild = 0;
    for (int j = 0; j < q; ++j) {
        int xh = readInt(), zh = readInt();
        int x = (int) (((long long) t + 1 + xh) % n);
        int z = (int) (((long long) t + 1 + zh) % n);

        if (par[x] != z) {
            int y = par[x];
            long long W = infOf(x);
            if (rroot[region[x]] != x) splitAt(x, W);

            // every region whose root is an ancestor of y loses W, every region whose
            // root is an ancestor of z gains W; above lca(y,z) the two cancel out
            for (int R = region[y]; R != -1; R = parReg[R]) T[R] -= W;
            unlinkChild(x);
            linkChild(x, z);
            removeChildReg(region[x]);                 // x's region now hangs below z
            addChildReg(region[x], region[z]);
            for (int R = region[z]; R != -1; R = parReg[R]) T[R] += W;

            int cand[3] = { t, climb(y), climb(z) };
            int best = -1; long long bestInf = 0;
            for (int i = 0; i < 3; ++i) {
                long long f = infOf(cand[i]);
                if (2 * f < M) continue;                       // not senior any more
                if (best < 0 || f < bestInf) { best = cand[i]; bestInf = f; }
            }
            t = best;

            if (++sinceRebuild >= B) { rebuild(); sinceRebuild = 0; }
        }
        printf("%d\n", t + 1);
    }
    return 0;
}
