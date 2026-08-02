#define _CRT_SECURE_NO_WARNINGS
#include <random>
#include <cstdio>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <cstring>
#include <string>
#include <cassert>
#include <ctime>
#include <functional>

using namespace std;
#define Assert assert
//#define Assert(x)

enum { MaxTests = 10000, MaxN = 200'000, MaxM = 200'000, MaxK = 1'000'000'000, MaxCoord = 1'000'000'000 };
enum { SlowChecks = false };

int main()
{
    //freopen("towers-tests\\test.01.001.in", "rt", stdin);
    //freopen("towers-tests\\test.02.012.in", "rt", stdin);
    //freopen("towers-tests\\towers99.in", "rt", stdin);
    typedef long long int llint;
    int totalComps = 0, totalTowers = 0, nTests;
    int ok_ = scanf("%d", &nTests); Assert(ok_ == 1);
    Assert(1 <= nTests); Assert(nTests <= MaxTests);
    for (int testNo = 0; testNo < nTests; ++testNo)
    {
        int nComps, nTowers, K; ok_ = scanf("%d %d %d", &nComps, &nTowers, &K); Assert(ok_ == 3);
        Assert(nComps >= 1); Assert(nTowers >= 1);
        Assert(nComps <= MaxN); Assert(nTowers <= MaxM);
        totalComps += nComps; totalTowers += nTowers;
        Assert(totalComps <= MaxN); Assert(totalTowers <= MaxM);
        Assert(K >= 0); Assert(K <= MaxK);
        vector<int> xComp(nComps), xTower(nTowers);
        for (auto &ai : xComp) { ok_ = scanf("%d", &ai); Assert(ok_ == 1); Assert(1 <= ai); Assert(ai <= MaxCoord); }
        for (auto &bi : xTower) { ok_ = scanf("%d", &bi); Assert(ok_ == 1); Assert(1 <= bi); Assert(bi <= MaxCoord); }
        sort(xComp.begin(), xComp.end());
        sort(xTower.begin(), xTower.end());
        for (int i = 1; i < nComps; ++i) Assert(xComp[i - 1] < xComp[i]);
        for (int i = 1; i < nTowers; ++i) Assert(xTower[i - 1] < xTower[i]);
        vector<llint> f1(nComps), g2(nComps), f3(nComps), h(nComps);
        // Calculate f1 and g2.
        for (int i = 0, s = 0; i < nComps; ++i)
        {
            int ai = xComp[i];
            int s2 = s; while (s2 < nTowers && xTower[s2] < ai) ++s2;
            if (s2 < nTowers) Assert(xTower[s2] > ai); // otherwise a computer and a tower are on the same position
            llint fi = 0;
            if (i > 0) fi = max(fi, f1[i - 1] + llint(K) * (s2 - s) - 2 * (ai - xComp[i - 1]));
            while (s < s2) { fi = max(fi, llint(K) * (s2 - s) - 2 * (ai - xTower[s])); ++s; }
            f1[i] = fi;
            g2[i] = llint(K) * s - ai;
            if (SlowChecks)
            {
                llint fi2 = 0; int nVisited = 0;
                for (int s3 = nTowers - 1; s3 >= 0; --s3)
                {
                    int bs = xTower[s3]; Assert(bs != ai);
                    if (bs > ai) continue;
                    ++nVisited; 
                    llint cand = llint(K) * nVisited - 2 * (ai - bs);
                    if (cand > fi2) fi2 = cand;
                }
                Assert(nVisited == s);
                Assert(fi2 == fi);
            }
        }
        // Calculate f3.
        for (int i = nComps - 1, s = nTowers - 1; i >= 0; --i)
        {
            int ai = xComp[i];
            int s2 = s; while (s2 >= 0 && xTower[s2] > ai) --s2;
            if (s2 >= 0) Assert(xTower[s2] < ai); // otherwise a computer and a tower are on the same position
            llint fi = 0;
            if (i < nComps - 1) fi = max(fi, f3[i + 1] + llint(K) * (s - s2) - 2 * (xComp[i + 1] - ai));
            while (s > s2) { fi = max(fi, llint(K) * (s - s2) - 2 * (xTower[s] - ai)); --s; }
            f3[i] = fi;
            if (SlowChecks)
            {
                llint fi2 = 0; int nVisited = 0;
                for (int s3 = 0; s3 < nTowers; ++s3)
                {
                    int bs = xTower[s3]; Assert(bs != ai);
                    if (bs < ai) continue;
                    ++nVisited; 
                    llint cand = llint(K) * nVisited - 2 * (bs - ai);
                    if (cand > fi2) fi2 = cand;
                }
                Assert(fi2 == fi);
            }
        }
        // Calculate h.
        llint baseline = 0;
        for (int i = 0; i < nComps; ++i) {
            llint hLeft = f1[i] - g2[i], hRight = f3[i] + g2[i];
            baseline += hLeft;
            h[i] = hLeft - hRight; }
        // Run the greedy algorithm.
        vector<pair<llint, int>> heap(nComps / 2);
        int nSelected = 0;
        for (int k = 1; k <= nComps; ++k)
        {
            llint hk = h[k - 1];
            Assert(k / 2 == nSelected || k / 2 == nSelected + 1);
            if (k / 2 == nSelected + 1) { // Just add a new element to the heap.
                heap[nSelected++] = pair<llint, int>(hk, k - 1);
                push_heap(heap.begin(), heap.begin() + nSelected); }
            else if (hk < heap[0].first) 
            { 
                // Replace the most expensive element in the heap with the new, cheaper one. 
                // This will involve sifting the new element because it might be too small to stay in the root.
                int i = 0; while (2 * i + 1 < nSelected) {
                    // Let ci be the greater of i's two children.
                    int ci = 2 * i + 1; if (ci + 1 < nSelected && heap[ci + 1].first > heap[ci].first) ++ci;
                    if (hk >= heap[ci].first) break;
                    heap[i] = heap[ci]; i = ci; }
                heap[i] = pair<llint, int>(hk, k - 1);
            }
        }
        Assert(nSelected == nComps / 2);
        llint score = 0; for (auto &[hk, k] : heap) score += hk;
        // Compare with the dynamic programming solution.
        if (SlowChecks)
        {
            // Let f(nn, k) be the smallest valid selection of k computers from the first nn computers.
            vector<llint> fOld(nComps + 1), fNew(nComps + 1, 0);
            for (int nn = 1; nn <= nComps; ++nn)
            {
                swap(fOld, fNew);
                fNew[0] = 0;
                for (int k = 1; k <= (nn / 2); ++k)
                {
                    // Option 1: use the nn'th computer.
                    llint ff = h[nn - 1] + fOld[k - 1];
                    // Option 2: ignore the nn'th computer, choose k computers from the first nn - 1.
                    if (k <= ((nn - 1) / 2)) {
                        llint cand = fOld[k];
                        if (cand < ff) ff = cand; }
                    fNew[k] = ff; 
                }
            }
            Assert(fNew[nComps / 2] == score);
        }
        printf("%lld\n", baseline - score);
    }
    return 0;
}