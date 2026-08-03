#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <queue>
#include <random>
#include <string>
#include <algorithm>
#include <cstdio>
#include "treasurehuntlib.h"

namespace {

using namespace std;

struct TTreasureHunt
{
    int n, k, nQueries = 0, nTreasuresFound = 0;
    vector<pair<int, int>> treasures;
    int treasuresFound; // a bitmask with the value from 0 to 2^k - 1

    // Initializes a small random test case.
    void Initialize(random_device &rnd)
    {
        n = uniform_int_distribution<int>(1, 20)(rnd);
        k = uniform_int_distribution<int>(1, (n == 1 ? 1 : 3))(rnd);
        nQueries = 0; treasures.clear(); treasuresFound = 0;
        auto distr = uniform_int_distribution<int>(0, n - 1);
        while (int(treasures.size()) < k)
        {
            pair<int, int> pr {distr(rnd), distr(rnd) };
            bool isNew = true; for (auto pr2 : treasures) if (pr2 == pr) { isNew = false; break; }
            if (isNew) treasures.push_back(pr); 
        }
    }
};

class TGrader
{
    vector<TTreasureHunt> hunts;
    int curHunt = -1, nHunts = -1;
    random_device rnd;

    typedef enum { stUninitialized = 0, stRunning = 1, stError = 2, stDone = 3 } Status;
    Status status = stUninitialized;
    // Reports an error and terminates the program.
    void Error(const char *errorMsg) {
        fprintf(stderr, "Error: %s\n", errorMsg); status = stError; exit(1); }
    void Error(const string &errorMsg) { Error(errorMsg.c_str()); }

    // Initializes a small set of random test cases.
    void Initialize()
    {
        nHunts = uniform_int_distribution<int>(5, 10)(rnd);
        curHunt = -1;
        hunts.clear(); hunts.resize(nHunts);
        for (auto &hunt : hunts) hunt.Initialize(rnd);
    }

    TGrader() { Initialize(); }
    // Reports an error if the user never called DoneSorting().
    ~TGrader() {
        if (status == stUninitialized) Error("NextHunt() has never been called.");
        if (status == stRunning) Error("NextHunt() has not been called to finish the last hunt."); }

public:

    // Provides access to a singleton instance of this class.
    static TGrader &GetInstance() {
        static TGrader instance; return instance; }

    void InitFromFile(const char *fileName)
    {
        FILE *f = fopen(fileName, "rt"); if (! f) Error("Error opening the input file."); 
        int ok = fscanf(f, "%d", &nHunts); if (ok != 1) Error("Error reading the number of hunts from the input file.");
        if (nHunts <= 0) Error("The number of hunts in the input file should be positive.");
        hunts.clear(); hunts.resize(nHunts);
        for (auto &hunt : hunts)
        {
            ok = fscanf(f, "%d %d", &hunt.n, &hunt.k); if (ok != 2) Error("Error reading the n and k from the input file.");
            if (hunt.k < 1 || hunt.k > 3) Error("Invalid k in the input file.");
            hunt.treasures.clear(); hunt.treasures.resize(hunt.k);
            for (auto &pr : hunt.treasures)
            {
                ok = fscanf(f, "%d %d", &pr.first, &pr.second); if (ok != 2) Error("Error reading treasure locations from the input file.");
                if (pr.first < 0 || pr.first >= hunt.n || pr.second < 0 || pr.second >= hunt.n) Error("Coordinates out of range in the input file.");
            }
            hunt.treasuresFound = 0; hunt.nTreasuresFound = 0; hunt.nQueries = 0;
        }
        fclose(f);
    }

    void NextHunt(int &n, int &k) 
    {
        if (status == stError) Error("NextHunt() has been called after an error.");
        if (status == stDone) Error("NextHunt() has been called after the end of the last hunt.");
        if (status == stRunning)
        {
            auto &hunt = hunts[curHunt];
            fprintf(stderr, "Hunt %d/%d (n = %d, k = %d): %d/%d treasures found in %d queries.\n", curHunt + 1, (int) hunts.size(), hunt.n, hunt.k,
                hunt.nTreasuresFound, hunt.k, hunt.nQueries);
        }
        if (curHunt + 1 >= (int) hunts.size()) { 
            fprintf(stderr, "All the treasure hunts have been processed.\n");
            n = -1; k = -1; status = stDone; return; }
        ++curHunt; auto &hunt = hunts[curHunt];
        hunt.nQueries = 0; hunt.nTreasuresFound = 0; hunt.treasuresFound = 0;
        n = hunt.n; k = hunt.k; status = stRunning; 
    }

    int Query(int qx, int qy)
    {
        if (status == stError) Error("Query() has been called after an error.");
        if (status == stDone) Error("Query() has been called after the end of the last hunt.");
        if (status == stUninitialized) Error("Query() has been called before the first call to NextHunt().");
        auto &hunt = hunts[curHunt];
        if (qx < 0 || qx >= hunt.n || qy < 0 || qy >= hunt.n) Error("Query() has been called with out-of-range coordinates.");
        if (hunt.nQueries >= 1000) Error("Too many queries.");
        int arrows = 0, minDist = -1; ++hunt.nQueries;
        for (int i = 0; i < hunt.k; ++i)
        {
            auto [tx, ty] = hunt.treasures[i];
            int d = abs(tx - qx) + abs(ty - qy);
            if (d == 0) {
                if ((hunt.treasuresFound & (1 << i)) == 0) {
                    hunt.treasuresFound |= (1 << i); ++hunt.nTreasuresFound; } 
                return TREASURE; }
            if (minDist < 0 || d < minDist) minDist = d, arrows = 0;
            if (d > minDist) continue;
            if (tx < qx) arrows |= DIR_LEFT; else if (tx > qx) arrows |= DIR_RIGHT;
            if (ty < qy) arrows |= DIR_UP; else if (ty > qy) arrows |= DIR_DOWN;
        }
        return arrows;
    }

};

}

void NextHunt(int &n, int &k) { TGrader::GetInstance().NextHunt(n, k); }
int Query(int x, int y) { return TGrader::GetInstance().Query(x, y); }

// This implementation of the library, unlike the one that will be used on the
// evaluation server, also supports the following function, which reads the 
// descriptions of one or more treasure hunts from a file and causes the library
// to use those instead of generating random ones.  The file should have the following contents:
// the first line should contain the number of the hunts, say H; then H lines should
// follow, each describing one hunt.  Each of this line should contain space-separated
// integers:  n  k  x[1] y[1] ... x[k] y[k],
// where n is the size of the grid, k is the number of treasures, and (x[i], y[i]) are
// the coordinates of the i'th treasure (in the range from 0 to n-1). 
void InitFromFile(const char *fileName) { TGrader::GetInstance().InitFromFile(fileName); }
// Sample contents of the input file:
/*
4
6 1 3 7
3 3 0 2 1 1 2 0
5 2 3 4 4 1
4 3 0 0 3 3 3 0
*/
// Sample usage:
/*
#include "treasurehuntlib.h"
extern void InitFromFile(const char *fileName);

int main()
{
    InitFromFile("myInputFile.txt");
    while (true)
    {
        int n, k; NextHunt(n, k); if (n < 0) break;
        for (int x = 0; x < n; ++x) for (int y = 0; y < n; ++y) Query(x, y);
    }
    return 0;
}
*/
