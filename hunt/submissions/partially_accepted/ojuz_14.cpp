// Downloaded from https://oj.uz/submission/1415481 (scored 14/100 on oj.uz).
// Scores 15 on this package, NOT 14 -- this package's test data differs from oj.uz's.
// Every group is AC; the score comes from partial credit inside the groups.
// @EXPECTED_GRADES@ AC AC AC
#include "treasurehuntlib.h"
#include <bits/stdc++.h>
using namespace std;

#define ll long long

// Najde největší y na zadaném x, kde kompas stále ukazuje DIR_DOWN.
// Vrací prvou pozici y, kde už DIR_DOWN neplatí.
ll getMaxYDown(ll fixed_x, int N) {
    ll low = 0, high = N - 1, ans = -1;
    while (low <= high) {
        ll mid = low + (high - low) / 2;
        int res = Query(fixed_x, mid);
        if (res & DIR_DOWN) {
            ans = mid;
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return ans + 1;
}

// Najde nejmenší y na zadaném x, kde kompas ukazuje DIR_UP.
// Vrací první pozici y (odspodu), kde už DIR_UP neplatí.
ll getMinYUp(ll fixed_x, int N) {
    ll low = 0, high = N - 1, ans = N;
    while (low <= high) {
        ll mid = low + (high - low) / 2;
        int res = Query(fixed_x, mid);
        if (res & DIR_UP) {
            ans = mid;
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    return ans - 1;
}

// Najde největší x na zadaném y, kde kompas ukazuje DIR_RIGHT.
ll getMaxXRight(ll fixed_y, int N) {
    ll low = 0, high = N - 1, ans = -1;
    while (low <= high) {
        ll mid = low + (high - low) / 2;
        int res = Query(mid, fixed_y);
        if (res & DIR_RIGHT) {
            ans = mid;
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return ans + 1;
}

// Najde nejmenší x na zadaném y, kde kompas ukazuje DIR_LEFT.
ll getMinXLeft(ll fixed_y, int N) {
    ll low = 0, high = N - 1, ans = N;
    while (low <= high) {
        ll mid = low + (high - low) / 2;
        int res = Query(mid, fixed_y);
        if (res & DIR_LEFT) {
            ans = mid;
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    return ans - 1;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, K;
    while (1){
        NextHunt(N, K);
        if (N == -1) break;

        // 1. Určíme hranice obdélníku pomocí 8 binárních vyhledávání
        ll y_min = min(getMaxYDown(0, N), getMaxYDown(N - 1, N));
        ll y_max = max(getMinYUp(0, N), getMinYUp(N - 1, N));
        ll x_min = min(getMaxXRight(0, N), getMaxXRight(N - 1, N));
        ll x_max = max(getMinXLeft(0, N), getMinXLeft(N - 1, N));

        // 2. Otestujeme horní levý roh obdélníka
        int res = Query(x_min, y_min);

        if (res == TREASURE) {
            // Pokud tam poklad je, druhý poklad musí ležet v dolním pravém rohu
            if (x_min != x_max || y_min != y_max) {
                Query(x_max, y_max);
            }
        } else {
            // Pokud tam poklad není, poklady leží na druhé úhlopříčce
            Query(x_max, y_min);
            Query(x_min, y_max);
        }
    }

    return 0;
}