#include <bits/stdc++.h>
using namespace std;

#define FOR(i,a,b) for (int i=(a);i<=(b);i++)
typedef array<int,3> III;

mt19937 rnd;

struct Node {
	int id, weight, size=1;
	int depth, min_depth, todo_depth=0;
	int memb, sum_memb;
	Node *left=NULL, *right=NULL, *up=NULL;
	Node() {}
	Node(int x, int d, int m) {
		id=x; weight=rnd();
		depth=d; min_depth=d;
		memb=m; sum_memb=m;
	}
};
typedef Node* NodePtr;
NodePtr root = NULL;

vector<Node> nodes;
int nodeCnt=0;

inline int size(NodePtr nd) { return nd ? nd->size : 0; }
inline int min_depth(NodePtr nd) { return nd ? nd->min_depth : INT_MAX; }
inline int sum_memb(NodePtr nd) { return nd ? nd->sum_memb : 0; }

inline void push_depth(NodePtr nd, int dif) {
	nd->depth += dif; nd->min_depth += dif; nd->todo_depth += dif;
}

inline void push(NodePtr nd) {
	if (!nd || nd->todo_depth==0) return;
	if (nd->left) push_depth(nd->left, nd->todo_depth);
	if (nd->right) push_depth(nd->right, nd->todo_depth);
	nd->todo_depth=0;
}

void pull(NodePtr nd) {
	if (!nd) return;
	push(nd->left); push(nd->right);
	nd->size = 1 + size(nd->left) + size(nd->right);
	nd->min_depth = min({nd->depth, min_depth(nd->left), min_depth(nd->right)});
	nd->sum_memb = nd->memb + sum_memb(nd->left) + sum_memb(nd->right);
}

void linkL(NodePtr nd, NodePtr ch) {
	nd->left = ch;
	if (ch) ch->up = nd;
	pull(nd);
}
void linkR(NodePtr nd, NodePtr ch) {
	nd->right = ch;
	if (ch) ch->up = nd;
	pull(nd);
}

// split of num nodes to left, remainder to right
pair<NodePtr, NodePtr> split(NodePtr nd, int num) {
	if (!nd) { return {NULL, NULL}; }
	push(nd);
	nd->up=NULL;
	if (size(nd->left) < num) {
		auto [left, right] = split(nd->right, num - size(nd->left) - 1);
		linkR(nd, left);
		return {nd, right};
	} else {
		auto [left, right] =  split(nd->left, num);
		linkL(nd, right);
		return {left, nd};
	}
}

NodePtr merge(NodePtr left, NodePtr right) {
	push(left); push(right);
	if (left == NULL) { return right; }
	if (right == NULL) { return left; }
	left->up=NULL; right->up=NULL;
	if (left->weight < right->weight) {
		linkR(left, merge(left->right, right));
		return left;
	} else {
		linkL(right, merge(left, right->left));
		return right;
	}
}

int order(NodePtr nd, NodePtr prev=NULL) {  // return position (1-based)
	int cnt=0;
	if (prev==NULL || nd->left!=prev) cnt+=1+size(nd->left);
	if (nd->up) cnt+=order(nd->up, nd);
	return cnt;
}

int get_depth(NodePtr nd) {
	if (nd->up!=NULL) get_depth(nd->up);
	push(nd);
	return nd->depth;
}

// rightmost node with min_depth <= k
NodePtr ancestor_search(NodePtr nd, int k) {
	push(nd);
	if (nd->right && nd->right->min_depth<=k) return ancestor_search(nd->right, k);
	else if (nd->depth<=k) return nd;
	else if (nd->left && nd->left->min_depth<=k) return ancestor_search(nd->left, k);
	else return NULL;
}

// k-tk ancestor of node
NodePtr ancestor(NodePtr nd, int k) {
	int d=get_depth(nd);
	auto [prefix, suffix] = split(root, order(nd));
	NodePtr ndA = ancestor_search(prefix, d-k);
	root = merge(prefix, suffix);
	return ndA;
}

#define N 1'000'000
#define Q 30'000

int n,q;
vector<int> ch[N+1];
int s[N+1], m[N+1], t[Q+1];
int M,r;

int first[4*N+1], last[4*N+1];
vector<III> tour;

void euler_tour(int root) {
    stack<array<int,3>> st; st.push({root, 0, 0});
    while (!st.empty()) {
        auto [x, d, i] = st.top(); st.pop();
        if (i == 0) {
            first[x] = tour.size(); tour.push_back({x, d, m[x]});
            st.push({x, d, 1});
            for (int y : ch[x]) st.push({y, d + 1, 0});
        } else {
            last[x] = tour.size(); tour.push_back({x, d, 0});
        }
    }
}

NodePtr firstNode[N+1], lastNode[N+1];

int prefix_memb(NodePtr nd, NodePtr prev=NULL) {
	int cnt=0;
	if (prev==NULL || nd->left!=prev) cnt+=nd->memb+sum_memb(nd->left);
	if (nd->up) cnt+=prefix_memb(nd->up, nd);
	return cnt;
}

int get_memb(int x) {
	return prefix_memb(lastNode[x])-prefix_memb(firstNode[x])+firstNode[x]->memb;
}

void EulerTourTreap() {
	euler_tour(r);
	nodes.resize(tour.size()); nodeCnt=0;
	for (auto [x,d,m] : tour) {
		nodes[nodeCnt] = Node(x,d,m);
		NodePtr nd = &nodes[nodeCnt++];
		if (!firstNode[x]) firstNode[x] = nd; else lastNode[x] = nd;
		root = merge(root, nd);
	}
}

NodePtr climb(NodePtr nd) {
	int d=get_depth(nd);
	for (int k=log2(d);k>=0;k--) {
		NodePtr a=ancestor(nd,1<<k);
		if (a!=NULL && get_memb(a->id)*2<M) { nd=a; }
	}
	if (get_memb(nd->id)*2<M) nd=ancestor(nd,1);
	return nd;
}

void update_center(NodePtr nd, int &center, int &sum_center) {
	int cnt;
	if (nd && (cnt=get_memb(nd->id))*2>=M && cnt<sum_center) { center=nd->id; sum_center=cnt; }
}

int update(int x, int z, int c) {
	NodePtr ndx = firstNode[x], ndz = firstNode[z], ndc = firstNode[c];
	NodePtr ndy = ancestor(ndx,1);
	int y = ndy->id;
	// move subtree
	int depth_dif=get_depth(firstNode[z])-get_depth(firstNode[x])+1;
	auto [left,other] = split(root, order(firstNode[x])-1);
	auto [mid,right] = split(other, order(lastNode[x]));
	push_depth(mid, depth_dif);
	other = merge(left, right);
	auto [front,back] = split(other, order(firstNode[z]));
	root = merge(merge(front,mid),back);
	// compute new solution
	ndy=climb(ndy);
	ndz=climb(ndz);
	int center=-1, sum_center=INT_MAX, cnt=0;
	update_center(ndc, center, sum_center);
	update_center(ndy, center, sum_center);
	update_center(ndz, center, sum_center);
	return center;
}

vector<int> ord;
void build_order(int root) {
    stack<int> st;
    st.push(root);
    while (!st.empty()) {
        int x = st.top(); st.pop();
        ord.push_back(x);
        for (int y : ch[x]) st.push(y);
    }
    reverse(ord.begin(), ord.end());
}

int sz[N+1];
void sizes(int x) {
	for (int x : ord) {
        sz[x] = m[x];
        for (int y : ch[x]) sz[x] += sz[y];
    }
}

int solve(int x) {
	for (int y : ch[x]) {
		if (2*sz[y]>=M) return solve(y);
	}
	return x;
}

int main() {
	//freopen("treasurer.04.15.in","r",stdin);
	//freopen("test.out","w",stdout);
	scanf("%d %d",&n,&q);
	FOR (i,1,n) {
		scanf("%d %d",&s[i],&m[i]);
		M+=m[i];
		if (s[i]==0) r=i;
		else ch[s[i]].push_back(i);
	}
	build_order(r);
	sizes(r);
	t[0]=solve(r);
	printf("%d\n",t[0]);
	EulerTourTreap();
	FOR (i,1,q) {
		int x,z;
		scanf("%d %d",&x,&z);
		x=1+(t[i-1]+x)%n; z=1+(t[i-1]+z)%n;
		t[i]=update(x,z,t[i-1]);
		printf("%d\n",t[i]);
	}
	return 0;
}
