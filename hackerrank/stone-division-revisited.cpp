// Simon St James (ssjgz) 2017-09-16 11:21
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cassert>

using namespace std;

map<int64_t, int64_t> lookup;

int64_t findMaxMoves(const vector<int64_t>& s, int sElementBeginIndex, int64_t numMovesSoFar, int depth, const map<int64_t, int64_t>& pileSizeHistogram, int64_t& highestMoves)
{
    const string indent(depth, ' ');
    int64_t numMoves = 0;
    for (auto& pileSizeFrequency : pileSizeHistogram)
    {
        const int64_t pileSize = pileSizeFrequency.first;
        //cout << indent << "pileSize " << pileSize << " occurs " << pileSizeFrequency.second << " times" << endl;
        //cout << "pileSize: " << pileSize << endl;
        if (pileSizeFrequency.second == 0)
            continue;

        int64_t maxMovesForThisPile = 0;

        auto sElementIter = s.begin();
        while (*sElementIter < pileSize && sElementIter != s.end())
        {
            const auto sElement = *sElementIter;
            //cout << indent << " sElement: " << sElement << endl;

            if ((pileSize % sElement) == 0)
            {
                const int64_t originalNumPiles = pileSizeFrequency.second;
                const int64_t numNewPiles = (pileSize / sElement) * originalNumPiles;
                const int64_t newPilesSize = sElement;
                //cout << indent << "divisible by " << sElement << " : newPilesSize: " << newPilesSize << " numNewPiles: " << numNewPiles << " originalNumPiles: " << originalNumPiles << endl;

                map<int64_t, int64_t> newPileSizeHistogram;
                newPileSizeHistogram[newPilesSize] = 1;
                auto lookedup = lookup.find(newPilesSize);
                if (lookedup == lookup.end())
                {
                    const int64_t maxMoves = findMaxMoves(s, sElementBeginIndex, numMovesSoFar + originalNumPiles, depth + 1, newPileSizeHistogram, highestMoves);
                    lookup[newPilesSize] = maxMoves;
                    lookedup = lookup.find(newPilesSize);
                }
                maxMovesForThisPile = max(maxMovesForThisPile, originalNumPiles + numNewPiles * lookedup->second);

            }
            sElementIter++;
        }
        //cout << indent << "maxMovesForThisPile: " << maxMovesForThisPile << endl;
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
        lookup.clear();
        //cout << " t: " << t << endl;
        int64_t n, m;
        cin >> n >> m;
        vector<int64_t> s(m);
        for (int i = 0; i < m; i++)
        {
            cin >> s[i];
        }
        sort(s.begin(), s.end());
#if 0
        auto sIter = s.begin();
        while (sIter != s.end())
        {
            bool sIterErased = false;
            for (auto sDiviserIter = s.begin(); sDiviserIter != sIter; sDiviserIter++)
            {
                if (*sIter != *sDiviserIter && *sDiviserIter != 1 && ((*sIter % *sDiviserIter) == 0))
                {
                    cout << "removing " << *sIter << " as it is a multiple of " << *sDiviserIter << endl;
                    //sIter = s.erase(sIter);
                    //sIterErased = true;
                    break;
                }
            }
            if (!sIterErased)
                sIter++;
        }
#endif
        map<int64_t, int64_t> pileSizeHistogram;
        pileSizeHistogram[n] = 1;
        int64_t highestMoves = 0;
        const int64_t blah = findMaxMoves(s, 0, 0, 0, pileSizeHistogram, highestMoves);
        cout << blah << endl;
    }

}

