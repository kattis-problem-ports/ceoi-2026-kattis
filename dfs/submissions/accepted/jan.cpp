
//
// The `official' solution to the DFS problem.
//
// To find the number of n-vertex graphs producing the same DFS output if the
// DFS is started from vertex n, we begin with the DFS tree defined by the
// given DFS output and then determine how many edges can be added to it
// without changing the output. The number of graphs is then equal to the
// number of subsets of the set of safe extra edges, i.e., 2 to the number of
// those edges.
//
// Let's assume that we currently consider a vertex v != n in the DFS tree and
// want to determine which edges incident to v can be added to the DFS tree
// without changing the DFS output. First, note that if we added an edge
// between v and one of its non-ancestors (say, u), the DFS output would
// change, since the DFS would reach v immediately from u or vice versa. Now,
// let a be an non-immediate ancestor of v, and let b be a's child on the path
// from a to v. We claim that the edge (a, v) can be added if and only if v >
// b. To see this, assume that v > b and that the DFS has just visited a. What
// will it do next? Well, since we added the edge (a, v), it could, in
// principle, proceed to v instead of b and hence produce a different output.
// However, the fact that v > b prevents this, and the DFS reaches v via the
// same path as before. Conversely, if v had a smaller number than b, the edge
// (a, v) could not have been added, since the DFS would then visit v earlier
// than b.
//
// Considering the fact that the DFS is started from the highest-numbered
// vertex, the number of extra edges incident to v that can be added to the
// DFS tree is simply the number of ancestors of v (including the parent of v)
// that have a smaller number than v. Indeed: for each ancestor u < v, an edge
// can be added between v and u's ancestor.
//
// We maintain a stack holding the path from the root of the DFS tree to
// the current vertex and an augmented balanced BST tree holding the same
// vertices, which is used to determine, in O(log n) time per query, the
// number of the smaller-numbered ancestors of the current vertex. The overall
// time complexity of the algorithm is thus O(n log n).
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <stack>

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

using namespace std;
using namespace __gnu_pbds;

typedef tree<int, null_type, less<int>, rb_tree_tag, tree_order_statistics_node_update> Tree;

const int MODULO = 1'000'000'007;

//
// Returns (base to exponent) % modulo.
//
long powerMod(int base, long exponent, int modulo) {
    if (exponent == 0) {
        return 1;
    }
    long p = powerMod(base, exponent / 2, modulo) % MODULO;
    long p2 = p * p % MODULO;
    return (exponent % 2 == 0) ? (p2) : (base * p2 % MODULO);
}

int main() {
    // A stack to store the vertices on the path from the root of the DFS tree
    // to the current vertex.
    stack<int> pathStack;

    // An augmented balanced BST that holds the same vertices as pathStack and
    // is used to determine how many vertices on the path from the root of the
    // DFS tree to the current vertex have a smaller number than the current
    // vertex.
    Tree pathTree;

    // The maximum number of edges that we can add to the DFS tree and still
    // obtain the same DFS output.
    long nExtraEdges = 0;

    int previousLevel = -1;
    string line;

    while (getline(cin, line)) {
        int pos = line.find('/');
        int level = stoi(line.substr(0, pos));
        int vertex = stoi(line.substr(pos + 1));

        // Determine the parent of the current vertex. All the intervening
        // vertices are removed because they don't lie on the path from the
        // root of the DFS tree to the current vertex.
        while (previousLevel >= level) {
            int v = pathStack.top();
            pathTree.erase(v);
            pathStack.pop();
            previousLevel--;
        }

        pathStack.push(vertex);
        pathTree.insert(vertex);
        nExtraEdges += pathTree.order_of_key(vertex);
        previousLevel = level;
    }

    cout << powerMod(2, nExtraEdges, MODULO) << endl;

    return 0;
}
