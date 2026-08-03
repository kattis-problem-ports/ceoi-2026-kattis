// Downloaded from https://oj.uz/submission/1407952 (scored 100/100 on oj.uz).
// Scores 100 on this package.
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <vector>
#include <random>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <atomic>

// 11 log n poizvedb

#define PUTKA

#ifndef PUTKA
#include <thread>
#endif

using namespace std;

#include <cassert>
#define Assert assert
//#define Assert(b)

enum TDirection { dirRight, dirUp, dirLeft, dirDown };
constexpr int DX[] = { 1, 0, -1, 0 }, DY[] = { 0, -1, 0, 1 };
enum TQueryResult { qrUp = 1 << dirUp, qrDown = 1 << dirDown, qrLeft = 1 << dirLeft, qrRight = 1 << dirRight, qrTreasure = 16 };
enum { MaxK = 3, MaxN = 1'000'000 };
typedef pair<int, int> TPair;

struct TPoint
{
    int x = -1, y = -1;
    TPoint() = default;
    constexpr TPoint(int x_, int y_) : x(x_), y(y_) { }
    constexpr TPoint(TPair pr) : x(pr.first), y(pr.second) { }
    TPoint &operator += (TPoint A) { x += A.x; y += A.y; return *this; }
    TPoint &operator -= (TPoint A) { x -= A.x; y -= A.y; return *this; }
    TPoint &operator *= (int a) { x *= a; y *= a; return *this; }
    TPoint operator + (TPoint A) const { return TPoint(x + A.x, y + A.y); }
    TPoint operator - (TPoint A) const { return TPoint(x - A.x, y - A.y); }
    TPoint operator * (int a) const { return TPoint(x * a, y * a); }
    TPoint operator -() const { return TPoint(-x, -y); }
    bool operator == (TPoint A) const { return x == A.x && y == A.y; }
    bool operator != (TPoint A) const { return x != A.x || y != A.y; }
};

TPoint operator * (int a, TPoint B) { return TPoint(a * B.x, a * B.y); }
constexpr TPoint Dirs[] = { TPoint(DX[0], DY[0]), TPoint(DX[1], DY[1]), TPoint(DX[2], DY[2]), TPoint(DX[3], DY[3]) };
int abs(TPoint A) { return abs(A.x) + abs(A.y); }

struct TInterface
{
    int n, k, cacheHits = 0;
    unordered_map<long long, int> cache;
    virtual ~TInterface() { }
    virtual int Query_(int x, int y) = 0;
    int Query(int x, int y)
    {
        Assert(0 <= x); Assert(x < n); Assert(0 <= y); Assert(y < n);
        long long key = ((long long) x) * n + y;
        auto [it, isNew] = cache.emplace(key, -1);
        if (! isNew) { ++cacheHits; return it->second; }
        //if (x == 0 && y == 19) printf("!");
        int result = Query_(x, y); it->second = result;
        //printf("  (%d, %d) -> %d\n", x, y, result);
        return result;
    }
    virtual int QueryDebug(int x, int y) = 0;
    void FoundTreasure(int x, int y)
    {
        Assert(0 <= x); Assert(x < n); Assert(0 <= y); Assert(y < n);
        // If we don't have to query a treasure to prove that we found it,
        // the next line can call QueryDebug instead.
        int qr = Query(x, y);
        if (qr >= 0) Assert(qr == qrTreasure);
    }
    int Query(TPoint A) { return Query(A.x, A.y); }
    int QueryDebug(TPoint A) { return QueryDebug(A.x, A.y); }
    void FoundTreasure(TPoint A) { FoundTreasure(A.x, A.y); }
};

#ifdef PUTKA
#include "treasurehuntlib.h"

struct TPutkaInterface : public TInterface
{
    virtual int Query_(int x, int y)
    {
        int qr = ::Query(x, y);
        if (qr == TREASURE) return qrTreasure;
        int qr2 = 0;
        if (qr & DIR_UP) qr2 |= qrUp;
        if (qr & DIR_DOWN) qr2 |= qrDown;
        if (qr & DIR_LEFT) qr2 |= qrLeft;
        if (qr & DIR_RIGHT) qr2 |= qrRight;
        return qr2;
    }

    virtual int QueryDebug(int x, int y) { return -1; }
};
#endif

struct TTestCase : public TInterface
{
    int tx[MaxK], ty[MaxK];
    int treasuresFound = 0, nQueries = 0;

    bool AnyDiag() const
    {
        for (int i = 1; i < k; ++i) for (int j = 0; j < i; ++j)
            if (abs(tx[i] - tx[j]) == abs(ty[i] - ty[j])) return true;
        return false;
    }

    int QueryEx(int x, int y, bool isDebug)
    {
        if (! isDebug) ++nQueries;
        int minDist = 2 * n;
        int result = 0;
        for (int i = 0; i < k; ++i) {
            int dx = tx[i] - x, dy = ty[i] - y;
            int dist = abs(dx) + abs(dy);
            if (dist == 0) {
                if (! isDebug) treasuresFound |= (1 << i);
                return qrTreasure; }
            if (dist < minDist) { minDist = dist; result = 0; }
            if (dist == minDist) {
                if (dx > 0) result |= qrRight;
                else if (dx < 0) result |= qrLeft;
                if (dy > 0) result |= qrDown;
                else if (dy < 0) result |= qrUp; }  }
        Assert(result != 0);
        return result;
    }

    virtual int QueryDebug(int x, int y) override { return QueryEx(x, y, true); }
    virtual int Query_(int x, int y) override { return QueryEx(x, y, false); } // y = 0 at the top, y = n - 1 at the bottom

    void Done()
    {
        Assert(treasuresFound + 1 == (1 << k));
    }
};

template<typename T>
struct TTwoParamCallSniffer
{
    typedef int yes_t;
    typedef struct { int a, b; } no_t;
    template<typename U> static yes_t Foo(decltype( (*((U *) nullptr))(0, 0) ) *);
    template<typename U> static no_t Foo(...);
    enum { has = (sizeof(Foo<T>(nullptr)) == sizeof(yes_t)) };
};

template<typename T, bool has>
struct TTwoParamCallDispatcher
{
    T &tester;
    bool operator()(int qr, int t) { return tester(qr, t); }
};

template<typename T>
struct TTwoParamCallDispatcher<T, false>
{
    T &tester;
    bool operator()(int qr, int t) { return tester(qr); }
};

// Returns the smallest t such that A + t * Dirs[dir] fails the test while A + (t - 1) * Dirs[dir] passes it.
// t may go from dL to dH.  If 'assumeLpasses' is false, then t = dL is also tried, otherwise the function assumes
// that A + dL * Dirs[dir] would pass the test and doesn't need to be tried.  If dH < 0, a value of dH is initialized
// so that the bisection extends to the edge of the grid.  If 'assumeHfails' is false, then t = dH + 1 may also be returned,
// otherwise the function assumes that A + dH * Dirs[dir] would fail the test and the highest possible return
// value will be dH.
//    The tester should support operator() with one or two parameters; the first is qr, the second is t.
template<typename TTester>
int Bisection(TInterface &interface, TPoint A, TDirection dir, int dL, int dH, bool assumeLpasses, bool assumeHfails, TTester &&tester_)
{
    TTwoParamCallDispatcher<TTester, TTwoParamCallSniffer<TTester>::has> tester {tester_};
    TPoint dirPt = Dirs[dir];
    if (! assumeLpasses) {
        int qr = interface.Query(A + dL * dirPt); if (qr == qrTreasure || ! tester(qr, dL)) return dL; }
    const int n = interface.n;
    int dx = DX[dir], dy = DY[dir];
    if (dH < 0) {
        dH = n - 1;
        if (dx < 0) dH = min(dH, A.x); else if (dx > 0) dH = min(dH, n - 1 - A.x);
        if (dy < 0) dH = min(dH, A.y); else if (dy > 0) dH = min(dH, n - 1 - A.y); }
    if (! assumeHfails) ++dH;
    while (dH - dL > 1) {
        // A + dL * dirPt passes the test, A + dH * dirPt fails it (or is just beyond the edge of the map).
        int dM = (dH + dL) / 2;
        int qr = interface.Query(A + dM * dirPt);
        if (qr == qrTreasure) return dM;
        if (tester(qr, dM)) dL = dM; else dH = dM; }
    return dH;
}

// Finds a treasure by starting in the middle of the grid and doing two bisections.
TPoint FindFirst3(TInterface &interface)
{
    const int n = interface.n;
    // Let A be any cell; we'll take the middle one so that the bisections can be shorter.
    TPoint A { n/2, n/2 }, B, C;
    int qr = interface.Query(A); if (qr == qrTreasure) return A;
    if ((qr & (qrLeft | qrRight)) == 0) B = A;
    else {
        // If A has a left (resp. right) arrow, do a bisection from A to the left (resp. right) edge
        // to find a cell with no left/right arrows.
        TDirection dir = (qr & qrLeft) ? dirLeft : dirRight; int qrDir = 1 << dir;
        int d = Bisection(interface, A, dir, 0, -1, true, true, [qrDir] (int qr) { return (qr & qrDir) != 0; });
        B = A + d * Dirs[dir]; }
    // B has no horizontal arrows; it is either a treasure or it has vertical arrows.
    qr = interface.Query(B); if (qr == qrTreasure) return B;
    Assert((qr & (qrLeft | qrRight)) == 0);
    Assert((qr & (qrUp | qrDown)) != 0);
    // If B has an up (resp. down) arrow, do a bisection from B to the top (resp. bottom) edge
    // to find a treasure.
    TDirection dir = (qr & qrUp) ? dirUp  : dirDown; int qrDir = 1 << dir;
    int d = Bisection(interface, B, dir, 0, -1, true, true, [qrDir] (int qr) { return (qr & (qrDir | qrLeft | qrRight)) == qrDir; });
    C = B + d * Dirs[dir];
    interface.FoundTreasure(C); return C;
}

TPoint FindFirstVid(TInterface &interface)
{
    const int n = interface.n;
    int x1 = 0, y1 = 0, x2 = n - 1, y2 = n - 1;
    printf("FindFirstVid, n = %d\n", n);
    while (x2 > x1 || y2 > y1)
    {
        int x = (x1 + x2) / 2, y = (y1 + y2) / 2;
        int qr = interface.Query(x, y);
        printf("%d..%d x %d..%d -> (%d, %d) -> qr = %d %s%s%s%s%s\n", x1, x2, y1, y2, x, y, qr,
            (qr & qrUp) ? "up " : "", (qr & qrDown) ? "down " : "", (qr & qrLeft) ? "left " : "", (qr & qrRight) ? "right " : "", (qr & qrTreasure) ? "treasure " : "");
        if (qr == qrTreasure) return {x, y};
        //int cx = x2 - x1 + 1, cy = y2 - y1 + 1;
        if ((qr & qrUp) && (qr & qrLeft)) {
            if (x2 == x && y2 == y) { if (x2 > x1) x2 = x - 1; else { Assert(y2 > y1); y2 = y - 1; } }
            else if (x2 == x) y2 = y; else if (y2 == y) x2 = x; else x2 = x, y2 = y; }
        else if ((qr & qrUp) && (qr & qrRight)) {
            if (x1 == x && y2 == y) { if (x2 > x1) x1 = x + 1; else { Assert(y2 > y1); y2 = y - 1; } }
            else if (x1 == x) y2 = y; else if (y2 == y) x1 = x; else x1 = x, y2 = y; }
        else if ((qr & qrDown) && (qr & qrLeft)) {
            if (x2 == x && y1 == y) { if (x2 > x1) x2 = x - 1; else { Assert(y2 > y1); y1 = y + 1; } }
            else if (x2 == x) y1 = y; else if (y1 == y) x2 = x; else x2 = x, y1 = y; }
        else if ((qr & qrDown) && (qr & qrRight)) {
            if (x1 == x && y1 == y) { if (x2 > x1) x1 = x + 1; else { Assert(y2 > y1); y1 = y + 1; } }
            else if (x1 == x) y1 = y; else if (y1 == y) x1 = x; else x1 = x, y1 = y; }
        else if (qr & qrUp) { Assert(y > y1); x1 = x; x2 = x; y2 = y - 1; }
        else if (qr & qrDown) { Assert(y < y2); x1 = x; x2 = x; y1 = y + 1; }
        else if (qr & qrLeft) { Assert(x > x1); y1 = y; y2 = y; x2 = x - 1; }
        else { Assert(qr & qrRight); Assert(x < x2); y1 = y; y2 = y; x1 = x + 1; }
        Assert(x1 <= x2); Assert(y1 <= y2);
    }
    Assert(x1 == x2); Assert(y1 == y2);
    interface.FoundTreasure(x1, y1); return {x1, y1};
}

struct TLineSegment
{
    TPoint A, B;
    bool isSet = false;
    bool Contains(TPoint C)
    {
        if (C.x < A.x && C.x < B.x) return false;
        if (C.x > A.x && C.x > B.x) return false;
        if (C.y < A.y && C.y < B.y) return false;
        if (C.y > A.y && C.y > B.y) return false;
        // Cy = Ay + (By - Ay) (Cx - Ax) / (Bx - Ax)
        // (Cy - Ay) (Bx - Ax) = (By - Ay) (Cx - Ax)
        return (C.y - A.y) * (B.x - A.x) == (B.y - A.y) * (C.x - A.x);
    }
};

// FindNextCheap and FindNextLineSeg work (in principle, but FindNextCheap was an absolute hell to get
// to work around all the edge cases), but in the end do more harm than good, bringing the worst-case
// performance of our algorithm from 12 log n queries to 13 log n queries in cases where the second and
// third treasure are in the same direction relative to A (the first treasure) and we find the closer of the
// two (say B) when going from A, and we later have to search from B in the other three directions to find
// C (the third treasure).

#if 1
enum { UseFindNextCheap = false };
void FindNextCheap(TInterface &interface, TPoint T, TDirection dir, TPoint &found1, TPoint &found2);
void FindNextLineSeg(TInterface &interface, TPoint T, TDirection dir, TPoint toAvoid, TPoint &found1, TPoint &found2, TLineSegment &foundSeg);
#else
// Removed to fit within Putka's 100 Kb limit on source size.
#endif

void FindNext3(TInterface &interface, TPoint T, TDirection dir, TPoint &found1, TPoint &found2, TLineSegment &foundSeg, bool skipB = false)
{
    //Assert(! skipB); // use FindNextCheap instead  // update: actually don't
    const int n = interface.n;
    // We haven't found anything yet.
    found1 = TPoint(-1, -1); found2 = TPoint(-1, -1); foundSeg.A = TPoint(-1, -1); foundSeg.B = TPoint(-1, -1); foundSeg.isSet = false;
    // We assume that T is a treasure.
    int qr = interface.QueryDebug(T); if (qr >= 0) Assert(qr == qrTreasure);
    int dx = DX[dir], dy = DY[dir]; TPoint dirPt = Dirs[dir];
    // How far can we go from T in the direction 'dir'?
    TDirection opDir = TDirection((dir + 2) % 4), perpDir = TDirection((dir + 1) % 4), perpDir2 = TDirection((dir + 3) % 4);;
    int qrDir = 1 << dir, qrOp = 1 << opDir, qrPerp = 1 << perpDir, qrPerp2 = 1 << perpDir2;
    int dMax = n - 1; if (dx < 0) dMax = min(dMax, T.x); else if (dx > 0) dMax = min(dMax, n - 1 - T.x);
    if (dy < 0) dMax = min(dMax, T.y); else if (dy > 0) dMax = min(dMax, n - 1 - T.y);
    if (dMax <= 0) return;
    // Examine T's neighbour in direction 'dir', A.
    TPoint A = T + dirPt; qr = interface.Query(A);
    if (qr == qrTreasure) { found1 = A; return; }
    Assert((qr & qrOp) == qrOp);
    // All arrows on A point to treasures in neighbouring cells.  One of these is T; there can be at most 2 others.
    Assert((qr & (qrDir | qrPerp | qrPerp2)) != (qrDir | qrPerp | qrPerp2));
    if (qr != qrOp)
    {
        for (int dir2 = 0; dir2 < 4; ++dir2) if (dir2 != opDir && ((qr & (1 << dir2)) != 0)) {
            TPoint TT = A + Dirs[dir2]; interface.FoundTreasure(TT);
            if (found1.x < 0) found1 = TT; else { Assert(found2.x < 0); found2 = TT; } }
        Assert(found1.x >= 0); // We must have found at least one new treasure.
        return;
    }
    //
    TPoint Z = T + dMax * dirPt; int qrZ = -1;
    bool skipDoubling = false;
    if (skipB)
    {
        // For the cheaper option where we assume that at most one treasure, rather than two,
        // is located in the direction 'dir' relative to T, we can now try the following:
        // examine the farthest cell in the direction 'dir', say Z.
        // - If it contains nothing but an arrow pointing back at B, we can proceed with
        //   doubling and we'll either conclude that everything between T and Z also contains
        //   nothing but arrows pointing back at T, so there is no treasure to be found (log n steps);
        //   or we'll find a cell pointing in the direction 'dir' and can then do
        //   a bisection between there and Z, thus finding a treasure in 2 log n steps.
        // - If Z contains only perpendicular arrows, we can use it as C and proceed as in
        //   the expensive version of the function, finding a treasure in log n steps.
        // - If Z contains an arrow pointing back at T as well as a perpendicular arrow,
        //   we can do two bisections and find the treasure in 2 log n steps.
        // Thus, using skipDoubling allows the cheaper version (skipB = true) to run in 2 log n
        // rather than 3 log n queries.  Unfortunately, as it turns out, the worst-case number
        // of queries for the whole treasure hunt is still 13 log n (2 (FindFirst) + 3 * 1 (FindNext from A that
        // finds nothing) + 4 (FindNext from A that finds B and a line segment for C) + 4 (FindNext from B
        // that finds A and a line segment for C) = 13; at no point did we do a FindNext with skipB = true).
        qrZ = interface.Query(Z);
        if (qrZ == qrTreasure) { found1 = Z; return; }
        if (qrZ != qrOp) skipDoubling = true;
    }
    // Now we know that A has only the arrow pointing back at T.  Find the closest point B
    // in direction 'dir' that does not have only the arrow pointing back at T.
    int dL = 1, dH; TPoint B;
    if (skipDoubling) dH = dMax;
    else while (dL < dMax)
    {
        // The points T + [1..dL] * dirPt have only the arrow pointing back at T.
        dH = min(2 * dL, dMax);
        B = T + dH * dirPt;
        qr = interface.Query(B); if (qr == qrTreasure) { found1 = B; return; }
        if (qr != qrOp) break;
        dL = dH;
    }
    if (dL >= dMax) return;
    // The points T + [1..dL] * dirPt have only the arrow pointing back at T,
    // but T + dH * dirPt does not.
    if (skipB)
    {
        // A shorter, simpler version of the function that saves one bisection but only finds
        // one treasure, and if it isn't the closest treasure in the direction 'dir' from T,
        // you get no information about a line segment containing the closest treasure.
        TPoint BB = T + dH * dirPt; int qrBB = interface.Query(BB);
        if (qrBB == qrTreasure) { found1 = BB; return; }
        TPoint CC, DD; int dC = 0, qrCC;
        if ((qrBB & (qrDir | qrOp)) == 0) dC = 0, CC = BB;
        else if (qrBB & qrDir) {
            dC = Bisection(interface, BB, dir, 0, -1, true, true, [qrDir] (int qr) { return (qr & qrDir) != 0; });
            CC = BB + dC * dirPt; }
        else { Assert(qrBB & qrOp);
            dC = Bisection(interface, T, dir, dL, dH, true, true, [qrOp, qrDir, qrPerp, qrPerp2] (int qr) { return (qr == qrOp) || (((qr & qrDir) == qrDir) && ((qr & (qrPerp | qrPerp2)) != 0)); });
            Assert(dC > dL);
            CC = T + dC * dirPt;
            qrCC = interface.Query(CC);
            Assert(qrCC != qrTreasure);
            if (qrCC & qrOp) {
                // From the fact that CC points back towards T as well as in some other direction(s), but not forward in the direction 'dir',
                // it follows that CC lies on the boundary between T's Voronoi cell and that of the nearest other treasure.
                // The other arrows on CC point directly to the next treasures,
                // which are at the same distance from CC as T is.
                Assert(dC == abs(CC.x - T.x) + abs(CC.y - T.y));
                if (qrCC & qrDir) { DD = CC + dC * dirPt; interface.FoundTreasure(DD); found1 = DD; return; }
                else if (qrCC & qrPerp) { DD = CC + dC * Dirs[perpDir]; interface.FoundTreasure(DD); found1 = DD; return; }
                else { Assert(qrCC & qrPerp2); DD = CC + dC * Dirs[perpDir2]; interface.FoundTreasure(DD); found1 = DD; return; }
            } else {
                Assert((qrCC & (qrPerp | qrPerp2)) == qrCC);
            }
        }
        //CC = BB + dC * dirPt;
        qrCC = interface.Query(CC); if (qrCC == qrTreasure) { found1 = CC; return; }
        Assert(skipDoubling); // If we did do doubling, that must be because Z pointed towards T, and in that
            // case we should already have found a treasure with the bisection between dL and dH.
        Assert((qrCC & (qrDir | qrOp)) == 0);
        int dD = 0;
        if (qrCC & qrPerp) {
            dD = Bisection(interface, CC, perpDir, 0, -1, true, true, [qrPerp, qrDir, qrOp] (int qr) { return (qr & (qrPerp | qrDir | qrOp)) == qrPerp; }); }
        else { Assert(qrCC & qrPerp2);
            dD = -Bisection(interface, CC, perpDir2, 0, -1, true, true, [qrPerp2, qrDir, qrOp] (int qr) { return (qr & (qrPerp2 | qrDir | qrOp)) == qrPerp2; }); }
        DD = CC + dD * Dirs[perpDir];
        interface.FoundTreasure(DD);
        found1 = DD; return;
    }
    // The points T + [1..dL] * dirPt have only the arrow pointing back at T,
    // but T + dH * dirPt does not.
    int d = Bisection(interface, T, dir, dL, dH, true, true, [qrOp] (int qr) { return qr == qrOp; });
    Assert(dL < d); Assert(d <= dH);
    B = T + d * dirPt;
    int qrB = interface.Query(B);
    if (qrB == qrTreasure) { found1 = B; return; }
    Assert(qrB != qrOp);
    if (qrB & qrOp) {
        // B still points back to T, and is at distance d from it; hence the other treasure(s)
        // pointed to by B are also at distance d from it.
    } else {
        // T, being at distance d from B, is not the closest treasure to it; hence the
        // treasures pointed to by B are at distance d - 1 from it.
        --d; }
    // If B does not contain an arrow pointing in 'dir', the new treasure(s) can only be directly
    // in the perpendicular directions from B.
    if ((qrB & qrDir) == 0)
    {
        Assert((qrB & (qrPerp | qrPerp2)) != 0);
        if (qrB & qrPerp) { found1 = B + d * Dirs[perpDir]; interface.FoundTreasure(found1); }
        if (qrB & qrPerp2) { auto &TT = (found1.x < 0) ? found1 : found2; TT = B + d * Dirs[perpDir2]; interface.FoundTreasure(TT); }
        Assert(found1.x >= 0); return;
    }
    // On the other hand, if B has no perpendicular arrows, then the next treasure can only be
    // directly in the direction 'dir'.
    bool bIsPerp = ((qrB & qrPerp) != 0), bIsPerp2 = ((qrB & qrPerp2) != 0);
    if (! (bIsPerp || bIsPerp2)) {
        found1 = B + d * dirPt; interface.FoundTreasure(found1); return; }
    // If B contains exactly one perpendicular arrow, we can determine the line segment that must contain a treasure.
    TLineSegment candSeg; if (bIsPerp && ~bIsPerp2) { candSeg.isSet = true; candSeg.A = B + d * Dirs[perpDir]; candSeg.B = B + (d - 1) * dirPt + Dirs[perpDir]; }
    else if (bIsPerp2 && ~bIsPerp) { candSeg.isSet = true; candSeg.A = B + d * Dirs[perpDir2]; candSeg.B = B + (d - 1) * dirPt + Dirs[perpDir2]; }
    else candSeg.isSet = false;
    // Now we would like to find a cell C = B + c * dirPt which points only in the perpendicular directions, or contains a treasure.
    // We know that such a cell must exist for some c from the range 1 <= c <= d,
    // but it might not be easy to find if the farthest of these points, B + d * dirPt,
    // contains a 'dir' arrow.  In that case we might have to extend our bisection range
    // all the way to the edge of the grid, where there certainly won't be a 'dir' arrow.
    int cMax = n - 1; if (dx < 0) cMax = min(cMax, B.x); else if (dx > 0) cMax = min(cMax, n - 1 - B.x);
    if (dy < 0) cMax = min(cMax, B.y); else if (dy > 0) cMax = min(cMax, n - 1 - B.y);
    int cL = 0, cH = min(cMax, d); TPoint C;
    if (cH == d && cH < cMax) {
        C = B + cH * dirPt;
        qr = interface.Query(C);
        if (qr == qrTreasure) { found1 = C; Assert(! (bIsPerp && bIsPerp2)); foundSeg = candSeg; return; }
        if (qr & qrDir) {
            // If B has arrows pointing to both perpendicular directions, this accounts for two treasures,
            // which together with A makes three.  Thus there can be no fourth treasure that could explain
            // the 'dir' arrow on B + cH * dirPt.
            Assert(! (bIsPerp && bIsPerp2));
            cH = cMax; } }
    qr = interface.QueryDebug(B + cH * dirPt); if (qr >= 0) Assert((qr & qrDir) == 0);
    // B contains an arrow in the direction 'dir' and in at least one of the perpendicular directions.
    // Find a cell C = B + c * dirPt which contains an arrow in a perpendicular direction but not in the direction 'dir'.
    int c = Bisection(interface, B, dir, cL, cH, true, true, [qrDir] (int qr) { return ((qr & qrDir) == qrDir); });
    C = B + c * dirPt;
    if (c > d) {
        // C is too far from B and certainly won't lead us to a new treasure at distance d from B (but to some more distant new treasure);
        // but we at least see which line segment that treasure must lie on.
        // But this now accounts for two new treasures, and there can't be a second new treasure at distance exactly d from B.
        Assert(! (bIsPerp && bIsPerp2)); foundSeg = candSeg; }
    int qrC = interface.Query(C);
    if (qrC == qrTreasure) {
        // If B had arrows in both perpendicular directions and in 'dir', the treasure at C, being directly in the
        // direction 'dir' from B, cannot explain the two perpendicular arrows at B; that would require two more
        // treasures, but then we'd have at least four treasures (including T), which is impossible.
        Assert(c >= d); Assert(! (bIsPerp && bIsPerp2));
        found1 = C; foundSeg = candSeg; return;  }
    Assert(qrC & (qrPerp | qrPerp2));
    for (int pass = 0; pass < 2; ++pass)
    {
        TDirection perpDirI = (pass == 0) ? perpDir : perpDir2;
        int qrPerpI = (pass == 0) ? qrPerp : qrPerp2;
        if ((qrC & qrPerpI) == 0) continue;
        // Now we're looking for a treasure D = C + p * Dir[perpDirI].  This treasure will be at distance p from C
        // and hence at distance c + p from B, so we know that c + p must surely be >= d; hence d - c is the lowest possible
        // value of p.
        int pL = max(0, d - c);
        int p = Bisection(interface, C, perpDirI, pL, -1, (pL == 0), true, [qrPerpI] (int qr) { return (qr & qrPerpI) == qrPerpI; });
        TPoint D = C + p * Dirs[perpDirI]; interface.FoundTreasure(D);
        if (found1.x < 0) found1 = D; else { Assert(found2.x < 0); found2 = D; }
        // We have found a treasure; if it's too far from B, we know there must be still another treasure on the line segment.
        if (c + p > d) { Assert(! (bIsPerp && bIsPerp2)); foundSeg = candSeg; }
        // Note that even if D is at distance exactly d from B, there could still be another new treasure on the
        // same line segment; but we have no easy way to find out otherwise than by running FindNextTreasure from D.
    }
    Assert(! (found1.x >= 0 && found2.x >= 0 && foundSeg.isSet)); // We can have found at most two new treasures.
}

// This function is meant to be used only in very specific circumstances after two treasures
// in a diagonal position have already been found.
// It either spends O(1) queries and finds out that there is no treasure in the rectangle,
// or it spends 2 log_2 n queries and finds the treasure.
bool FindInRectangle(TInterface &interface, int xInner, int yInner, int xOuter, int yOuter, TPoint &C)
{
    const int n = interface.n;
    Assert(xOuter == 0 || xOuter == n - 1); Assert(yOuter == 0 || yOuter == n - 1);
    TDirection dirInwardX = (xOuter == 0) ? dirRight : dirLeft, dirOutwardX = (xOuter == 0) ? dirLeft : dirRight;
    TDirection dirInwardY = (yOuter == 0) ? dirDown : dirUp, dirOutwardY = (yOuter == 0) ? dirUp : dirDown;
    int qrInwardX = 1 << int(dirInwardX), qrOutwardX = 1 << int(dirOutwardX), qrInwardY = 1 << int(dirInwardY), qrOutwardY = 1 << int(dirOutwardY);
    TPoint corner { xOuter, yOuter };
    int qr = interface.Query(corner);
    if (qr == qrTreasure) { C = corner; return true; }
    Assert((qr & qrOutwardX) == 0); Assert((qr & qrOutwardY) == 0);
    //
    TPoint U { xInner, yOuter };
    qr = interface.Query(U);
    if (qr == qrTreasure) { C = U; return true; }
    if ((qr & qrOutwardX) == 0) return false;
    /*
    if (qr & qrOutwardX)
    {
        int dHor = Bisection(interface, U, dirOutwardX, 0, -1, true, true, [qrOutwardX] (int qr) { return (qr & qrOutwardX) == qrOutwardX; });
        TPoint V { xInner + dHor * DX[dirOutwardX], yOuter };
        qr = interface.Query(V);
        if (qr == qrTreasure) { C = V; return true; }
        Assert(qr == qrInwardY);
        int dVer = Bisection(interface, V, dirInwardY, 0, abs(yInner - yOuter), true, false, [qrInwardY] (int qr) { return (qr & qrInwardY) == qrInwardY; });
        TPoint W { V.x, V.y + dVer * DY[dirInwardY] };
        interface.FoundTreasure(W); C = W; return true;
    }
    */
    //
    U = TPoint(xOuter, yInner);
    qr = interface.Query(U);
    if (qr == qrTreasure) { C = U; return true; }
    if (qr & qrOutwardY)
    {
        int dVer = Bisection(interface, U, dirOutwardY, 0, -1, true, true, [qrOutwardY] (int qr) { return (qr & qrOutwardY) == qrOutwardY; });
        TPoint V { xOuter, yInner + dVer * DY[dirOutwardY] };
        qr = interface.Query(V);
        if (qr == qrTreasure) { C = V; return true; }
        Assert(qr == qrInwardX);
        int dHor = Bisection(interface, V, dirInwardX, 0, abs(xInner - xOuter), true, false, [qrInwardX] (int qr) { return (qr & qrInwardX) == qrInwardX; });
        TPoint W { V.x + dHor * DX[dirInwardX], V.y };
        interface.FoundTreasure(W); C = W; return true;
    }
    //
    return false;
}

TPoint IntersectSegments(TInterface &interface, TLineSegment seg1, TLineSegment seg2)
{
    for (int i = 0; i < 2; ++i) {
        auto &S = (i == 0 ? seg1 : seg2);
        Assert(S.isSet);
        Assert(! (S.A == S.B)); // When FindNext3 sets a line segment, that's because it reached B, which is at least 2 steps from T,
            // so each line segment of the rhombus contains at least 3 tiles, or 2 if we ignore one of the endpoints;
            // hence we get a line segment of length at least 2.
        TPoint D = S.B - S.A; Assert(D.x != 0 && D.y != 0);
        Assert(abs(D.x) == abs(D.y)); }
    TPoint d1 = seg1.B - seg1.A, d2 = seg2.B - seg2.A;
    int slope1 = (d1.x == d1.y) ? 1 : -1, slope2 = (d2.x == d2.y) ? 1 : -1;
    if (slope1 == -1) { swap(d1, d2); swap(slope1, slope2); swap(seg1, seg2); }
    Assert(slope1 == 1); Assert(slope2 == -1);
    // A1.y + slope1 (x - A1.x) = A2.y + slope2 (x - A2.x)
    // A1.y - A2.y - slope1 A1.x + slope2 A2.x = slope2 x - slope1 x
    // x = (A1.y - A2.y - slope1 A1.x + slope2 A2.x) / (slope2 - slope1)
    int xNum = (seg1.A.y - seg2.A.y - slope1 * seg1.A.x + slope2 * seg2.A.x); Assert(xNum % 2 == 0);
    int x = xNum / (slope2 - slope1);
    int y = seg1.A.y + slope1 * (x - seg1.A.x), y2 = seg2.A.y + slope2 * (x - seg2.A.x); Assert(y2 == y);
    const int n = interface.n;
    Assert(0 <= x); Assert(x < n); Assert(0 <= y); Assert(y < n);
    TPoint C(x, y);
    Assert(seg1.Contains(C)); Assert(seg2.Contains(C));
    interface.FoundTreasure(C); return C;
}

void Solve4(TInterface &interface)
{
    const int k = interface.k, n = interface.n;
    if (k < 1) return;
    // Find the first treasure, A.
    TPoint A = FindFirst3(interface);
    vector<TPoint> v; v.emplace_back(A);
    auto AddIfNew = [&v, &interface] (TPoint T) {
        for (auto TT : v) if (TT == T) return;
        v.push_back(T); interface.FoundTreasure(T); };
    if (k == 1) return;
    // Search for other treasures in all four directions from A.
    vector<TLineSegment> segs;
    constexpr bool PostponeLineSeg = UseFindNextCheap;
    int dirsToSkip = 0, dirAtoB = -1;
    for (int dir = 0; dir < 4; ++dir)
    {
        if (dirsToSkip & (1 << dir)) continue;
        TPoint B, C; TLineSegment seg;
        if constexpr (PostponeLineSeg) { seg.isSet = false; FindNextCheap(interface, A, TDirection(dir), B, C); }
        else FindNext3(interface, A, TDirection(dir), B, C, seg);
        Assert(B.x < 0 || (B.x >= 0 && C.x < 0) || (B.x >= 0 && C.x >= 0 && ! seg.isSet));
        int oldSize = (int) v.size();
        if (B.x >= 0) AddIfNew(B);
        if (v.size() == 2 && oldSize == 1) {
            dirsToSkip |= (1 << dir); dirAtoB = dir;
            if (B.x >= 0 && abs(B.x - A.x) == abs(B.y - A.y)) {
                int oldDirsToSkip = dirsToSkip; dirsToSkip = 0;
                if (B.x > A.x) dirsToSkip |= qrRight; else if (B.x < A.x) dirsToSkip |= qrLeft;
                if (B.y > A.y) dirsToSkip |= qrDown; else if (B.y < A.y) dirsToSkip |= qrUp;
                Assert((dirsToSkip & oldDirsToSkip) == oldDirsToSkip); } }
        if (C.x >= 0) AddIfNew(C);
        if (seg.isSet) segs.push_back(seg);
        if (int(v.size()) >= k) return;
        if (v.size() >= 2 && segs.size() >= 1) break;
    }
    // If we reached here, there must surely be more than one treasure, and in
    // that case looking from A must surely have found at least one treasure;
    // and since we haven't exited yet, we can't have found all the treaures yet.
    // So the only option is that we have found 2 out of 3 treasures.  How to proceed
    // depends on whether we have found a line segment containing the third treasure or not.
    Assert(v.size() == 2); Assert(k == 3);
    if constexpr (PostponeLineSeg) Assert(segs.size() == 0);
    else Assert(segs.size() <= 1);
    TPoint B = v[1];
    // Try to obtain a line segment in the direction from A to B.
    if constexpr (PostponeLineSeg)
    {
        TPoint C1, C2; TLineSegment seg;
        FindNextLineSeg(interface, A, TDirection(dirAtoB), B, C1, C2, seg);
        if (C1.x >= 0) AddIfNew(C1);
        if (C2.x >= 0) AddIfNew(C2);
        if (int(v.size()) >= k) return;
        if (seg.isSet) {
            Assert(abs(seg.A.x - seg.B.x) == abs(seg.A.y - seg.B.y));
            segs.push_back(seg); }
    }
    //
    bool abDiag = abs(A.x - B.x) == abs(A.y - B.y);
    if (segs.size() == 1)
    {
        TPoint U = segs[0].A, V = segs[0].B;
        if (U == V) {
            // An edge case when the side of the rhomb was just 2 tiles long, one is B and one will be U = V = C.
            // .B..
            // C...
            // ....
            // .A..
            AddIfNew(U); Assert(v.size() == 3); return;
        }
        bool containsU[4], containsV[4], containsA[4];
        for (int pass = 0; pass < 3; ++pass)
        {
            TPoint T = (pass == 0) ? U : (pass == 1) ? V : A;
            int dx = T.x - B.x, dy = T.y - B.y;
            bool *dest = (pass == 0) ? containsU : (pass == 1) ? containsV : containsA;
            dest[dirRight] = (dx >= 0 && abs(dy) <= abs(dx));
            dest[dirLeft] = (dx <= 0 && abs(dy) <= abs(dx));
            dest[dirDown] = (dy >= 0 && abs(dy) >= abs(dx));
            dest[dirUp] = (dy <= 0 && abs(dy) >= abs(dx));
        }
        // Since we already have one line segment for C, we can use the extra-cheap version
        // of FindNext3 to get a second line segment in only 2 log n operations.
        // We should go in the direction that contains the whole UV; if there is no such direction,
        // we should go in the direction that contains A and one of U or V.
        // [Update: the above was true when we discovered B (and the line segment containing C)
        // using the expensive FindNext3, which guaranteed that in situations where we'd
        // have to go from B into the direction containing the other of U and V but not A
        // wouldn't have happened in the first place, because FindNext3 would have found C rather than B
        // when going from A.  Now, however, that we switched to FindNextCheap + FindLineSegment,
        // this can no longer be guaranteed.]
        for (int pass = 0; pass < 2; ++pass) for (int dir = 0; dir < 4; ++dir)
        {
            if (pass == 0 && ! (containsU[dir] && containsV[dir])) continue;
            //if (pass == 1 && ! (containsA[dir] && (containsU[dir] || containsV[dir]))) continue;  // when using FindNext3
            if (pass == 1 && ! (containsU[dir] || containsV[dir])) continue;  // when using FindNextCheap + FindLineSegment
            if (pass >= 2) break;
            if (pass == 0) pass = 2;
            Assert(segs.size() == 1);
            TLineSegment seg = segs[0];
            // In case some part of the line segment lies outside the grid, clip it.
            // The line segment maybe described as U + t D for t1 <= t <= t2.
            TPoint U = seg.A, V = seg.B; TPoint D { (V.x > U.x) ? 1 : -1, (V.y > U.y) ? 1 : -1 };
            int t1 = 0, t2 = abs(V.x - U.x); Assert(t2 == abs(V.y - U.y)); Assert(V == U + t2 * D);
            // 0 <= Ux + t Dx <= n-1
            // -Ux <= t Dx <= n-1 - Ux
            // If Dx = 1:  -Ux <= t Dx <= n-1 - Ux
            // If Dx = -1: Ux >= t >= -(n-1 - Ux), i.e. Ux + 1 - n <= n <= Ux.
            if (D.x == 1) t1 = max(t1, -U.x), t2 = min(t2, n - 1 - U.x);
            else t1 = max(t1, U.x + 1 - n), t2 = min(t2, U.x);
            if (D.y == 1) t1 = max(t1, -U.y), t2 = min(t2, n - 1 - U.y);
            else t1 = max(t1, U.y + 1 - n), t2 = min(t2, U.y);
            Assert(t1 <= t2); // The segment must lie at least partly in the grid, otherwise
                // there is no space for the treasure, but the treasure must exist or the
                // arrows that led us to discover the segment wouldn't be there.
            // The diagonals through B divide the grid into four triangle-like "quarters".
            // Consider the quarter in direction 'dir' from B.  We know that C must lie in it,
            // but if U and V are not both within that quarter, we can clip the segment and
            // keep only the parts within the quarter.
            //   Let S = Dirs[dir].
            //   Consider the point P = U + t D.  It lies in the quarter we're interested in if:
            //  S = ( 1, 0) --> |Py| <= |Px| and Px > 0, or |Py| <= Px
            //  S = (-1, 0) --> |Py| <= |Px| and Px < 0, or |Py| <= -Px
            //  S = (0,  1) --> |Py| >= |Px| and Py > 0, or |Px| <= Py
            //  S = (0, -1) --> |Py| >= |Px| and Py < 0, or |Px| <= -Py
            //   Let us consider first the case where S = (+/-1, 0).  We have the condition |Py| <= Px Sx and Px Sx > 0.
            // Hence |Uy + t Dy| <= Sx (Ux + t Dx) and Sx (Ux + t Dx) > 0.
            // Or  (Uy + t Dy) <= Sx (Ux + t Dx) and -(Uy + t Dy) <= Sx (Ux + t Dx) and Sx (Ux + t Dx) > 0.
            // Or  Uy + t Dy <= Sx Ux + t Sx Dx and -Uy - t Dy <= Sx Ux + t Sx Dx and Sx Ux + t Sx Dx > 0.
            // Or  Uy - Sx Ux  <= t (Sx Dx - Dy) and -Uy - Sx Ux <= t (Sx Dx + Dy) and Sx Ux + t Sx Dx > 0.
            // - Correction: of course P should be expressed relative to B for these conditions to make sense.
            // So instead of P = U + tD, the above formulas should work with P = U - B + tD.  It is enough
            // to replace U by U' := U - B.
            auto ClipT = [&t1, &t2] (int L, int R) { // L <= R t
                if (R == 0) { Assert(L <= 0); return; }
                if (R < 0) { // L/R >= t
                    L = -L; R = -R;  // Now R > 0.
                    if (L < 0) {
                        L = -L; // (-L)/R >= t,  with L and R both > 0
                        // Let L = q R + p; the the condition becomes -(qR +p)/R >= t, or -q - p/R >= t
                        t2 = min(t2, -(L/R) - (L % R == 0 ? 0 : 1)); }
                    else { // L/R >= t, with L >= 0 and R > 0
                        // Let L = q R + p; the condition becomes q + p/R >= t
                        t2 = min(t2, L / R); } }
                else { // L/R <= t with R > 0
                    if (L < 0) {
                        L = -L; // -L/R <= t, with L and R both > 0
                        // Hence -(qR + p)/R <= t, i.e. -q - p/R <= t.  Since t is an integer,
                        // this last condition is the same as -q <= t.
                        t1 = max(t1, -(L/R)); }
                    else {  // L/R <= t, with L >= 0 and R > 0
                        // Hence q + p/R <= t
                        t1 = max(t1, L/R + (L % R == 0 ? 0 : 1)); } }
                //Assert(t1 <= t2);
            };
            TPoint S = Dirs[dir], UB = U - B;
            if (S.y == 0) { ClipT(UB.y - S.x * UB.x, S.x * D.x - D.y); ClipT(-UB.y - S.x * UB.x, S.x * D.x + D.y); ClipT(1 - S.x * UB.x, S.x * D.x); }
            else { ClipT(UB.x - S.y * UB.y, S.y * D.y - D.x); ClipT(-UB.x - S.y * UB.y, S.y * D.y + D.x); ClipT(1 - S.y * UB.y, S.y * D.y); }
            if (t1 > t2) continue;
            V = U + t2 * D; U = U + t1 * D; Assert(0 <= U.x); Assert(U.x < n); Assert(0 <= U.y); Assert(U.y < n);
            for (int i = 0; i < 2; ++i) {
                TPoint P = (i == 0 ? U : V) - B;
                if (S.y == 0) { Assert(abs(P.y) <= abs(P.x)); Assert(P.x * S.x > 0); }
                else { Assert(abs(P.x) <= abs(P.y)); Assert(P.y * S.y > 0); } }
            // Let UU and VV be the projections of U and V onto the line going in direction 'dir' from B.
            // UU is the one farther from B, while VV is the one closer to B.
            int sL, sH; if (S.y == 0) sL = (U.x - B.x) * S.x, sH = (V.x - B.x) * S.x; else sL = (U.y - B.y) * S.y, sH = (V.y - B.y) * S.y;
            if (sH < sL) swap(sL, sH);
            Assert(sL > 0);
            TPoint UU = B + sH * S, VV = B + sL * S;
            int opDir = (dir + 2) % 4, perpDir = (dir + 1) % 4, perpDir2 = (dir + 3) % 4;
            int qrDir = 1 << dir, qrOp = 1 << opDir, qrPerp = 1 << perpDir, qrPerp2 = 1 << perpDir2;
            // Then if we move slowly from UU to VV, (a) the first few cells have arrows pointing towards B (dirOp) and perpendicular to it,
            // since C is the closest treasure to them.  (b) Then follows a cell with only a perpendicular arrow, pointing directly to C.
            // (c) After this there are cells with arrows in the direction 'dir' and perpendicular to it, still caused by C being the closest.
            // (d) Finally there are cells with only an arrow in the direction 'dirOp', pointing at B, which is now the closest treasure to that cell.
            //       (op+perp)*    perp        (dir+perp)*                  op*            (1)
            // But there can be a tile that is equally distant from B and C:
            //       (op+perp)*    perp        (dir+perp)*   (dir+perp+op)  op*            (2)
            // or    (op+perp)*    (perp+op)                                op*            (3)
            // The following bisection will find the first cell *after* the (op+perp) group.
            // In cases (1) and (2) that is exactly the pure-perp cell pointing at C.
            // But in case (3) what we found is the next cell.  That is probably an op, but if
            // the perp+op pointing at B and C was none other than VV, it is possible that
            // what we found might even be B itself:
            //       01234
            //     0 ..U.u        C = V   u = UU   v = VV
            //     1 ...Cv        VV is the cell we're looking for, but our bisection (from UU to VV) finds
            //     2 A...B        the first cell beyond the [UU, VV] range, which happens to be B.
            //     3 .....
            //     4 .....
            // Another edge case: the tile that would normally be a pure perp pointing at C might be
            // equally distant from C and A:
            //       01234
            //     0 .....
            //     1 .....
            //     2 A.uvB   u = UU,  v = VV        UU is the cell we're looking for (and our bisection finds it).
            //     3 ...V.
            //     4 ..C..   C = U
            // Well, this just means that we mustn't assume that the cell we're looking for won't also
            // contain e.g. an arrow in the direction 'dir', as in the above case (pointing towards A).
            /*
            // - Normally, while we're doing bisection to find the end of the initial (op+perp)* sequence,
            // we want those cells to not have a 'dir' arrow, so as not to confuse them with the (dir+perp+op) cell
            // that may appear later in (2).  However, the initial cell at UU may have a 'dir' arrow (in
            // addition to perp+op) pointing back at A, as in the following example:
            */
            // - Here's another edge case: the line U-V is not necessarily disjoint from UU-VV, and
            // the treasure C might hence actually lie *on* UU-VV.  This means that on UU-VV we have the sequence
            //      op*    (treasure C)  dir*   (dir+op)*   op*,   followed by the treasure B (not part of UU-VV).     (4)
            // An example of this:
            //       012345678
            //     0 ....U....
            //     1 ....uCvB.  u = UU, v = VV
            //     2 ......V..
            //     3 .........
            //     4 A........
            // In this case our initial bisection from UU to VV will immediately notice that UU (containing just an opDir arrow)
            // fails the test, so it will return UU in O(1) queries.  But we can easily find C as the intersection of
            // the line segments U-V and UU-VV.
            //    But the case where our bisection ends immediately because UU itself contains only an op arrow can also
            // happen when our current direction (from B) doesn't point towards C at all, so we should be prepared for
            // the possibility that U-V and UU-VV do not intersect after all.
            // - Here's another edge case:
            //       012345678
            //     0 ..A..u.vB    u = UU, v = VV
            //     1 .......C.    C = V
            //     2 A........    u..v = op+perp, op+perp, op+perp
            //     3 .....U...
            // In this example VV points directly at C, but it also points directly at B, which happens to be
            // at the same distance.
            int tMax = abs(UU.x - VV.x) + abs(UU.y - VV.y);
            int t = Bisection(interface, UU, (TDirection) opDir, 0, tMax, false, false, [qrOp, qrDir, qrPerp, qrPerp2] (int qr, int t) {
                return (qr & qrOp) != 0 && (qr & (qrPerp | qrPerp2)) != 0 && (t == 0 || (qr & qrDir) == 0); });
            TPoint WW = UU + Dirs[opDir] * t; int qrWW;
            if (t > tMax) {
                Assert(t == tMax + 1); Assert(t > 0);
                --t; WW = UU + Dirs[opDir] * t;
                qrWW = interface.Query(WW); Assert(qrWW & qrOp); Assert(qrWW & (qrPerp | qrPerp2)); }
            else {
                qrWW = interface.Query(WW);
                if (qrWW == qrTreasure) { AddIfNew(WW); Assert(v.size() == 3); return; }
                if ((qrWW & (qrPerp | qrPerp2)) == 0) {
                    Assert(qrWW == qrOp);
                    if (t == 0) { // Case (4) from above.
                        // The treasure C should lie at the intersection of U-V and UU-VV.
                        // Points on U-V are of the form (x, y) := U + c D.  Thus x = Ux + c Dx and y = Uy + c Dy; hence
                        // c = (x - Ux) / Dx = (y - Uy) / Dy.  The goal is to find a point that matches B in the x- or y-coordinate,
                        // depending on whether we have been moving in a vertical or horizontal direction.
                        int c =  (S.x == 0) ? (B.x - U.x) / D.x : (B.y - U.y) / D.y;
                        if (0 <= c && c <= max(abs(U.x - V.x), abs(U.y - V.y))) {
                            TPoint C = U + c * D;
                            int qrC = interface.Query(C);
                            if (qrC == qrTreasure) { AddIfNew(C); Assert(v.size() == 3); return; }
                        }
                            continue; }
                    Assert(t > 0);
                    --t; WW = UU + Dirs[opDir] * t;
                    qrWW = interface.Query(WW);
                    if (qrWW == qrTreasure) { AddIfNew(WW); Assert(v.size() == 3); return; }
                    Assert(qrWW & qrOp); Assert(qrWW & (qrPerp | qrPerp2));  } }
            /*
            if (qrWW == (qrPerp | qrOp) || qrWW == (qrPerp2 | qrOp)) {
                int qr = interface.QueryDebug(WW + Dirs[opDir]);
                if (qr >= 0) Assert(qr == qrOp); }
            Assert((qrWW & (perpDir | perpDir2)) == qrWW);
            */
            // Which point on the segment UU-VV matches WW in the coordinate we were moving along?  The segment
            // has points of the form (x, y) := U + c D.  Thus x = Ux + c Dx and y = Uy + c Dy; hence
            // c = (x - Ux) / Dx = (y - Uy) / Dy.
            int c = (S.y == 0) ? (WW.x - U.x) / D.x : (WW.y - U.y) / D.y;
            TPoint C = U + c * D;
            if (S.y == 0) Assert(C.x == WW.x); else Assert(C.y == WW.y);
            AddIfNew(C); Assert(v.size() == 3); return;
        }
        Assert(false);
    }
    Assert(segs.empty());
    Assert(v.size() == 2);
    // B is the closest treasure to A in the direction where we found it, so there's no              B....
    // point in going in the opposite direction.  However, if we found B in two directions           .....
    // because it lies diagonally from A, then it isn't necessarily the closest treasure to A        .TA..
    // in *both* directions, and we should still go in the opposite direction.                       .....
    // See the example on the right.  If we go up from A, we find B as the closest, but not if       .....
    // we go left from A.  The opposite directions are down and right; we should go down from B,
    // but need not go right from B.  -- QW In a case like the one here, when we go down from B,
    // can we be sure to find T instead of A (A being diagonal) when we use the more expensive version of FindNext3?  It seems to work that way.
    TPoint Zs[4];
    for (int pass = -1; pass < 3; ++pass)
    {
        // pass = -1: this is only used if B lies diagonally of A, and we have to check the two corner rectangles.
        // pass = 0: for each of the directions from B away from A, we check the farthest cell Z in that direction.
        //    If it doesn't point purely back towards B, we can be sure that C is in that quarter and not somewhere else,
        //    and we'll find it with a cheap FindNext there.
        // pass = 1: if we don't find C in pass 0, we have spent only O(1) queries to conclude that C must lie
        // on the line between C and Z in one of those three directions, if it lies in any of those directions at all;
        // we'll do bisections on all three in pass 2.
        //   The idea of using the Z's here to quickly check if the treasure lies in a certain direction (except maybe
        // on the direct line between Z and B) is an improvement suggested by Google Gemini and allows us to reduce
        // the worst-case performance from 12 log_2 n to 11 log_2 n queries.
        // pass = 2: we try the direction from B towards A.
        if (pass >= 0)
        for (int dir = 0; dir < 4; ++dir)
        {
            int opDir = (dir + 2) % 4; int qrOp = 1 << opDir;
            bool isDirTowardsA = (dirsToSkip & (1 << opDir)) != 0;
            //if (pass == 0 ? isDirTowardsA : ! isDirTowardsA) continue;
            if (pass <= 1 ? isDirTowardsA : ! isDirTowardsA) continue;
            //if (isDirTowardsA && ! abDiag) continue;
            if (abDiag && isDirTowardsA && (opDir != dirAtoB)) continue;
            //
            TPoint C1, C2; TLineSegment seg; bool findCalled = false;
            if (pass == 2) {
                Assert(isDirTowardsA);
                FindNext3(interface, B, TDirection(dir), C1, C2, seg);
                findCalled = true;
            } else if (pass == 0) {
                Assert(! isDirTowardsA);
                int d = n - 1; if (DX[dir] < 0) d = min(d, B.x); else if (DX[dir] > 0) d = min(d, n - 1 - B.x);
                if (DY[dir] < 0) d = min(d, B.y); else if (DY[dir] > 0) d = min(d, n - 1 - B.y);
                if (d <= 0) continue; // B must be at the edge of the grid.
                auto &Z = Zs[dir]; Z = B + d * Dirs[dir];
                int qrZ = interface.Query(Z); if (qrZ == qrTreasure) { AddIfNew(Z); Assert(int(v.size()) == 3); return; }
                if (qrZ == qrOp) continue; // There might be a treasure on the line between Z and B, but nowhere else in this quarter.
                // In the quarter defined by direction 'dir' relative to B, there is no cell closer to Z than B is,
                // and any cell outside that quarter is stricly farther from Z than B is.  Hence, since Z isn't pointing purely at B,
                // it is pointing towards some other treasure in this quarter.
                if constexpr (UseFindNextCheap) FindNextCheap(interface, B, TDirection(dir), C1, C2);
                else FindNext3(interface, B, TDirection(dir), C1, C2, seg, true);
                findCalled = true;
            } else {
                Assert(pass == 1); Assert(! isDirTowardsA);
                auto &Z = Zs[dir]; if (Z.x < 0) continue;
                int dMax = abs(Z - B), qrDir = 1 << dir;
                // If there is a treasure in this direction, it must lie directly on the line between B and Z.
                int d = Bisection(interface, B, TDirection(dir), 0, dMax, true, true, [qrOp, qrDir, dMax] (int qr, int t) { return (qr == qrDir || qr == (qrOp | qrDir)) && t != dMax; });
                TPoint C = B + d * Dirs[dir];
                int qrC = interface.Query(C); if (qrC == qrTreasure) { AddIfNew(C); Assert(v.size() == 3); return; }
            }
            if (findCalled) {
                Assert(! seg.isSet);
                if (C1.x >= 0) AddIfNew(C1);
                if (C2.x >= 0) AddIfNew(C2);
                //if (pass == 1) Assert(int(v.size()) == 3);
                if (int(v.size()) >= k) return;
            }
        }
        if (pass == -1 && abDiag)
        {
            // Update: actually, it makes sense to test the rectangles first, because if the treasure
            // isn't there we'll waste only O(1) queries to find this out.
            // Update 2: testing the rectangles first can lead to errors where FindInRectangle thinks it
            // has found something in the rectangle, but was actually confused by the arrows being
            // influenced by C outside the rectangle.  Here's an example:
            //     01234    We're in B and will eventually find C by going down.  However, if we test
            //   0 .A...    the right bottom rectangle first ('#' and 'u' cells), the 'u' corner
            //   1......    includes a down arrow due to C, which will eventually mislead FindInRectangle
            //   2 .....    into thinking there's a treasure in the rectangle.
            //   3 u#..B
            //   4 ##.C.
            // The solution to this is to first try the directions from B away from A (with FindNextCheap),
            // then we can do the rectangles and finally do the direction from B towards A (with the
            // more expensive FindNext).
            // Update 3: instead of checking the corners one at a time, we should check both:
            // note how we say below that if C is in the f/g area, the upper left corner points at it,
            // and if it's in the g/h area, the lower right corner points at it; but we're really
            // only interested in C's from the g area, which means that *both* corners must point at it.
            // In this way we avoid getting confused when C is outside the rectangle, e.g. to the right of B
            // in the pictures below; and this in turn means that we can check the rectangles first,
            // before calling FindNext from B.  Shifting the rectangle checks before B.FindNext
            // decreases the worst-case performance for the diagonal case from 12 log2 n to 11 log2 n.  Nice!
            // --
            // If we still didn't find the third treasure, the only remaining possibility is
            // that the first two, say A and B, lie on the same diagonal, causing us to bounce from one
            // to the other and back when using FindNext3:
            //          ........|########   The cells marked with + / - | # are equally far from A and B.
            //          ........|########   If we go right from A, we find B, and if we go up from B, we find A.
            //          ........|########   If C is in one of the '#' rectangles, we won't find it this way.
            //          ....A...+--------
            //          ......./.........
            //          ....../..........
            //          ...../...........
            //          ----+...B........
            //          ####|............
            //          ####|............
            // Let us consider one of the '#' rectangles more closely, say the upper right one.
            //          ........|ffff/ggggggg/hhhh    We have drawn two diagonal lines from A and B through the rectangle.
            //          ........|fff/ggggggg/hhhhh    If C is in the area 'f' or 'g', then the cell in the upper left
            //          ........|ff/ggggggg/hhhhhh    corner of the rectangle points at C rather than at A or B.
            //          ........|f/ggggggg/hhhhhhh    Similarly, if C is in the area 'g' or 'h', then the cell in the lower right
            //          ........|/ggggggg/hhhhhhhh    corner of the rectangle points at C rather than at A or B.
            //          ......../ggggggg/hhhhhhhhh    Thus, if we do something very much like FindFirst from the upper right
            //          ......./|gggggg/hhhhhhhhhh    corner, and limit ourselves to the rectangle, we will be in a good
            //          ....../.|ggggg/hhhhhhhhhhh    position to find C from at least one of these two direction (both if C is in the 'g' area).
            //          ...../..|gggg/hhhhhhhhhhhh
            //          ....A...+---/-------------
            //          ......./.../..............
            //          ....../.../...............
            //          ...../.../................
            //          ----+...B.................
            //          ####|.....................
            //          ####|.....................
            bool swapped = false;
            if (B.x < A.x) swap(A, B), swapped = true;
            int dx = B.x - A.x, dy = B.y - A.y;
            Assert(dx == dy || dx == -dy); Assert(dx > 0);
            TPoint C;
            if (dy == dx)
            {
                Assert(B.x > A.x); Assert(B.y > A.y);
                if (FindInRectangle(interface, B.x, A.y, n - 1, 0, C)) AddIfNew(C);
                else if (FindInRectangle(interface, A.x, B.y, 0, n - 1, C)) AddIfNew(C);
            }
            else
            {
                Assert(dy == -dx); Assert(B.x > A.x); Assert(B.y < A.y);
                if (FindInRectangle(interface, B.x, A.y, n - 1, n - 1, C)) AddIfNew(C);
                else if (FindInRectangle(interface, A.x, B.y, 0, 0, C)) AddIfNew(C);
            }
            //Assert(int(v.size()) == k); return;
            if (int(v.size()) == k) return;
            if (swapped) swap(A, B); // restore the old state
        }
    }
    Assert(int(v.size()) == k); return;
}

void Solve3(TInterface &interface)
{
    const int k = interface.k, n = interface.n;
    if (k < 1) return;
    // Find the first treasure, A.
    TPoint A = FindFirst3(interface);
    //TPoint A = FindFirstVid(interface);
    vector<TPoint> v; v.emplace_back(A);
    auto AddIfNew = [&v] (TPoint T) {
        for (auto TT : v) if (TT == T) return;
        v.push_back(T); };
    if (k == 1) return;
    // Search for other treasures in all four directions from A.
    vector<TLineSegment> segs;
    int dirsToSkip = 0;
    for (int dir = 0; dir < 4; ++dir)
    {
        if (dirsToSkip & (1 << dir)) continue;
        TPoint B, C; TLineSegment seg;
        FindNext3(interface, A, TDirection(dir), B, C, seg);
        int oldSize = (int) v.size();
        if (B.x >= 0) AddIfNew(B);
        if (v.size() == 2 && oldSize == 1) {
            dirsToSkip |= (1 << dir);
            if (B.x >= 0 && abs(B.x - A.x) == abs(B.y - A.y)) {
                int oldDirsToSkip = dirsToSkip; dirsToSkip = 0;
                if (B.x > A.x) dirsToSkip |= qrRight; else if (B.x < A.x) dirsToSkip |= qrLeft;
                if (B.y > A.y) dirsToSkip |= qrDown; else if (B.y < A.y) dirsToSkip |= qrUp;
                Assert((dirsToSkip & oldDirsToSkip) == oldDirsToSkip); } }
        if (C.x >= 0) AddIfNew(C);
        if (seg.isSet) segs.push_back(seg);
        if (int(v.size()) >= k) return;
    }
    // If there are any other treasures, we should have found at least some.
    // However, if we had found all of them, we'd have exited the function by now.
    Assert(v.size() == 2); Assert(k == 3);
    Assert(segs.size() <= 1);
    TPoint B = v[1]; //bool seenAfromB = false;
    for (int dir_ = 0; dir_ < 8; ++dir_)
    {
        int opDir = (dir_ + 2) % 4;
        bool isDirTowardsA = (dirsToSkip & (1 << opDir)) != 0;
        bool towardsAfirst = (segs.size() > 0);
        // if (dir_ < 4 ? isDirTowardsA  : ! isDirTowardsA) continue;
        if (dir_ < 4 ? isDirTowardsA != towardsAfirst : isDirTowardsA == towardsAfirst) continue;
        // If we went, say, right from A and found only B, but no trace of C, then there is no
        // point in going left from B; we will find nothing but A there.  If C were somewhere
        // left of B, we would already have found it in some other direction from A as well.
        if (isDirTowardsA && segs.size() == 0 && abs(B.x - A.x) != abs(B.y - A.y)) continue;
        //if (abs(B.x - A.x) == abs(B.y - A.y)) { if (dir_ >= 4 && seenAfromB) break; }
        int dir = dir_ % 4;
        /*if (segs.size() == 1 && ! isDirTowardsA) continue; // ne, to ni v redu */
        // If the current direction is the opposite of the one in which we got from A to B,
        // going from B in the current direction will likely just lead us to A again,
        // thus wasting about 4 log_2 n queries.  An exception to this is if going from A to B
        // also gave us a line segment containing C.  In this case going from B in the opposite
        // direction will either lead us to C, or it will lead us to A but give us a second line
        // segment with which we can then determine C; in either case we will discover C, which
        // makes it worth paying for the extra bisections.
        //if (dir == ((dirAtoB + 2) % 4) && segs.empty()) continue;
        //
        TPoint C1, C2; TLineSegment seg;
        FindNext3(interface, B, TDirection(dir), C1, C2, seg, ! isDirTowardsA);
        //if (C1 == A || C2 == A) seenAfromB = true;
        if (C1.x >= 0) AddIfNew(C1);
        if (C2.x >= 0) AddIfNew(C2);
        if (seg.isSet) segs.push_back(seg);
        if (int(v.size()) >= k) return;
        if (segs.size() >= 2) break; // this should be enough to determine the position of the third treasure
    }
    // If we reached this point without exiting, we have still not found
    // the third treasure.  But we must surely have found two line segments,
    // one from A and one from B, one in the direction / and one in the direction \,
    // such that the new treasure lies at their intersection.
    Assert(v.size() == 2); Assert(k == 3);
    Assert(segs.size() == 2 || segs.empty());
    if (! segs.empty())
    {
        TLineSegment seg1, seg2; TPoint C; bool cFound = false;
        for (int i = 0; i < (int) segs.size(); ++i) {
            auto &S = segs[i];
            Assert(! (S.A == S.B)); // When FindNext3 sets a line segment, that's because it reached B, which is at least 2 steps from T,
                // so each line segment of the rhombus contains at least 3 tiles, or 2 if we ignore one of the endpoints;
                // hence we get a line segment of length at least 2.
            //if (S.A == S.B) { C = S.A; cFound = true; break; }
            TPoint D = S.B - S.A; Assert(D.x != 0 && D.y != 0);
            if (D.x == D.y) seg1 = S;
            else if (D.x == -D.y) seg2 = S;
            else Assert(false); }
        if (! cFound)
        {
            Assert(seg1.isSet); Assert(seg2.isSet);
            TPoint d1 = seg1.B - seg1.A, d2 = seg2.B - seg2.A;
            int slope1 = (d1.x == d1.y) ? 1 : -1, slope2 = (d2.x == d2.y) ? 1 : -1;
            Assert(slope1 == 1); Assert(slope2 == -1);
            // A1.y + slope1 (x - A1.x) = A2.y + slope2 (x - A2.x)
            // A1.y - A2.y - slope1 A1.x + slope2 A2.x = slope2 x - slope1 x
            // x = (A1.y - A2.y - slope1 A1.x + slope2 A2.x) / (slope2 - slope1)
            int xNum = (seg1.A.y - seg2.A.y - slope1 * seg1.A.x + slope2 * seg2.A.x); Assert(xNum % 2 == 0);
            int x = xNum / (slope2 - slope1);
            int y = seg1.A.y + slope1 * (x - seg1.A.x), y2 = seg2.A.y + slope2 * (x - seg2.A.x); Assert(y2 == y);
            Assert(0 <= x); Assert(x < n); Assert(0 <= y); Assert(y < n);
            C = TPoint(x, y);
        }
        for (auto &S : segs) Assert(S.Contains(C));
        interface.FoundTreasure(C); AddIfNew(C);
    }
    else
    {
        // If we still didn't find the third treasure, the only remaining possibility is
        // that the first two, say A and B, lie on the same diagonal, causing us to bounce from one
        // to the other and back when using FindNext3:
        //          ........|########   The cells marked with + / - | # are equally far from A and B.
        //          ........|########   If we go right from A, we find B, and if we go up from B, we find A.
        //          ........|########   If C is in one of the '#' rectangles, we won't find it this way.
        //          ....A...+--------
        //          ......./.........
        //          ....../..........
        //          ...../...........
        //          ----+...B........
        //          ####|............
        //          ####|............
        // Let us consider one of the '#' rectangles more closely, say the upper right one.
        //          ........|ffff/ggggggg/hhhh    We have drawn two diagonal lines from A and B through the rectangle.
        //          ........|fff/ggggggg/hhhhh    If C is in the area 'f' or 'g', then the cell in the upper left
        //          ........|ff/ggggggg/hhhhhh    corner of the rectangle points at C rather than at A or B.
        //          ........|f/ggggggg/hhhhhhh    Similarly, if C is in the area 'g' or 'h', then the cell in the lower right
        //          ........|/ggggggg/hhhhhhhh    corner of the rectangle points at C rather than at A or B.
        //          ......../ggggggg/hhhhhhhhh    Thus, if we do something very much like FindFirst from the upper right
        //          ......./|gggggg/hhhhhhhhhh    corner, and limit ourselves to the rectangle, we will be in a good
        //          ....../.|ggggg/hhhhhhhhhhh    position to find C from at least one of these two direction (both if C is in the 'g' area).
        //          ...../..|gggg/hhhhhhhhhhhh
        //          ....A...+---/-------------
        //          ......./.../..............
        //          ....../.../...............
        //          ...../.../................
        //          ----+...B.................
        //          ####|.....................
        //          ####|.....................
        if (B.x < A.x) swap(A, B);
        int dx = B.x - A.x, dy = B.y - A.y;
        Assert(dx == dy || dx == -dy); Assert(dx > 0);
        TPoint C;
        if (dy == dx)
        {
            Assert(B.x > A.x); Assert(B.y > A.y);
            if (FindInRectangle(interface, B.x, A.y, n - 1, 0, C)) AddIfNew(C);
            else if (FindInRectangle(interface, A.x, B.y, 0, n - 1, C)) AddIfNew(C);
        }
        else
        {
            Assert(dy == -dx); Assert(B.x > A.x); Assert(B.y < A.y);
            if (FindInRectangle(interface, B.x, A.y, n - 1, n - 1, C)) AddIfNew(C);
            else if (FindInRectangle(interface, A.x, B.y, 0, 0, C)) AddIfNew(C);
        }
    }
    Assert(int(v.size()) == interface.k);
}

int TestK1()
{
    for (int n = 1; n <= 128; ++n)
    {
        int maxQueries = 0;
        for (int x = 0; x < n; ++x) for (int y = 0; y < n; ++y)
        {
            TTestCase tc; tc.n = n; tc.k = 1;
            tc.tx[0] = x; tc.ty[0] = y;
            //if (x == 1 && y == 2) printf("!");
            //Solve(tc);
            Solve3(tc);
            tc.Done();
            maxQueries = max(maxQueries, tc.nQueries);
        }
        fprintf(stderr, "k = 1, n = %d -> maxQueries = %d\n", n, maxQueries);
    }
    return 0;
}

void GenCase3(TTestCase &tc, mt19937_64 &rnd, int n)
{
    auto distr = uniform_int_distribution<long long>(0, ((long long) n) * n - 1);
    long long t1 = distr(rnd), t2, t3;
    do { t2 = distr(rnd); } while (t2 == t1);
    do { t3 = distr(rnd); } while (t3 == t1 || t3 == t2);
    tc = TTestCase();
    tc.n = n; tc.k = 3;
    tc.tx[0] = int(t1 % n); tc.ty[0] = int(t1 / n);
    tc.tx[1] = int(t2 % n); tc.ty[1] = int(t2 / n);
    tc.tx[2] = int(t3 % n); tc.ty[2] = int(t3 / n);
    //
    int rndDiag = uniform_int_distribution<int>(0, 99)(rnd);
    if (n <= 4) { }  // don't bother with diagonal cases when n is small, we'll find them randomly anyway
    else if (rndDiag <= 10) { // all 3 treasures on the same diagonal
        auto distr2 = uniform_int_distribution<int>(0, n - 1);
        int x1 = distr2(rnd), x2, x3;
        do { x2 = distr2(rnd); } while (x2 == x1);
        do { x3 = distr2(rnd); } while (x3 == x1 || x3 == x2);
        int xMin = min(x1, min(x2, x3)); x1 -= xMin; x2 -= xMin; x3 -= xMin;
        int xMax = max(x1, max(x2, x3));
        int x0 = uniform_int_distribution<int>(0, n - 1 - xMax)(rnd);
        int y0 = uniform_int_distribution<int>(0, n - 1 - xMax)(rnd);
        int y1 = x1 + y0, y2 = x2 + y0, y3 = x3 + y0;
        x1 += x0; x2 += x0; x3 += x0;
        if (uniform_int_distribution<int>(0, 1)(rnd) == 1) { y1 = n - 1 - y1; y2 = n - 1 - y2; y3 = n - 1 - y3; }
        tc.tx[0] = x1; tc.tx[1] = x2; tc.tx[2] = x3; tc.ty[0] = y1; tc.ty[1] = y2; tc.ty[2] = y3; }
    else if (rndDiag <= 50) { // 2 treasures on the same diagonal
        auto distr2 = uniform_int_distribution<int>(0, n - 1);
        int x1 = distr2(rnd), x2;
        do { x2 = distr2(rnd); } while (x2 == x1);
        int xMin = min(x1, x2); x1 -= xMin; x2 -= xMin;
        int xMax = max(x1, x2);
        int x0 = uniform_int_distribution<int>(0, n - 1 - xMax)(rnd);
        int y0 = uniform_int_distribution<int>(0, n - 1 - xMax)(rnd);
        int y1 = x1 + y0, y2 = x2 + y0;
        x1 += x0; x2 += x0;
        if (uniform_int_distribution<int>(0, 1)(rnd) == 1) { y1 = n - 1 - y1; y2 = n - 1 - y2; }
        tc.tx[0] = x1; tc.tx[1] = x2; tc.ty[0] = y1; tc.ty[1] = y2;
        do { t3 = distr(rnd); tc.tx[2] = int(t3 % n); tc.ty[2] = int(t3 / n); }
        while ((tc.tx[0] == tc.tx[2] && tc.ty[0] == tc.ty[2]) || (tc.tx[1] == tc.tx[2] && tc.ty[1] == tc.ty[2]));
    }
    //
    for (int i = 0; i < 3; ++i) { Assert(tc.tx[i] >= 0); Assert(tc.ty[i] >= 0); Assert(tc.tx[i] < n); Assert(tc.ty[i] < n);
        for (int j = 0; j < i; ++j) Assert(tc.tx[i] != tc.tx[j] || tc.ty[i] != tc.ty[j]); }
}

int GenBigRandom(int nMin, int nMax, int nToTry, int nToKeep, int subTaskNo, int caseNo)
{
    vector<TTestCase> tcD, tcND;
    mt19937_64 rnd(69000 + nMin * 17 + nMax);
    for (int i = 0; i < nToTry; ++i)
    {
        if (i % 1000 == 0) fprintf(stderr, "%d/%d    \r", i, nToTry);
        int n = uniform_int_distribution<int>(nMin, nMax)(rnd);
        TTestCase tc; GenCase3(tc, rnd, n);
        TTestCase tc_ = tc; Solve4(tc_);
        tc.nQueries = tc_.nQueries;
        (tc.AnyDiag() ? tcD : tcND).emplace_back(tc);
    }
    auto Cmp = [] (const TTestCase &x, const TTestCase &y) { return x.nQueries > y.nQueries; };
    sort(tcD.begin(), tcD.end(), Cmp);
    sort(tcND.begin(), tcND.end(), Cmp);
    vector<TTestCase> tcAll;
    for (int i = 0; i < nToKeep / 2 && i < int(tcD.size()); ++i) tcAll.push_back(tcD[i]);
    for (int i = 0; int(tcAll.size()) < nToKeep && i < int(tcND.size()); ++i) tcAll.push_back(tcND[i]);
    shuffle(tcAll.begin(), tcAll.end(), rnd);
    char buf[50]; sprintf(buf, "treasurehunt.%02d.%02d.in", subTaskNo, caseNo);
    FILE *f = fopen(buf, "wt");
    fprintf(f, "%d\n", (int) tcAll.size());
    for (auto &tc : tcAll)
    {
        fprintf(f, "%d %d", tc.n, tc.k);
        for (int i = 0; i < tc.k; ++i) fprintf(f, " %d %d", tc.tx[i], tc.ty[i]);
        fprintf(f, "\n");
    }
    fclose(f);
    return 0;
}

int TestK3()
{
    int prevMax = 0, prevN = 1;
    const bool tryAllN = false;
    for (int n = (tryAllN ? 2 : 100'000'000); ; )
    {
        int maxQueries = 0, maxQueriesD = 0, maxQueriesNonD = 0;
        // n = 5, seed = 18632786
        // n = 100M, seed = 1327817 (D), 99194164 (nd)
        long long seed = 0; // 470930; // 311258772 // 1986879 // 645409 // 28627 // 322608
        long long nTriesD = 0, nTriesNonD = 0;
        for (int giveUp = 0; giveUp < (tryAllN ? 1'000'0+00 : 1'000'000'000); ++seed, ++giveUp)
        {
            mt19937_64 rnd((seed * 200) ^ n);
            //int n = uniform_int_distribution<int>(1, 100)(rnd);
            TTestCase tc; GenCase3(tc, rnd, n);
            //if (tc.AnyDiag()) continue;
            //Solve(tc);
            //Solve2(tc);
            //Solve3(tc);
            Solve4(tc);
            tc.Done();
            bool isNew = false;
            if (tc.AnyDiag()) { ++nTriesD; if (tc.nQueries > maxQueriesD) maxQueriesD = tc.nQueries, isNew = true; }
            else { ++nTriesNonD; if (tc.nQueries > maxQueriesNonD) maxQueriesNonD = tc.nQueries, isNew = true; }
            if (giveUp % 10000 == 0) fprintf(stderr, "k = 3, n = %d -> maxQueries = %d  (%d diag, %d non-diag)  [%lld tries (%lld diag, %lld non-diag)]    \r", n, maxQueries, maxQueriesD, maxQueriesNonD, seed, nTriesD, nTriesNonD);
            if (tc.nQueries > maxQueries) { giveUp = 0; maxQueries = tc.nQueries; isNew = true; }
            if (true) if (isNew) if (maxQueries > 10.5 * log(n) / log(2))
                fprintf(stderr, "k = 3, n = %d (%d, %d) (%d, %d) (%d, %d) [%s] -> %d queries = %.6f log_2 n [seed %lld]     \n", n, tc.tx[0], tc.ty[0],
                    tc.tx[1], tc.ty[1], tc.tx[2], tc.ty[2], (tc.AnyDiag() ? "D" : "nd"), tc.nQueries, tc.nQueries / log(n) * log(2), seed);
            if (maxQueries < prevMax) giveUp = 0;
        }
        fprintf(stderr, "k = 3, n = %d -> maxQueries = %d  (%d diag, %d non-diag)  [%lld tries (%lld diag, %lld non-diag)]\n", n, maxQueries, maxQueriesD, maxQueriesNonD, seed, nTriesD, nTriesNonD);
        if (tryAllN) {
            FILE *f = fopen("treasurehun2-testk3-report.txt", "at");
            fprintf(f, "k = 3, n = %d -> maxQueries = %d (%d diag, %d non-diag) [%lld tries (%lld diag, %lld non-diag)]\n", n, maxQueries, maxQueriesD, maxQueriesNonD, seed, nTriesD, nTriesNonD);
            fclose(f); }
        prevN = n; prevMax = maxQueries;
        if (n < 100) n += 1;
        else if (n < 1000) n += 10;
        else if (n < 10000) n += 100;
        else if (n < 100'000) n += 1000;
        else if (n < 1'000'000) n += 10'000;
        else if (n < 10'000'000) n += 100'000;
        else n += 1'000'000;
    }
    return 0;
}

int TestK3All(int n)
{
    int maxQueries1 = 0, maxQueries2 = 0, maxQueries3 = 0, maxQueries2d = 0, maxQueries3d = 0;
    long long nCells = n * n;
    long long nConfigs = nCells + (nCells * (nCells - 1)) / 2 + (nCells * (nCells - 1) * (nCells - 2)) / 6, nDone = 0;
    vector<long long> distr; int maxQueriesAll = 0;
    constexpr bool showDistr = false;
    auto Add = [&distr, &maxQueriesAll] (const TTestCase &tc)
    {
        int q = tc.nQueries;
        while (int(distr.size()) <= q) distr.emplace_back(0);
        ++distr[q]; if (q > maxQueriesAll) {
            maxQueriesAll = q;
            fprintf(stderr, " n = %d, k = %d,", tc.n, tc.k);
            for (int i = 0; i < tc.k; ++i) fprintf(stderr, " (%d, %d)", tc.tx[i], tc.ty[i]);
            fprintf(stderr, " [%s] -> %d queries       \n", tc.AnyDiag() ? "D" : "nd", tc.nQueries); }
    };
    for (int t1 = 0; t1 < n * n; ++t1, ++nDone) {
        TTestCase tc; tc.n = n; tc.k = 1;
        tc.tx[0] = t1 % n; tc.ty[0] = t1 / n;
        Solve4(tc); tc.Done(); if constexpr (showDistr) Add(tc); maxQueries1 = max(maxQueries1, tc.nQueries); }
    for (int t1 = 0; t1 < n * n; ++t1) for (int t2 = 0; t2 < t1; ++t2, ++nDone) {
        TTestCase tc; tc.n = n; tc.k = 2;
        tc.tx[0] = t1 % n; tc.ty[0] = t1 / n;
        tc.tx[1] = t2 % n; tc.ty[1] = t2 / n;
        //if constexpr (! allowDiag) if (tc.AnyDiag()) continue;
        Solve4(tc); tc.Done(); if constexpr (showDistr) Add(tc);
        if (tc.AnyDiag()) maxQueries2d = max(maxQueries2d, tc.nQueries);
        else maxQueries2 = max(maxQueries2, tc.nQueries); }
    for (int t1 = 0; t1 < n * n; ++t1) for (int t2 = 0; t2 < t1; ++t2) for (int t3 = 0; t3 < t2; ++t3, ++nDone)
    {
        if (nDone % 100000 == 0) { fprintf(stderr, "k = 3, n = %d -> maxQueries = %d %d %d | %d %d      [%lld/%lld]        \r", n, maxQueries1, maxQueries2, maxQueries3, maxQueries2d, maxQueries3d, nDone, nConfigs); fflush(stderr); }
        TTestCase tc; tc.n = n; tc.k = 3;
        tc.tx[0] = t1 % n; tc.ty[0] = t1 / n;
        tc.tx[1] = t2 % n; tc.ty[1] = t2 / n;
        tc.tx[2] = t3 % n; tc.ty[2] = t3 / n;
        //if constexpr (! allowDiag) if (tc.AnyDiag()) continue;
        //Solve(tc);
        //Solve2(tc);
        //Solve3(tc);
        Solve4(tc);
        tc.Done(); if constexpr (showDistr) Add(tc);
        if (tc.AnyDiag()) maxQueries3d = max(maxQueries3d, tc.nQueries);
        else maxQueries3 = max(maxQueries3, tc.nQueries);
    }
    fprintf(stderr, "k = 3, n = %d -> maxQueries = %d %d %d | %d %d      [%lld/%lld]       \n", n, maxQueries1, maxQueries2, maxQueries3, maxQueries2d, maxQueries3d, nDone, nConfigs); fflush(stderr);
    if constexpr (showDistr) {
        for (int q = 0; q < (int) distr.size(); ++q) if (distr[q] > 0) fprintf(stderr, "   %3d queries : %15lld cases\n", q, distr[q]); }
    return 0;
}

int TestK3All()
{
    for (int n = 2; n <= 128; ++n) TestK3All(n);
    return 0;
}

#ifdef PUTKA

int main()
{
    while (true)
    {
        int n, k; NextHunt(n, k);
        if (n < 0) break;
        TPutkaInterface interface;
        interface.n = n; interface.k = k;
        Solve4(interface);
    }
    return 0;
}

#endif

#ifndef PUTKA

int TestK3All_Mt()
{
    int nThreads = 20;
    struct TThread {
        atomic<bool> done { false };
        int n;
        thread thr;
        void Run() { TestK3All(n); done.store(true); }
    };
    vector<TThread*> threads;
    int nextN = 1;
    while (true)
    {
        // See if any threads have finished.
        for (int i = 0; i < (int) threads.size(); ++i) if (threads[i]->done.load()) {
            threads[i]->thr.join();
            delete threads[i]; threads[i] = threads.back(); threads.pop_back(); }
        // Start new threads as needed.
        while (int(threads.size()) < nThreads)
        {
            TThread *T = new TThread();
            T->n = nextN; ++nextN;
            T->thr = thread(&TThread::Run, T);
            threads.emplace_back(T);
        }
        this_thread::sleep_for(1s);
    }
}

struct TDecTree
{
    typedef long long int myint; // int is ok up to 35
    int n;
    vector<myint> allStates;

    explicit TDecTree(int n_) : n(n_)
    {
        myint N = n * n; N *= (n * n - 1); N *= (n * n - 2); N /= 6;
        fprintf(stderr, "Starting n = %d -> %lld states...\n", n, (long long) N); fflush(stderr);
        allStates.resize(N);
        myint dest = 0;
        int n2 = n * n;
        for (int i = 2; i < n2; ++i) for (int j = 1; j < i; ++j) for (int k = 0; k < j; ++k)
        {
            myint state = i; state *= n2; state += j; state *= n2; state += k;
            allStates[dest++] = state;
        }
        Assert(dest == N);
    }

    int Query(myint state, int qx, int qy)
    {
        int result = 0, dMin = 2 * n + 1;
        for (int j = 0; j < 3; ++j)
        {
            int tx = state % n; state /= n;
            int ty = state % n; state /= n;
            int d = abs(tx - qx) + abs(ty - qy);
            if (d == 0) return 0;
            if (d < dMin) dMin = d, result = 0;
            if (d > dMin) continue;
            if (tx < qx) result |= qrLeft; else if (tx > qx) result |= qrRight;
            if (ty < qy) result |= qrUp; else if (ty > qy) result |= qrDown;
        }
        Assert(0 < result); Assert(result < 16);
        return result;
    }

    struct TResult {
        int maxDepth = 0; long long sumDepths = 0;
        void Add(const TResult &other) {
            if (other.maxDepth > maxDepth) maxDepth = other.maxDepth;
            sumDepths += other.sumDepths; } };

    TResult Recurse(int level, myint idxFrom, myint idxToPlus1)
    {
        myint nStates = idxToPlus1 - idxFrom; if (nStates <= 1) return TResult();
        int qxBest = -1, qyBest = -1; double bestScore = -1;
        myint counts[16];
        for (int qx = 0; qx < n; ++qx) for (int qy = 0; qy < n; ++qy) {
            for (int i = 0; i < 16; ++i) counts[i] = 0;
            for (myint i = idxFrom; i < idxToPlus1; ++i) ++counts[Query(allStates[i], qx, qy)];
            // H = -sum_i p_i log p_i = -sum_i (n_i/N) log (n_i/N)
            //   = -sum_i (n_i/N) log n_i + sum_i (n_i/N) log N
            //   = -(1/N) (sum_i n_i log n_i) + log N
            // Thus, to maximize H, we simply have to minimize sum_i n_i log n_i.
            double score = 0;
            for (int i = 0; i < 16; ++i) if (counts[i] > 1) score += counts[i] * log(counts[i]);
            if (qxBest < 0 || score < bestScore) qxBest = qx, qyBest = qy, bestScore = score; }
        //
        if (level <= 1) {
            myint N = idxToPlus1 - idxFrom;
            double H = log(N) - bestScore / N;
            fprintf(stderr, "- [n = %d] Level %d: q = (%d, %d) -> gain %.6f bits \n", n, level, qxBest, qyBest, H); fflush(stderr);
        }
        for (int i = 0; i < 16; ++i) counts[i] = 0;
        for (myint i = idxFrom; i < idxToPlus1; ++i) ++counts[Query(allStates[i], qxBest, qyBest)];
        int nNonzCounts = 0; for (int i = 0; i < 16; ++i) if (counts[i] > 0) ++nNonzCounts;
        Assert(nNonzCounts > 1);
        myint bucketStart[16], bucketEnd[16];
        for (int i = 0; i < 16; ++i) {
            bucketStart[i] = (i == 0) ? idxFrom : bucketEnd[i - 1] + counts[i - 1];
            bucketEnd[i] = bucketStart[i]; }
        Assert(bucketEnd[15] + counts[15] == idxToPlus1);
        for (int i = 0; i < 16; ++i) {
            auto &BE = bucketEnd[i];
            while (BE - bucketStart[i] < counts[i])
            {
                int r = Query(allStates[BE], qxBest, qyBest);
                if (r == i) ++BE;
                else {
                    Assert(r > i);
                    auto &BE2 = bucketEnd[r];
                    Assert(BE2 - bucketStart[r] < counts[r]);
                    swap(allStates[BE], allStates[BE2]);
                    ++BE2; }
            } }
        TResult result;
        if (level == 0)
        {
            struct TThread { TResult result; int i; thread thr; };
            vector<TThread> threads; threads.reserve(16);
            for (int i = 0; i < 16; ++i) {
                if (counts[i] <= 1) continue;
                threads.emplace_back();
                auto *T = &threads.back(); T->i = i;
                T->thr = thread([this] (TThread *T, int level, myint from, myint to) { T->result = Recurse(level, from, to); },
                    T, level + 1, bucketStart[i], bucketEnd[i]); }
            for (auto &T : threads) T.thr.join();
            fprintf(stderr, "[n = %d ->", n); for (auto &T : threads) {
                fprintf(stderr, " %d:%lld:%.3f", T.result.maxDepth, (long long) counts[T.i], T.result.sumDepths / double(counts[T.i])); result.Add(T.result); }
            result.sumDepths += nStates; result.maxDepth += 1;
            fprintf(stderr, " -> +1 = max %d, avg = %lld/%lld = %.3f]\n", result.maxDepth, result.sumDepths, (long long) nStates, result.sumDepths / double(nStates)); fflush(stderr);
        }
        else {
            for (int i = 0; i < 16; ++i) if (counts[i] > 1) result.Add(Recurse(level + 1, bucketStart[i], bucketEnd[i]));
            result.sumDepths += nStates; result.maxDepth += 1; }
        return result;
    }

    TResult Recurse() { return Recurse(0, 0, (myint) allStates.size()); }
    /*
    n             2  3  4  5  6  7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31  32  33  34  35
    Tree depth    4  6  6  7  8  9  10  11  12  12  13  14  14  15  15  16  16  17  17  17  18  18  19  19  19  20  20  21  21  22  22  22  22  22
    The query in the root node is ((n-1)/2, n/2).
    Note: the number of queries = tree depth - 1, because you don't need
    to do a query when you're in a leaf.
    */
};

int TestDecTree()
{
    for (int n = 2; ; ++n)
    {
        TDecTree tree(n);
        auto result = tree.Recurse();
        fprintf(stderr, "n = %d -> max queries = %d, avg = %lld / %lld = %.8f\n", n, result.maxDepth, result.sumDepths, (long long) tree.allStates.size(), result.sumDepths / double(tree.allStates.size()));
        fflush(stderr);
        FILE *f = fopen("treasurehunt-dectree-report.txt", "at");
        fprintf(f, "n = %d -> max queries = %d, avg = %lld / %lld = %.8f\n", n, result.maxDepth, result.sumDepths, (long long) tree.allStates.size(), result.sumDepths / double(tree.allStates.size()));
        fclose(f);
    }
}

int main()
{
    //return TestDecTree();
    //return TestK1();
    return TestK3();
    //return TestK3All();
    //return TestK3All_Mt();
    //return GenBigRandom(950'000, 1'000'000, 1'000'000, 100'000, 2, 1);
}

#endif
