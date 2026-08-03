// Downloaded from https://oj.uz/submission/1420516 (scored 63/100 on oj.uz).
// Scores 63 on this package.
// @EXPECTED_GRADES@ AC AC AC AC TLE
#include <bits/stdc++.h>
using namespace std;
#define m_p make_pair
#define all(x) (x).begin(),(x).end()
#define sz(x) ((int)(x).size())
#define fi first
#define se second
typedef long long ll;
mt19937 rnd(chrono::steady_clock::now().time_since_epoch().count());
mt19937 rnf(2106);
const int N = 400005;
const ll INF = 1000000009000000009;

int n, m;
ll f;
pair<ll, bool> a[N];

ll ul[N], ur[N];

ll dp[N], ndp[N];

void solv()
{
    cin >> n >> m >> f;
    for (int i = 1; i <= n; ++i)
    {
        cin >> a[i].fi;
        a[i].se = true;
    }
    for (int i = n + 1; i <= n + m; ++i)
    {
        cin >> a[i].fi;
        a[i].se = false;
    }
    n += m;

    sort(a + 1, a + n + 1);
    a[0].fi = -INF;
    a[n + 1].fi = INF;

    ll u = 0;
    for (int i = 1; i <= n; ++i)
    {
        u -= 2 * (a[i].fi - a[i - 1].fi);
        u = max(u, 0LL);
        if (!a[i].se)
            u += f;
        else
            ul[i] = u + a[i].fi;
    }

    u = 0;
    for (int i = n; i >= 1; --i)
    {
        u -= 2 * (a[i + 1].fi - a[i].fi);
        u = max(u, 0LL);
        if (!a[i].se)
            u += f;
        else
            ur[i] = u - a[i].fi;
    }

    dp[0] = 0;
    int q = 0;
    for (int i = 1; i <= n; ++i)
    {
        if (!a[i].se)
        {
            for (int j = 0; j <= q; ++j)
            {
                dp[j] += (2 * j - q) * f;
            }
        }
        else
        {
            for (int j = 0; j <= q + 1; ++j)
                ndp[j] = -INF;
            for (int j = 0; j <= q; ++j)
            {
                ndp[j + 1] = max(ndp[j + 1], dp[j] + ul[i]);
                if (2 * j - q > 0)
                    ndp[j] = max(ndp[j], dp[j] + ur[i]);
            }
            ++q;
            for (int j = 0; j <= q; ++j)
                dp[j] = ndp[j];
        }
    }

    assert(q % 2 == 0);
    cout << dp[q / 2] << "\n";
}

int main()
{
    #ifdef SOMETHING
    freopen("input.txt", "r", stdin);
    //freopen("output.txt", "w", stdout);
    #endif // SOMETHING
    ios_base::sync_with_stdio(false), cin.tie(0);

    int tt = 1;
    cin >> tt;
    while (tt--)
    {
        solv();
    }
    return 0;
}
