// Downloaded from https://oj.uz/submission/1420476 (scored 0/100 on oj.uz).
// Scores 0 on this package.
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
const int N = 1003;

int n, m;
bitset<N> a[N];

int p[N];
int fin(int x)
{
    if (x == p[x])
        return x;
    return p[x] = fin(p[x]);
}

void kpc(int x, int y)
{
    x = fin(x);
    y = fin(y);
    if (rnd() % 2)
        swap(x, y);
    p[x] = y;
}

int q[N];
void solv()
{
    cin >> n >> m;
    for (int i = 0; i < m; ++i)
    {
        int x, y;
        cin >> x >> y;
        a[x][y] = a[y][x] = 1;
    }

    for (int x = 1; x <= n; ++x)
        p[x] = x;
    for (int x = 1; x <= n; ++x)
    {
        for (int y = x + 1; y <= n; ++y)
        {
            if ((a[x] & a[y]).count() >= 2)
            {
                kpc(x, y);
            }
        }
    }

    for (int x = 1; x <= n; ++x)
        q[fin(x)]++;
    int ans = 0;
    for (int i = 1; i <= n; ++i)
    {
        if (q[i] >= 4)
            ans += (q[i] * q[i] - 5 * q[i] + 8) / 2;
    }
    cout << ans << "\n";
}

int main()
{
    #ifdef SOMETHING
    freopen("input.txt", "r", stdin);
    //freopen("output.txt", "w", stdout);
    #endif // SOMETHING
    ios_base::sync_with_stdio(false), cin.tie(0);

    int tt = 1;
    //cin >> tt;
    while (tt--)
    {
        solv();
    }
    return 0;
}
