// Downloaded from https://oj.uz/submission/1417135 (scored 50/100 on oj.uz).
// Scores 50 on this package.
// Every group is AC; the 50 comes from partial credit inside the groups.
// @EXPECTED_GRADES@ AC AC AC
#include<bits/stdc++.h>
typedef long long ll;
#define pb push_back
#define fr first
#define sc second
#define endl '\n'
using namespace std;
#define mid ((left+right)>>1)
#include "treasurehuntlib.h"

int n,k;
map<pair<int,int>,int>mp;
vector<pair<int,int>>var;

int query(int x,int y){
	if(mp.count({x,y}))return mp[{x,y}];
	mp[{x,y}]=Query(x,y);
	if(mp[{x,y}]==0){
		var.pb({x,y});
	}
	return mp[{x,y}];
}

signed main(){
	ios_base::sync_with_stdio(23^23);cin.tie(0);
	//InitFromFile("test.in");
	while(true){
		mp.clear();
		var.clear();
		NextHunt(n,k);
		if(n==-1)break;
		int l,r;
		int c,c2;
		l=0;r=n-1;
		while(l<=r){
			int mi=(l+r)/2;
			int x=query(mi,0);
			if(x&4)r=mi-1;
			else if(x&1)l=mi+1;
			else{
				l=r=mi;
				break;
			}
		}
		c=l;
		l=0;r=n-1;
		while(l<=r){
			int mi=(l+r)/2;
			int x=query(c,mi);
			c2=mi;
			if(x&(2|1|4))r=mi-1;
			else l=mi+1;
		}
		if(var.size()==k)continue;

		l=0;r=n-1;
		while(l<=r){
			int mi=(l+r)/2;
			int x=query(0,mi);
			if(x&8)l=mi+1;
			else if(x&2)r=mi-1;
			else{
				l=r=mi;
				break;
			}
		}
		c=l;
		l=0;r=n-1;
		while(l<=r){
			int mi=(l+r)/2;
			int x=query(mi,c);
			c2=mi;
			if(x&(4|2|8))r=mi-1;
			else l=mi+1;
		}
		if(var.size()==k)continue;

		l=0;r=n-1;
		while(l<=r){
			int mi=(l+r)/2;
			int x=query(mi,n-1);
			if(x&1)l=mi+1;
			else if(x&4)r=mi-1;
			else{
				l=r=mi;
				break;
			}
		}
		c=l;
		l=0;r=n-1;
		while(l<=r){
			int mi=(l+r)/2;
			int x=query(c,mi);
			c2=mi;
			if(x&(8|1|4))l=mi+1;
			else r=mi-1;
		}
		if(var.size()==k)continue;

		l=0;r=n-1;
		while(l<=r){
			int mi=(l+r)/2;
			int x=query(n-1,mi);
			if(x&2)r=mi-1;
			else if(x&8)l=mi+1;
			else{
				l=r=mi;
				break;
			}
		}
		c=l;
		l=0;r=n-1;
		while(l<=r){
			int mi=(l+r)/2;
			int x=query(mi,c);
			c2=mi;
			if(x&(1|2|8))l=mi+1;
			else r=mi-1;
		}
		if(var.size()==k)continue;



		pair<int,int>p=var[0];
		l=1;
		while(p.fr-l>=0){
			int x=query(p.fr-l,p.sc);
			if((x&4)||x==0){
				break;
			}
			l*=2;
		}
		if(p.fr-l>=0){
			r=p.fr-l;
			l=0;
			while(l<=r){
				int mi=(l+r)/2;
				int x=query(mi,p.sc);
				if(x&4)r=mi-1;
				else l=mi+1;
			}
		}
		if(var.size()==k)continue;

		l=1;
		while(p.fr+l<n){
			int x=query(p.fr+l,p.sc);
			if((x&1)||x==0){
				break;
			}
			l*=2;
		}
		if(p.fr+l<n){
			r=n-1;
			l=p.fr+l;
			while(l<=r){
				int mi=(l+r)/2;
				int x=query(mi,p.sc);
				if(x&1)l=mi+1;
				else r=mi-1;
			}
		}
		if(var.size()==k)continue;

		l=1;
		while(p.sc-l>=0){
			int x=query(p.fr,p.sc-l);
			if((x&2)||x==0){
				break;
			}
			l*=2;
		}
		if(p.sc-l>=0){
			r=p.sc-l;
			l=0;
			while(l<=r){
				int mi=(l+r)/2;
				int x=query(p.fr,mi);
				if(x&2)r=mi-1;
				else l=mi+1;
			}
		}
		if(var.size()==k)continue;

		l=1;
		while(p.sc+l<n){
			int x=query(p.fr,p.sc+l);
			if((x&8)||x==0){
				break;
			}
			l*=2;
		}
		if(p.sc+l<n){
			r=n-1;
			l=p.sc+l;
			while(l<=r){
				int mi=(l+r)/2;
				int x=query(p.fr,mi);
				if(x&8)l=mi+1;
				else r=mi-1;
			}
		}
		if(var.size()==k)continue;
	}
}