#include <iostream>
#include <vector>
#include <set>

using namespace std;

const int maxPileSize = 600;
vector<int> grundyNumberForPileSizeLookup(maxPileSize + 1, -1);

void computeGrundyNumbersForMovesWithPileSize(int numStonesRemaining, int nextMinStones, vector<int>& pilesSoFar, set<int>& grundyNumbers);

int grundyNumberForPileSize(int pileSize)
{
    if (grundyNumberForPileSizeLookup[pileSize] != -1)
        return grundyNumberForPileSizeLookup[pileSize];

    vector<int> pilesSoFar;
    set<int> grundyNumbersFromMoves;
    computeGrundyNumbersForMovesWithPileSize(pileSize, 1, pilesSoFar, grundyNumbersFromMoves);

    // Mex of all found grundy numbers.
    int grundyNumber = 0;
    while (grundyNumbersFromMoves.find(grundyNumber) != grundyNumbersFromMoves.end())
    {
        grundyNumber++;
    }

    grundyNumberForPileSizeLookup[pileSize] = grundyNumber;
    return grundyNumber;
}

void computeGrundyNumbersForMovesWithPileSize(int numStonesRemaining, int nextMinStones, vector<int>& pilesSoFar, set<int>& grundyNumbers)
{
    if (numStonesRemaining == 0 && pilesSoFar.size() >= 2)
    {
        cout << "Found new set of piles: " << endl;
        for (const auto x : pilesSoFar)
        {
            cout << " " << x;
        }
        cout << endl;
        int xorSum = 0;
        for (const auto x : pilesSoFar)
        {
            xorSum ^= grundyNumberForPileSize(x);
        }
        grundyNumbers.insert(xorSum);
        return;
    }

    for (int i = nextMinStones; numStonesRemaining - i >= 0; i++)
    {
        pilesSoFar.push_back(i);
        computeGrundyNumbersForMovesWithPileSize(numStonesRemaining - i, i, pilesSoFar, grundyNumbers);
        pilesSoFar.pop_back();
    }
}
int main(int argc, char* argv[])
{
    for (int i = 1; i <= 10; i++)
    {
        //cout << "grundyNumber[" << i << " ] = " << grundyNumberForPileSize(i) << endl;
        grundyNumberForPileSize(i);
    }
    for (int i = 1; i <= 10; i++)
    {
        cout << "grundyNumber[" << i << " ] = " << grundyNumberForPileSize(i) << endl;
    }

}

