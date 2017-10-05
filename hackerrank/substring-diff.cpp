// Simon St James (ssjgz) - 2017-10-05 14:30
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
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
        // Consider all suffixes where the suffix of a is sizeDiffAB larger
        // than the suffix of b, working backwards.
        int i = a.size() - 1 - sizeDiffAB;
        int j = b.size() - 1;

        int lastIndexInAOfMismatch = -1;
        int lastIndexInBOfMismatch = -1;

        int currentLengthWithAllowedMismatches = 0;
        int numMismatches = 0;

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
    // Fairly tricky one, I thought :) Anyway, the recurrence relation for the total number of mismatches (ignoring length)
    // is:
    //
    //  numMismatches[i][j] = (a[i] != a[j] ? 1 : 0) + numMismatches[i + 1][j + 1].
    //
    // So imagine that we knew the largest value of k such that a.substr(i + 1, k) and b.substr(j + 1, k) has <= S
    // mismatches, and that we also new numMismatches[i + 1][j + 1].  How can we find k for i, j?
    //
    // Well, if a[i] == b[j] then we're in luck: no more mismatches are introduced, so the suffixes a.substr(i, k + 1)
    // and b.substr(j, k + 1) have length k + 1 and <= S mismatches and, since k was maximal for i + 1, j + 1, so k + 1 is maximal for i, j.
    //
    // If a[i] != b[j], then we're perhaps still OK: if the computed value of numMismatches[i][j] is still <= S, then
    // k + 1 will again do for i, j.
    //
    // If, however, the new mismatch a[i] != b[j] bumps the total number of mismatches to S + 1, then we're in a bit
    // more trouble.  How do we find the maximal k for i, j now? Well, that's quite easy: just decrement k
    // until the last mismatch in the substrings a.substr(i, k) and b.substr(j, k) has been "knocked off the end", and this 
    // will be our k.
    //
    // Now, in general, performing this adjustment of k adds an extra O(N) (N = max(a.size(), b.size())) to our already O(N^2)
    // algorithm, leaving us with an intractable O(N^3).  However, by adjusting the order in which we process i, j, we can
    // amortise the cost of tracking the "last mismatch" into the cost of iterating over pairs i, j.
    //
    // The way we do this is, instead of the naive way of letting i, j range over 0 ... a.size() - 1 and 0 ... b.size() - 1
    // respectively, we instead fix the distance between i and j to some value (sizeDiffAB) and work backwards
    // through all pairs i, j with j = i + sizeDiffAB, and then do this over all sizeDiffAB.  We see that, in doing this, the 
    // position of the last match is always monotonically decreasing for a given sizeDiffAB: in fact, the number of 
    // values of lastIndexInAOfMismatch and lastIndexInBOfMismatch we consider for a given sizeDiffAB is bounded by the number
    // of pairs i, j we consider for this sizeDiffAB (lastIndexInAOfMismatch, lastIndexInBOfMismatch will always only 
    // every be equal to one of the pairs (i, j) that we've already processed for this sizeDiffAB, and will monotonically decrease).
    // Hmmm ... that's really hard to explain, but trust me that it is overall an O(N^2) algorithm ;)
    //
    // And that's it!
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
