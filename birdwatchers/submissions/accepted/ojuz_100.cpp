// Downloaded from https://oj.uz/submission/1409753 (scored 100/100 on oj.uz).
// Scores 100 on this package.
#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

class FastInput {
    static constexpr int BUFSIZE = 1 << 20;

    char buffer[BUFSIZE];
    int position = 0;
    int length = 0;

public:
    inline char getChar() {
        if (position == length) {
            length = static_cast<int>(
                fread(buffer, 1, BUFSIZE, stdin)
            );
            position = 0;

            if (length == 0) {
                return 0;
            }
        }

        return buffer[position++];
    }

    int nextInt() {
        char c;

        do {
            c = getChar();
        } while (c <= ' ');

        int value = 0;

        do {
            value = value * 10 + (c - '0');
            c = getChar();
        } while (c > ' ');

        return value;
    }
};

class LinkCutTree {
    struct Node {
        int child[2] = {0, 0};

        // Auxiliary-tree parent or represented-tree path parent.
        int parent = 0;

        // Weight of this vertex.
        int64 value = 0;

        // Total weight connected through virtual edges.
        int64 virtualSum = 0;

        // Sum over this node's auxiliary splay subtree.
        int64 totalSum = 0;

        bool reversed = false;
    };

    vector<Node> tree;
    vector<int> stackBuffer;

    inline bool isAuxiliaryRoot(int x) const {
        int p = tree[x].parent;

        return p == 0 ||
               (tree[p].child[0] != x &&
                tree[p].child[1] != x);
    }

    inline void pull(int x) {
        int left = tree[x].child[0];
        int right = tree[x].child[1];

        tree[x].totalSum =
            tree[left].totalSum +
            tree[x].value +
            tree[x].virtualSum +
            tree[right].totalSum;
    }

    inline void applyReverse(int x) {
        if (x == 0) {
            return;
        }

        swap(tree[x].child[0], tree[x].child[1]);
        tree[x].reversed ^= 1;
    }

    inline void push(int x) {
        if (!tree[x].reversed) {
            return;
        }

        applyReverse(tree[x].child[0]);
        applyReverse(tree[x].child[1]);
        tree[x].reversed = false;
    }

    inline void rotate(int x) {
        int y = tree[x].parent;
        int z = tree[y].parent;

        int direction = (tree[y].child[1] == x);
        int middle = tree[x].child[direction ^ 1];

        if (!isAuxiliaryRoot(y)) {
            tree[z].child[tree[z].child[1] == y] = x;
        }

        tree[x].parent = z;

        tree[y].child[direction] = middle;
        if (middle != 0) {
            tree[middle].parent = y;
        }

        tree[x].child[direction ^ 1] = y;
        tree[y].parent = x;

        pull(y);
        pull(x);
    }

    void splay(int x) {
        int stackSize = 0;

        stackBuffer[stackSize++] = x;

        for (int y = x; !isAuxiliaryRoot(y); ) {
            y = tree[y].parent;
            stackBuffer[stackSize++] = y;
        }

        while (stackSize > 0) {
            push(stackBuffer[--stackSize]);
        }

        while (!isAuxiliaryRoot(x)) {
            int y = tree[x].parent;
            int z = tree[y].parent;

            if (!isAuxiliaryRoot(y)) {
                bool sameDirection =
                    ((tree[y].child[0] == x) ==
                     (tree[z].child[0] == y));

                rotate(sameDirection ? y : x);
            }

            rotate(x);
        }
    }

    void access(int x) {
        int last = 0;

        for (int y = x; y != 0; y = tree[y].parent) {
            splay(y);

            /*
             * The old preferred right child becomes virtual.
             * 'last' changes from virtual to preferred.
             */
            tree[y].virtualSum +=
                tree[tree[y].child[1]].totalSum;

            tree[y].virtualSum -=
                tree[last].totalSum;

            tree[y].child[1] = last;
            pull(y);

            last = y;
        }

        splay(x);
    }

    void makeRoot(int x) {
        access(x);
        applyReverse(x);
    }

    // x and y must be directly connected.
    void cutEdge(int x, int y) {
        makeRoot(x);
        access(y);

        tree[y].child[0] = 0;
        tree[x].parent = 0;

        pull(y);
    }

    // x and y must belong to different represented trees.
    void linkEdge(int x, int y) {
        makeRoot(x);
        access(y);

        tree[x].parent = y;
        tree[y].virtualSum += tree[x].totalSum;

        pull(y);
    }

    /*
     * Expose path a--b and find the first path vertex whose
     * inclusive prefix weight is at least half of wholeWeight.
     */
    int firstHalfVertex(
        int a,
        int b,
        int64 wholeWeight,
        bool& exactHalf
    ) {
        makeRoot(a);
        access(b);

        int x = b;
        int64 prefixBefore = 0;

        while (true) {
            push(x);

            int left = tree[x].child[0];

            int64 endOfLeft =
                prefixBefore + tree[left].totalSum;

            if (2 * endOfLeft >= wholeWeight) {
                x = left;
                continue;
            }

            int64 throughX =
                endOfLeft +
                tree[x].value +
                tree[x].virtualSum;

            if (2 * throughX >= wholeWeight) {
                exactHalf = (2 * throughX == wholeWeight);
                return x;
            }

            prefixBefore = throughX;
            x = tree[x].child[1];
        }
    }

    // x currently belongs to an exposed auxiliary path.
    int pathSuccessor(int x) {
        splay(x);

        int y = tree[x].child[1];
        push(y);

        while (tree[y].child[0] != 0) {
            y = tree[y].child[0];
            push(y);
        }

        return y;
    }

public:
    explicit LinkCutTree(int n)
        : tree(n + 1),
          stackBuffer(n + 1) {}

    /*
     * Initially every represented edge is virtual and every
     * auxiliary tree consists of one vertex.
     */
    void initializeNode(
        int vertex,
        int representedParent,
        int64 ownWeight,
        int64 initialSubtreeWeight
    ) {
        tree[vertex].parent = representedParent;
        tree[vertex].value = ownWeight;

        tree[vertex].virtualSum =
            initialSubtreeWeight - ownWeight;

        tree[vertex].totalSum =
            initialSubtreeWeight;
    }

    int64 rootedSubtreeWeight(int fixedRoot, int x) {
        makeRoot(fixedRoot);
        access(x);

        /*
         * The path toward the root is the left auxiliary subtree.
         * Everything below x is virtual.
         */
        return tree[x].value + tree[x].virtualSum;
    }

    void moveSubtree(
        int x,
        int oldParent,
        int newParent
    ) {
        cutEdge(x, oldParent);
        linkEdge(x, newParent);
    }

    int treasurerOnPath(
        int fixedRoot,
        int from,
        int to,
        int64 wholeWeight
    ) {
        bool exactHalf = false;

        int first = firstHalfVertex(
            from,
            to,
            wholeWeight,
            exactHalf
        );

        if (!exactHalf) {
            return first;
        }

        /*
         * The edge after 'first' divides the total weight equally.
         * Both endpoints are weighted centroids.
         */
        int second = pathSuccessor(first);

        /*
         * The deeper endpoint is the child with rooted subtree
         * weight exactly wholeWeight / 2.
         */
        int64 firstSubtree =
            rootedSubtreeWeight(fixedRoot, first);

        if (2 * firstSubtree == wholeWeight) {
            return first;
        }

        return second;
    }
};

int main() {
    FastInput input;

    int n = input.nextInt();
    int q = input.nextInt();

    vector<int> parent(n + 1);
    vector<int> firstChild(n + 1, -1);
    vector<int> nextSibling(n + 1, -1);
    vector<int64> ownWeight(n + 1);

    int root = 0;

    for (int i = 1; i <= n; ++i) {
        parent[i] = input.nextInt();
        ownWeight[i] = input.nextInt();

        if (parent[i] == 0) {
            root = i;
        } else {
            nextSibling[i] = firstChild[parent[i]];
            firstChild[parent[i]] = i;
        }
    }

    /*
     * Produce a parent-before-child ordering without recursion.
     */
    vector<int> order(n);
    int orderSize = 0;

    order[orderSize++] = root;

    for (int position = 0; position < orderSize; ++position) {
        int vertex = order[position];

        for (
            int child = firstChild[vertex];
            child != -1;
            child = nextSibling[child]
        ) {
            order[orderSize++] = child;
        }
    }

    vector<int64> subtreeWeight = ownWeight;

    for (int i = n - 1; i > 0; --i) {
        int vertex = order[i];

        subtreeWeight[parent[vertex]] +=
            subtreeWeight[vertex];
    }

    const int64 totalWeight = subtreeWeight[root];

    /*
     * Initially find the senior node with minimum subtree weight.
     */
    int treasurer = root;

    for (int vertex = 1; vertex <= n; ++vertex) {
        if (
            2 * subtreeWeight[vertex] >= totalWeight &&
            subtreeWeight[vertex] < subtreeWeight[treasurer]
        ) {
            treasurer = vertex;
        }
    }

    LinkCutTree lct(n);

    for (int vertex = 1; vertex <= n; ++vertex) {
        lct.initializeNode(
            vertex,
            parent[vertex],
            ownWeight[vertex],
            subtreeWeight[vertex]
        );
    }

    string output;
    output.reserve((q + 1) * 9);

    output += to_string(treasurer);
    output.push_back('\n');

    for (int query = 0; query < q; ++query) {
        int encodedX = input.nextInt();
        int encodedZ = input.nextInt();

        int x = (treasurer + encodedX) % n + 1;
        int z = (treasurer + encodedZ) % n + 1;

        int oldParent = parent[x];

        // The statement permits a move that changes nothing.
        if (oldParent != z) {
            int64 movedWeight =
                lct.rootedSubtreeWeight(root, x);

            bool containsTreasurer =
                (2 * movedWeight >= totalWeight);

            lct.moveSubtree(x, oldParent, z);
            parent[x] = z;

            if (!containsTreasurer) {
                treasurer = lct.treasurerOnPath(
                    root,
                    treasurer,
                    z,
                    totalWeight
                );
            }
        }

        output += to_string(treasurer);
        output.push_back('\n');
    }

    fwrite(output.data(), 1, output.size(), stdout);
    return 0;
}