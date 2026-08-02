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
#include <queue>

using namespace std;
//#define Assert assert
#define Assert(x)

//typedef int myint;
typedef int_fast64_t myint;

myint Sqrt(myint n)
{
    myint r = (myint) sqrt(n);
    Assert(r * r == n);
    return r;
}

// Returns the length of the longest string that can be constructed
// with S copy and paste operations of which k are copy operations.
myint Dsk(myint S, myint k) // O(1) time
{
    myint m = S - k, r = S % k;
    myint F = m / k;
    myint val = m * m - r * (F + 1) * (F + 1) - (k - r) * F * F;
    Assert((val & 1) == 0);
    return 1 + m + val / 2;
}

// Finds the k that maximizes Dsk(S, k) and returns it, along with
// the value of Dsk(S, k).
myint DskOpt(myint S, myint &k) // O(1) time
{
    double u = sqrt(S / 2.0);
    myint b1 = (myint) floor(u), b2 = (myint) ceil(u + 0.25);
    Assert(b2 - b1 == 1 || b2 - b1 == 2);
    if (b2 - b1 == 2) { k = b1 + 1; return Dsk(S, k); }
    myint D1 = Dsk(S, b1), D2 = Dsk(S, b2);
    if (D1 > D2) { k = b1; return D1; }
    else { k = b2; return D2; }
}

myint DskOpt(myint S) { myint dummy; return DskOpt(S, dummy); }

constexpr char ChCopy = 'Y', ChPaste = 'P', ChLeft = 'h', ChRight = 'l';

struct Solution
{
    // Represents a solution in S moves, of which k are copies;
    // there are m_i pastes after the i'th copy, and the vector
    // [m_1, ..., m_k] can be obtained (to use python-style notation)
    // as  miExtra + [miLow] * nLow + [miLow + 1] * nHigh.
    // Note that for solutions found with the greedy algorithm,
    // the length of 'miExtra' will be O(log log n), even though 
    // k is O(sqrt sqrt n) and S is O(sqrt n).  Hence this structure allows us
    // the represent a solution using less than O(k) space and time.
    myint S = 0, k = 0, miLow = 0, nLow = 0, nHigh = 0;
    vector<myint> miExtra; // length: O(log log n)

    void Clear() { S = 0; k = 0; miLow = 0; nLow = 0; nHigh = 0; miExtra.clear(); }

    myint Len() const // O(log log n) time
    {
        myint m = S - k;
        myint val = m * m - nLow * miLow * miLow - nHigh * (miLow + 1) * (miLow + 1);
        for (auto mi : miExtra) val -= mi * mi;
        Assert((val & 1) == 0);
        Assert(nLow + nHigh + (myint) miExtra.size() == k);
        myint m_ = nLow * miLow + nHigh * (miLow + 1); for (auto mi : miExtra) m_ += mi;
        Assert(m_ == m);
        return 1 + m + val / 2;
    }

    void GetStr(string& dest) const // O(S) = O(sqrt n) time
    {
        dest.resize(S);
        int j = 0;
        for (int i = 0; i < nLow; ++i) { dest[j++] = ChCopy; for (int ii = 0; ii < miLow; ++ii) dest[j++] = ChPaste; }
        for (int i = 0; i < nHigh; ++i) { dest[j++] = ChCopy; for (int ii = 0; ii <= miLow; ++ii) dest[j++] = ChPaste; }
        for (int mi : miExtra) { dest[j++] = ChCopy; for (int ii = 0; ii < mi; ++ii) dest[j++] = ChPaste; }
        Assert(j == S);
    }

    bool SolveGreedy(myint n) // O(sqrt sqrt n) time, with room for improvement
    {
        Clear(); 
        if (n <= 1) return true; // 0 operations
        // n = 2: k = 1, m_1 = 1.
        if (n == 2) { S = 2; k = 1; miLow = 1; nLow = 1; return true; }
        // Find the least S for which a string of length n can be constructed in <= S operations. 
        // This takes O(log n) time due to bisection.  The resulting S will be approx. sqrt(n / 2), hence O(sqrt n).
        myint SL = 1; S = 2; while (DskOpt(S) < n) { SL = S; S <<= 1; }
        while (S - SL > 1) { 
            myint SM = (S + SL) / 2;
            if (DskOpt(SM) >= n) S = SM; else SL = SM; }
        myint kOpt, gamma; gamma = DskOpt(S, kOpt) - n; 
        Assert(gamma >= 0);
        // The longest string for S copy and paste operations is too long by 'gamma'.
        // Try to shorten it by increasing k as much as possible without D_S(k) dropping below 'n'.
        // It can be shown that at most O(sqrt sqrt S)  increments will be made in the 'while' loop below.
        k = kOpt; // kOpt = sqrt(S / 2) + O(1)
        while (k + 1 <= S / 2) { // O(sqrt sqrt S) time
            myint gamma1 = Dsk(S, k + 1) - n; if (gamma1 < 0) break;
            ++k; gamma = gamma1; }
        if constexpr (true)
        {
            // Alternatively, we could use bisection to find the maximum k in only O(log n) time.
            double u = sqrt(S / 2.0);
            myint KL = kOpt, KH = 1 + myint(ceil(u + sqrt(u)));
            Assert(KL < KH);
            Assert(Dsk(S, KL) >= n); Assert(n > Dsk(S, KH));
            myint gamma_ = gamma;
            while (KH - KL > 1)
            {
                myint KM = (KL + KH) / 2;
                myint gamma1 = Dsk(S, KM) - n;
                if (gamma1 < 0) KH = KM;
                else KL = KM, gamma_ = gamma1;
            }
            Assert(KL == k); Assert(gamma_ == gamma);
        }
        // Now kOpt <= k <= ceil(u + sqrt(u)), for u = sqrt(S / 2).  Hence k is still O(sqrt S) = O(sqrt sqrt n).
        // Initialize the solution representing the longest string for S operations of which k are copies.
        myint m = S - k; miLow = m / k; nHigh = m % k; nLow = k - nHigh;
        myint p = k;
        // We now need to make the string shorter by 'gamma'.
        // The outer loop makes O(log log n) iterations; in the first of these, the inner loop
        // makes O(k) = O(sqrt sqrt n) iterations, and leaves gamma at O(k); in each subsequent 
        // iteration of the outer loop, the inner loop makes O(sqrt gamma) iterations and leaves gamma
        // at less than the square root of its old value.  Thus gamma decreases exponentially and
        // the total number of iterations of the inner loop remains O(sqrt sqrt n), which is thus
        // also the total cost of the outer loop.
        while (p >= 2 && gamma > 0)
        {
            // We are looking at p groups of paste operations (the other k-p groups have already been fixed
            // and we won't change them any further).  Of these groups, nLow have miLow paste operations
            // and nHigh groups have miLow + 1 paste operations.  We'll start by declaring one of the
            // larger groups to be 'mp' and exclude it from the counts 'nLow' and 'nHigh'.
            Assert(nLow + nHigh == p);
            myint mp; if (nHigh > 0) mp = miLow + 1, --nHigh; 
            else mp = miLow, --nLow;
            // We can replace the inner loop with an O(1) calculation.  In the following if statement
            // we'll perform it and then later check that its results match those of the inner loop.
            myint miLow_ = miLow, nLow_ = nLow, nHigh_ = nHigh, gamma_ = gamma, mp_ = mp;
            constexpr bool DoConstantTime = true;
            //fprintf(stderr, "n = %d, p = %d, gamma = %d %d\n", n, p, gamma, gamma_);
            if constexpr (DoConstantTime)
            {
                auto Root = [] (myint A, myint B, myint C) { return (myint) floor((-B + sqrt(B * B - 4 * A * C)) / (2.0 * A)); };
                while (true)
                {
                    // It's constant-time per each pass that the original loop-based solution would make
                    // through all the p-1 values m_i that we can still decrease.  It can be proven that
                    // for all sufficiently large n, the first iteration of the outer loop will require
                    // at most two passes (for even larger n it will require just one pass) and all
                    // subsequent iterations of the outer loop will require just one pass, so this is
                    // still an O(1) solution per each iteration of the outer loop.
                    myint d = mp_ - (nHigh_ > 0 ? miLow_ + 1 : miLow_);
                    if (nHigh_ == 0)
                    {
                        // In the first t iterations of the inner loop, the decreases of 'gamma' will be by
                        // d+1, d+2, ..., d+t, for a total of dt + t (t + 1) / 2.  Thus we have to choose the largest t
                        // for which t (t + 1) / 2 does not exceed 'gamma'.
                        //      dt + t(t+1) / 2 <= gamma
                        //      t^2 + (2d + 1) t <= 2 gamma
                        //      t^2 + (2d + 1) t - 2 gamma <= 0
                        //      t_{1,2} = [-(2d + 1) +/- sqrt(1 + 8 gamma)] / 2
                        // The quadratic function t^2 + t - 2 gamma is negative between its roots, so
                        // the highest suitable t will be the floor of the larger root.
                        myint t = Root(1, 2 * d + 1, -2 * gamma_);
                        if (t >= p) t = p - 1;
                        Assert(t >= 0);
                        if (t <= 0) break;
                        myint delta = d * t + (t * (t + 1)) / 2; Assert(delta <= gamma_);
                        mp_ += t; --miLow_; gamma_ -= delta;
                        if (t < p - 1) { nHigh_ = nLow_ - t; nLow_ = t; }
                    }
                    else 
                    {
                        const myint r = nHigh_;
                        // In the first t iterations of the inner loop, the decreases of 'gamma' will be by
                        // d+1, d+2, ..., d+r, d+r + 2, d+r + 3, ..., d+t + 1.  Thus, for t <= r, the total decrease
                        // is dt + t (t + 1) / 2, same as in the previous case.  
                        if (gamma_ <= d * r + r * (r + 1) / 2)
                        {
                            // We'll have to choose a t <= r using the same formula as when r == 0.
                            myint t = Root(1, 2 * d + 1, -2 * gamma_);
                            Assert(t >= 0); Assert(t <= r);
                            if (t <= 0) break;
                            myint delta = d * t + (t * (t + 1)) / 2; Assert(delta <= gamma_);
                            mp_ += t; nHigh_ -= t; nLow_ += t; gamma_ -= delta;
                        }
                        else
                        {
                            // For t > r, the total decrease is
                            //    dt + r (r + 1) / 2 + [(r + 2) + (r + 3) + ... + (t + 1)]
                            // =  dt + r (r + 1) / 2 + (t - r) (r + 1) + [1 + 2 + ... + (t - r)]       
                            // =  ds + dr + r (r + 1) / 2 + (r + 1) s + s (s + 1) / 2             for s := t - r
                            // = (1/2) s^2 + (d + r + 3/2) s + dr + r (r + 1) / 2.
                            // Thus we have the condition
                            //   (1/2) s^2 + (d + r + 3/2) s + dr + r (r + 1) / 2 <= gamma
                            //   (1/2) s^2 + (d + r + 3/2) s + dr + r (r + 1) / 2 - gamma <= 0
                            //   s^2 + (2d + 2r + 3) s + 2dr + r (r + 1) - 2 gamma <= 0
                            myint s = Root(1, 2 * (d + r) + 3, (2 * d + r + 1) * r - 2 * gamma_);
                            Assert(s >= 0); myint t = r + s;
                            if (t >= p) { t = p - 1; s = t - r; }
                            if (t <= 0) break;
                            myint delta = d * t + (r * (r + 1)) / 2 + (r + 1) * s + (s * (s + 1)) / 2;
                            Assert(delta <= gamma_);
                            mp_ += t; gamma_ -= delta;
                            // The first r steps change all the high mi's into low ones.
                            Assert(t >= r); nHigh_ -= r; nLow_ += r; 
                            Assert(nHigh_ == 0); Assert(nLow_ == p - 1);
                            // The remaining s steps change s mi's into new (still lower) low ones
                            // while the remaining p-1-s mi's are the new high ones (same as the old low ones).
                            Assert(t == s + r); if (s > 0) { --miLow_; if (s < p - 1) { nHigh_ = nLow_ - s; nLow_ = s; }}
                        }
                    }
                }
            }
            // In each iteration of this inner loop, we'll decrease the largest of the p-1 remaining groups
            // by one and increase 'mp' by one.  This makes the resulting string shorter by 't'.
            // When this would make it too short, the inner loop stops.
            while (true)
            {
                int mj = (nHigh > 0) ? miLow + 1 : miLow; Assert(mj <= mp);
                int t = mp - mj + 1; 
                if (t > gamma) break;
                ++mp; gamma -= t;
                if (nHigh == 0) { nHigh = nLow; --miLow; nLow = 0; }
                Assert(nHigh > 0);
                --nHigh; ++nLow; 
            }
            // Check that the results of the inner loop and the constant-time version match.
            if constexpr (DoConstantTime) {
                Assert(miLow_ == miLow); Assert(nLow_ == nLow); Assert(nHigh_ == nHigh);
                Assert(gamma_ == gamma); Assert(mp_ == mp); }
            // Now we are ready to retire the group represented by 'mp' and transfer it into 'miExtra'.
            // If mp dropped below zero in the course of the above loop, the solution is now invalid
            // and our greedy algorithm has failed to find a solution (perhaps because no solution 
            // exists for this S and k).
            if (mp < 0) return false;
            miExtra.emplace_back(mp); --p;
        }
        // The solution is valid if gamma dropped to 0 (i.e. the generated string is exactly n characters long)
        // and if none of the group sizes dropped below 0 (as it makes no sense to have a group with a
        // negative number of paste operations).  
        return gamma == 0 && (nLow == 0 || miLow >= 0) && (nHigh == 0 || miLow + 1 >= 0);
    }

    bool SolveGreedyFast(myint n) // O(log n) time
    {
        Clear(); 
        if (n <= 1) return true; // 0 operations
        // n = 2: k = 1, m_1 = 1.
        if (n == 2) { S = 2; k = 1; miLow = 1; nLow = 1; return true; }
        // Find the least S for which a string of length n can be constructed in <= S operations. 
        // This takes O(log n) time due to bisection.  The resulting S will be approx. sqrt(n / 2), hence O(sqrt n).
        myint SL = 1; S = 2; while (DskOpt(S) < n) { SL = S; S <<= 1; }
        while (S - SL > 1) { 
            myint SM = (S + SL) / 2;
            if (DskOpt(SM) >= n) S = SM; else SL = SM; }
        myint kOpt, gamma; gamma = DskOpt(S, kOpt) - n; 
        Assert(gamma >= 0);
        // The longest string for S copy and paste operations is too long by 'gamma'.
        // Try to shorten it by increasing k as much as possible without D_S(k) dropping below 'n'.
        // It can be shown that the largest suitable k is in the range kOpt <= k <= ceil(u + sqrt u), for u = sqrt(S / 2),
        // while kOpt = u + O(1).  Hence we will be doing bisection over a range of O(sqrt u) possible values of k,
        // which will cost us O(log sqrt u) = O(log sqrt sqrt S) = O(log sqrt sqrt sqrt n) = O(log n).
        double u = sqrt(S / 2.0);
        myint KL = kOpt, KH = 1 + myint(ceil(u + sqrt(u)));
        Assert(KL < KH);
        Assert(Dsk(S, KL) >= n); Assert(n > Dsk(S, KH));
        while (KH - KL > 1)
        {
            myint KM = (KL + KH) / 2;
            myint gamma1 = Dsk(S, KM) - n;
            if (gamma1 < 0) KH = KM;
            else KL = KM, gamma = gamma1;
        }
        k = KL;
        // Now kOpt <= k <= ceil(u + sqrt(u)), for u = sqrt(S / 2).  Hence k is still O(sqrt S) = O(sqrt sqrt n).
        // Initialize the solution representing the longest string for S operations of which k are copies.
        myint m = S - k; miLow = m / k; nHigh = m % k; nLow = k - nHigh;
        myint p = k;
        // We now need to make the string shorter by 'gamma'.
        // The outer loop makes O(log log n) iterations; in the first of these, the inner loop
        // can make at most two iteratins (for large enough n), in later iterations of the outer loop
        // the inner loop makes only one iteration.  Thus the cost of the inner loop as a whole is O(log log n).
        while (p >= 2 && gamma > 0)
        {
            // We are looking at p groups of paste operations (the other k-p groups have already been fixed
            // and we won't change them any further).  Of these groups, nLow have miLow paste operations
            // and nHigh groups have miLow + 1 paste operations.  We'll start by declaring one of the
            // larger groups to be 'mp' and exclude it from the counts 'nLow' and 'nHigh'.
            Assert(nLow + nHigh == p);
            myint mp; if (nHigh > 0) mp = miLow + 1, --nHigh; 
            else mp = miLow, --nLow;
            // We can replace the inner loop with an O(1) calculation.  In the following if statement
            // we'll perform it and then later check that its results match those of the inner loop.
            auto Root = [] (myint A, myint B, myint C) { return (myint) floor((-B + sqrt(B * B - 4 * A * C)) / (2.0 * A)); };
            while (true)
            {
                // It's constant-time per each pass that the original loop-based solution would make
                // through all the p-1 values m_i that we can still decrease.  It can be proven that
                // for all sufficiently large n, the first iteration of the outer loop will require
                // at most two passes (for even larger n it will require just one pass) and all
                // subsequent iterations of the outer loop will require just one pass, so this is
                // still an O(1) solution per each iteration of the outer loop.
                myint d = mp - (nHigh > 0 ? miLow + 1 : miLow);
                if (nHigh == 0)
                {
                    // In the first t iterations of the inner loop, the decreases of 'gamma' will be by
                    // d+1, d+2, ..., d+t, for a total of dt + t (t + 1) / 2.  Thus we have to choose the largest t
                    // for which t (t + 1) / 2 does not exceed 'gamma'.
                    //      dt + t(t+1) / 2 <= gamma
                    //      t^2 + (2d + 1) t <= 2 gamma
                    //      t^2 + (2d + 1) t - 2 gamma <= 0
                    //      t_{1,2} = [-(2d + 1) +/- sqrt(1 + 8 gamma)] / 2
                    // The quadratic function t^2 + t - 2 gamma is negative between its roots, so
                    // the highest suitable t will be the floor of the larger root.
                    myint t = Root(1, 2 * d + 1, -2 * gamma);
                    if (t >= p) t = p - 1;
                    Assert(t >= 0);
                    if (t <= 0) break;
                    myint delta = d * t + (t * (t + 1)) / 2; Assert(delta <= gamma);
                    mp += t; --miLow; gamma -= delta;
                    if (t < p - 1) { nHigh = nLow - t; nLow = t; }
                }
                else 
                {
                    const myint r = nHigh;
                    // In the first t iterations of the inner loop, the decreases of 'gamma' will be by
                    // d+1, d+2, ..., d+r, d+r + 2, d+r + 3, ..., d+t + 1.  Thus, for t <= r, the total decrease
                    // is dt + t (t + 1) / 2, same as in the previous case.  
                    if (gamma <= d * r + r * (r + 1) / 2)
                    {
                        // We'll have to choose a t <= r using the same formula as when r == 0.
                        myint t = Root(1, 2 * d + 1, -2 * gamma);
                        Assert(t >= 0); Assert(t <= r);
                        if (t <= 0) break;
                        myint delta = d * t + (t * (t + 1)) / 2; Assert(delta <= gamma);
                        mp += t; nHigh -= t; nLow += t; gamma -= delta;
                    }
                    else
                    {
                        // For t > r, the total decrease is
                        //    dt + r (r + 1) / 2 + [(r + 2) + (r + 3) + ... + (t + 1)]
                        // =  dt + r (r + 1) / 2 + (t - r) (r + 1) + [1 + 2 + ... + (t - r)]       
                        // =  ds + dr + r (r + 1) / 2 + (r + 1) s + s (s + 1) / 2             for s := t - r
                        // = (1/2) s^2 + (d + r + 3/2) s + dr + r (r + 1) / 2.
                        // Thus we have the condition
                        //   (1/2) s^2 + (d + r + 3/2) s + dr + r (r + 1) / 2 <= gamma
                        //   (1/2) s^2 + (d + r + 3/2) s + dr + r (r + 1) / 2 - gamma <= 0
                        //   s^2 + (2d + 2r + 3) s + 2dr + r (r + 1) - 2 gamma <= 0
                        myint s = Root(1, 2 * (d + r) + 3, (2 * d + r + 1) * r - 2 * gamma);
                        Assert(s >= 0); myint t = r + s;
                        if (t >= p) { t = p - 1; s = t - r; }
                        if (t <= 0) break;
                        myint delta = d * t + (r * (r + 1)) / 2 + (r + 1) * s + (s * (s + 1)) / 2;
                        Assert(delta <= gamma);
                        mp += t; gamma -= delta;
                        // The first r steps change all the high mi's into low ones.
                        Assert(t >= r); nHigh -= r; nLow += r; 
                        Assert(nHigh == 0); Assert(nLow == p - 1);
                        // The remaining s steps change s mi's into new (still lower) low ones
                        // while the remaining p-1-s mi's are the new high ones (same as the old low ones).
                        Assert(t == s + r); if (s > 0) { --miLow; if (s < p - 1) { nHigh = nLow - s; nLow = s; }}
                    }
                }
            }
            // Now we are ready to retire the group represented by 'mp' and transfer it into 'miExtra'.
            // If mp dropped below zero in the course of the above loop, the solution is now invalid
            // and our greedy algorithm has failed to find a solution (perhaps because no solution 
            // exists for this S and k).
            if (mp < 0) return false;
            miExtra.emplace_back(mp); --p;
        }
        // The solution is valid if gamma dropped to 0 (i.e. the generated string is exactly n characters long)
        // and if none of the group sizes dropped below 0 (as it makes no sense to have a group with a
        // negative number of paste operations).  
        return gamma == 0 && (nLow == 0 || miLow >= 0) && (nHigh == 0 || miLow + 1 >= 0);
    }

};

struct TBfsSolver_N2_UnorderedMap // O(n^2) space and time, uses an unordered_map to store the states
{
    int cBits, cMask, nMax;
    typedef int TState;
    inline TState Encode(int d, int p, int c) { return (((d << cBits) | p) << cBits) | c; };
    inline void Decode(TState state, int &d, int &p, int &c) {
        c = state & cMask; state >>= cBits; p = state & cMask; d = state >> cBits; };
    unordered_map<TState, TState> states;
    queue<TState> toDo; 
    vector<TState> endState; // index: n
    int nEndStates; TState s0;
    void Consider(TState prevState, int d, int p, int c)
    {
        TState s = Encode(d, p, c); 
        auto [it, isNew] = states.emplace(s, prevState);
        if (! isNew) return;
        auto &es = endState[d]; if (es < 0) es = s, ++nEndStates;
        toDo.emplace(s); 
    }
    TBfsSolver_N2_UnorderedMap(int nMax_) : nMax(nMax_)
    {
        int root = 1; while ((root + 1) * (root + 1) < nMax) ++root;
        int maxS = 4 * root + 3; // an easy to prove upper bound
        if (nMax >= 100) maxS = 2 * (root + 1); // an empirically observed upper bound, a good deal tighter than the one above
        cBits = 1; while ((1 << cBits) <= maxS) ++cBits;
        cMask = (1 << cBits) - 1; 
        endState.clear(); endState.resize(nMax + 1, -1);
        s0 = Encode(1, 1, 0); states.emplace(s0, -1); toDo.emplace(s0);
        endState[1] = s0; nEndStates = 1;
        //
        TState endState = (nMax <= 1) ? s0 : -1;
        while (! toDo.empty() && nEndStates < nMax)
        {
            TState s1 = toDo.front(); toDo.pop();
            int d, p, c; Decode(s1, d, p, c);
            if (c > 0 && d + c <= nMax) Consider(s1, d + c, p + 1, c); // paste
            if (c != p) Consider(s1, d, p, p); // copy
            if (p < d) Consider(s1, d, p + 1, c); // left
            if (p > 1) Consider(s1, d, p - 1, c); // right
        }
        Assert(nEndStates == nMax);
        string dest; Solve(nMax, dest);
        fprintf(stderr, "TBfsSolver(nMax = %d) -> S = %d moves (maxS = %d), %lld states\n", nMax, (int) dest.length(), maxS, (long long) states.size());
    }

    void Solve(int n, string &dest)
    {
        dest.clear(); Assert(1 <= n); Assert(n <= nMax);
        for (TState s = endState[n]; s != s0; ) {
            auto it = states.find(s); Assert(it != states.end());
            TState prevState = it->second;
            int d1, p1, c1, d2, p2, c2;
            Decode(s, d2, p2, c2); Decode(prevState, d1, p1, c1);
            if (d2 == d1 + c1 && p2 == p1 + 1 && c2 == c1) dest += ChPaste;
            else if (d2 == d1 && p2 == p1 && c2 == p1) dest += ChCopy;
            else if (d2 == d1 && p2 == p1 + 1 && c2 == c1) dest += ChLeft;
            else if (d2 == d1 && p2 == p1 - 1 && c2 == c1) dest += ChRight;
            else Assert(false); 
            s = prevState; }
        for (int i = 0, j = (int) dest.length() - 1; i < j; ++i, --j) swap(dest[i], dest[j]); 
    }
};

struct TBfsSolver_N2_Vector // O(n^2) space and time, uses a vector to store the states, lowest two dimensions are rounded up to powers of 2
{
    int cBits, cMask, nMax;
    typedef int TState;
    inline TState Encode(int d, int p, int c) { return (((d << cBits) | p) << cBits) | c; };
    inline void Decode(TState state, int &d, int &p, int &c) {
        c = state & cMask; state >>= cBits; p = state & cMask; d = state >> cBits; };
    vector<TState> states;
    queue<TState> toDo; 
    vector<TState> endState; // index: n
    int nEndStates; TState s0;
    void Consider(TState prevState, int d, int p, int c)
    {
        TState s = Encode(d, p, c); 
        auto &st = states[s]; if (st >= 0) return;
        st = prevState;
        auto &es = endState[d]; if (es < 0) es = s, ++nEndStates;
        toDo.emplace(s); 
    }
    TBfsSolver_N2_Vector(int nMax_) : nMax(nMax_)
    {
        int root = 1; while ((root + 1) * (root + 1) < nMax) ++root;
        int maxS = 4 * root + 3; // an easy to prove upper bound
        if (nMax >= 100) maxS = 2 * (root + 1); // an empirically observed upper bound, a good deal tighter than the one above
        cBits = 1; while ((1 << cBits) <= maxS) ++cBits;
        fprintf(stderr, "nMax = %d, maxS = %d\n", nMax, maxS);
        cMask = (1 << cBits) - 1; 
        endState.clear(); endState.resize(nMax + 1, -1);
        states.clear(); states.resize(((nMax + 1) << (2 * cBits)) + 1, -1);
        fprintf(stderr, "%d states\n", (int) states.size());
        s0 = Encode(1, 1, 0); states[s0] = -1; toDo.emplace(s0);
        endState[1] = s0; nEndStates = 1;
        //
        TState endState = (nMax <= 1) ? s0 : -1;
        while (! toDo.empty() && nEndStates < nMax)
        {
            TState s1 = toDo.front(); toDo.pop();
            int d, p, c; Decode(s1, d, p, c);
            if (c > 0 && d + c <= nMax) Consider(s1, d + c, p + 1, c); // paste
            if (c != p) Consider(s1, d, p, p); // copy
            if (p < d) Consider(s1, d, p + 1, c); // left
            if (p > 1) Consider(s1, d, p - 1, c); // right
        }
        Assert(nEndStates == nMax);
        string dest; Solve(nMax, dest);
        fprintf(stderr, "TBfsSolver(nMax = %d) -> S = %d moves (maxS = %d), %lld states\n", nMax, (int) dest.length(), maxS, (long long) states.size());
    }

    void Solve(int n, string &dest)
    {
        dest.clear(); Assert(1 <= n); Assert(n <= nMax);
        for (TState s = endState[n]; s != s0; ) {
            TState prevState = states[s]; Assert(prevState >= 0);
            int d1, p1, c1, d2, p2, c2;
            Decode(s, d2, p2, c2); Decode(prevState, d1, p1, c1);
            if (d2 == d1 + c1 && p2 == p1 + 1 && c2 == c1) dest += ChPaste;
            else if (d2 == d1 && p2 == p1 && c2 == p1) dest += ChCopy;
            else if (d2 == d1 && p2 == p1 + 1 && c2 == c1) dest += ChLeft;
            else if (d2 == d1 && p2 == p1 - 1 && c2 == c1) dest += ChRight;
            else Assert(false); 
            s = prevState; }
        for (int i = 0, j = (int) dest.length() - 1; i < j; ++i, --j) swap(dest[i], dest[j]); 
    }
};

struct TBfsSolver_N2_Vector2 // O(n^2) space and time, uses a vector to store the states
{
    int nMax, maxSPlus1;
    typedef int TState;
    inline TState Encode(int d, int p, int c) { return (d * maxSPlus1 + p) * maxSPlus1 + c; };
    inline void Decode(TState state, int &d, int &p, int &c) {
        auto div1 = div(state, maxSPlus1); c = div1.rem;
        auto div2 = div(div1.quot, maxSPlus1); p = div2.rem; d = div2.quot; };
    vector<TState> states;
    queue<TState> toDo; 
    vector<TState> endState; // index: n
    int nEndStates; TState s0;
    void Consider(TState prevState, int d, int p, int c)
    {
        TState s = Encode(d, p, c); 
        auto &st = states[s]; if (st >= 0) return;
        st = prevState;
        auto &es = endState[d]; if (es < 0) es = s, ++nEndStates;
        toDo.emplace(s); 
    }
    TBfsSolver_N2_Vector2(int nMax_) : nMax(nMax_)
    {
        int root = 1; while ((root + 1) * (root + 1) < nMax) ++root;
        int maxS = 4 * root + 3; // an easy to prove upper bound
        if (nMax >= 100) maxS = 2 * (root + 1); // an empirically observed upper bound, a good deal tighter than the one above
        maxSPlus1 = maxS + 1;
        fprintf(stderr, "nMax = %d, maxS = %d\n", nMax, maxS);
        endState.clear(); endState.resize(nMax + 1, -1);
        states.clear(); states.resize((nMax + 1) * maxSPlus1 * maxSPlus1 + 1, -1);
        fprintf(stderr, "%d states\n", (int) states.size());
        s0 = Encode(1, 1, 0); states[s0] = -1; toDo.emplace(s0);
        endState[1] = s0; nEndStates = 1;
        //
        TState endState = (nMax <= 1) ? s0 : -1;
        while (! toDo.empty() && nEndStates < nMax)
        {
            TState s1 = toDo.front(); toDo.pop();
            int d, p, c; Decode(s1, d, p, c);
            if (c > 0 && d + c <= nMax) Consider(s1, d + c, p + 1, c); // paste
            if (c != p) Consider(s1, d, p, p); // copy
            if (p < d) Consider(s1, d, p + 1, c); // left
            if (p > 1) Consider(s1, d, p - 1, c); // right
        }
        Assert(nEndStates == nMax);
        string dest; Solve(nMax, dest);
        fprintf(stderr, "TBfsSolver(nMax = %d) -> S = %d moves (maxS = %d), %lld states\n", nMax, (int) dest.length(), maxS, (long long) states.size());
    }

    void Solve(int n, string &dest)
    {
        dest.clear(); Assert(1 <= n); Assert(n <= nMax);
        for (TState s = endState[n]; s != s0; ) {
            TState prevState = states[s]; Assert(prevState >= 0);
            int d1, p1, c1, d2, p2, c2;
            Decode(s, d2, p2, c2); Decode(prevState, d1, p1, c1);
            if (d2 == d1 + c1 && p2 == p1 + 1 && c2 == c1) dest += ChPaste;
            else if (d2 == d1 && p2 == p1 && c2 == p1) dest += ChCopy;
            else if (d2 == d1 && p2 == p1 + 1 && c2 == c1) dest += ChLeft;
            else if (d2 == d1 && p2 == p1 - 1 && c2 == c1) dest += ChRight;
            else Assert(false); 
            s = prevState; }
        for (int i = 0, j = (int) dest.length() - 1; i < j; ++i, --j) swap(dest[i], dest[j]); 
    }
};

struct TBfsSolver_NSqrtN_Vector2 // O(n^2) time, O(n^1.5) space, uses a vector to store the states
{
    int nMax, maxSPlus1;
    typedef int TState;
    inline TState Encode(int d, int p) { return d * maxSPlus1 + p; };
    inline void Decode(TState state, int &d, int &p) {
        auto div1 = div(state, maxSPlus1); p = div1.rem; d = div1.quot; };
    struct TStateInfo { int nMoves = -1; TState prevState = -1; };
    vector<TStateInfo> states;
    vector<queue<TState>> toDo; 
    vector<TState> endState; // index: n
    int nEndStatesDequeued; TState s0;
    void Consider(TState prevState, int d, int p, int nMoves)
    {
        TState s = Encode(d, p); 
        auto &st = states[s]; if (st.nMoves >= 0 && st.nMoves <= nMoves) return;
        st.nMoves = nMoves; st.prevState = prevState;
        toDo[nMoves].emplace(s); 
    }
    TBfsSolver_NSqrtN_Vector2(int nMax_) : nMax(nMax_)
    {
        int root = 1; while ((root + 1) * (root + 1) < nMax) ++root;
        int maxS = 4 * root + 3; // an easy to prove upper bound
        if (nMax >= 100) maxS = 2 * (root + 1); // an empirically observed upper bound, a good deal tighter than the one above
        maxSPlus1 = maxS + 1;
        fprintf(stderr, "nMax = %d, maxS = %d\n", nMax, maxS);
        endState.clear(); endState.resize(nMax + 1, -1);
        states.clear(); states.resize((nMax + 1) * maxSPlus1 + 1);
        toDo.clear(); toDo.resize(maxSPlus1);
        fprintf(stderr, "%d states\n", (int) states.size());
        s0 = Encode(1, 1); toDo[0].emplace(s0);
        nEndStatesDequeued = 0; states[s0].nMoves = 0;
        //
        for (int nMoves = 0; nMoves <= maxS && nEndStatesDequeued < nMax; ++nMoves) {
            auto &ToDo = toDo[nMoves];
            while (! ToDo.empty() && nEndStatesDequeued < nMax)
            {
                TState s1 = ToDo.front(); ToDo.pop();
                if (states[s1].nMoves != nMoves) continue; // has been processed before in an earlier queue
                int d, p; Decode(s1, d, p);
                if (endState[d] < 0) { endState[d] = s1; ++nEndStatesDequeued; }
                if (p < d) Consider(s1, d, p + 1, nMoves + 1); // left
                if (p > 1) Consider(s1, d, p - 1, nMoves + 1); // right
                // copy and 1 or more pastes
                for (int nMoves2 = nMoves + 2, c = p, d2 = d + p, p2 = p + 1; d2 <= nMax && nMoves2 <= maxS; ) {
                    Consider(s1, d2, p2, nMoves2);
                    d2 += c; ++p2; ++nMoves2; }
            } }
        Assert(nEndStatesDequeued == nMax);
        string dest; Solve(nMax, dest);
        fprintf(stderr, "TBfsSolver(nMax = %d) -> S = %d moves (maxS = %d), %lld states\n", nMax, (int) dest.length(), maxS, (long long) states.size());
    }

    void Solve(int n, string &dest)
    {
        dest.clear(); Assert(1 <= n); Assert(n <= nMax);
        for (TState s = endState[n]; s != s0; ) {
            TState prevState = states[s].prevState; Assert(prevState >= 0);
            int d1, p1, d2, p2;
            Decode(s, d2, p2); Decode(prevState, d1, p1);
            if (d2 == d1 && p2 == p1 + 1) dest += ChLeft;
            else if (d2 == d1 && p2 == p1 - 1) dest += ChRight;
            else {
                int nPastes = p2 - p1; Assert(nPastes >= 1);
                Assert((d2 - d1) % nPastes == 0);
                int c = (d2 - d1) / nPastes;
                Assert(c == p1);
                for (int i = 0; i < nPastes; ++i) dest += ChPaste;
                dest += ChCopy;
            }
            s = prevState; }
        for (int i = 0, j = (int) dest.length() - 1; i < j; ++i, --j) swap(dest[i], dest[j]); 
    }
};

int main1()
{
    for (myint n = 2'000'000'000; /*n <= 1000000*/; ++n)
    {
        Solution sol; bool ok = sol.SolveGreedy(n);
        Solution sol2; bool ok2 = sol2.SolveGreedyFast(n);
        Assert(ok == ok2);
        if (ok) {
            Assert(sol.S == sol2.S);
            Assert(sol.k == sol2.k);
            Assert(sol.miLow == sol2.miLow);
            Assert(sol.nLow == sol2.nLow);
            Assert(sol.nHigh == sol2.nHigh);
            Assert(sol.miExtra == sol2.miExtra); }
        if (! ok) { printf("SolveGreedy fails on n = %lld.\n", (long long) n); 
            TBfsSolver_N2_UnorderedMap solver(n);
            string sol; solver.Solve(n, sol);
            printf("SolveBfs(%lld): %d moves, %s\n", (long long) n, (int) sol.length(), sol.c_str()); 
            continue; }
        if (false) { string solBfs; TBfsSolver_N2_UnorderedMap(n).Solve(n, solBfs); Assert(int(solBfs.length()) == sol.S); }
        myint n_ = sol.Len(); Assert(n == n_);
        if (n % 1000 == 0) printf("Solve(%lld) = %lld.\n", (long long) n, (long long) sol.S);
    }
    return 0;
}

enum { MaxCases = 100, MaxN = 10'000'000 };
#define mainCeoiEfficient main

int mainCeoiEfficient() // CEOI, use the sublinear algorithm when possible
{
    int nCases; int ok_ = scanf("%d", &nCases);
    Assert(ok_ == 1); Assert(1 <= nCases); Assert(nCases <= MaxCases);
    int maxN = -1, nBfsCases = 0;
    Solution sol; string solStr;
    for (int caseNo = 0; caseNo < nCases; ++caseNo)
    {
        int n; int ok_ = scanf("%d", &n);
        Assert(ok_ == 1); Assert(1 <= n); Assert(n <= MaxN);
        if (n > maxN) maxN = n;
        // With SolveGreedyFast taking only O(log n) time, the most time-consuming part of this
        // solution is printing the output string, which takes O(sqrt n) time.
        if (sol.SolveGreedyFast(n)) {
            Assert(sol.Len() == n);
            sol.GetStr(solStr); }
        else {
            TBfsSolver_N2_UnorderedMap(n).Solve(n, solStr);
            ++nBfsCases; }
        printf("%d %s\n", int(solStr.length()), solStr.c_str()); 
    }
    fprintf(stderr, "%d cases (%d required BFS), maxN = %d\n", nCases, nBfsCases, maxN);   
    return 0;
}

int mainCeoiBfs() // CEOI, always use BFS in O(n^2) time and space, but only once per batch
{
    int nCases; int ok_ = scanf("%d", &nCases);
    Assert(ok_ == 1); Assert(1 <= nCases); Assert(nCases <= MaxCases);
    vector<int> ns(nCases); int nMax = 0;
    for (int &n : ns) {
        int ok_ = scanf("%d", &n);
        Assert(ok_ == 1); Assert(1 <= n); Assert(n <= MaxN);
        if (n > nMax) nMax = n; }
    fprintf(stderr, "%d cases, nMax = %d\n", nCases, nMax); fflush(stderr);
    //TBfsSolver_N2_UnorderedMap solver(nMax);
    //TBfsSolver_N2_Vector solver(nMax);
    //TBfsSolver_N2_Vector2 solver(nMax);
    TBfsSolver_NSqrtN_Vector2 solver(nMax);
    Solution sol; string solStr;
    for (int n : ns) {
        solver.Solve(n, solStr);
        printf("%d %s\n", int(solStr.length()), solStr.c_str());  }
    return 0;
}