#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <algorithm>
#include <vector>
using namespace std;

#include <cassert>
#include <cmath>
#define Assert assert

enum { MaxN = 1000, MaxM = 100'000 };

int main()
{
    int n, m;
    int ok_ = scanf("%d %d", &n, &m); Assert(ok_ == 2);
    Assert(1 <= n); Assert(n <= MaxN); Assert(1 <= m); Assert(m <= MaxM);
    vector<vector<int>> neigh(n);
    for (int i = 0; i < m; ++i)
    {
        int u, v; ok_ = scanf("%d %d", &u, &v); Assert(ok_ == 2);
        Assert(1 <= u); Assert(u <= n); Assert(1 <= v); Assert(v <= n);
        Assert(u != v);
        --u; --v; neigh[max(u, v)].emplace_back(min(u, v));
    }
    int dummy_; ok_ = scanf("%d", &dummy_); Assert(ok_ < 0);
    vector<vector<int>> cliques;
    vector<int> uNeigh(n, -1);
    for (int u = 0; u < n; ++u)
    {
        for (int v : neigh[u]) 
        { 
            Assert(0 <= v); Assert(v < u); 
            // Make sure that all input edges were distinct.
            Assert(uNeigh[v] < u); uNeigh[v] = u; 
            //printf("%d has neigh %d.\n", u, v);
        }
        // See which cliques u will join.
        for (int c = 0; c < (int) cliques.size(); ++c) 
        {
            auto &clique = cliques[c];
            int cNeigh = 0, cSize = (int) clique.size();
            // Is u connected to all nodes of this clique?
            for (int v : clique) if (uNeigh[v] == u) ++cNeigh;
            Assert(cNeigh <= 1 || cNeigh == cSize);
            if (cNeigh != cSize) continue;
            // If yes, u joins the clique and we can imagine that we have
            // processed the edges connecting u to this clique.
            //printf("%d joins clique %d\n", u, c);
            for (int v : clique) { Assert(uNeigh[v] == u); uNeigh[v] = -1; }
            clique.emplace_back(u);
        }
        // Any remaining edges incident on u form invididual 2-node cliques.
        for (int v : neigh[u]) {
            Assert(uNeigh[v] == u || uNeigh[v] == -1);
            if (uNeigh[v] != u) continue;
            cliques.push_back({u, v});
            //printf("%d:%d forms a new clique.\n", u, v);
        }
    }
    // Calculate how many edges need to be kept.
    int nEdgesNeeded = 0;
    for (auto &clique : cliques) {
        int s = (int) clique.size(); 
        //printf("{"); for (int u : clique) printf(" %d", u); printf(" } ");
        // A clique of size s can be regrown from (3s + s % 2) / 2 - 2 edges, but no fewer.
        nEdgesNeeded += (3 * s + s % 2) / 2 - 2; }
    printf("%d\n", m - nEdgesNeeded); 
    if (true)
    {
        int maxDeg = 0; for (auto &Nu : neigh) maxDeg = max(maxDeg, int(Nu.size()));
        int k = (int) cliques.size();
        fprintf(stderr, "n = %d verts, m = %d edges; maxDeg = %d; k = %d maximal cliques\nm / (n sqrt n) = %.6f; k / (n sqrt n) = %.6f  ", n, m, maxDeg, k, m / (n * sqrt(n)), k / (n * sqrt(n)));
        if (n <= 10 && m <= 20) fprintf(stderr, " [subtask 1]");
        if (maxDeg <= 8) fprintf(stderr, " [subtask 2]");
        if (2 * m == n * (n - 1)) fprintf(stderr, " [subtask 3]");
        fprintf(stderr, "\n");
    }
    return 0;
}