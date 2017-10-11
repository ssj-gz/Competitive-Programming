#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

struct ArrayInfo
{
    int arraySize = -1;
    int numArraysWithSize = -1;
};

const int64_t mymodulus = 1'000'000'007;
vector<int64_t> factorialLookup;

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
    result /= factorial(n - r);
    result /= factorial(r);
    //cout << " nCr " << n << "," << r << " = " << result << endl;
    return result;
}

int64_t power(int64_t base, int64_t exponent)
{
    int64_t result = 1;
    for (int i = 0; i < exponent; i++)
    {
        result = (result * base) % mymodulus;
    }
    return result;
}

vector<vector<int64_t>> lookup;

int64_t solve(vector<int>& array, const int posInArray,  int remaining, int previousLayerLength, string indent = string())
{
    if (remaining == 0)
    {
        //cout << indent << "woo!" << endl;
        return 1;
    }
    assert(remaining < lookup.size());
    assert(previousLayerLength == -1 || previousLayerLength < lookup[0].size());
    if (previousLayerLength != -1)
    {
        if (lookup[remaining][previousLayerLength] != -1)
        {
            //cout << "returning precomputed" << endl;
            return lookup[remaining][previousLayerLength];
        }
    }
    //cout << "previousLayerLength: " << previousLayerLength << " remaining: " << remaining << endl;
    int64_t result = 0;
    const int maxLayerLength = (previousLayerLength == -1 ? remaining : previousLayerLength - 1);
    for (int numInLayer = 1; numInLayer <= maxLayerLength; numInLayer++)
    {
        int newPosInArray = posInArray;
        for (int numWithLayerSize = 1; numWithLayerSize <= remaining; numWithLayerSize++)
        {
            //cout << indent << "remaining: " << remaining << " posInArray: " << posInArray << " previousLayerLength: " << previousLayerLength << " numInLayer: " << numInLayer << " numWithLayerSize: " << numWithLayerSize << endl;
#if 0
            bool dbgOk = true;
            int dbgNewPosInArray = posInArray;
            for (int i = 0; i < numWithLayerSize; i++)
            {
                dbgNewPosInArray++;
                for (int k = 0; k < numInLayer - 1; k++)
                {
                    if (dbgNewPosInArray == array.size() || array[dbgNewPosInArray] < array[dbgNewPosInArray - 1])
                    {
                        dbgOk = false;
                        //cout << indent << "not ok" << endl;
                    }
                    dbgNewPosInArray++;
                }
            }
#endif
            newPosInArray++;
            bool ok = true;
            for (int k = 0; k < numInLayer - 1; k++)
            {
                if (newPosInArray == array.size() || array[newPosInArray] < array[newPosInArray - 1])
                {
                    ok = false;
                    //cout << indent << "not ok" << endl;
                }
                newPosInArray++;
            }
            //assert(ok == dbgOk);
            //assert(newPosInArray == dbgNewPosInArray);
            if (!ok)
                break;
            int64_t numPermutationForTheseLayers = 1;
            if (previousLayerLength == -1)
            {
                if (numWithLayerSize != 1)
                {
                    //numPermutationForTheseLayers = (numWithLayerSize - 1) * factorial(numInLayer);
                    numPermutationForTheseLayers = power(factorial(numInLayer), numWithLayerSize - 1);
                }
            }
            else
            {
                //numPermutationForTheseLayers = numWithLayerSize * factorial(numInLayer);
                numPermutationForTheseLayers = power(factorial(numInLayer), numWithLayerSize);
            }
            int64_t contributionFromFirstLayer = (previousLayerLength == -1 ? 1 : nCr(previousLayerLength - numInLayer, previousLayerLength));
            //cout << indent << "numPermutationForTheseLayers: " << numPermutationForTheseLayers << " contributionFromFirstLayer: " << contributionFromFirstLayer << endl;
            const int nextRemaining = remaining - (numInLayer * numWithLayerSize) % mymodulus;
            if (nextRemaining >= 0)
            {
                assert(newPosInArray == posInArray + numWithLayerSize * numInLayer);
                const auto oldResult = result;
                result +=  (((numPermutationForTheseLayers * contributionFromFirstLayer) % mymodulus) * solve(array, newPosInArray, nextRemaining, numInLayer, indent + " ")) % mymodulus;
                //cout << indent << "result bumped to : " << result << " from " << oldResult << endl;

            }
        }
    }
    if (previousLayerLength != -1)
    {
        assert(result != -1);
        //cout << "storing previousLayerLength: " << previousLayerLength << " remaining: " << remaining << " result: " << result << endl;
        lookup[remaining][previousLayerLength] = result;
    }
    //cout << indent << "returning " << result << endl;
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
    factorialLookup[0] = 1;
    int64_t factorial = 1;
    for (int i = 1; i <= M; i++)
    {
        factorial = (factorial * i) % mymodulus;
        factorialLookup[i] = factorial;
    }

    vector<ArrayInfo> arrayInfoSoFar;
    ArrayInfo blah;
    blah.arraySize = 0;
    blah.numArraysWithSize = 0;
    arrayInfoSoFar.push_back(blah);
    //int64_t result = 0;
    //solve(M, arrayInfoSoFar, M, result);
    const auto result = solve(array, 0, M, -1);
    cout << result << endl;
}

