// Simon St James (ssjgz) - 2018-02-10
#include <iostream>
#include <vector>
#include <limits>
#include <cassert>

using namespace std;

int main()
{
    int n;
    cin >> n;

    vector<int64_t> x(n);
    for (int i= 0; i < n; i++)
        cin >> x[i];
    vector<int64_t> y(n);
    for (int i= 0; i < n; i++)
        cin >> y[i];
    vector<int64_t> z(n);
    for (int i= 0; i < n; i++)
        cin >> z[i];

    vector<int64_t> h(n);
    vector<int64_t> c(n);
    vector<int64_t> l(n);

    vector<int64_t> G(n);
    G[0] = 0;

    h[0] = x[0];
    c[0] = y[0];
    l[0] = z[0];

    vector<int64_t> F(n);
    F[0] = 0;

    vector<int64_t> maxHCCH(n);
    //maxHCCH[0] = h[0] 

    for (int i = 1; i < n; i++)
    {
        h[i] = x[i] ^ G[i - 1];
        c[i] = y[i] ^ G[i - 1];
        l[i] = z[i] ^ G[i - 1];

        int64_t maxInRange = numeric_limits<int64_t>::min();
        const auto jMax = i - l[i];
        for (int j = 0; j <= jMax; j++)
        {
            maxInRange = max(maxInRange, int64_t(h[j]) * c[i] - int64_t(c[j]) * h[i]);
        }
        F[i] = maxInRange;

        G[i] = (G[i - 1] + F[i]) % 1'000'000'007UL;

        //cout << "i: " << i << " jMax: " << jMax << " h: " << h[i] << " c: " << c[i] << " l: " << l[i] <<  "F: " << F[i] << " G: " << G[i] << endl;
    }

    cout << G[n - 1] << endl;

}
