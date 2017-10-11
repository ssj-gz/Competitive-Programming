#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

struct ArrayInfo
{
    int arraySize = -1;
    int numArraysWithSize = -1;
};

int64_t factorial(int64_t n)
{
    assert(n >= 0);
    int64_t result = 1;
    for (int64_t i = 1; i <= n; i++)
    {
        result *= i;
    }
    //cout << " factorial " << n << " = " << result << endl;
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
        result *= base;
    }
    return result;
}

int64_t solve(vector<int>& array, const int posInArray,  int remaining, bool isFirst, int maxLayerLength, int previousLayerLength, string indent = string())
{
    if (remaining == 0)
    {
        cout << indent << "woo!" << endl;
        return 1;
    }
    int64_t result = 0;
    for (int numInLayer = 1; numInLayer <= maxLayerLength; numInLayer++)
    {
        for (int numWithLayerSize = 1; numWithLayerSize <= remaining; numWithLayerSize++)
        {
            cout << indent << "remaining: " << remaining << " posInArray: " << posInArray << " previousLayerLength: " << previousLayerLength << " numInLayer: " << numInLayer << " numWithLayerSize: " << numWithLayerSize << endl;
            bool ok = true;
            int newPosInArray = posInArray;
            for (int i = 0; i < numWithLayerSize; i++)
            {
                newPosInArray++;
                for (int k = 0; k < numInLayer - 1; k++)
                {
                    if (newPosInArray == array.size() || array[newPosInArray] < array[newPosInArray - 1])
                    {
                        ok = false;
                        cout << indent << "not ok" << endl;
                    }
                    newPosInArray++;
                }
            }
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
            cout << indent << "numPermutationForTheseLayers: " << numPermutationForTheseLayers << " contributionFromFirstLayer: " << contributionFromFirstLayer << endl;
            const int nextRemaining = remaining - (numInLayer * numWithLayerSize);
            if (nextRemaining >= 0)
            {
                assert(newPosInArray == posInArray + numWithLayerSize * numInLayer);
                const auto oldResult = result;
                result +=  numPermutationForTheseLayers * contributionFromFirstLayer * solve(array, newPosInArray, nextRemaining, false, numInLayer - 1, numInLayer, indent + " ");
                cout << indent << "result bumped to : " << result << " from " << oldResult << endl;

            }
        }
    }
    cout << indent << "returning " << result << endl;
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

    vector<ArrayInfo> arrayInfoSoFar;
    ArrayInfo blah;
    blah.arraySize = 0;
    blah.numArraysWithSize = 0;
    arrayInfoSoFar.push_back(blah);
    //int64_t result = 0;
    //solve(M, arrayInfoSoFar, M, result);
    const auto result = solve(array, 0, M, true, M, -1);
    cout << result << endl;
}

