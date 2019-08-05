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

    const int maxElement = *std::max_element(a.begin(), a.end());
    int powerOf2 = 1;
    while (powerOf2 <= maxElement)
    {
        powerOf2 <<= 1;
    }
    const int maxXorSum = powerOf2 - 1;

    vector<XorSumInfo> infoForXorSum(maxXorSum + 1);

#ifdef DEBUG_VERIFY
    vector<int> dbgXorSumUpTo;
    {
        int xorSum = 0;
        for (int i = 0; i < n; i++)
        {
            xorSum ^= a[i];
            assert(xorSum <= maxXorSum);
            dbgXorSumUpTo.push_back(xorSum);
        }
    }
#endif

    int xorSum = 0;
    for (int k = 0; k < n; k++)
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

#ifdef DEBUG_VERIFY
        int64_t dbgAmountToAdd = 0;
        for (int i = 0; i < k; i++)
        {
            if (dbgXorSumUpTo[i] == dbgXorSumUpTo[k])
            {
                dbgAmountToAdd += (k - i - 1);
            }
        }
        if (dbgXorSumUpTo[k] == 0)
            dbgAmountToAdd += k;

        cout << "xorSum: " << xorSum << " amountToAdd: " << amountToAdd << " dbgAmountToAdd: " << dbgAmountToAdd << endl;
        assert(dbgAmountToAdd == amountToAdd);
#endif

        result += amountToAdd;
    }


    return result;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    const int T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        vector<int> a(N);
        for (auto& x : a)
        {
            x = read<int>();
        }
        const auto numTriples = findNumTriples(a);
        cout << numTriples << endl;
    }
}

