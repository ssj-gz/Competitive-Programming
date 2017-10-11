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
    cout << " factorial " << n << " = " << result << endl;
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
    cout << " nCr " << n << "," << r << " = " << result << endl;
    return result;
}

void solve(int M, vector<ArrayInfo>& arrayInfoSoFar, int remaining, int64_t& result)
{
    cout << "remaining: " << remaining << " arrayInfoSoFar.size(): " << arrayInfoSoFar.size() << endl;
    if (remaining == 0)
    {
        cout << " woo:" << endl;
        int dbgNumValues = 0;
        for (const auto& arrayInfo : arrayInfoSoFar)
        {
            dbgNumValues += arrayInfo.numArraysWithSize * arrayInfo.arraySize;
            cout << " [arraySize: " << arrayInfo.arraySize << " numArraysWithSize: " << arrayInfo.numArraysWithSize << "]";
        }
        cout << endl;
        assert(dbgNumValues == M);
        int64_t increase = 1;
        int64_t numArraysRemaining = 0;
        for (const auto& arrayInfo : arrayInfoSoFar)
        {
            numArraysRemaining += arrayInfo.numArraysWithSize;
        }
        cout << "numArraysRemaining: " << numArraysRemaining << endl;
        const auto origNumArraysRemaining = numArraysRemaining;
        for (const auto& arrayInfo : arrayInfoSoFar)
        {
            assert(numArraysRemaining >= 0);
            increase *= nCr(arrayInfo.numArraysWithSize, numArraysRemaining);
            numArraysRemaining -= arrayInfo.numArraysWithSize;
        }
        numArraysRemaining = origNumArraysRemaining;
        int64_t increaseFromFactorials = 1;
        int beginIndex = 1;
        if (arrayInfoSoFar[1].arraySize == 1)
        {
            numArraysRemaining -= arrayInfoSoFar[1].numArraysWithSize;
            beginIndex++;
        }
        for (int i = beginIndex; i < arrayInfoSoFar.size(); i++)
        {
            const auto& arrayInfo = arrayInfoSoFar[i];
            cout << " i: " << i << " arrayInfo.numArraysWithSize: " << arrayInfo.numArraysWithSize << " numArraysRemaining: " << numArraysRemaining << endl;
            assert(numArraysRemaining >= 0);
            //if (arrayInfo.numArraysWithSize == 0)
            //continue; 
            const int adjustedNumArraysWithSize = (arrayInfo.numArraysWithSize - (i == 1 ? 1 : 0));
            cout << "i: " << i << "  adjustedNumArraysWithSize: " << adjustedNumArraysWithSize << endl;
            if (adjustedNumArraysWithSize != 0)
            {
                //increaseFromFactorials *= adjustedNumArraysWithSize * factorial(numArraysRemaining);
                increaseFromFactorials *= adjustedNumArraysWithSize * factorial(numArraysRemaining);
            }
            numArraysRemaining -= arrayInfo.numArraysWithSize;
        }
        cout << " increase from factorials: " << increaseFromFactorials << endl;
        increase *= increaseFromFactorials;
        cout << "increase: " << increase << endl;
        result += increase;
        return;
    }
    for (int arraySize = arrayInfoSoFar.back().arraySize + 1; arraySize <= remaining; arraySize++)
    {
        for (int numArraysWithSize = 1; numArraysWithSize <= remaining; numArraysWithSize++)
        {
            cout << " trying arraySize: " << arraySize << " numArraysWithSize: " << numArraysWithSize << " remaining: " << remaining << endl;
            const int nextRemaining = remaining - arraySize * numArraysWithSize;
            if (nextRemaining >= 0)
            {
                ArrayInfo arrayInfo;
                arrayInfo.arraySize = arraySize;
                arrayInfo.numArraysWithSize = numArraysWithSize;

                arrayInfoSoFar.push_back(arrayInfo);
                solve(M, arrayInfoSoFar, nextRemaining, result);
                arrayInfoSoFar.pop_back();
            }
        }
    }

}

int main()
{
    int M;
    cin >> M;

    vector<ArrayInfo> arrayInfoSoFar;
    ArrayInfo blah;
    blah.arraySize = 0;
    blah.numArraysWithSize = 0;
    arrayInfoSoFar.push_back(blah);
    int64_t result = 0;
    solve(M, arrayInfoSoFar, M, result);
    cout << result << endl;
}

