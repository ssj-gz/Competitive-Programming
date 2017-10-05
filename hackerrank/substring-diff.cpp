// Simon St James (ssjgz) - 2017-10-05 14:30
#define BRUTE_FORCE
//#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#undef BRUTE_FORCE
#endif
#include <iostream>
#include <vector>
#include <string>
#include <cassert>

using namespace std;

// Calculates the largest L with M(i, j, L) <= S *and* i <= j.
int findLongestWithiLessThanj(const string& a, const string& b, int S)
{
    const int maxSizeDiffAB = a.size();

    int maxLengthWithAllowedMismatches = 0;
    for (int sizeDiffAB = 0; sizeDiffAB <= maxSizeDiffAB; sizeDiffAB++)
    {
        // Consider all suffixes were the suffix of a is sizeDiffAB larger
        // than the suffix of b.
        int lastIndexInAOfMismatch = -1;
        int lastIndexInBOfMismatch = -1;

        int i = a.size() - 1 - sizeDiffAB;
        int j = b.size() - 1;

        int currentLengthWithAllowedMismatches = 0;
        int numMismatches = 0;

        // Consider all pairs of suffix begin positions, working backwards, which give length of 
        // suffix of a = length of suffix of b + sizeDiffAB.
        while (i >= 0)
        {
            // Bump currentLengthWithAllowedMismatches indiscriminately; we'll adjust it if need be.
            currentLengthWithAllowedMismatches++;

            if (a[i] != b[j])
            {
                // Mismatch!
                numMismatches++;
                if (lastIndexInAOfMismatch == -1 || lastIndexInBOfMismatch == -1)
                {
                    lastIndexInAOfMismatch = i;
                    lastIndexInBOfMismatch = j;
                }

                if (numMismatches > S)
                {
                    assert(lastIndexInAOfMismatch != -1 && lastIndexInBOfMismatch != -1);
                    assert(lastIndexInAOfMismatch >= i);
                    assert(lastIndexInBOfMismatch == lastIndexInAOfMismatch + sizeDiffAB);

                    // Knock off the last mismatch between the suffixes of A and B, reducing the number of mismatches by 1 ...
                    numMismatches--;
                    // ... and use the position of this last mismatch to compute the new largest match between the current
                    // suffixes of a and b.
                    currentLengthWithAllowedMismatches = (lastIndexInAOfMismatch - i);

                    // Update lastIndexInAOfMismatch, lastIndexInBOfMismatch.  Note that, amortised over a given sizeDiffAB, 
                    // this takes time O(max(a.size(), b.size()) as, intuitively, for a given sizeDiffAB, they
                    // pass over no more values than do i and j.
                    while (lastIndexInAOfMismatch - 1 >= 0 && lastIndexInBOfMismatch - 1 >= 0)
                    {
                        lastIndexInAOfMismatch--;
                        lastIndexInBOfMismatch--;
                        if (a[lastIndexInAOfMismatch] != b[lastIndexInBOfMismatch])
                        {
                            break;
                        }
                    }
                }
            }

            maxLengthWithAllowedMismatches = max(maxLengthWithAllowedMismatches, currentLengthWithAllowedMismatches);
            i--;
            j--;
        }
    }
    return maxLengthWithAllowedMismatches;
}

int main()
{
    int T;
    cin >> T;
    
    for (int t = 0; t < T; t++)
    {
        int S;
        cin >> S;
        string a, b;
        cin >> a >> b;

        const auto result = max(findLongestWithiLessThanj(a, b, S), findLongestWithiLessThanj(b, a, S));
        cout <<  result << endl;
    }
}
