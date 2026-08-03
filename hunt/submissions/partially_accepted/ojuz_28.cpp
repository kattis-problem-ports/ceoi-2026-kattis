// Downloaded from https://oj.uz/submission/1417258 (scored 28/100 on oj.uz).
// Scores 28 on this package.
// Every group is AC; the 28 comes from partial credit inside the groups.
// @EXPECTED_GRADES@ AC AC AC
#include "treasurehuntlib.h"
#include <bits/stdc++.h>
using namespace std;

int main() {
	cin.tie(0)->sync_with_stdio(0);

	int n, k;
	while(1) {
		NextHunt(n, k);
		if(n == -1) return 0;

		if(k == 1) {
			int low = 0, high = n - 1, x = -1, y = -1;
			while(low <= high) {
				int c = (low + high) / 2;
				int res = Query(c, 0);
				if(res & 1) low = c + 1;
				else if(res & 4) high = c - 1;
				else {
					x = c;
					break;
				}
			}

			low = 0, high = n - 1;
			while(low <= high) {
				int c = (low + high) / 2;
				int res = Query(x, c);
				if(res & 2) high = c - 1;
				else if(res & 8) low = c + 1;
				else {
					y = c;
					break;
				}
			}

			Query(x, y);
			continue;
		}

		int low = 0, high = n - 1;
		vector<int> xs(2, -1), ys(2, -1);

		while(low <= high) {
			int c = (low + high) / 2;
			int res = Query(c, 0);
			if(res & 1) low = c + 1;
			else high = c - 1, xs[0] = c;
		}

		low = 0, high = n - 1;
		while(low <= high) {
			int c = (low + high) / 2;
			int res = Query(c, n - 1);
			if(res & 1) low = c + 1;
			else high = c - 1, xs[1] = c;
		}

		low = 0, high = n - 1;
		while(low <= high) {
			int c = (low + high) / 2;
			int res = Query(0, c);
			if(res & 8) low = c + 1;
			else high = c - 1, ys[0] = c;
		}

		low = 0, high = n - 1;
		while(low <= high) {
			int c = (low + high) / 2;
			int res = Query(n - 1, c);
			if(res & 8) low = c + 1;
			else high = c - 1, ys[1] = c;
		}

		vector<int> foundx(2, 0), foundy(2, 0);
		set<pair<int, int>> foundl;

		for(int i = 0; i < 2; i ++) {
			for(int j = 0; j < 2; j ++) {
				if(Query(xs[i], ys[j]) == 0) {
					foundx[i] = foundy[j] = 1;
					foundl.insert({xs[i], ys[j]});
				}
			}
		}

		if(foundl.size() == 2) continue;

		if(xs[0] != xs[1]) {
			if(foundx[0]) swap(xs[0], xs[1]);
			int res1 = Query(xs[0], 0);
			int tng = (res1 & 1) || (res1 & 4);
			low = 0, high = n - 1;
			while(low <= high) {
				int c = (low + high) / 2;
				int res = Query(xs[0], c);
				if((res & 1) || (res & 4)) {
					if(tng) low = c + 1;
					else high = c - 1;
				}
				else if(res & 2) high = c - 1;
				else if(res & 8) low = c + 1;
				else break;
			}
			continue;
		}

		if(ys[0] != ys[1]) {
			if(foundy[0]) swap(ys[0], ys[1]);
			int res1 = Query(0, ys[0]);
			int tng = (res1 & 2) || (res1 & 8);
			low = 0, high = n - 1;
			while(low <= high) {
				int c = (low + high) / 2;
				int res = Query(c, ys[0]);
				if((res & 2) || (res & 8)) {
					if(tng) low = c + 1;
					else high = c - 1;
				}
				else if(res & 4) high = c - 1;
				else if(res & 1) low = c + 1;
				else break;
			}
			continue;
		}

		int res1;
		if(ys[0] != 0) {
			res1 = Query(xs[0], 0);
			if(res1 == 0) continue;
			if(res1 == 8) {
				low = 0, high = xs[0] - 1;
				while(low <= high) {
					int c = (low + high) / 2;
					int res = Query(c, ys[0]);
					if(res & 4) high = c - 1;
					else if(res & 1) low = c + 1;
					else break;
				}
				low = xs[0] + 1, high = n - 1;
				while(low <= high) {
					int c = (low + high + 1) / 2;
					int res = Query(c, ys[0]);
					if(res & 1) low = c + 1;
					else if(res & 4) high = c - 1;
					else break;
				}
			}
		}

		if(xs[0] != 0) {
			res1 = Query(0, ys[0]);
			if(res1 == 0) continue;
			if(res1 == 1) {
				low = 0, high = ys[0] - 1;
				while(low <= high) {
					int c = (low + high) / 2;
					int res = Query(xs[0], c);
					if(res & 2) high = c - 1;
					else if(res & 8) low = c + 1;
					else break;
				}
				low = ys[0] + 1, high = n - 1;
				while(low <= high) {
					int c = (low + high + 1) / 2;
					int res = Query(xs[0], c);
					if(res & 8) low = c + 1;
					else if(res & 2) high = c - 1;
					else break;
				}
			}
		}
	}
}
