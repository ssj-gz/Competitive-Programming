// Simon St James (ssjgz) - 2018-02-10
#include <iostream>
#include <vector>
#include <limits>
#include <cassert>

using namespace std;

int64_t calcF(int i, const vector<int64_t>& h, const vector<int64_t>& c, const vector<int64_t>& l)
{
    int64_t maxInRange = numeric_limits<int64_t>::min();
    for (int j = 1; j <= i - l[i]; j++)
    {
        const auto blah = h[j] * c[i] - c[j] * h[i];
        //cout << " j: " << j << " blah: " << blah << endl;
        maxInRange = max(maxInRange, blah);
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

    const auto M = 1'000'000'007UL;
    //cout << "M: " << M << endl;

    for (int i = 2; i <= n; i++)
    {
        h[i] = x[i] ^ G[i - 1];
        c[i] = y[i] ^ G[i - 1];
        l[i] = z[i] ^ G[i - 1];

        F[i] = calcF(i, h, c, l);

        G[i] = (G[i - 1] + F[i]) % M;

        //cout << "i: " << i << " h: " << h[i] << " c: " << c[i] << " l: " << l[i] <<  " F: " << F[i] << " G: " << G[i] << endl;
        assert(l[i] <= i - 1);
    }

    cout << G[n] << endl;

}
