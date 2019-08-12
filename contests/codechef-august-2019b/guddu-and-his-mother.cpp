// Simon St James (ssjgz) - 2019-08-03
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int findMaxXorSum(const vector<int>& a)
{
    const auto maxElement = *std::max_element(a.begin(), a.end());
    auto maxPowerOf2 = 1;
    while (maxPowerOf2 <= maxElement)
    {
        maxPowerOf2 <<= 1;
    }
    const auto maxXorSum = maxPowerOf2 - 1;

    return maxXorSum;
}

int64_t findNumTriples(const vector<int>& a)
{
    const int n = a.size();

    struct XorSumInfo
    {
        int lastOccurrencePos = -1;
        int64_t numTriplesEndingAtPreviousOccurrence = 0;
        int64_t numOccurrences = 0;
    };

    int64_t result = 0;

    const auto maxXorSum = findMaxXorSum(a);
    vector<XorSumInfo> infoForXorSum(maxXorSum + 1);

    auto xorSum = 0;
    for (auto k = 0; k < n; k++)
    {
        xorSum ^= a[k];

        auto& currentXorSumInfo = infoForXorSum[xorSum];

        assert(xorSum <= maxXorSum);
        int64_t numTriplesEndingAtK = 0;
        if (currentXorSumInfo.lastOccurrencePos != -1)
        {
            // Add amount from previous occurrence.
            numTriplesEndingAtK += (k - currentXorSumInfo.lastOccurrencePos - 1);
            // Add amounts from all occurrences before the previous occurence (all numOccurrences - 1 of them!).
            numTriplesEndingAtK += currentXorSumInfo.numTriplesEndingAtPreviousOccurrence + (k - currentXorSumInfo.lastOccurrencePos) * (currentXorSumInfo.numOccurrences - 1);
        }
        // Update currentXorSumInfo.
        currentXorSumInfo.numTriplesEndingAtPreviousOccurrence = numTriplesEndingAtK;
        currentXorSumInfo.lastOccurrencePos = k;
        currentXorSumInfo.numOccurrences++;

        if (xorSum == 0)
        {
            // Account for the "empty" left set, which has xorSum == 0; i.e. the case where "i" = 0, [0, i - 1]
            // is empty.
            numTriplesEndingAtK += k;
        }

        result += numTriplesEndingAtK;
    }


    return result;
}

int main(int argc, char* argv[])
{
    // Fairly easy one.  The original problem uses 1-relative indexing;
    // let's switch to 0-relative.
    //
    // Define
    //
    //    xorSum[l, r] = a[l] ^ a[l + 1] ^ ... ^ a[r]
    //
    // Then we asked to count the number of triples (i, j, k) with 
    // 0 <= i < j <= k < n such that:
    //
    //    xorSum[i, j - 1] = xorSum[j, k]
    //
    // Using the fact that X ^ X == 0 for any X >= 0, let's Xor both sides with
    // xorSum[j, k]:
    //
    //    xorSum[i, j - 1] ^ xorSum[j, k] = xorSum[j, k] ^ xorSum[j, k] = 0
    //
    // i.e.
    //
    //    xorSum[i, k] = 0
    //
    // That is:
    //
    // Lemma 1
    //
    // If xorSum[i, j - 1] = xorSum[j, k], then xorSum[i, k] == 0
    //
    // Proof
    //
    // As above ;)
    // 
    // QED
    //
    // This helps to restrict our search somewhat: for each k, we can restrict
    // the i's we consider to just those for which xorSum[i, k] == 0.  This doesn't
    // seem to help that much, though, as there can still be O(N) of these for each
    // of the N values of k, and we are still ignoring j altogether.
    //
    // Assume we have i < k such that xorSum[i, k] == 0: Is there an easy way of determining the required j's? 
    // For i <= j <= k, xorSum[i, k] = xorSum[i, j - 1] ^ xorSum[j, k], by definition of xorSum.
    // But xorSum[i, k] == 0 by assumption, and a well-known property of xor is that A ^ B == 0 <=> A == B.
    // Thus: 
    // 
    // Lemma 2 
    // 
    // For any j, i <= j <= k, xorSum[i, k] == 0 => xorSum[i, j - 1] == xorSum[j, k].
    //
    // Proof
    //
    // As above ;)
    // 
    // QED
    //
    // We're interested in j such that i < j <= k, and there are (k - i) such values of j, so:
    //
    // Corollary 
    //
    // For every pair i, k, i < k, such that xorSum[i, k] == 0, we can add (k - i) triples
    // to the total.
    //
    // Proof
    //
    // As above ;)
    // 
    // QED
    //
    // What next? If xorSum[i, k] == 0, then xorSum[0, i - 1] ^ xorSum[i, k] == xorSum[0, i - 1] i.e.
    // xorSum[0, k] == xorSum[0, i - 1] i.e. 
    //
    // Lemma 3
    //
    // xorSum[i, k] == 0 if and only if xorSum[0, i - 1] == xorSum[0, k].
    //
    // Proof
    //
    // As above ;)
    // 
    // QED
    //
    // Therefore:
    //
    // Theorem 1
    //
    // For every pair l and k such that l < k and xorSum[0, l] == xorSum[0, k], we can add (k - (l + 1)) = (k - l - 1)
    // to the number of triples.
    //
    // Proof
    //
    // As above ;)
    // 
    // QED
    //
    // Let numTriplesEndingAt[k] = sum [l < k and xorSum(0, l) == xorSum(0, k)] { k - l - 1 }
    //
    // Thus to solve the problem we need to compute, for each k = 0, 1 ... n - 1, numTriplesEndingAt[k].
    //
    // Unfortunately, there can still be O(N) such l's for each of the N k's.  Can we do better? Let X be the value of xorSum[0, k].
    // Assume k is the mth element with xorSum[0, k] == X i.e. the set of l < k with xorSum[0, l] == xorSum[0, k] = {X1, X2, ... , Xm = k},
    // X1 < X2 < ... < Xm.  Let's go back in time and assume that we calculated numTriplesEndingAt for k = X_(m-1) i.e. that we know 
    // the answer to numTriplesEndingAt[X_(m-1)].
    //
    // Can we use this to compute the new sum, numTriplesEndingAt[X_m]?
    //
    // We can re-write numTriplesEndingAt[X_(m-1)], using the definitions of the X_i's, as:
    //
    //     numTriplesEndingAt[X_(m-1)] = sum [i = 1 to m - 2] { X_(m-1) - X_i - 1 }
    //
    // Similarly, 
    //
    //     numTriplesEndingAt[X_m] = sum [i = 1 to m - 1] { X_m - X_i - 1 }
    //
    // We can re-write this as:
    //
    //     numTriplesEndingAt[X_m] = sum [i = 1 to m - 1] { X_m - X_i - 1 }
    //                             = X_m - X_(m-1) - 1 + sum [i = 1 to m - 2] { X_m - X_i - 1}
    //                             = X_m - X_(m-1) - 1 + sum [i = 1 to m - 2] { Xm - X_(m_1) + X_(m-1) - X_i - 1}
    //                             = X_m - X_(m-1) - 1 + sum [i = 1 to m - 2] { X_(m-1) - X_i - 1 } + (m - 2) * (Xm - X_(m-1))
    //                             = X_m - X_(m-1) - 1 + (m - 2) * (Xm - X_(m-1)) + numTriplesEndingAt[X_(m-1)]
    //
    // Note that m - 2 is the number of elements l < k such that xorSum[0, l] == xorSum[0, k], less one.
    //
    // Thus, with a little bit of book-keeping, we can use the value we computed for numTriplesEndingAt[X_(m-1)] to compute the value of numTriplesEndingAt[X_m] in O(1).
    //
    // In the code, currentXorSumInfo keeps track of all the pertinent stuff.
    //
    // And that's about it! One implementation detail, though - there's /an edge case occurring when xorSum[0, k] == 0 - our currentXorSumInfo
    // doesn't contain information about the "empty range", which also has xorSum equal to 0 (this corresponds to i = 0).  So we need
    // to consider the case xorSum[i, k] == 0, i == 0 case separately - just add k - i == k, as per Theorem 2.  And that really is it!
    ios::sync_with_stdio(false);

    const auto T = read<int>();

    for (auto t = 0; t < T; t++)
    {
        const auto N = read<int>();
        vector<int> a(N);
        for (auto& x : a)
        {
            x = read<int>();
        }
        const auto numTriples = findNumTriples(a);
        cout << numTriples << endl;
    }
}

