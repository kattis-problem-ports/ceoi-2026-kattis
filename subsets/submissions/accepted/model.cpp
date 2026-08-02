#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
using namespace std;

typedef int StVozlisca, StBarve;
struct Vozlisce
{
    StBarve barva;
    StVozlisca stars = -1;
    StVozlisca otrok = -1; // prvi otrok tega vozlišča
    StVozlisca sor = -1; // naslednji sorojenec tega vozlišča
    StVozlisca naslIsteBarve = -1;
    StVozlisca starsB = -1; // starš v skrčenem drevesu za barvo 'barva'
    StBarve dodan = -1; // Zadnje skrčeno drevo, v katero je bilo to vozlišče dodano.
    // Naslednji podatki se nanašajo na skrčeno drevo za barvo 'dodan'.
    StVozlisca otrokS = -1, sorS = -1; // prvi otrok, naslednji sorojenec
};
int n, b; StVozlisca koren = -1;
vector<Vozlisce> vozl;

vector<int> veja; // vozlišča na poti od korena do u
vector<int> zadnjiPoBarvi; // zadnjiPoBarvi[b] je indeks zadnjega vozlišča barve b v 'veja'
void DolociStarseB(int u)
{
    auto &U = vozl[u]; 
    int pi = zadnjiPoBarvi[U.barva];  // Najbližji u-jev prednik enake barve kot u
    int ui = (int) veja.size(); veja.emplace_back(u); // Dodajmo u na konec veje.
    // Če je u-jev starš enake barve kot u, bo ostal njegov starš tudi v skrčenem
    // drevesu u-jeve barve.  Sicer pa bo u-jev starš v skrčenem drevesu postal 
    // zadnji prednik pred 'pi' (najbližjim prednikom enake barve kot u).
    int ri = min(pi + 1, ui - 1); U.starsB = (ri < 0) ? -1 : veja[ri];
    // Rekurzivno obdelajmo še u-jeve potomce.
    zadnjiPoBarvi[U.barva] = ui; 
    for (StVozlisca v = U.otrok; v >= 0; v = vozl[v].sor) DolociStarseB(v);
    zadnjiPoBarvi[U.barva] = pi; veja.pop_back(); // Pospravimo za sabo.
}

StBarve c = -1; // Barva, za katero trenutno obravnavamo skrčeno drevo.
// Vrne par (največja lepa podmnožica barve c, največje število vozlišč barve c na poti od u navzdol).
pair<int, int> ObdelajSkrcenoDrevo(StVozlisca u) 
{
    auto &U = vozl[u];
    // Za vsakega u-jevega otroka v poiščimo največje število vozlišč barve c na poti od v navzdol;
    // največji dve od teh števil si zapomnimo v 'maxNaVeji1' in 'maxNaVeji2'.
    // V 'maxLepa' hranimo največjo lepo podmnožico po poddrevesih u-jevih otrok.
    int maxNaVeji1 = 0, maxNaVeji2 = 0, maxLepa = 0;
    for (StVozlisca v = U.otrokS; v >= 0; )
    {
        auto &V = vozl[v];
        auto [vMaxLepa, vMaxNaVeji] = ObdelajSkrcenoDrevo(v);
        maxLepa = max(maxLepa, vMaxLepa);
        if (vMaxNaVeji > maxNaVeji1) maxNaVeji2 = maxNaVeji1, maxNaVeji1 = vMaxNaVeji;
        else if (vMaxNaVeji > maxNaVeji2) maxNaVeji2 = vMaxNaVeji;
        v = V.sorS;
    }
    // Znotraj u-jevega poddrevesa je možna tudi taka lepa podmnožica, ki
    // jo pokriva pot z vrhom pri u, ki se pred in po tem spušča v dva različna u-jeva
    // otroka (v enem pokrije maxNaVeji1 vozlišč barve c, v drugem pa maxNaVeji2).
    maxLepa = max(maxLepa, maxNaVeji1 + maxNaVeji2 + (U.barva == c ? 1 : 0));
    return {maxLepa, maxNaVeji1 + (U.barva == c ? 1: 0)};
}

int main()
{
    // Preberimo vhodne podatke.
    ios_base::sync_with_stdio(false);
    cin >> n >> b; vozl.resize(n); 
    vector<StVozlisca> prviPoBarvi(b, -1); // prviPoBarvi[b] = prvo vozlišče barve b
    const bool velikoDrevo = (n > 200'000);
    if (! velikoDrevo) for (int u = 0; u < n; ++u) {
        auto &U = vozl[u]; cin >> U.stars >> U.barva; --U.stars; --U.barva; }
    else { // Velika drevesa so predstavljena parametrično.
        long long A, B, M, K, AA, BB, MM, r = 0, rr = 0; 
        cin >> A >> B >> M >> K >> AA >> BB >> MM;
        for (int u = 0; u < n; ++u) {
            r = (A * r + B) % M; rr = (AA * rr + BB) % MM;
            auto &U = vozl[u]; U.stars = (u == 0) ? -1 : (u - 1 - r % min((long long) u, K)); U.barva = rr % b; } }
    // Pripravimo sezname vozlišč posamezne barve.
    for (int u = 0; u < n; ++u) { auto &U = vozl[u];
        if (U.stars < 0) koren = u;
        else { auto &P = vozl[U.stars]; U.sor = P.otrok; P.otrok = u; } 
        U.naslIsteBarve = prviPoBarvi[U.barva]; prviPoBarvi[U.barva] = u; }
    // Za vsako vozlišče u določimo njegovega starša v skrčenem drevesu za u-jevo barvo.
    zadnjiPoBarvi.resize(b, -1); DolociStarseB(koren);
    // Obdelajmo vse barve.
    long long vsota = 0;
    for (c = 0; c < b; ++c)
    {
        vector<StVozlisca> vozlS; // vozlišča v skrčenem drevesu
        // Pripravimo skrčeno drevo za barvo 'c'.
        for (StVozlisca u = prviPoBarvi[c]; u >= 0; u = vozl[u].naslIsteBarve)
        {
            // 'u' je barve 'c', zato ga je treba dodati v skrčeno drevo,
            // kjer bo njegov starš vozlišče U.starsB.
            auto &U = vozl[u]; 
            U.dodan = c; vozlS.emplace_back(u);
            if (U.starsB < 0) continue;
            auto &P = vozl[U.starsB]; U.sorS = P.otrokS; P.otrokS = u;
            // Če u-jev starš v skrčenem drevesu ni barve 'c', ga bo treba šele
            // dodati v drevo (razen če ga nismo že dodali pri kakšnem prejšnjem u).
            if (P.barva != c && P.dodan < c) {
                P.dodan = c; vozlS.emplace_back(U.starsB); 
                // P-jev starš v skrčenem drevesu bo isto vozlišče, ki je njegov
                // starš tudi v prvotnem drevesu (in ki je gotovo barve 'c').
                if (P.stars >= 0) { auto &G = vozl[P.stars]; 
                    P.sorS = G.otrokS; G.otrokS = U.starsB; } }
        }
        // Rešimo nalogo v skrčenem drevesu.
        int maxLepa = ObdelajSkrcenoDrevo(koren).first;
        if (velikoDrevo) vsota += maxLepa; else cout << maxLepa << '\n'; 
        // Pospravimo skrčeno drevo.
        for (StVozlisca u : vozlS) { auto &U = vozl[u]; U.otrokS = -1; U.sorS = -1; }
    }
    if (velikoDrevo) cout << vsota << '\n';
    return 0;
}
