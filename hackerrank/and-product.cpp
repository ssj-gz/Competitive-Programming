#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;


int main() {
    int n;
    cin >> n;
    for (int i = 0; i < n; i++)
    {
        // Fairly easy one :) We do it bitwise.  If a does not have bit N set, then
        // obviously that bit will not appear in the result.
        // If a does have bit N set: note that, in consecutive numbers starting from 0, 
        // the bit N (value == 2 ** N) will be off for the first 2 ** N; then on for the next 
        // 2 ** N, then off for the next 2 ** N, etc.
        // So if a does have bit N set, we can compute the next number after a (x, say), which does
        // not have bit N set.  If x occurs after b, then N is set for all values between a and b (inclusive)
        // and so N will appear in the result; else, it isn't, and will not.
        // Need to be fairly subtle to avoid overflow!
        uint32_t a, b;
        cin >> a >> b;
        const uint32_t distance = b - a;

        uint32_t result = 0;
        for (uint32_t bitNum = 0; bitNum < 32; bitNum++)
        {
            const uint32_t powerOf2 = 1 << bitNum;
            if ((a & powerOf2) == 0)
                continue;

            // When was this bit last switched on?
            const uint32_t highestBeforeAWithBitSet = (a / powerOf2) * powerOf2;
            // ... so when will it next switch off?
            const uint32_t lowestAfterAWithoutBitSet = highestBeforeAWithBitSet + powerOf2;
            // We use this rather than the more obvious "lowestAfterAWithoutBitSet > b" 
            // due to the possibility of overflow.
            if (lowestAfterAWithoutBitSet - a > distance)
                result |= powerOf2;
        }
        cout << result << endl;
    }
    return 0;
}
