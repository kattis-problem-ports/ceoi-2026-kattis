#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <vector>
/*
#include <random>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <cstring>
#include <string>
#include <ctime>
#include <functional>
*/

using namespace std;

#include <cassert>
#define Assert assert
//#define Assert(x)

int n = 0; enum { MaxN = 200'000, M = 1'000'000'007, SlowTestsN = 10000 };
vector<int> FT; // Fenwick tree, indices: 0..n.  FT[0] = 0; for u > 0: FT[u] = sum of A[v] for f(u) < v <= u, where f(u) = u & (u - 1) and A[v] == true if node v-1 is currently in the branch.

void FenwickUpdate(int u, int delta) // A[u + 1] += delta
{
    ++u;
    while (u <= n) { FT[u] += delta; u += (u & ~(u - 1)); }
}

int FenwickQuery(int u) // returns the sum of A[1..u]
{
    int result = 0;
    while (u > 0) { result += FT[u]; u &= (u - 1); }
    return result;
}

int main()
{
    //freopen("dfs99.in", "rt", stdin);
    //freopen("dfs-tests\\test.01.04.in", "rt", stdin);
    vector<int> branch; long long result = 1;
    for (int i = 0; i == 0 || i < n; ++i)
    {
        // Read the next d/u pair.
        int d, u, ok_; ok_ = scanf("%d", &d);
        Assert(ok_ == 1); Assert(d >= 0);
        int slash = fgetc(stdin); Assert(slash == '/');
        ok_ = scanf("%d", &u); Assert(ok_ == 1); Assert(u >= 0);
        int eoln = fgetc(stdin); Assert(eoln == '\r' || eoln == '\n');
        if (i > 0) Assert(u < n);
        else {
            // The first line tells us n 1 .  We can now initialize n and the Fenwick tree.
            n = u + 1; Assert(n <= MaxN);
            FT.clear(); FT.resize(MaxN + 1, 0); }
        Assert(d <= (int) branch.size());
        // Remove nodes from the branch until we reach the correct depth.
        while ((int) branch.size() > d) {
            int v = branch.back(); branch.pop_back();
            FenwickUpdate(v, -1); }
        // Let v be an ancestor of u, let w be the parent of v; then an edge (u, w) can be
        // added iff u > v.  Thus if u has, say, k ancestors v for which u > v, this means
        // that each of k possible edges can be added.  Note that when v is the root, there
        // is no parent w of v and hence no (u, w) can be added, but we don't need to worry
        // about this special case because in our task the root is always n-1 and so the 
        // condition u > v will not be met there.
        //   Since all of u's ancestors are currently in 'branch' and also in the Fenwick tree,
        // we simply have to count how many of them are less than u.
        int k = FenwickQuery(u);
        if constexpr (false) if (n <= SlowTestsN) {
            int kk = 0; for (int v : branch) if (u > v) ++kk;
            Assert(kk == k); }
        // Now we have to multiply 'result' by 2^k modulo M.
        int b = 1; long long two_b = 2;
        while (b <= k) {
            if (k & b) result = (result * two_b) % M;
            b <<= 1; two_b = (two_b * two_b) % M; }
        // Now we can add u to the branch and to the Fenwick tree.
        branch.emplace_back(u); 
        FenwickUpdate(u, 1);
    }
    printf("%lld\n", result); return 0;
}