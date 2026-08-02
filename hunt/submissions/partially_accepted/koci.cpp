#include <bits/stdc++.h>
#include "treasurehuntlib.h"
using namespace std;

int N, K;

struct Point {
    int x, y;
    map<char, vector<pair<int,int>>> checked;
    Point(int x, int y) : x(x), y(y) {
        for (char d : {'u','d','l','r'}) checked[d].push_back({x, y});
    }
};

static set<pair<int,int>> known;
static vector<Point> pts;

int sx_of(char d)   { return d=='l'?-1:d=='r'?1:0; }
int sy_of(char d)   { return d=='u'?1:d=='d'?-1:0; }
char opp_of(char d) { return d=='u'?'d':d=='d'?'u':d=='l'?'r':'l'; }

string query(int x, int y) {
    int ret = Query(x - 1, N - y);
    if (ret == TREASURE) return "TREASURE";
    string resp;
    if (ret & DIR_UP)    resp += 'u';
    if (ret & DIR_DOWN)  resp += 'd';
    if (ret & DIR_LEFT)  resp += 'l';
    if (ret & DIR_RIGHT) resp += 'r';
    return resp;
}

tuple<vector<int>, string, int> closest_points(int x, int y) {
    int best = 3*N+1;
    vector<int> ans;
    string dir;
    for (int e = 0; e < (int)pts.size(); e++) {
        int d = abs(x-pts[e].x) + abs(y-pts[e].y);
        string cd;
        if (x > pts[e].x) cd += 'l';
        if (x < pts[e].x) cd += 'r';
        if (y < pts[e].y) cd += 'u';
        if (y > pts[e].y) cd += 'd';
        if (d < best) { best = d; ans.clear(); dir.clear(); }
        if (d == best) {
            ans.push_back(e);
            for (char c : cd) if (dir.find(c) == string::npos) dir += c;
        }
    }
    return {ans, dir, best};
}

tuple<int,int,bool> adjust_point(int pt_i, int x, int y) {
    bool adj = false;
    if (x < 1) { x = 1; adj = true; } if (y < 1) { y = 1; adj = true; }
    if (x > N) { x = N; adj = true; } if (y > N) { y = N; adj = true; }
    auto [cp, cd, dist] = closest_points(x, y);
    if (find(cp.begin(), cp.end(), pt_i) != cp.end()) return {x, y, adj};
    int lx = pts[pt_i].x, rx = x, ly = pts[pt_i].y, ry = y;
    int xo = lx!=rx ? (lx<rx?1:-1) : 0;
    int yo = ly!=ry ? (ly<ry?1:-1) : 0;
    while (lx != rx || ly != ry) {
        int sx = (lx+rx)/2; if (lx<rx && (lx+rx)%2==1) sx++;
        int sy = (ly+ry)/2; if (ly<ry && (ly+ry)%2==1) sy++;
        auto [p2, d2, dist2] = closest_points(sx, sy);
        if (find(p2.begin(), p2.end(), pt_i) != p2.end()) { lx = sx; ly = sy; }
        else { rx = sx-xo; ry = sy-yo; }
    }
    return {lx, ly, true};
}

bool line_bs(int x1, int y1, int x2, int y2, char d) {
    x1=max(1,min(N,x1)); y1=max(1,min(N,y1));
    x2=max(1,min(N,x2)); y2=max(1,min(N,y2));
    int xo = x1!=x2 ? (x1<x2?1:-1) : 0;
    int yo = y1!=y2 ? (y1<y2?1:-1) : 0;
    while (x1 != x2 || y1 != y2) {
        int sx = (x1+x2)/2, sy = (y1+y2)/2;
        string resp = query(sx, sy);
        if (resp == "TREASURE" && !known.count({sx,sy})) {
            known.insert({sx,sy}); pts.emplace_back(sx,sy); return true;
        }
        if (resp == string(1, d)) { x1 = sx+xo; y1 = sy+yo; }
        else {
            if (x1==sx && xo!=0) return false;
            if (y1==sy && yo!=0) return false;
            x2 = sx-xo; y2 = sy-yo;
        }
    }
    string resp = query(x1, y1);
    if (resp == "TREASURE" && !known.count({x1,y1})) {
        known.insert({x1,y1}); pts.emplace_back(x1,y1); return true;
    }
    return false;
}

bool orth_bs(int x1, int y1, int x2, int y2, char d_orth, char d_par, int max_reach, bool inwards) {
    x1=max(1,min(N,x1)); y1=max(1,min(N,y1));
    x2=max(1,min(N,x2)); y2=max(1,min(N,y2));
    int xo = 0, yo = 0;
    if (x1 != x2) xo = x1<x2?1:-1;
    if (y1 != y2) yo = y1<y2?1:-1;
    while (x1 != x2 || y1 != y2) {
        int sx = (x1+x2)/2, sy = (y1+y2)/2;
        if (x1 < x2 && (x1+x2)%2==1) sx++;
        if (y1 < y2 && (y1+y2)%2==1) sy++;
        string resp = query(sx, sy);
        if (resp == "TREASURE" && !known.count({sx,sy})) {
            known.insert({sx,sy}); pts.emplace_back(sx,sy);
            if ((int)pts.size() == K) return true;
        }
        if (resp.find(d_orth)!=string::npos &&
                resp.find(d_par)==string::npos &&
                resp.find(opp_of(d_par))==string::npos) { x1 = x2 = sx; y1 = y2 = sy; break; }
        if (resp == string(1, d_orth)) { x1 = x2 = sx; y1 = y2 = sy; break; }
        auto [cp, exp_dir, dist] = closest_points(sx, sy);
        string unexpected;
        for (char c : resp) if (exp_dir.find(c) == string::npos) unexpected += c;
        if (unexpected.find(d_orth)==string::npos)
            { x2 = sx-xo; y2 = sy-yo; }
        else if (inwards && resp.find(opp_of(d_par))!=string::npos)
            { x2 = sx; y2 = sy; }
        else if (inwards && resp.find(d_par)!=string::npos)
            { x1 = sx; y1 = sy; }
        else if (!inwards && resp.find(opp_of(d_par))!=string::npos)
            { x2 = sx; y2 = sy; }
        else if (!inwards && resp.find(d_par)!=string::npos)
            { x1 = sx; y1 = sy; }
        else
            { x2 = sx; y2 = sy; }
    }
    auto [cp, cd, dist] = closest_points(x1, y1);
    dist = min(dist, max_reach);
    return line_bs(x1+sx_of(d_orth), y1+sy_of(d_orth), x1+sx_of(d_orth)*dist, y1+sy_of(d_orth)*dist, d_orth);
}

void first_bs() {
    int lx = 1, rx = N, ly = 1, ry = N;
    while (lx < rx) {
        int sx = (lx+rx)/2;
        string resp = query(sx, ly);
        if (resp == "TREASURE" && !known.count({sx,ly})) { known.insert({sx,ly}); pts.emplace_back(sx,ly); return; }
        bool has_l = resp.find('l') != string::npos;
        bool has_r = resp.find('r') != string::npos;
        if (!has_l && !has_r) { lx = rx = sx; break; }
        if (has_l) rx = sx - 1;
        else       lx = sx + 1;
    }
    while (ly < ry) {
        int sy = (ly+ry)/2;
        string r = query(lx, sy);
        if (r == "TREASURE" && !known.count({lx,sy})) { known.insert({lx,sy}); pts.emplace_back(lx,sy); return; }
        if (r == "u") ly = sy+1; else ry = sy-1;
    }
    string r2 = query(lx, ly);
    if (r2 == "TREASURE" && !known.count({lx,ly})) {
        known.insert({lx,ly}); pts.emplace_back(lx,ly);
    }
}

bool check_direction(int pt_i, char d) {
    while (true) {
        Point& pt = pts[pt_i];
        int mul = 1 << ((int)pt.checked[d].size() - 1);
        int x = pt.x + sx_of(d)*mul;
        int y = pt.y + sy_of(d)*mul;
        auto [nx, ny, adj] = adjust_point(pt_i, x, y);
        auto [lx, ly] = pt.checked[d].back();
        if (abs(nx-pt.x)+abs(ny-pt.y) == abs(lx-pt.x)+abs(ly-pt.y)) return false;
        while (abs(nx-pt.x)+abs(ny-pt.y) < abs(lx-pt.x)+abs(ly-pt.y)) {
            pt.checked[d].pop_back();
            tie(lx, ly) = pt.checked[d].back();
        }
        if (abs(nx-pt.x)+abs(ny-pt.y) == abs(lx-pt.x)+abs(ly-pt.y)) return false;
        pt.checked[d].push_back({nx, ny});
        x = nx; y = ny;
        string resp = query(x, y);
        if (resp == "TREASURE" && !known.count({x,y})) {
            known.insert({x,y}); pts.emplace_back(x,y); return true;
        }
        if (resp == string(1, opp_of(d))) { if (adj) return false; continue; }
        auto [cp, exp_dir, dist] = closest_points(x, y);
        string resp_sorted = resp; sort(resp_sorted.begin(), resp_sorted.end());
        string exp_sorted  = exp_dir; sort(exp_sorted.begin(), exp_sorted.end());
        if (resp_sorted == exp_sorted) { if (adj) return false; continue; }
        string unexpected;
        for (char c : resp) if (exp_dir.find(c) == string::npos) unexpected += c;
        if (resp.size() == 1) {
            return line_bs(x+sx_of(resp[0]), y+sy_of(resp[0]), x+sx_of(resp[0])*mul, y+sy_of(resp[0])*mul, resp[0]);
        }
        for (char od : unexpected) {
            if ((int)pts.size() >= K) return true;
            if (od == opp_of(d)) continue;
            if (od == d) continue;
            if (resp.find(opp_of(d)) != string::npos)
                orth_bs(x,y, x-sx_of(d)*mul/2+(mul>1?sx_of(d):0), y-sy_of(d)*mul/2+(mul>1?sy_of(d):0), od, opp_of(d), mul, true);
            if ((int)pts.size() >= K) return true;
            if (resp.find(d) != string::npos)
                orth_bs(x,y, x+sx_of(d)*mul-sx_of(d), y+sy_of(d)*mul-sy_of(d), od, d, mul, false);
            if (resp.find(d)==string::npos && resp.find(opp_of(d))==string::npos)
                return line_bs(x+sx_of(od), y+sy_of(od), x+sx_of(od)*mul, y+sy_of(od)*mul, od);
            if ((int)pts.size() >= K) return true;
        }
        return true;
    }
}

void solve() {
    first_bs();
    while ((int)pts.size() < K) {
        for (int i = 0; i < (int)pts.size(); i++) {
            for (char d : {'d','l','r','u'}) {
                if (check_direction(i, d) && (int)pts.size() >= K) return;
            }
        }
    }
}

int main() {
    while (true) {
        NextHunt(N, K);
        if (N == -1) break;
        known.clear();
        pts.clear();
        solve();
    }
    return 0;
}
