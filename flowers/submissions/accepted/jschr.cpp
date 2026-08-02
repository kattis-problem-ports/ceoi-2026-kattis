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
