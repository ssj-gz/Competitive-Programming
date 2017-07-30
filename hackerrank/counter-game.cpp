#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

int main() {
    int T;
    cin >> T;
    const uint64_t highestBit = static_cast<uint64_t>(1) << 63;
    for (int t = 0; t < T; t++)
    {
        uint64_t n;
        cin >> n;
        bool isLouisesTurn = true;
        while (n != 1)
        {
            const bool nIsPowerOf2 = ((n & (n - 1)) == 0); // Well-known trick - see e.g. "Hacker's Delight".
            if (!nIsPowerOf2)
            {
                // Remove the most significant 1.  Deal with the
                // highest bit as a special case as otherwise
                // we might get overflow when computing the highest
                // power of 2 <= n.
                if (n >= highestBit)
                {
                    n -= highestBit;
                }
                else
                {
                    uint64_t powerOf2 = 1;
                    while (powerOf2 * 2 <= n)
                    {
                        powerOf2 <<= 1;
                    }
                    n -= powerOf2;
                }
            }
            else
            {
                n >>= 1;
            }
            isLouisesTurn = !isLouisesTurn;
        }
        if (isLouisesTurn)
        {
            cout << "Richard" << endl;
        }
        else
        {
            cout << "Louise" << endl;
        }
    }
    return 0;
}

