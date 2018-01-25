// Simon St James (ssjgz) - 2018-01-25
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int mex(const vector<int>& numbers)
{
    if (numbers.empty())
        return 0;
    auto numbersSorted = numbers;
    sort(numbersSorted.begin(), numbersSorted.end());

    int mex = 0;
    for (const auto number : numbersSorted)
    {
        if (number == mex)
            mex++;
        else if (number > mex)
            break;
    }

    return mex;
}

int findGrundyNumber(const int pileSize, vector<int>& grundyNumberForPileSizeLookup);

void findGrundyNumbersForMoves(int minPileSize, int numStonesRemaining, vector<int>& pileSizesSoFar, vector<int>& grundyNumberForPileSizeLookup, vector<int>& grundyNumbersForAllMoves)
{
    if (numStonesRemaining == 0 && pileSizesSoFar.size() > 1)
    {
        // We have a partition of the original pile into at least two piles of different sizes i.e. a "Move" in the Game.
        // The Grundy number for this move is the xor of the Grundy numbers for each pile in the partition.
        vector<int> grundyNumberForEachPile;
        for (const auto partitionPileSize : pileSizesSoFar)
        {
            grundyNumberForEachPile.push_back(findGrundyNumber(partitionPileSize, grundyNumberForPileSizeLookup));
        }
        int grundyNumberForMove = 0;
        for (const auto grundyNumberForPile : grundyNumberForEachPile)
        {
            grundyNumberForMove ^= grundyNumberForPile;
        }

        grundyNumbersForAllMoves.push_back(grundyNumberForMove);

        return;
    }
    for (int pileSize = minPileSize; pileSize <= numStonesRemaining; pileSize++)
    {
        pileSizesSoFar.push_back(pileSize);

        // Recurse, splitting the remaining stones use a minimum pile size strictly greater than the one we just chose: this ensures
        // we don't end up with piles of the same size.
        findGrundyNumbersForMoves(pileSize + 1, numStonesRemaining - pileSize, pileSizesSoFar, grundyNumberForPileSizeLookup, grundyNumbersForAllMoves);

        pileSizesSoFar.pop_back();
    }
}

int findGrundyNumber(const int pileSize, vector<int>& grundyNumberForPileSizeLookup)
{
    if (grundyNumberForPileSizeLookup[pileSize] != -1)
        return grundyNumberForPileSizeLookup[pileSize];
    vector<int> grundyNumbersForAllMoves;
    vector<int> pileSizesSoFar;
    findGrundyNumbersForMoves(1, pileSize, pileSizesSoFar, grundyNumberForPileSizeLookup, grundyNumbersForAllMoves);

    const auto grundyNumberForPileSize = mex(grundyNumbersForAllMoves);

    grundyNumberForPileSizeLookup[pileSize] = grundyNumberForPileSize;
    
    return grundyNumberForPileSize;
}

int main()
{
    // Easy one: as always, the Grundy sum for the game is just the xor of the Grundy sums of the list of pile sizes, so
    // the solution reduces to finding the Grundy sum for a given pile size (which cannot exceed 50).
    //
    // It's easy to enumerate all moves for a given pile size: just recursively find all ways of partitioning that pile size
    // into distinct pile sizes (each such partitioning corresponds to a "move") - for uniqueness, we assume that the partitioned
    // pile sizes are in strictly increasing order.  The Grundy number for the result of such a move
    // is - again - just the xor of the Grundy sums of each pile in the resulting partition, so a bit of dynamic programming
    // gets the job done!
    //
    // Once we have the Grundy numbers for each possible move for a given pile size, the Grundy number for that pile size is just
    // the mex of these.
    const int maxN = 50;
    vector<int> grundyNumberForPileSizeLookup(maxN + 1, -1);

    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int N;
        cin >> N;

        vector<int> numInPiles(N);

        for (int i = 0; i < N; i++)
        {
            cin >> numInPiles[i];
        }

        int xorSum = 0;
        for (const auto numInPile : numInPiles)
        {
            xorSum ^= findGrundyNumber(numInPile, grundyNumberForPileSizeLookup);
        }

        if (xorSum == 0)
            cout << "BOB";
        else
            cout << "ALICE";
        cout << endl;
    }
}

