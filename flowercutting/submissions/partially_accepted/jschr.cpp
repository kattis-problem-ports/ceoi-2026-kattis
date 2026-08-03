// Expected score: 34 (groups 1 and 2 only).
//
// One of the organizers' solutions. It is WRONG, not merely slow: the loop below finds at
// most one maximal clique per flower, because it marks every member of a discovered clique
// visited (line `nodes[j].v=true`) and never revisits it. A maximal clique all of whose
// flowers were already claimed by other cliques is therefore never counted.
//
// The organizers' own data does not contain that configuration -- in every shipped case
// the maximal cliques are essentially disjoint -- so this solution was certified as fully
// accepted, and was shipped in submissions/accepted. data/gen_blocktree.py now builds
// gardens whose maximal cliques are glued together at shared flowers, which exposes it.
//
// Smallest witness (12 flowers, 18 connections: one K4 hub, each of whose four flowers
// carries its own triangle): exhaustive search over all 2^18 subsets gives 2, as do
// janez, koci and tomaz; this solution gives 0. Kept for provenance.
//
// It still earns group 1 (n <= 10 leaves no room for the structure: a hub needs a spoke on
// each of its four flowers, so 12 flowers minimum) and group 2 (a complete garden is a
// single clique, which it handles correctly).

#include<bits/stdc++.h>

using namespace std;

struct Node{
	vector<int> pov;
	bool c[1000];
	bool v;
};

Node nodes[1000];
bool bv[1000];

int nrem(int n){
	if(n<4) return 0;
	return n*(n-1)/2-(4+3*((n-4)/2)+2*(n%2));
}

int main(){
	int n, m, i, j, u, v, cs, ans=0;
	scanf("%d %d", &n, &m);
	for(i=0;i<m;++i){
		scanf("%d %d", &u, &v); --u; --v;
		nodes[u].c[v]=true;
		nodes[v].c[u]=true;
		nodes[u].pov.push_back(v);
		nodes[v].pov.push_back(u);
	}
	for(i=0;i<n;++i){
		if(nodes[i].v) continue;
		nodes[i].v=true;
		u=v=-1;
		for(int j:nodes[i].pov){
			for(int k:nodes[j].pov){
				if(nodes[i].c[k]){
					u=j;v=k;
					break;
				}
			}
			if(u!=-1) break;
		}
		if(u==-1) continue;
		cs=1;
		for(int j:nodes[i].pov){
			if(nodes[u].c[j] || j==u){
				++cs;
				nodes[j].v=true;
			}
		}
		ans+=nrem(cs);
	}
	printf("%d\n", ans);
	return 0;
}
