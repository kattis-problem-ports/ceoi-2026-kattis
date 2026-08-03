// Downloaded from https://oj.uz/submission/1411907 (scored 6/100 on oj.uz).
// Scores 6 on this package.
// Every group is AC; the 6 comes from partial credit inside the groups.
// @EXPECTED_GRADES@ AC AC AC
#include "treasurehuntlib.h"
#include <bits/stdc++.h>

using namespace std;

mt19937 rng(67);

int main() {
    while (true) {
        int n, k; NextHunt(n, k); if (n < 0) break;
        for (int i = 0; i < 5; i++) {
        	int sx = rng() % n, sy = rng() % n;
        	int t = Query(sx, sy);
        	if (t & DIR_RIGHT) {
        		for (int c = 20; c >= 0; c--) if (sx + (1 << c) < n) {
        			int tx = Query(sx + (1 << c), sy);
        			if (tx & DIR_RIGHT) sx += (1 << c);
        		}
        		sx++;
        	} else if (t & DIR_LEFT) {
        		for (int c = 20; c >= 0; c--) if (sx - (1 << c) >= 0) {
        			int tx = Query(sx - (1 << c), sy);
        			if (tx & DIR_LEFT) sx -= (1 << c);
        		}
        		sx--;
        	}
        	t = Query(sx, sy);
        	if (t & DIR_DOWN) {
        		for (int c = 20; c >= 0; c--) if (sy + (1 << c) < n) {
        			int tx = Query(sx, sy + (1 << c));
        			if (tx & DIR_DOWN) sy += (1 << c);
        		}
        		sy++;
        	} else if (t & DIR_UP) {
        		for (int c = 20; c >= 0; c--) if (sy - (1 << c) >= 0) {
        			int tx = Query(sx, sy - (1 << c));
        			if (tx & DIR_UP) sy -= (1 << c);
        		}
        		sy--;
        	}
        	// cout << sx << ' ' << sy << '\n';
	        // int l = 0, r = n-1, X = 0, Y = 0;
	        // int f = 0;
	        // int kxx = rng() % n;
	        // while (l <= r) {
	        // 	int m = l + (r-l)/2;
	        // 	int x = Query(m, kxx);
	        // 	if (x == TREASURE) {
	        // 		f = 1;
	        // 		break;
	        // 	} else if (!(x & DIR_LEFT) && !(x & DIR_RIGHT)) {
	        // 		X = m;
	        // 		break;
	        // 	} else if (x & DIR_LEFT) {
	        // 		r = m-1;
	        // 	} else {
	        // 		l = m+1;
	        // 	}
	        // }
	        // if (f) continue;
	        // l = 0, r = n-1;
	        // while (l <= r) {
	        // 	int m = l + (r-l)/2;
	        // 	int x = Query(X, m);
	        // 	if (x == TREASURE) {
	        // 		f = 1;
	        // 		break;
	        // 	} else if (!(x & DIR_UP) && !(x & DIR_DOWN)) {
	        // 		X = m;
	        // 		break;
	        // 	} else if (x & DIR_UP) {
	        // 		r = m-1;
	        // 	} else {
	        // 		l = m+1;
	        // 	}
	        // }
	        // // cout << X << ' ' << Y << '\n';
	    }
    }
}
