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

void findGrundyNumbersForMoves(int minPileSize, int numStonesRemaining, vector<int>& pileSizesSoFar, vector<int>& grundyNumberForPileSizeLookup, vector<int>& grundyNumbersForMoves)
{
    if (numStonesRemaining == 0 && pileSizesSoFar.size() > 1)
    {
        // We have a partition of the original pile into several piles of different sizes i.e. a "Move" in the Game.
        // The Grundy number for this move is the xor of the Grundy numbers for each pile.
        vector<int> grundyNumberForEachPile;
        for (const auto pileSize : pileSizesSoFar)
        {
            grundyNumberForEachPile.push_back(findGrundyNumber(pileSize, grundyNumberForPileSizeLookup));
        }
        int grundyNumberForMove = 0;
        for (const auto grundyNumber : grundyNumberForEachPile)
        {
            grundyNumberForMove ^= grundyNumber;
        }

        grundyNumbersForMoves.push_back(grundyNumberForMove);

        return;
    }
    for (int pileSize = minPileSize; pileSize <= numStonesRemaining; pileSize++)
    {
        pileSizesSoFar.push_back(pileSize);

        // Recurse, splitting the remaining stones use a minimum pile size strictly greater than the one we just chose: this ensures
        // we don't end up with piles of the same size.
        findGrundyNumbersForMoves(pileSize + 1, numStonesRemaining - pileSize, pileSizesSoFar, grundyNumberForPileSizeLookup, grundyNumbersForMoves);

        pileSizesSoFar.pop_back();
    }
}

int findGrundyNumber(const int pileSize, vector<int>& grundyNumberForPileSizeLookup)
{
    if (grundyNumberForPileSizeLookup[pileSize] != -1)
        return grundyNumberForPileSizeLookup[pileSize];
    vector<int> grundyNumbersForMoves;
    vector<int> pileSizesSoFar;
    findGrundyNumbersForMoves(1, pileSize, pileSizesSoFar, grundyNumberForPileSizeLookup, grundyNumbersForMoves);

    const auto grundyNumber = mex(grundyNumbersForMoves);

    grundyNumberForPileSizeLookup[pileSize] = grundyNumber;
    
    return grundyNumber;
}

int main()
{
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

