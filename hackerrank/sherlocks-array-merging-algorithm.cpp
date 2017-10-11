// Simon St James (ssjgz) - 2017-10-11 15:10
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

const int64_t mymodulus = 1'000'000'007;
vector<int64_t> factorialLookup;
vector<int64_t> factorialInverseLookup;

int64_t factorial(int64_t n)
{
    assert(n >= 0);
    return factorialLookup[n];
    int64_t result = 1;
    for (int64_t i = 1; i <= n; i++)
    {
        result  = (result * i) % mymodulus;
    }
    //cout << " factorial " << n << " = " << result << endl;
    assert(result == factorialLookup[n]);
    return result;
}

int64_t nCr(int64_t n, int64_t r)
{
    if (n < r)
        swap(n, r);
    assert(n >= 0 && r >= 0);
    int64_t result = factorial(n);
    result = (result * factorialInverseLookup[n - r]) % mymodulus;
    result = (result * factorialInverseLookup[r] ) % mymodulus;
    //cout << " nCr " << n << "," << r << " = " << result << endl;
    return result;
}

int64_t quickPower(int64_t n, int64_t m)
{
    // Raise n to the m mod modulus using as few multiplications as 
    // we can e.g. n ^ 8 ==  (((n^2)^2)^2).
    long result = 1;
    int power = 0;
    while (m > 0)
    {
        if (m & 1)
        {
            long subResult = n;
            for (int i = 0; i < power; i++)
            {
                subResult = (subResult * subResult) % mymodulus;
            }
            result = (result * subResult) % mymodulus;
        }
        m >>= 1;
        power++;
    }
    return result;
}

int64_t power(int64_t base, int64_t exponent)
{
#if 0
    int64_t dbgResult = 1;
    for (int i = 0; i < exponent; i++)
    {
        dbgResult = (dbgResult * base) % mymodulus;
    }
    assert(dbgResult == quickPower(base, exponent));
#endif
    auto result = quickPower(base, exponent);
    return result;
}

vector<vector<int64_t>> lookup;

int64_t solve2(vector<int>& array, const int posInArray, bool isFirst, int remaining, int layerSize, string indent = string())
{
    assert(remaining >= 0 && remaining < lookup.size());
    assert(layerSize >= 1 && layerSize < lookup[0].size());
    //cout << indent << "remaining: " << remaining << " posInArray: " << posInArray << " layerSize: " << layerSize << " isFirst: " << isFirst << endl;
    if (remaining == 0)
    {
        //cout << indent << "returning 1 as remaining == 0" << endl;
        return 1;
    }
    if (remaining < layerSize)
    {
        //cout << indent << "returning 0 as remaining < layerSize" << endl;
        return 0; 
    }
    if (lookup[remaining][layerSize] != -1)
        return lookup[remaining][layerSize];

    int64_t result = 0;
    bool layerIsInOrder = true;
    for (int i = posInArray + 1; i < posInArray + layerSize; i++)
    {
        if (array[i] < array[i - 1])
        {
            //cout << indent << "returning 0 as the next layerSize are not in order" << endl;
            layerIsInOrder = false;
            break;
        }
    }
    if (layerIsInOrder)
    {
        int64_t numPermutationsForThisLayer = 1;
        if (!isFirst)
        {
            numPermutationsForThisLayer = factorial(layerSize);
        }
        if (remaining != layerSize)
        {
            for (int nextLayerSize = layerSize; nextLayerSize >= 1; nextLayerSize--)
            {
                //int64_t contributionFromFirstLayer = (isFirst ? 1 : nCr(layerSize, layerSize - nextLayerSize));
                int64_t contributionFromFirstLayer = nCr(layerSize, layerSize - nextLayerSize);
                //cout << indent << "trying next layer size: " << nextLayerSize << " numPermutationsForThisLayer: " << numPermutationsForThisLayer << " contributionFromFirstLayer: " << contributionFromFirstLayer << endl;
                result = (result + (((numPermutationsForThisLayer * contributionFromFirstLayer) % mymodulus) * solve2(array, posInArray + layerSize, false, remaining - layerSize, nextLayerSize, indent + " ") % mymodulus)) % mymodulus;
            }
        }
        else
        {
            //cout << indent << "returning " << numPermutationsForThisLayer << " as remaining == layerSize " << numPermutationsForThisLayer << endl;
            result = numPermutationsForThisLayer;
        }
    }
    //cout << indent << "returning " << result << endl;
    assert(lookup[remaining][layerSize] == -1);
    lookup[remaining][layerSize] = result;
    return result;

}

int main()
{
    int M;
    cin >> M;

    vector<int> array(M);

    for (int i = 0; i < M; i++)
    {
        cin >> array[i];
    }

    lookup.resize(M + 1, vector<int64_t>(M + 1, -1));

    factorialLookup.resize(M + 1);
    factorialInverseLookup.resize(M + 1);
    factorialLookup[0] = 1;
    factorialInverseLookup[0] = 1;
    int64_t factorial = 1;
    for (int64_t i = 1; i <= M; i++)
    {
        factorial = (factorial * i) % mymodulus;
        factorialLookup[i] = factorial;
        const auto factorialInverse = quickPower(factorial, mymodulus - 2);
        assert((factorial * factorialInverse) % mymodulus == 1);
        factorialInverseLookup[i] = factorialInverse;
    }

    int64_t result = 0;
    for (int i = 1; i <= M; i++)
    {
        result += solve2(array, 0, true, M, i);
    }
    cout << result << endl;
}

