// Simon St James (ssjgz) - 2018-02-10
#include <iostream>
#include <vector>
#include <limits>
#include <cassert>

using namespace std;

int64_t calcF(const int i, const vector<int64_t>& h, const vector<int64_t>& c, const vector<int64_t>& l)
{
    cout << "calcF - i: " << i << endl;
    int64_t maxInRange = numeric_limits<int64_t>::min();
    for (int j = 1; j <= i - l[i]; j++)
    {
        const int64_t blah = h[j] * c[i] - c[j] * h[i];
        maxInRange = max(maxInRange, blah);
        cout << " j: " << j << " blah: " << blah << " maxInRange: " << maxInRange << endl;
    }

    return maxInRange;
}

int main()
{
    int n;
    cin >> n;

    vector<int64_t> x(n + 1);
    for (int i = 1; i <= n; i++)
        cin >> x[i];
    vector<int64_t> y(n + 1);
    for (int i= 1; i <= n; i++)
        cin >> y[i];
    vector<int64_t> z(n + 1);
    for (int i = 1; i <= n; i++)
        cin >> z[i];

    vector<int64_t> h(n + 1);
    vector<int64_t> c(n + 1);
    vector<int64_t> l(n + 1);

    h[1] = x[1];
    c[1] = y[1];
    l[1] = z[1];

    vector<int64_t> F(n + 1);
    F[1] = calcF(1, h, c, l);
    vector<int64_t> G(n + 1);
    G[1] = F[1];

    //vector<int64_t> maxHCCH(n);
    //maxHCCH[0] = h[0] 

    const int64_t M = 1000000007UL;
    //cout << "M: " << M << endl;

    {
        int i = 1;
        //cout << "i: " << i << " h: " << h[i] << " c: " << c[i] << " l: " << l[i] <<  " F: " << F[i] << " G: " << G[i] << endl;
    }
    for (int i = 2; i <= n; i++)
    {
        h[i] = x[i] ^ G[i - 1];
        c[i] = y[i] ^ G[i - 1];
        l[i] = z[i] ^ G[i - 1];

        F[i] = calcF(i, h, c, l);
        if (F[i] < 0)
        {
            const auto blee = (F[i] / -M) + 1;
            F[i] += blee * M;
        }

        G[i] = (G[i - 1] + F[i]) % M;

        cout << "i: " << i << " h: " << h[i] << " c: " << c[i] << " l: " << l[i] <<  " F: " << F[i] << " G: " << G[i] << endl;
        assert(l[i] <= i - 1);
        assert(F[i] >= 0);
        assert(1 <= c[i] && c[i] <= 10'000'000);
        assert(1 <= h[i] && h[i] <= 10'000'000);
        assert(h[i] > h[i - 1]);
    }

    cout << G[n] << endl;

}
