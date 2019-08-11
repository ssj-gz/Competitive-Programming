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
            // Account for the "empty" left set, which has xorSum == 0.
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
    //    xorSum[l, r] = a[l] ^ a[l + 1] ^ ... ^ a[r];
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
    // The problem stipulates that j > i, but let's look at j = i anyway, and
    // consider xorSum[i, j - 1] ^ xorSum[j, k] (== xorSum[i, k] == 0, by assumption).
    //
    // Then xorSum[i, j - 1] is 0, as it is an "empty range".  xorSum[j, k] = xorSum[i, k] = 0.
    // That is, for j = i, xorSum[i, j - 1] = xorSum[k, j].
    //
    // Now let's increase j by 1, to i + 1.  Then the range [i, j - 1] is no longer empty; it 
    // now includes a[i].  The range [j, k] now has one less element in it: the a[i] has been
    // dropped off the front.  Thus changing j from i to i + 1, we change xorSum[i, j - 1] from
    // its previous value (0) to a[i] i.e. its previous value ^ a[i], and, by virtue of the fact 
    // that a[i] ^ a[i] == 0, we've changed xorSum[j, k] from its previous value to its previous value ^ a[i].
    // So after increasing j, we *still* have a[i, j - 1] == a[j, k].  Very interesting :)
    //
    // Let's increase j by one more, so now j = i + 2.  Compared to j = i + 2, the range [i, j - 1] now
    // incorporates a[2], so the value of xorSum[i, j - 1] changes from its previous value to its 
    // previous value ^ a[2].  Similarly, the range [j, k] now loses a[2] and so again, using the fact
    // that a[2] ^ a[2] == 0, the value of xorSum[j, k] changes from its previous value to its previous
    // value ^ a[2].  So, once again, xorSum[i, j - 1] == xorSum[j, k]!

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

