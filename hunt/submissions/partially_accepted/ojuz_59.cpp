// Downloaded from https://oj.uz/submission/1414246 (scored 59/100 on oj.uz).
// Scores 59 on this package.
// Every group is AC; the 59 comes from partial credit inside the groups.
// @EXPECTED_GRADES@ AC AC AC
#include "treasurehuntlib.h"
#include <bits/stdc++.h>
using namespace std;
pair<int,int> find_treasure_X(int lx, int ly, int rx, int ry, int hint) {
	while(rx>lx) {
		int mx = (lx+rx)/2;
		int res = Query(mx,hint);
		if(res & 1)
			lx = mx+1;
		else if(res & 4)
			rx = mx-1;
		else
			lx = rx = mx;
	}
	bool dir;
	if(Query(lx,hint) == 2)
		ry = hint, dir = 1;
	else
		ly = hint, dir = 0;
	while(ry>ly) {
		int my = (ly+ry)/2;
		int res = Query(lx,my);
		if(my == ly) {
			if(res == 0)
				return {lx, ly};
			Query(lx, ly+1);
			return {lx, ly+1};
		}
		if(res & 5) {
			if(dir)
				ly = my+1;
			else
				ry = my-1;
		} else if(res & 2)
			ry = my - 1;
		else if(res & 8)
			ly = my + 1;
		else
			ly = ry = my;
	}
	Query(lx,ly);
	return {lx,ly};
}
pair<int,int> find_treasure_Y(int lx, int ly, int rx, int ry, int hint) {
	while(ry>ly) {
		int my = (ly+ry)/2;
		//printf("ly ry %d %d\n",ly,ry);
		//printf("query %d\n",my);
		int res = Query(hint,my);
		if(res & 2)
			ry = my - 1;
		else if(res & 8)
			ly = my + 1;
		else
			ly = ry = my;
	}
	//printf("y = %d\n",ly);
	bool dir;
	if(Query(hint,ly) == 4)
		rx = hint, dir = 1;
	else
		lx = hint, dir = 0;
	while(rx>lx) {
		int mx = (lx+rx)/2;
		int res = Query(mx,ly);
		if(mx == lx) {
			if(res == 0)
				return {lx, ly};
			Query(lx+1,ly);
			return {lx+1,ly};
		}
		if(res & 10) {
			if(dir)
				lx = mx+1;
			else
				rx = mx-1;
		}
		else if(res & 1)
			lx = mx+1;
		else if(res & 4)
			rx = mx-1;
		else
			lx = rx = mx;
	}
	Query(lx,ly);
	return {lx,ly};
}
int dir(int dx, int dy) {
	int ans = 0;
	if(dx < 0)
		ans |= 4;
	else if(dx > 0)
		ans |= 1;
	if(dy < 0)
		ans |= 2;
	else if(dy > 0)
		ans |= 8;
	return ans;
}
int main() {
	while(1) {
		int N, K;
		NextHunt(N,K);
		if(K == -1)
			return 0;
		auto clamp = [&N](int x) {return max(0,min(N-1,x));};
		auto [tx, ty] = find_treasure_X(0,0,N-1,N-1,0);
		if(K == 1)
			continue;
		int dx[8] = {1,1,1,0,-1,-1,-1,0};
		int dy[8] = {-1,0,1,1,1,0,-1,-1};
		for(int d=1;d<N;d<<=1)
			for(int i=0;i<8;i++) {
				int qx = clamp(tx+dx[i]*d);
				int qy = clamp(ty+dy[i]*d);
				if(qx == tx && qy == ty)
					continue;
				int res = Query(qx,qy);
				if(res & dir(qx-tx,qy-ty)) {
					int common = res & dir(qx-tx,qy-ty);
					if(common & 1)
						find_treasure_X(qx+1,0,N-1,N-1,qy);
					else if(common & 2)
						find_treasure_Y(0,0,N-1,qy-1,qx);
					else if(common & 4)
						find_treasure_X(0,0,qx-1,N-1,qy);
					else
						find_treasure_Y(0,qy+1,N-1,N-1,qx);
					d = N+5;
					i = 8;
				}
			}
	}
}
