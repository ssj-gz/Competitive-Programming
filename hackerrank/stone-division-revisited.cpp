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
        const int64_t originalNumPiles = pileSizeFrequency.second;

        int64_t maxMovesForThisPile = 0;

        for (const auto sElement : s)
        {
            if (sElement >= pileSize)
                break;
            
            if ((pileSize % sElement) == 0)
            {
                // For *all* piles of this size, find max moves resulting from splitting the piles.
                const int64_t numNewPiles = (pileSize / sElement) * originalNumPiles;
                const int64_t newPilesSize = sElement;

                map<int64_t, int64_t> newPileSizeHistogram;
                newPileSizeHistogram[newPilesSize] = 1; // Just find max moves for a single pile; result for *all* numNewPiles new piles is maxMovesForPileSize * numNewPiles.
                                                        // Finding max moves for just a single pile allows us to memo-ise more easily.
                auto lookedupMaxMovesIter = maxMovesForPileSizeLookup.find(newPilesSize);
                if (lookedupMaxMovesIter == maxMovesForPileSizeLookup.end())
                {
                    const int64_t maxMovesForPileSize = findMaxMoves(s, newPileSizeHistogram, maxMovesForPileSizeLookup);
                    maxMovesForPileSizeLookup[newPilesSize] = maxMovesForPileSize;
                    lookedupMaxMovesIter = maxMovesForPileSizeLookup.find(newPilesSize);
                }
                const auto lookedupMaxMoves = lookedupMaxMovesIter->second;
                maxMovesForThisPile = max(maxMovesForThisPile, originalNumPiles + numNewPiles * lookedupMaxMoves);
            }
        }
        numMoves += maxMovesForThisPile;
    }
    return numMoves;
}

int main()
{
    // Ugh.  Severely misjudged this one, or perhaps more accurately, I was slightly led astray: historically,
    // the "Dynamic Programming" section of Algorithms has been a dumping-ground for things that often
    // don't require Dynamic Programming at all (people in the Discussions for such problems often complain
    // about this), so since this isn't in the Dynamic Programming section, I assumed that it wouldn't
    // need Dynamic Programming at all and spent ages trying to avoid it: big mistake, as it turns out
    // that the Editorial solution is very explicitly Dynamic Programming XD
    // Anyway, let's see how to solve this.  Firstly, note that the order in which we choose the x in S is important:
    // If n = 70, say, and S = { 7, 35 }, then choosing x = 7 first gives us 10 lots of 7-stone piles, and we can't
    // go any further.  Conversely, choosing x = 35 gives us 2 lots of 35-stone piles, and we can subdivide these
    // further by using x = 7.  Mutual co-primalcy and divisibility of elements of S obviously influence the
    // order in which we should choose elements of S (so if s1 and s2 are in S, and s1 divides s2, then we should
    // probably do s2 before s1) but in general I can't think of a good, optimal strategy, so I guess we have to
    // resort to trial and error.
    // The number of stones in a pile can be very large; likewise, the *number* of piles can also be very large 
    // (a pile of 10**12 stones can be split into 10**12/2 piles of size 2 if 2 is in S!), so we operate on
    // "histograms" (pileSizeHistogram) of pile sizes: pileSizeHistogram[y] is the number of piles with y stones.
    // Say there are z piles of stones of size y, and s1 and s2 are both in S and both divide y.  We might be
    // tempted to explore by splitting z' of the piles of size y by s1 and the remaining z - z' by s2; as it
    // happens, we *don't* need to do this: if some s in S divides y, then we should divide *all* z piles by
    // s, or none of them.  To see this, let max(y, s) be the maximum number of moves we can make from a pile
    // of size y if we choose s first.  If max(y, s1) == max(y, s2), then for any of the z' piles that we could
    // split by s1, we may as well split by s2 instead - it yields exactly the same max moves.  If max(y, s1) != max(y, s2),
    // then we should only split by the si that gives the max(y, si).
    // And that's about it - mainly just a trial-and-error splitting of y by s, memo-ising the max moves that can
    // be gained for each pile-size y.
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

