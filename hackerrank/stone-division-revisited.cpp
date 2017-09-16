// Simon St James (ssjgz) 2017-09-16 11:21
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cassert>

using namespace std;

void findMaxMoves(const vector<int64_t>& s, int sElementBeginIndex, int64_t numMovesSoFar, int depth, map<int64_t, int64_t>& pileSizeHistogram, int64_t& highestMoves)
{
    if (numMovesSoFar > highestMoves)
        highestMoves = numMovesSoFar;
    const string indent(depth, ' ');
    cout << indent << "findMaxMoves: numMovesSoFar: " << numMovesSoFar << endl;
    //int numMoves = 0;
    int sElementIndex = sElementBeginIndex;
    int64_t largestPileSize = 0;
    auto pileSizeHistogramReverseIter = pileSizeHistogram.rbegin();
    while (pileSizeHistogramReverseIter != pileSizeHistogram.rend())
    {
        if (pileSizeHistogramReverseIter->second > 0)
        {
            largestPileSize = pileSizeHistogramReverseIter->first;
            break;
        }
        pileSizeHistogramReverseIter++;
    }
    cout << indent << "largestPileSize: " << largestPileSize << endl;
    while (sElementIndex < s.size())
    {
        const auto sElement = s[sElementIndex];
        cout << indent << "sElement: " << sElement << endl;
        if (sElement > largestPileSize)
            break;

        for (auto& pileSizeFrequency : pileSizeHistogram)
        {
            const int64_t pileSize = pileSizeFrequency.first;
            cout << indent << "pileSize " << pileSize << " occurs " << pileSizeFrequency.second << " times" << endl;
            if (pileSizeFrequency.second == 0)
                continue;

            if ((pileSize % sElement) == 0 && pileSize != sElement)
            {
                const int64_t originalNumPiles = pileSizeFrequency.second;
                const int64_t numNewPiles = (pileSize / sElement) * originalNumPiles;
                const int64_t newPilesSize = sElement;
                cout << indent << "divisible: newPilesSize: " << newPilesSize << " numNewPiles: " << numNewPiles << endl;

                assert(pileSizeHistogram[pileSize] != 0);
                pileSizeHistogram[newPilesSize] += numNewPiles;
                pileSizeHistogram[pileSize] = 0;

                findMaxMoves(s, sElementBeginIndex, numMovesSoFar + originalNumPiles, depth + 1, pileSizeHistogram, highestMoves);

                pileSizeHistogram[pileSize] = originalNumPiles;
                pileSizeHistogram[newPilesSize] -= numNewPiles;
            }
        }

        sElementIndex++;
    }
}

int main()
{
    int q;
    cin >> q;
    for (int t = 0; t < q; t++)
    {
        cout << " t: " << t << endl;
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
        int64_t highestMoves = 0;
        findMaxMoves(s, 0, 0, 0, pileSizeHistogram, highestMoves);
        cout << highestMoves << endl;
    }

}

