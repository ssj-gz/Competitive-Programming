#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <cassert>

using namespace std;

int64_t findMaxMoves(const int64_t stickSize, map<int64_t, int64_t>& maxMovesLookup)
{
    if (stickSize == 1)
        return 1;
    if (maxMovesLookup.find(stickSize) != maxMovesLookup.end())
    {
        return maxMovesLookup[stickSize];
    }
    int64_t maxMoves = 0;
    const int64_t sqrtStickSize = sqrt(stickSize);
    for (int factor = 1; factor <= sqrtStickSize; factor++)
    {
        if ((stickSize % factor) == 0)
        {
            {
                const int64_t numParts = factor;
                const int64_t newStickSize = stickSize / factor;
                if (numParts != 1)
                {
                    const int64_t maxMovesFromHere = numParts * findMaxMoves(newStickSize, maxMovesLookup);
                    //cout << "stickSize: " << stickSize << " numParts: " << numParts << " newStickSize: " << newStickSize << " maxMovesFromHere: " << maxMovesFromHere << endl;
                    maxMoves = max(maxMoves, maxMovesFromHere);
                }
            }
            {
                const int64_t numParts = stickSize / factor;
                const int64_t newStickSize = factor;
                if (numParts != 1)
                {
                    const int64_t maxMovesFromHere = numParts * findMaxMoves(newStickSize, maxMovesLookup);
                    //cout << "stickSize: " << stickSize << " numParts: " << numParts << " newStickSize: " << newStickSize << " maxMovesFromHere: " << maxMovesFromHere << endl;
                    maxMoves = max(maxMoves, maxMovesFromHere);
                }
            }

        }
    }
   
    maxMoves++;

    maxMovesLookup[stickSize] = maxMoves;
    return maxMoves;
}

int main()
{
    int n;
    cin >> n;

    vector<int64_t> d(n);
    for (int i = 0; i < n; i++)
    {
        cin >> d[i];
    }

    int64_t maxMoves = 0;
    map<int64_t, int64_t> maxMovesLookup;
    for (const auto stickSize : d)
    {
        maxMoves += findMaxMoves(stickSize, maxMovesLookup);
    }
    cout << maxMoves << endl;
    
}

