// Simon St James (ssjgz) - 2017-10-11 15:10
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>

using namespace std;

const int64_t modulus = 1'000'000'007ULL;

namespace 
{
    vector<int64_t> factorialLookup;
    vector<int64_t> factorialInverseLookup;

    int64_t factorial(int64_t n)
    {
        assert(n >= 0 && n < factorialLookup.size());
        return factorialLookup[n];
    }

    int64_t nCr(int64_t n, int64_t r, int64_t modulus)
    {
        assert(n >= 0 && r >= 0);
        assert(n >= r);
        int64_t result = factorial(n);
        assert(r < factorialInverseLookup.size());
        assert(n - r < factorialInverseLookup.size());
        result = (result * factorialInverseLookup[n - r]) % modulus;
        result = (result * factorialInverseLookup[r] ) % modulus;
        return result;
    }

    int64_t quickPower(int64_t base, int64_t exponent, int64_t modulus)
    {
        // Raise base to the exponent mod modulus using as few multiplications as 
        // we can e.g. base ^ 8 ==  (((base^2)^2)^2).
        int64_t result = 1;
        int64_t power = 0;
        while (exponent > 0)
        {
            if (exponent & 1)
            {
                int64_t subResult = base;
                for (int i = 0; i < power; i++)
                {
                    subResult = (subResult * subResult) % modulus;
                }
                result = (result * subResult) % modulus;
            }
            exponent >>= 1;
            power++;
        }
        return result;
    }

    void buildFactorialLookups(int maxN)
    {
        factorialLookup.resize(maxN + 1);
        factorialLookup[0] = 1;
        factorialInverseLookup.resize(maxN + 1);
        factorialInverseLookup[0] = 1;
        int64_t factorial = 1;
        for (int64_t i = 1; i <= maxN; i++)
        {
            factorial = (factorial * i) % ::modulus;
            factorialLookup[i] = factorial;
            const auto factorialInverse = quickPower(factorial, ::modulus - 2, ::modulus);
            assert((factorial * factorialInverse) % ::modulus == 1);
            factorialInverseLookup[i] = factorialInverse;
        }

    }

    vector<vector<int64_t>> lookup;
    vector<int> a;

}

int64_t findNumWaysOfFillingRemainingStartingWithLayerSize(int numRemaining, int layerSize)
{
    assert(numRemaining >= 0 && numRemaining < lookup.size());
    assert(layerSize >= 1 && layerSize < lookup[0].size());
    if (numRemaining == 0)
        return 1;
    if (numRemaining < layerSize)
        return 0; 

    int64_t& memoiseEntryRef = lookup[numRemaining][layerSize];
    if (memoiseEntryRef != -1)
        return memoiseEntryRef;

    int64_t result = 0;
    bool layerIsInOrder = true;
    const int posInArray = a.size() - numRemaining;
    for (int i = posInArray + 1; i < posInArray + layerSize; i++)
    {
        assert(i < a.size());
        assert(i - 1 >= 0);
        if (a[i] < a[i - 1])
        {
            layerIsInOrder = false;
            break;
        }
    }
    if (layerIsInOrder)
    {
        const bool isFirst = (numRemaining == a.size());
        const int64_t numPermutationsForThisLayer = isFirst ? 1 : factorial(layerSize);
        if (numRemaining != layerSize)
        {
            for (int nextLayerSize = layerSize; nextLayerSize >= 1; nextLayerSize--)
            {
                const auto contributionFromFirstLayer = nCr(layerSize, layerSize - nextLayerSize, ::modulus);
                const auto permutationFactor = (numPermutationsForThisLayer * contributionFromFirstLayer) % ::modulus;
                const auto nextLayerResult = findNumWaysOfFillingRemainingStartingWithLayerSize(numRemaining - layerSize, nextLayerSize);
                const auto adjustedNextLayerResult = (permutationFactor * nextLayerResult) % ::modulus;
                result = (result + adjustedNextLayerResult) % ::modulus;
            }
        }
        else
        {
            // Not enough elements left for further layers!
            result = numPermutationsForThisLayer;
        }
    }

    assert(result >= 0 && result < ::modulus);
    assert(memoiseEntryRef == -1);
    memoiseEntryRef = result;
    return result;

}

int main()
{
    int M;
    cin >> M;

    a.resize(M);
    for (int i = 0; i < M; i++)
    {
        cin >> a[i];
    }

    lookup.resize(M + 1, vector<int64_t>(M + 1, -1));

    buildFactorialLookups(M);

    int64_t result = 0;
    for (int bottomLayerSize = 1; bottomLayerSize <= M; bottomLayerSize++)
    {
        result = (result + findNumWaysOfFillingRemainingStartingWithLayerSize(M, bottomLayerSize)) % ::modulus;
    }
    cout << result << endl;
}

