// Simon St James (ssjgz) - 2017-11-17
// This is just a "correctness" submission - it's too slow to pass all testcase!
#include <iostream>

using namespace std;

int64_t quickPower(int64_t n, uint64_t k, int64_t m)
{
    int64_t result = 1;
    int64_t power = 0;
    while (k > 0)
    {
        if (k & 1)
        {
            int64_t subResult = n;
            for (int i = 0; i < power; i++)
            {
                subResult = (subResult * subResult) % m;
            }
            result = (result * subResult) % m;
        }
        k >>= 1;
        power++;
    }
    return result;
}


constexpr auto modulus = 1'000'000'009ULL;

int main()
{
    int q;
    cin >> q;

    for (int t = 0; t < q; t++)
    {
        int n, k;
        cin >> n >> k;

        //cout << "n: " << n << " k: " << k << endl;

        int64_t total = 0;

        for (int i = 2; i <= n - 1; i++)
        {
            //cout << "i : " << i << " i ^^ k: " << quickPower(i, k, ::modulus) << endl;
            //total = (total + (n - 1 - i) * quickPower(i, k, ::modulus)) % ::modulus;
            total = (total + quickPower(i, k, ::modulus)) % ::modulus;
            //cout << "i: " << i << " total: " << total << endl;
            
        }
        cout << total << endl;
    }
}
