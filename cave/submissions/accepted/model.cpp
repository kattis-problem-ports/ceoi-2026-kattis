#define  _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <utility>
#include <algorithm>
#include <vector>
using namespace std;

typedef long long int llint;

#include <cassert>
#define Assert assert
constexpr llint MaxV = 1'000'000'000'000'000'000LL;
constexpr int MaxN = 100000;

int main()
{
//    freopen("kapniki99.in", "rt", stdin);
//    freopen("tests\\kapniki07.in", "rt", stdin);
    int ok_;
    // Preberimo število in tip kapnikov.
    llint v; int n, m = 0;
    ok_ = scanf("%lld %d\n", &v, &n); 
    Assert(ok_ == 2); 
    Assert(1 <= v); 
    Assert(v <= MaxV); 
    Assert(1 <= n); 
    Assert(n <= MaxN);
    char s[MaxN + 2]; fgets(s, sizeof(s), stdin);
    for (int i = 0; i < n; i++) Assert(s[i] == 'M' || s[i] == 'T'); Assert(s[n] == '\n');
    // Preberimo višine kapnikov in pripravimo tabelo parov (y, d), ki povedo,
    // da nas neki kapnik na novo (d < 0 ? začne : neha) motiti, ko višina železnice zraste na y.
    vector<pair<llint, int>> spremembe(n + 1);
    for (int i = 0; i < n; ++i)
    {
        llint ki; ok_ = scanf("%lld", &ki); Assert(ok_ == 1);
        Assert(1 <= ki); Assert(ki < v);
        // Če je to stalagmit, nas neha motiti pri višini ki + 1.
        if (s[i] == 'M') ++m, spremembe[i] = {ki + 1, 1};
        // Če je stalaktit, nas začne motiti pri višini v - ki + 1.
        else spremembe[i] = {v - ki + 1, -1};
    }
    spremembe[n] = {v + 1, 0};
    // Pregledujmo spremembe po naraščajoči višini.
    sort(spremembe.begin(), spremembe.end());
    int naj = n + 1; llint koliko = 0;
    for (int i = 0; i <= n; i++)
    {
        llint dv = spremembe[i].first - (i == 0 ? 1 : spremembe[i - 1].first);
        if (false) fprintf(stderr, "y = %lld..%lld: %d motecih; pri y = %lld se spremeni na %d.\n", (i == 0 ? 1 : spremembe[i - 1].first), spremembe[i].first - 1, m, spremembe[i].first, m + spremembe[i].second);
        if (dv >= 0) 
            // Od prejšnje spremembe do trenutne nas moti m kapnikov.
            if (m < naj) naj = m, koliko = dv; // Nova najboljša rešitev.
            else if (m == naj) koliko += dv; // Izenačena dosedanja najboljša rešitev.
        // Upoštevajmo spremembo v številu motečih kapnikov.
        m -= spremembe[i].second; 
    }
    printf("%d %lld\n", naj, koliko); return 0;
}