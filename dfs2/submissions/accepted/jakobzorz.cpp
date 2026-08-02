#include<bits/stdc++.h>

typedef long long ll;
typedef unsigned long long ull;
typedef long double ld;
using namespace std;
const int MOD=1e9+7;
// BEGIN inlined from algo/mod_int.h
const int PC=1e5;
ll M_inv[PC+1];
struct M{
    ll val;
    static void precomp(){M_inv[1]=1;for(ll i=2;i<=PC;++i){M_inv[i]=(MOD-MOD/i)*M_inv[MOD%i]%MOD;if(M_inv[i]<0)M_inv[i]+=MOD;}}
    static ll calc_inv(ll i){if(i<=PC)return M_inv[i];return(MOD-MOD/i)*calc_inv(MOD%i)%MOD;}
    M(ll x){if(M_inv[2]==0)precomp();val=(x%MOD+MOD)%MOD;}
    M(){if(M_inv[2]==0)precomp();val=0;}
template<class T>bool operator==(T b){return val==M(b).val;}
    template<class T>bool operator!=(T b){return val!=M(b).val;}
    template<class T>M operator+(T bt){M b(bt);M r(val);r.val+=b.val;if(r.val>=MOD)r.val-=MOD;return r;}
    template<class T>M operator-(T bt){M b(bt);M r(val);r.val+=MOD-b.val;if(r.val>=MOD)r.val-=MOD;return r;}
    template<class T>M operator*(T bt){M b(bt);M r(val);r.val*=b.val;r.val%=MOD;return r;}
    template<class T>M&operator+=(T bt){M b(bt);*this=*this+b;return*this;}
    template<class T>M&operator-=(T bt){M b(bt);*this=*this-b;return*this;}
    template<class T>M&operator*=(T bt){M b(bt);*this=*this*b;return*this;}
    M pow(ll e){M r(1);M a(val);while(e){if(e%2)r=r*a;a=a*a;e/=2;}return r;}
    template<class T>M operator/(T bt){ M b(bt); return M(val*calc_inv(b.val)); }
    template<class T>M&operator/=(T bt){M b(bt);*this*=calc_inv(b.val);return*this;}
};
namespace std{template<>struct hash<M>{
    inline size_t operator()(const M&x)const{return x.val;}
};}
bool operator<(M a,M b){return a.val<b.val;}
ostream&operator<<(ostream&s,M m){s<<m.val;return s;}
istream&operator>>(istream&s,M&m){s>>m.val;return s;}

// END inlined from algo/mod_int.h

pair<int,int>read(){
	string s1,s2,s;
	cin>>s;
	bool f=true;
	for(char c:s){
		if(c=='/')
			f=false;
		else if(f)
			s1.push_back(c);
		else
			s2.push_back(c);
	}
	return {stoi(s1),stoi(s2)};
}

int n;
vector<int>ch[200000];
int depth[200000];
int par[200000];
int el[200000],er[200000];
int ci=0;
ll res=0;
const int TREE_SIZE=1<<18;
int tree[2*TREE_SIZE];

void upd(int i,int x){
	i+=TREE_SIZE;
	tree[i]=x;
	while(i>1){
		i/=2;
		tree[i]=tree[2*i]+tree[2*i+1];
	}
}

int get(int node,int rl,int rr,int l,int r){
	if(rr<=l||r<=rl)
		return 0;
	if(l<=rl&&rr<=r)
		return tree[node];
	int mid=(rl+rr)/2;
	return get(2*node,rl,mid,l,r)+get(2*node+1,mid,rr,l,r);
}

void euler(int node){
	el[node]=ci++;
	for(int c:ch[node])
		euler(c);	
	er[node]=ci;
}

void solve(){
	n=read().second+1;	
	int cnode=n-1;
	for(int i=1;i<n;i++){
		auto[d,v]=read();
		//cout<<d<<" "<<v<<"\n";
		while(depth[cnode]+1!=d)
			cnode=par[cnode];
		ch[cnode].push_back(v);
		par[v]=cnode;
		cnode=v;
		depth[cnode]=d;
	}
	euler(n-1);
	for(int i=n-1;i>=0;i--){
		if(i!=n-1){
			res+=get(1,0,TREE_SIZE,el[i],er[i]);
		}
		upd(el[i],1);	
	}
	cout<<M(2).pow(res)<<"\n";
}

int main(){
	ios::sync_with_stdio(false);cout.tie(0);cin.tie(0);
	int t=1;//cin>>t;
	while(t--)solve();
	return 0;
}

/*
 */

