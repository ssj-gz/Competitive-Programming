//#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <array>
#include <map>
#include <algorithm>
#include <cassert>

using namespace std;

constexpr int numComponents = 4;

uint64_t bruteForce(const array<int, numComponents>& maxComponentValues)
{
    uint64_t numBeautiful = 0;
    uint64_t numDistinct = 0;
    for (int i = 1; i <= maxComponentValues[0]; i++)
    {
        for (int j = i; j <= maxComponentValues[1]; j++)
        {
            for (int k = j; k <= maxComponentValues[2]; k++)
            {
                for (int l = k; l <= maxComponentValues[3]; l++)
                {
                    const bool isBeautiful = ((i ^ j ^ k ^ l) != 0);
                    if (isBeautiful)
                        numBeautiful++;
                    numDistinct++;
                }
            }
        }
    }
    //cout << "numDistinct: " << numDistinct << " numBeautiful: " << numBeautiful << " num non-beautiful: " << (numDistinct - numBeautiful)  << endl;
    return numBeautiful;
}

uint64_t optimised(const array<int, numComponents>& maxComponentValues)
{
    constexpr int maxXor = 4095;
    uint64_t numUnbeautiful = 0;
    uint64_t numDistinct = 0;
    uint64_t numDistinctWX = 0; // Total number of pairs W, X (W <= X, W <= maxComponentValues[0], X <= maxComponentValues[1]) we've seen so far.
    int numWXXorsWithValue[maxXor] = {};
    for (int Y = 1; Y <= maxComponentValues[2]; Y++)
    {
        uint64_t numDistinctYZForThisY = 0; // Num distinct pairs Y, Z for this Y.
        int numYZXorsWithValueForThisY[maxXor] = {};
        // Generate Y ^ Z for this Y.
        for (int Z = Y; Z <= maxComponentValues[3]; Z++)
        {
            numDistinctYZForThisY++;
            numYZXorsWithValueForThisY[Y ^ Z]++;
        }
        if (maxComponentValues[1] >= Y)
        {
            // Bump W; xor it with all X satisfying X <= maxComponentValues[0] and X <= W (<= Y) to find
            // numYZXorsWithValue.
            for (int X = 1; X <= min(maxComponentValues[0], Y); X++)
            {
                numWXXorsWithValue[X ^ Y]++;
                numDistinctWX++;
            } 
        }
        uint64_t numUnbeautifulForThisY = 0;
        for (int value = 0; value <= maxXor; value++)
        {
            numUnbeautifulForThisY += numWXXorsWithValue[value] * numYZXorsWithValueForThisY[value];
        }

        numUnbeautiful += numUnbeautifulForThisY;
        numDistinct += (numDistinctWX * numDistinctYZForThisY);
    }
    //cout << "numDistinct: " << numDistinct << endl;
    return numDistinct - numUnbeautiful;
}

int main()
{
#define BRUTE_FORCE
#define RANDOM
    array<int, numComponents> maxComponentValues;
#ifdef RANDOM
    while (true) 
    {
        for (int i = 0; i < numComponents; i++)
        {
            maxComponentValues[i] = rand() % 100 + 1;
        }
#else
    for (int i = 0; i < numComponents; i++)
    {
        cin >> maxComponentValues[i];
    }
#endif
    sort(maxComponentValues.begin(), maxComponentValues.end());

    const auto numBeautifulOptimised = optimised(maxComponentValues);
    cout << numBeautifulOptimised << endl;
#ifdef BRUTE_FORCE
    const auto numBeautifulBruteForce = bruteForce(maxComponentValues);
    cout << "numBeautifulBruteForce: " << numBeautifulBruteForce << " numBeautifulOptimised: " << numBeautifulOptimised << endl;
    assert(numBeautifulBruteForce == numBeautifulOptimised);
#endif
#ifdef RANDOM
    }
#endif
}
