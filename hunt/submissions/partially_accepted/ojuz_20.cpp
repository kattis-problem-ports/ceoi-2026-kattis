// Downloaded from https://oj.uz/submission/1410941 (scored 20/100 on oj.uz).
// Scores 20 on this package.
// @EXPECTED_GRADES@ AC RTE AC
#include<bits/stdc++.h>
#include "treasurehuntlib.h"
using namespace std;
#define fi first
#define se second
#define pb push_back
#define ll long long
template<typename T1,typename T2>void chmn(T1 &x,const T2 &y){x=x<y?x:y;}
template<typename T1,typename T2>void chmx(T1 &x,const T2 &y){x=x>y?x:y;}
int n,K;
bool left(int z){return z>>2&1;}
bool right(int z){return z&1;}
bool up(int z){return z>>1&1;}
bool down(int z){return z>>3&1;}
bool vertical(int z){return up(z)||down(z);}
bool horizontal(int z){return left(z)||right(z);}
int Getxline(int y){
    int x=0;
    int l=0,r=n-1;
    while(l<=r){
        int mid=l+r>>1;
        int z=Query(mid,y);
        if(left(z))r=mid-1;
        else if(right(z))l=mid+1;
        else {x=mid;break;}
    }
    return x;
}
int Getyline(int x){
    int y=0;
    int l=0,r=n-1;
    while(l<=r){
        int mid=l+r>>1;
        int z=Query(x,mid);
        if(up(z))r=mid-1;
        else if(down(z))l=mid+1;
        else {y=mid;break;}
    }
    return y;
}
int FindOnxline(int x){
    int dir=0;
    if(horizontal(Query(x,0)))dir=1;
    int l=0,r=n-1,y=0;
    while(l<=r){
        int mid=l+r>>1;
        int z=Query(x,mid);
        if(horizontal(z)){
            if(dir==0)r=mid-1;
            else l=mid+1;
        }
        else{
            if(z>>1&1)r=mid-1;
            else if(z>>3&1)l=mid+1;
            else{y=0;break;}
        }
    }
    return y;
}
int FindOnyline(int y){
    int dir=0;
    if(vertical(Query(0,y)))dir=1;
    int l=0,r=n-1,x=0;
    while(l<=r){
        int mid=l+r>>1;
        int z=Query(mid,y);
        if(vertical(z)){
            if(dir==0)r=mid-1;
            else l=mid+1;
        }
        else{
            if(z>>2&1)r=mid-1;
            else if(z>>0&1)l=mid+1;
            else{x=0;break;}
        }
    }
    return x;
}
void Solve1(){
    int x=Getxline(0),y=Getyline(0);
    Query(x,y);
}
void Solve2(){
    int x1=Getxline(0),y1=Getyline(0);
    vector<int>xs={x1},ys={y1};
    if(Query(x1,y1)==0){
        auto binarna1=[&](int L,int R,int y){
            int l=L,r=R,x=0;
            while(l<=r){
                int mid=l+r>>1;
                int z=Query(mid,y);
                if(left(z))r=mid-1;
                else if(right(z))l=mid+1;
                else {x=mid;break;}
            }
            return x;
        };
        if(left(Query(x1,0))){
            int x=binarna1(0,x1,0);
            xs.pb(x);
        }
        if(right(Query(x1,0))){
            int x=binarna1(x1,n-1,0);
            xs.pb(x);
        }

        if(left(Query(x1,n-1))){
            int x=binarna1(0,x1,n-1);
            xs.pb(x);
        }
        if(right(Query(x1,n-1))){
            int x=binarna1(x1,n-1,n-1);
            xs.pb(x);
        }

        auto binarna2=[&](int L,int R,int x){
            int l=L,r=R,y=0;
            while(l<=r){
                int mid=l+r>>1;
                int z=Query(x,mid);
                if(up(z))r=mid-1;
                else if(down(z))l=mid+1;
                else {y=mid;break;}
            }
            return y;
        };
        if(up(Query(0,y1))){
            int y=binarna2(0,y1,0);
            ys.pb(y);
        }
        if(down(Query(0,y1))){
            int y=binarna2(y1,n-1,0);
            ys.pb(y);
        }

        if(up(Query(n-1,y1))){
            int y=binarna2(0,y1,n-1);
            ys.pb(y);
        }
        if(down(Query(n-1,y1))){
            int y=binarna2(y1,n-1,n-1);
            ys.pb(y);
        }

        if(xs.size()==1&&ys.size()==1){
            int l=0,r=x1-1;
            while(r-l>=8){
                int mid=(l+r>>1)-1;
                int z=Query(mid,y1);
                if(left(z))r=mid-1;
                else if(right(z))l=mid+1;
                else{xs.pb(mid);break;}
            }
            for(int i=l;i<=r;i++)Query(i,y1);
            l=x1+1,r=n-1;
            while(r-l>=8){
                int mid=(l+r>>1)-1;
                int z=Query(mid,y1);
                if(left(z))r=mid-1;
                else if(right(z))l=mid+1;
                else{xs.pb(mid);break;}
            }
            for(int i=l;i<=r;i++)Query(i,y1);


            l=0,r=y1-1;
            while(r-l>=8){
                int mid=(l+r>>1)+1;
                int z=Query(x1,mid);
                if(up(z))r=mid-1;
                else if(down(z))l=mid+1;
                else{ys.pb(mid);break;}
            }
            for(int i=l;i<=r;i++)Query(x1,i);
            l=y1+1,r=n-1;
            while(r-l>=8){
                int mid=(l+r>>1)+1;
                int z=Query(x1,mid);
                if(up(z))r=mid-1;
                else if(down(z))l=mid+1;
                else{ys.pb(mid);break;}
            }
            for(int i=l;i<=r;i++)Query(x1,i);
        }
    }
    sort(xs.begin(),xs.end());
    xs.resize(unique(xs.begin(),xs.end())-xs.begin());
    sort(ys.begin(),ys.end());
    ys.resize(unique(ys.begin(),ys.end())-ys.begin());
    for(auto x:xs){
        int y=FindOnxline(x);
        ys.pb(y);
    }
    for(auto y:ys){
        int x=FindOnyline(y);
        xs.pb(x);
    }
    sort(xs.begin(),xs.end());
    xs.resize(unique(xs.begin(),xs.end())-xs.begin());
    sort(ys.begin(),ys.end());
    ys.resize(unique(ys.begin(),ys.end())-ys.begin());
    for(auto x:xs)for(auto y:ys)Query(x,y);
}
mt19937 rng(time(0));
void Solve3(){
    int ct=15;
    vector<int>xs;
    for(int i=0;i<ct;i++){
        int y=rng()%n;
        xs.pb(Getxline(y));
    }
    vector<int>ys;
    for(int i=0;i<ct;i++){
        int x=rng()%n;
        ys.pb(Getyline(x));
    }
    sort(xs.begin(),xs.end());
    xs.resize(unique(xs.begin(),xs.end())-xs.begin());
    sort(ys.begin(),ys.end());
    ys.resize(unique(ys.begin(),ys.end())-ys.begin());
    for(auto x:xs){
        int y=FindOnxline(x);
        ys.pb(y);
    }
    for(auto y:ys){
        int x=FindOnyline(y);
        xs.pb(x);
    }
    sort(xs.begin(),xs.end());
    xs.resize(unique(xs.begin(),xs.end())-xs.begin());
    sort(ys.begin(),ys.end());
    ys.resize(unique(ys.begin(),ys.end())-ys.begin());
    for(auto x:xs)for(auto y:ys)Query(x,y);
}
int main(){
    while(1){
        NextHunt(n,K);
        if(n==-1)break;
        if(K==1)Solve1();
        else if(K==2)Solve2();
        else Solve3();
    }
    return 0;
}