// Simon St James (ssjgz) 2017-09-16 11:21
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cassert>

using namespace std;

int64_t findMaxMoves(const vector<int64_t>& s, const map<int64_t, int64_t>& pileSizeHistogram, map<int64_t, int64_t>& maxMovesForPileSizeLookup)
{
    int64_t numMoves = 0;
    for (auto& pileSizeFrequency : pileSizeHistogram)
    {
        const int64_t pileSize = pileSizeFrequency.first;
        if (pileSizeFrequency.second == 0)
            continue;

        int64_t maxMovesForThisPile = 0;

        auto sElementIter = s.begin();
        while (*sElementIter < pileSize && sElementIter != s.end())
        {
            const auto sElement = *sElementIter;

            if ((pileSize % sElement) == 0)
            {
                const int64_t originalNumPiles = pileSizeFrequency.second;
                const int64_t numNewPiles = (pileSize / sElement) * originalNumPiles;
                const int64_t newPilesSize = sElement;

                map<int64_t, int64_t> newPileSizeHistogram;
                newPileSizeHistogram[newPilesSize] = 1; // Just find max moves for a single pile; result for all numNewPiles new piles is max moves * numNewPiles.
                auto lookedup = maxMovesForPileSizeLookup.find(newPilesSize);
                if (lookedup == maxMovesForPileSizeLookup.end())
                {
                    const int64_t maxMoves = findMaxMoves(s, newPileSizeHistogram, maxMovesForPileSizeLookup);
                    maxMovesForPileSizeLookup[newPilesSize] = maxMoves;
                    lookedup = maxMovesForPileSizeLookup.find(newPilesSize);
                }
                maxMovesForThisPile = max(maxMovesForThisPile, originalNumPiles + numNewPiles * lookedup->second);

            }
            sElementIter++;
        }
        numMoves += maxMovesForThisPile;
    }
    return numMoves;
}

int main()
{
    int q;
    cin >> q;
    for (int t = 0; t < q; t++)
    {
        int64_t n, m;
        cin >> n >> m;
        vector<int64_t> s(m);
        for (int i = 0; i < m; i++)
        {
            cin >> s[i];
        }
        sort(s.begin(), s.end());
        map<int64_t, int64_t> pileSizeHistogram;
        pileSizeHistogram[n] = 1;
        map<int64_t, int64_t> maxMovesForPileSizeLookup;
        const int64_t maxMoves = findMaxMoves(s, pileSizeHistogram, maxMovesForPileSizeLookup);
        cout << maxMoves << endl;
    }

}

