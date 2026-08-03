// Downloaded from https://oj.uz/submission/1417319 (scored 0/100 on oj.uz).
// Scores 0 on this package.
// Exits 1 (the library aborts it); an attempt at reading the test file directly.
#include "treasurehuntlib.h"
extern void InitFromFile(const char *fileName);

#include <bits/stdc++.h>

using i64 = long long;

#ifdef DEBUG
    #include "../debug.h"
#else
    #define debug(...) void(23)
#endif

#define x first
#define y second

void solve(int N, int K) {
    std::map<std::pair<int, int>, int> save;
    auto ask = [&](int a, int b) {
        if (save.count({a, b})) {
            return save[{a, b}];
        }
        return save[{a, b}] = Query(a, b);
    };

    std::set<std::pair<int, int>> treasures;
    std::set<std::array<int, 4>> vis;

    // check all points p: (x, y) s.t. l<=x<=r and a<=y<=b
    auto solve = [&](auto&& self, int l, int r, int a, int b) -> bool {
        if (l > r || a > b || vis.count({l, r, a, b})) {
            return false;
        }
        vis.insert({l, r, a, b});
        debug(l, r, a, b, treasures);
        int cnt = 0;
        for (auto[c, d] : treasures) {
            cnt += l <= c && c <= r && a <= d && d <= b;
        }
        if (cnt == 1LL * (r - l + 1) * (b - a + 1)) {
            return false;
        }
        if (l == r && a == b) {
            if (ask(l, a) == 0) {
                treasures.emplace(l, a);
                return true;
            }
            return false;
        }
        int mx = (l + r) >> 1;
        int my = (a + b) >> 1;
        bool res = false;
        if (cnt) {
            std::set<std::array<int, 4>> all;
            all.insert({l, mx, a, my});
            all.insert({mx + 1, r, a, my});
            all.insert({l, mx, my + 1, b});
            all.insert({mx + 1, r, my + 1, b});
            for (auto c : all) {
                if (c[0] > c[1] || c[2] > c[3]) {
                    continue;
                }
                res |= self(self, c[0], c[1], c[2], c[3]);
                if (treasures.size() == K) {
                    break;
                }
            }
        } else {
            int resq = ask(mx, my);
            debug(l, r, a, b, mx, my, resq);
            if (resq == 0) {
                if (!treasures.count({mx, my})) {
                    treasures.emplace(mx, my);
                    res = true;
                }
            } else {
                std::vector<std::pair<int, int>> cx;
                std::vector<std::pair<int, int>> cy;
                cx.emplace_back(l, r);
                cy.emplace_back(a, b);
                if ((resq & 1) && (resq & 4)) {
                    cx.emplace_back(l, mx);
                    cx.emplace_back(mx + 1, r);
                } else if (resq & 1) { // right
                    if (l + 1 == r) {
                        cx.emplace_back(l, l);
                        cx.emplace_back(r, r);
                    } else {
                        cx.emplace_back(mx, r);
                    }
                } else if (resq & 4) { // left
                    cx.emplace_back(l, mx);
                } else {
                    cx.emplace_back(mx, mx);
                }
                if ((resq & 2) && (resq & 8)) {
                    cy.emplace_back(a, my);
                    cy.emplace_back(my + 1, b);
                } else if (resq & 2) { // up
                    cy.emplace_back(a, my);
                } else if (resq & 8) { // down
                    if (a + 1 == b) {
                        cy.emplace_back(a, a);
                        cy.emplace_back(b, b);
                    } else {
                        cy.emplace_back(my, b);
                    }
                } else {
                    cy.emplace_back(my, my);
                }
                debug(cx, cy);
                for (auto[nl, nr] : cx) {
                    if (nl > nr) {
                        continue;
                    }
                    for (auto [na, nb] : cy) {
                        if (na > nb) {
                            continue;
                        }
                        res |= self(self, nl, nr, na, nb);
                        if (treasures.size() == K) {
                            break;
                        }
                    }
                    if (treasures.size() == K) {
                        break;
                    }
                }
            }
        }
        return res;
    };

    while (treasures.size() != K) {
        vis.clear();
        debug(treasures);
        std::vector<int> xs {0, N}, ys {0, N};
        for (auto[a, b] : treasures) {
            xs.emplace_back(a);
            ys.emplace_back(b);
        }
        std::sort(xs.begin(), xs.end());
        xs.erase(std::unique(xs.begin(), xs.end()), xs.end());
        std::sort(ys.begin(), ys.end());
        ys.erase(std::unique(ys.begin(), ys.end()), ys.end());
        debug(xs, ys);
        bool found = false;
        for (int i = 0; i < int(xs.size()) - 1; ++i) {
            for (int j = 0; j < int(ys.size()) - 1; ++j) {
                found |= solve(solve, xs[i] + 1, xs[i + 1] - 1, ys[j] + 1, ys[j + 1] - 1);
                if (treasures.size() == K) {
                    break;
                }
            }
            if (treasures.size() == K) {
                break;
            }
        }
        if (treasures.size() == K) {
            break;
        }
        for (int i = 0; i < int(xs.size()) - 1; ++i) {
            for (int j = 0; j < int(ys.size()) - 1; ++j) {
                found |= solve(solve, xs[i], xs[i], ys[j] + 1, ys[j + 1] - 1);
                if (treasures.size() == K) {
                    break;
                }
            }
            if (treasures.size() == K) {
                break;
            }
        }
        if (treasures.size() == K) {
            break;
        }
        for (int i = 0; i < int(xs.size()) - 1; ++i) {
            for (int j = 0; j < int(ys.size()) - 1; ++j) {
                found |= solve(solve, xs[i] + 1, xs[i + 1] - 1, ys[j], ys[j]);
                if (treasures.size() == K) {
                    break;
                }
            }
            if (treasures.size() == K) {
                break;
            }
        }
        debug('f', treasures);
        assert(found);
    }
}

int main()
{
    #ifdef DEBUG
        InitFromFile("02.in");
    #endif
    while (true) {
        int n, k; NextHunt(n, k); if (n < 0) break;
        solve(n, k);
    }
    return 0;
}