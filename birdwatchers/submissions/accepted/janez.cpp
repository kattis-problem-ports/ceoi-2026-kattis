// Note: despite the prominent mentions of a treap in the source code, it's actually a splay tree!
// Now updated with proper splaying.
#define _CRT_SECURE_NO_WARNINGS
#include <random>
#include <cstdio>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
typedef int TOrigNodeId; // ID of a node in the original tree (0..n-1)
typedef int TTreapNodeId; // ID of a node in the treap (0..2n-2)

struct TOrigNode
{
    TOrigNodeId parent = -1;
    TTreapNodeId firstTreapNode = -1, lastTreapNode = -1; int value = 1;
};

struct TTreapNode
{
    TTreapNodeId leftChild = -1, rightChild = -1, parent = -1; TOrigNodeId origNode;
    int depthDeltaForSubtree = -1, value = 0, subtreeValue = 0, mark = 0;
    int minSubtreeRelDepth = -1; TTreapNodeId minWhere = -1;
    TTreapNode() = default;
    TTreapNode(TOrigNodeId origNode_, int depthDeltaForSubtree_) : origNode(origNode_), depthDeltaForSubtree(depthDeltaForSubtree_) { }
    void ReplaceChild(TTreapNodeId oldChild, TTreapNodeId newChild) { if (leftChild == oldChild) leftChild = newChild; else rightChild = newChild; }
};

struct TTreap
{
    vector<TOrigNode> origNodes; TOrigNodeId origRoot = -1; int n = -1, totalValue = -1;
    vector<TTreapNode> treapNodes; TTreapNodeId treapRoot = -1; int mark = 0;
    TOrigNodeId centroid;
    TTreap(int n_) : origNodes(n_), n(n_) { }
    void Init();  // O(n); assumes that the 'parent' and 'value' members of 'origNodes' have already been initialized
    void SplitBefore(/*TTreapNodeId oldRoot,*/ TTreapNodeId splitBefore, TTreapNodeId &leftRoot, TTreapNodeId &rightRoot);
    TTreapNodeId Concatenate(TTreapNodeId leftRoot, TTreapNodeId rightRoot); // O(log n)
    TTreapNodeId PrevNode(TTreapNodeId u); // O(log n)
    TTreapNodeId NextNode(TTreapNodeId u); // O(log n)
    TTreapNodeId FarthestDescendant(TTreapNodeId u, bool left) {
        while (true) {
            auto &U = treapNodes[u]; TTreapNodeId v = (left) ? U.leftChild : U.rightChild;
            if (v < 0) return u; else u = v; } }
    void Rotate(TTreapNodeId u); // O(1)
    void Splay(TTreapNodeId u) {
		while (true) {
			TTreapNodeId p = treapNodes[u].parent; if (p < 0) return;
			auto &P = treapNodes[p]; if (TTreapNodeId g = P.parent; g >= 0)
			Rotate((treapNodes[g].leftChild == p) == (P.leftChild == u) ? p : u);
			Rotate(u); } }
    void DoubleSplay(TTreapNodeId u, TTreapNodeId v) { // ensures that v is the root and u is its child
		Splay(u); Splay(v); if (TTreapNodeId p = treapNodes[u].parent; p != v) Rotate(u); }
    void RecalcAggregates(TTreapNodeId u);
    void RecalcAggregatesRec(TTreapNodeId u) { while (u >= 0) { RecalcAggregates(u); u = treapNodes[u].parent; } }
    TTreapNodeId RangeMinimumQuery(TTreapNodeId u, TTreapNodeId v); // returns the node from [u, v] with the minimum origdepth; also works if v is to the left of u
    TOrigNodeId OrigLca(TOrigNodeId u, TOrigNodeId v) { return treapNodes[RangeMinimumQuery(origNodes[u].firstTreapNode, origNodes[v].firstTreapNode)].origNode; }
    bool OrigIsAncestorOf(TOrigNodeId candAncestor, TOrigNodeId candDescendant) { return OrigLca(candAncestor, candDescendant) == candAncestor; }
    int OrigSubtreeValue(TOrigNodeId x);
    TOrigNodeId LowestValuableAncestor(TOrigNodeId y, TOrigNodeId p);
    void MoveSubtree(TOrigNodeId x, TOrigNodeId z);
protected:
    TTreapNodeId InitSubtree(TTreapNodeId from, TTreapNodeId toExcl, TTreapNodeId parent, const vector<int> &origDepths) {
        if (toExcl <= from) return -1;
        TTreapNodeId u = (from + toExcl) / 2; auto &U = treapNodes[u]; U.parent = parent;
        U.depthDeltaForSubtree = origDepths[U.origNode] - (parent < 0 ? 0 : origDepths[treapNodes[parent].origNode]);
        U.leftChild = InitSubtree(from, u, u, origDepths);
        U.rightChild = InitSubtree(u + 1, toExcl, u, origDepths);
        RecalcAggregates(u); return u; }
};

TTreapNodeId TTreap::RangeMinimumQuery(TTreapNodeId u, TTreapNodeId v)
{
    if (u == v) return u;
    DoubleSplay(u, v); auto &U = treapNodes[u], &V = treapNodes[v];
    // Now VT is the root, UT is one of its children, and everything between them constitutes one of UT's subtrees.
    TTreapNodeId bestCand = v; int bestDepth = V.depthDeltaForSubtree;
    int uDepth = bestDepth + U.depthDeltaForSubtree; if (uDepth < bestDepth) bestCand = u, bestDepth = uDepth;
    TTreapNodeId w = (u == V.leftChild ? U.rightChild : U.leftChild); if (w >= 0) { auto &W = treapNodes[w];
        int wDepth = uDepth + W.depthDeltaForSubtree;
        int candDepth = wDepth + W.minSubtreeRelDepth; if (candDepth < bestDepth) bestDepth = candDepth, bestCand = W.minWhere; }
    return bestCand;
}

int TTreap::OrigSubtreeValue(TOrigNodeId x)
{
    auto &X = origNodes[x]; TTreapNodeId xl = X.firstTreapNode, xr = X.lastTreapNode;
    if (xl == xr) return treapNodes[xl].value;
    DoubleSplay(xl, xr);
    TTreapNode &XL = treapNodes[xl], &XR = treapNodes[xr];
    return XL.value + (XL.rightChild >= 0 ? treapNodes[XL.rightChild].subtreeValue : 0) + XR.value;
}

void TTreap::Rotate(TTreapNodeId u)
{
    TTreapNode &U = treapNodes[u]; TTreapNodeId p = U.parent; if (p < 0) return;
    TTreapNodeId a = U.leftChild, b = U.rightChild; TTreapNode &P = treapNodes[p];
    TTreapNodeId gp = P.parent; P.parent = u; U.parent = gp;
    if (gp < 0) treapRoot = u; else treapNodes[gp].ReplaceChild(p, u);
    TTreapNodeId transferredChild = (P.leftChild == u) ? b : a;
    if (P.leftChild == u) { U.rightChild = p; P.leftChild = transferredChild; }
    else { U.leftChild = p; P.rightChild = transferredChild; }
    // We need to make sure that the new deltas amount to the same depths as the old deltas.
    int duOld = U.depthDeltaForSubtree; U.depthDeltaForSubtree += P.depthDeltaForSubtree;
    P.depthDeltaForSubtree = -duOld;
    if (transferredChild >= 0) {
        auto &TC = treapNodes[transferredChild];
        TC.parent = p; TC.depthDeltaForSubtree += duOld; }
    RecalcAggregates(p); RecalcAggregates(u);
}

void TTreap::Init()
{
    centroid = -1; origRoot = -1; totalValue = 0;
    vector<TOrigNodeId> firstChild(n, -1), nextSib(n, -1);
    for (TOrigNodeId u = 0; u < n; ++u) { auto &U = origNodes[u]; totalValue += U.value;
        if (U.parent < 0) { origRoot = u; continue; }
        nextSib[u] = firstChild[U.parent]; firstChild[U.parent] = u; }
    if (n <= 0) { treapNodes.clear(); treapRoot = -1; return; }
    const int nTreapNodes = 2 * n - 1;
    vector<int> subtreeValue(n, 0); vector<bool> hasValuableChild(n, false);
    vector<int> origDepths(n, -1);
    treapNodes.clear(); treapNodes.reserve(2 * n - 1); treapRoot = -1;
    TOrigNodeId curNode = origRoot, prevNode = -1; int curDepth = 0;
    while (curNode >= 0)
    {
        TTreapNodeId treapNode = (TTreapNodeId) treapNodes.size();
        treapNodes.emplace_back(curNode, curDepth); origDepths[curNode] = curDepth;
        TOrigNode &CurNode = origNodes[curNode];
        TTreapNode &TreapNode = treapNodes[treapNode];
        // For each orignode, we remember its leftmost and rightmost occurrence in the treap.
        // The leftmost occurrence gets the value of the orignode, all subsequent occurrences get the value 0.
        if (CurNode.firstTreapNode < 0) { CurNode.firstTreapNode = treapNode; TreapNode.value = CurNode.value; }
        else TreapNode.value = 0;
        CurNode.lastTreapNode = treapNode;
        // Continue on our traversal of the original tree.
        // If we entered the current node from its parent, continue into its first child.
        if (prevNode == CurNode.parent) {
            subtreeValue[curNode] += CurNode.value;
            if (firstChild[curNode] >= 0) { prevNode = curNode; curNode = firstChild[curNode]; ++curDepth; }
            // But if it has no first child, return to its parent.
            else { prevNode = curNode; curNode = CurNode.parent; --curDepth; } }
        // If we entered the current node from a child, continue into its next child.
        else {
            subtreeValue[curNode] += subtreeValue[prevNode];
            if (2 * subtreeValue[prevNode] >= totalValue) hasValuableChild[curNode] = true;
            if (nextSib[prevNode] >= 0) { TOrigNodeId nextNode = nextSib[prevNode]; prevNode = curNode; curNode = nextNode; ++curDepth; }
            // But if it has no next child, return to the current node's parent.
            else { prevNode = curNode; curNode = CurNode.parent; --curDepth; } }
    }
    // Now we can calculate the initial centroid.
    for (TOrigNodeId u = 0; u < n; ++u) if (2 * subtreeValue[u] >= totalValue && ! hasValuableChild[u]) centroid = u;
    treapRoot = InitSubtree(0, nTreapNodes, -1, origDepths); // Build a balanced tree.
}

void TTreap::RecalcAggregates(TTreapNodeId u)
{
    TTreapNode &U = treapNodes[u];
    U.minSubtreeRelDepth = 0; U.minWhere = u; U.subtreeValue = U.value;
    for (int i = 0; i < 2; ++i) {
        TTreapNodeId v = (i == 0) ? U.leftChild : U.rightChild; if (v < 0) continue;
        TTreapNode &V = treapNodes[v]; U.subtreeValue += V.subtreeValue;
        int cand = V.minSubtreeRelDepth + V.depthDeltaForSubtree;
        if (cand < U.minSubtreeRelDepth) U.minSubtreeRelDepth = cand, U.minWhere = V.minWhere; }
}

void TTreap::SplitBefore(/*TTreapNodeId oldRoot,*/ TTreapNodeId splitBefore, TTreapNodeId &leftRoot, TTreapNodeId &rightRoot)
{
    Splay(splitBefore); rightRoot = splitBefore;
    auto &U = treapNodes[splitBefore]; leftRoot = U.leftChild; U.leftChild = -1;
    if (leftRoot >= 0) {
        auto &L = treapNodes[leftRoot]; L.parent = -1; L.depthDeltaForSubtree += U.depthDeltaForSubtree;
        RecalcAggregates(rightRoot); }
}

TTreapNodeId TTreap::Concatenate(TTreapNodeId leftRoot, TTreapNodeId rightRoot)
{
    if (leftRoot < 0) return rightRoot; else if (rightRoot < 0) return leftRoot;
    TTreapNodeId newRoot = rightRoot;
    while (true) {
        TTreapNodeId u = treapNodes[newRoot].leftChild;
        if (u < 0) break; else newRoot = u; }
    Splay(newRoot); auto &U = treapNodes[newRoot];
    U.leftChild = leftRoot;
    auto &L = treapNodes[leftRoot]; L.parent = newRoot; L.depthDeltaForSubtree -= U.depthDeltaForSubtree;
    RecalcAggregates(newRoot); return newRoot;

}

TTreapNodeId TTreap::NextNode(TTreapNodeId u)
{
    // If u has a right child, return the leftmost descendant of that child.
    TTreapNodeId v = treapNodes[u].rightChild;
    if (v >= 0) return FarthestDescendant(v, true);
    // Otherwise examine u's ancestors until we find one where u lies in its left subtree.
    while (true) {
        TTreapNodeId p = treapNodes[u].parent;
        if (p < 0) return -1;
        auto &P = treapNodes[p];
        if (P.leftChild == u) return p;
        u = p; }
}

TTreapNodeId TTreap::PrevNode(TTreapNodeId u)
{
    // If u has a left child, return the rightmost descendant of that child.
    TTreapNodeId v = treapNodes[u].leftChild;
    if (v >= 0) return FarthestDescendant(v, false);
    // Otherwise examine u's ancestors until we find one where u lies in its right subtree.
    while (true) {
        TTreapNodeId p = treapNodes[u].parent; if (p < 0) return -1;
        auto &P = treapNodes[p]; if (P.rightChild == u) return p;
        u = p; }
}

TOrigNodeId TTreap::LowestValuableAncestor(TOrigNodeId y, TOrigNodeId p)
{
    if (OrigSubtreeValue(y) * 2 >= totalValue) return y;
    TTreapNodeId yt = origNodes[y].lastTreapNode, pt = origNodes[p].lastTreapNode;
    // We assume that p is an ancestor of y; hence pt is sure to lie to the right of yt in the treap.
    TTreapNodeId ul = yt, ur = pt; TOrigNodeId result = p;
    while (true)
    {
        // Find the shortest favourable range [yt, t].  t = ul is too short, t = ur is long enough.
        DoubleSplay(ul, ur);
        TTreapNodeId um = treapNodes[ul].rightChild;
        if (um < 0) return result; // If there are no nodes between ul and ur, then [yt, ur] is the shortest favourable range.
        TOrigNodeId w = treapNodes[RangeMinimumQuery(yt, um)].origNode;
        if (2 * OrigSubtreeValue(w) >= totalValue) ur = um, result = w; else ul = um;
    }
}

void TTreap::MoveSubtree(TOrigNodeId x, TOrigNodeId z)
{
    auto &X = origNodes[x]; TOrigNodeId y = X.parent;
    auto &Y = origNodes[y], &Z = origNodes[z];
    if (y == z) return; // important; much of our splay tree code later assumes that y and z are two distinct nodes
    TTreapNodeId yt = Y.firstTreapNode, zt = Z.firstTreapNode;
    TOrigNodeId p = OrigLca(y, z);

    DoubleSplay(zt, yt); int depthChange = treapNodes[zt].depthDeltaForSubtree;
    // Move the subtree in 'origTree'.
    X.parent = z;
    // Now move the appropriate range of nodes in the treap.
    TTreapNodeId aRoot = -1, bRoot = -1, cRoot = -1, bcRoot = -1;
    TTreapNodeId bStart = PrevNode(X.firstTreapNode), cStart = NextNode(X.lastTreapNode);
    TTreapNode &BS = treapNodes[bStart], &CS = treapNodes[cStart];
    SplitBefore(/*treap.root,*/ bStart, aRoot, bcRoot); SplitBefore(/*bcRoot,*/ cStart, bRoot, cRoot);
    /*TTreapNodeId acRoot = */ Concatenate(aRoot, cRoot);
    if (Y.firstTreapNode == bStart) {
        Y.firstTreapNode = cStart; CS.value = Y.value;
        RecalcAggregatesRec(cStart); }
    TTreapNodeId eStart = Z.firstTreapNode, dRoot = -1, eRoot = -1;
    SplitBefore(/*acRoot,*/ eStart, dRoot, eRoot);
    BS.origNode = z; BS.value = Z.value; Z.firstTreapNode = bStart; RecalcAggregatesRec(bStart);
    treapNodes[bRoot].depthDeltaForSubtree += depthChange;
    treapNodes[eStart].value = 0; RecalcAggregatesRec(eStart);
    treapRoot = Concatenate(dRoot, Concatenate(bRoot, eRoot));
    // Now update the centroid.
    if (OrigIsAncestorOf(p, centroid)) {
        if (OrigIsAncestorOf(centroid, y)) centroid = LowestValuableAncestor(y, p);
        if (OrigIsAncestorOf(centroid, z)) centroid = LowestValuableAncestor(z, p); }
}

int main()
{
    int n, q; scanf("%d %d", &n, &q); TTreap treap(n);
    for (auto &U : treap.origNodes) { scanf("%d %d", &U.parent, &U.value); --U.parent; }
    treap.Init(); printf("%d\n", 1 + treap.centroid);
    for (int j = 0; j < q; ++j) {
        int x, z; scanf("%d %d", &x, &z);
        x = (x + treap.centroid + 1) % n; z = (z + treap.centroid + 1) % n;
        treap.MoveSubtree(x, z); printf("%d\n", 1 + treap.centroid); }
    return 0;
}
