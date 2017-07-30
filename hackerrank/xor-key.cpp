#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>
using namespace std;

int main() {
    int T;
    cin >> T;
    for (int t = 0; t < T; t++)
    {
        long N, Q;
        cin >> N >> Q;

        vector<int> x(N);
        for (long i = 0; i < N; i++)
        {
            cin >> x[i];
        }
        // Fairly tricky one.  Say that the maxBit'th bit of a is 1:
        // then the x with maximum value of a ^ x has a 0 in the maxBit'th
        // place, so if in the range p <= i <= q there is an i where x[i]
        // has the maxBit'th equal to 0, then the eventual answer has the 
        // maxBit'th bit equal to 0.
        // Likewise, if the maxBit'th bit of a is 0, then the max value of a^x
        // will have a 1 in the maxBit'th place, so if there is an i where
        // x[i] has the maxBit'th equal to 1, then the eventual answer has
        // the maxBit'th bit equal to 1.
        // If, in either of the two cases, we *can't* find such an i with p <= i <= q
        // then we must compromise: the maxBit'th bit in the eventual answer 
        // is the same as that of a.
        // Whether we find it or not, we log the maxBit's value in bestMBitsInRange
        // and repeat to the process, trying to find the maximimum value this time
        // with the maxBit-1th value, and incorporating the current bestMBitsInRange.
        // Thus: on the mth iteration (0 <= m <= maxBits), we look at the  value of
        // the (maxBits - m)th bit of a, negate that (call it b), and see if we can find a
        // value in the allowed subarray whose first m-1 bits are the previous
        // bestMBitsInRange, and whose mth bit is b.
        // After all iterations are complete, bestMBitsInRange will have the highest value
        // in the allowed subarray when xor'd with a.
        const int maxBits = 15;
        const int maxXor = (1 << (maxBits + 1)) - 1;
        // Create a lookup table to help us: indicesWithHighMBits[val][m] is the 
        // (ordered) list of indices i in x such that the first m bits of x[i]
        // are equal to val i.e. if y = x[i] with all bits after the first m set to 0,
        // x & y == val.
        vector<vector<vector<long>>> indicesWithHighMBits(maxXor + 1, vector<vector<long>>(maxBits + 1));
        for (vector<int>::size_type i = 0; i < N; i++)
        {
            long highestMBits = 0;
            for (int highBit = maxBits; highBit >= 0; highBit--)
            {
                const int m = maxBits - highBit;
                highestMBits |= (1 << highBit);
                indicesWithHighMBits[x[i] & highestMBits][m].push_back(i);
            }
        }
        for (int l = 0; l < Q; l++)
        {
            long a, p, q;
            cin >> a >> p >> q;
            // Make range 0-relative.
            p--;
            q--;

            unsigned long bestMBitsInRange = 0;
            for (int highBit = maxBits; highBit >= 0; highBit--)
            {
                const int m = maxBits - highBit;
                const bool bestNextBitHasHighBitSet = ((a & (1 << highBit)) == 0);
                if (bestNextBitHasHighBitSet)
                {
                    // Speculatively set the next bit of bestMBitsInRange to 1;
                    // we'll have to set it back to 0 if it doesn't work out.
                    bestMBitsInRange |= (1 << highBit);
                }
                else
                {
                    // Next bit is 0, which is already 0 in bestMBitsInRange;
                    // do nothing.
                }

                const auto& indicesWithBestMHighBits = indicesWithHighMBits[bestMBitsInRange][m];
                const auto firstIndexInRange = lower_bound(indicesWithBestMHighBits.begin(), indicesWithBestMHighBits.end(), p);
                const auto lastIndexInRange = upper_bound(indicesWithBestMHighBits.begin(), indicesWithBestMHighBits.end(), q);
                // i.e. in the list of indices indicesWithBestMHighBits, is there an i
                // such that p <= i <= q?
                const bool haveElementInRangeWithBestHighBits = (firstIndexInRange != indicesWithBestMHighBits.end() && (*firstIndexInRange <= q) &&
                        (lastIndexInRange == indicesWithBestMHighBits.end() || *(lastIndexInRange - 1) <= q));
                if (!haveElementInRangeWithBestHighBits)
                {
                    // Have to compromise: we can't have our "best" value.
                    if (bestNextBitHasHighBitSet)
                    {
                        // We speculatively set the highBit of bestMBitsInRange to 0,
                        // but we couldn't find such a number in our range; revert it.
                        assert((bestMBitsInRange & (1 << highBit)) != 0);
                        bestMBitsInRange -= (1 << highBit);
                    }
                    else
                    {
                        bestMBitsInRange |= (1 << highBit);
                    }
                }
            }
            cout << (a ^ bestMBitsInRange) << endl;
        }

    }
    return 0;
}



