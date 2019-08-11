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
        int64_t amountAddedAtLastOccurrence = 0;
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
        int64_t amountToAdd = 0;
        if (currentXorSumInfo.lastOccurrencePos != -1)
        {
            // Add amount from previous occurrence.
            amountToAdd += (k - currentXorSumInfo.lastOccurrencePos - 1);
            // Add amounts from all occurrences before the previous occurence (all numOccurrences - 1 of them!).
            amountToAdd += currentXorSumInfo.amountAddedAtLastOccurrence + (k - currentXorSumInfo.lastOccurrencePos) * (currentXorSumInfo.numOccurrences - 1);
        }
        // Update currentXorSumInfo.
        currentXorSumInfo.amountAddedAtLastOccurrence = amountToAdd;
        currentXorSumInfo.lastOccurrencePos = k;
        currentXorSumInfo.numOccurrences++;

        if (xorSum == 0)
        {
            // Account for the "empty" left set, which has xorSum == 0; i.e. the case where "i" = 0, [0, i - 1]
            // is empty.
            amountToAdd += k;
        }

        result += amountToAdd;
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
    //    if xorSum[i, j - 1] = xorSum[j, k], then xorSum[i, k] == 0
    //
    // This helps to restrict our search somewhat: for each k, we can restrict
    // the i's we consider to just those for which xorSum[i, k] == 0.  This doesn't
    // seem to help that much, though, as there can still be O(N) of these for each
    // of the N values of k, and we are still ignoring j altogether.
    //
    // However, let's assume that we have i and k such that xorSum[i, k] == 0.
    // For p = 0, 1, ... k - i, let LeftXor(p) = xorSum[i, i - 1 + p] and RightXor(p) = xorSum[i + p, k].
    //
    // For p = 0, LeftXor(p) = xorSum[i, i - 1 + p]  = xorSum[i, i - 1] is 0, as [i, i - 1] is an "empty range".  
    // RightXor(p) = xorSum[i + p, k] = xorSum[i, k] also equals 0, from assumption.  That is, for p == 0, LeftXor(p) ==
    // RightXor(p).
    //
    // Now let's increase p by 1 to p == 1.  Then the range [i, i - 1 + p] = [i, i] is no longer empty; it 
    // now includes a[i].  Thus LeftXor(1) = LeftXor(0) ^ a[i].  
    // The range [i + p, k] == [i + 1, k] now has one less element in it compared to
    // [i, k]: the a[i] has been dropped off the front.  Thus, by virtue of the fact that a[i] ^ a[i] == 0, 
    // RightXor(1) = RightXor(0) ^ a[i].  That is, for p == 1, we *still* have LeftXor(p) == RightXor(p).
    // Very interesting :)
    //
    // Let's increase p by 1 again, to p == 2.  Then the range [i, i - 1 + p] == [i, i - 1 + 2] = [i, i + 1]
    // now includes a[i + 1], so LeftXor(2) = LeftXor(1) ^ a[i + 1].  Similarly, the range [i + p, k] == [i + 2, k]
    // no longer includes a[i + 1], so RightXor(2) = RightXor(1) ^ a[i + 1], and again, LeftXor(p) = RightXor(p)!
    //
    // In general, then, it's hopefully obvious that for p >= 1, LeftXor(p) = LeftXor(p - 1) ^ a[i + p - 1] and RightXor(p) = 
    // RightXor(p - 1) ^ a[i + p - 1].  Since LeftXor(0) == RightXor(0), we can see by induction that LeftXor(p) = RightXor(p)
    // for all p = 0, 1, ... k - i.
    //
    // Therefore, if xorSum[i, k] == 0:
    //
    //    LeftXor(0) == RightXor(0) => xorSum[i, i - 1] == xorSum[i, k]
    //    LeftXor(1) == RightXor(1) => xorSum[i, i] == xorSum[i + 1, k]
    //    LeftXor(2) == RightXor(2) => xorSum[i, i + 1] == xorSum[i + 2, k]
    //      ....
    //    LeftXor(k - i) == RightXor(k - i) => xorSum[i, i - 1 + k - i] == xorSum[i + k - i, k]
    //
    // Or, written a different way:
    //
    //    xorSum[i, j - 1] == xorSum[j, k] for j = i
    //    xorSum[i, j - 1] == xorSum[j, k] for j = i + 1
    //    xorSum[i, j - 1] == xorSum[j, k] for j = i + 2
    //      ....
    //    xorSum[i, j - 1] == xorSum[j, k] for j = k - i
    //
    // That is, for the (k - i) values j = i + 1, j = i + 2, ... j = k - i, we have that
    // i < j <= k and xorSum[i, j - 1] == xorSum[j, k] (recall that j == i is not allowed); that is:
    //
    // Theorem 1
    //
    // For every pair i, k, i < k, such that xorSum[i, k] == 0, we can add (k - i) triples
    // to the total.
    //
    // Proof omitted; all the work above serves as a sketch proof.
    //
    // What next? If xorSum[i, k] == 0, then xorSum[0, i - 1] ^ xorSum[i, k] == xorSum[0, i - 1] i.e.
    // xorSum[0, k] == xorSum[0, i - 1] i.e. 
    //
    // Lemma 2
    //
    // xorSum[i, k] == 0 if and only if xorSum[0, i - 1] == xorSum[0, k].
    //
    // Therefore:
    //
    // Theorem 3
    //
    // For every pair l and k such that l < k and xorSum[0, l] == xorSum[0, k], we can add (k - (l + 1)) = (k - l - 1)
    // to the number of triples.
    //
    // Proof is immediate from combining Theorem 1 and Lemma 2.
    //
    // Thus to solve the problem we need to compute, for each k = 0, 1 ... n - 1:
    //
    //     sum [l < k and xorSum(0, l) == xorSum(0, k)] { k - l - 1 }
    //
    // Unfortunately, there can still be O(N) such l's for each of the N k's.  Can we do better? Let X be the value of xorSum[0, k].
    // Assume k is the mth element with xorSum[0, k] == X i.e. the set of l < k with xorSum[0, l] == xorSum[0, k] = {X1, X2, ... , Xm = k},
    // X1 < X2 < ... < Xm.  Let's go back in time and assume that we calculated the desired sum for k = X_(m-1) i.e. that we know 
    // the answer to
    //
    //     sum [i = 1 to m - 2] { X_(m-1) - X_i - 1 }          (1)
    //
    // Can we use this to compute the new sum
    //
    //     sum [i = 1 to m - 1] { X_m - X_i - 1 }              (2)
    //
    // ?
    //
    // We can re-write (1) as
    //
    //     sum [i = 1 to m - 1] { X_m - X_i - 1 }  = X_m - X_(m-1) - 1 + sum [i = 1 to m - 2] { X_m - X_i - 1}
    //                                             = X_m - X_(m-1) - 1 + sum [i = 1 to m - 2] { Xm - X_(m_1) + X_(m-1) - X_i - 1}
    //                                             = X_m - X_(m-1) - 1 + sum [i = 1 to m - 2] { X_(m-1) - X_i - 1 } + (m - 2) * (Xm - X_(m-1))
    //                                             = X_m - X_(m-1) - 1 + (m - 2) * (Xm - X_(m-1)) + [ Equation (1) goes here! ]
    //
    // where m - 2 is the number of elements l < k such that xorSum[0, l] == xorSum[0, k], less one.
    //
    // Thus, with a little bit of book-keeping, we can use the value we computed for K = X_(m-1) to compute the value for K = X_m in O(1).
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

