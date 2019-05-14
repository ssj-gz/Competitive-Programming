#include <iostream>
#include <vector>
#include <set>

#include <chrono>



using namespace std;

const int maxPileSize = 600;
vector<int> grundyNumberForPileSizeLookup(maxPileSize + 1, -1);

const int maxNewPilesPerMove = 4;

void computeGrundyNumbersForMovesWithPileSize(int numStonesRemaining, int nextMinStones, vector<int>& pilesSoFar, set<int>& grundyNumbers);

int grundyNumberForPileSize(int pileSize)
{
    if (grundyNumberForPileSizeLookup[pileSize] != -1)
        return grundyNumberForPileSizeLookup[pileSize];

    cout << "Computing grundyNumberForPileSize: " << pileSize << endl;

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
    cout << "Finished computing grundyNumberForPileSize: " << pileSize << " (result is: " << grundyNumber << "; max(grundyNumbersFromMoves): " << (grundyNumbersFromMoves.empty() ? -1 : *std::prev(grundyNumbersFromMoves.end())) << ")" << endl;
    return grundyNumber;
}

void computeGrundyNumbersForMovesWithPileSize(int numStonesRemaining, int nextMinStones, vector<int>& pilesSoFar, set<int>& grundyNumbers)
{
    if (pilesSoFar.size() > maxNewPilesPerMove)
    {
        return;
    }
    if (numStonesRemaining == 0 && pilesSoFar.size() >= 2)
    {
        //cout << "Found new set of piles: " << endl;
        //for (const auto x : pilesSoFar)
        //{
            //cout << " " << x;
        //}
        //cout << endl;
        int xorSum = 0;
        for (const auto x : pilesSoFar)
        {
            xorSum ^= grundyNumberForPileSize(x);
        }
        grundyNumbers.insert(xorSum);
        return;
    }
#if 1
    if (pilesSoFar.size() == maxNewPilesPerMove - 1)
    {
        if (numStonesRemaining >= 1)
        {
            pilesSoFar.push_back(numStonesRemaining);
            computeGrundyNumbersForMovesWithPileSize(0, 0, pilesSoFar, grundyNumbers);
            pilesSoFar.pop_back();
        }
        return;
    }
#endif

    for (int i = nextMinStones; numStonesRemaining - i >= 0; i++)
    {
        pilesSoFar.push_back(i);
        computeGrundyNumbersForMovesWithPileSize(numStonesRemaining - i, i, pilesSoFar, grundyNumbers);
        pilesSoFar.pop_back();
    }
}
int main(int argc, char* argv[])
{
    const int maxGrundyNumber = 100;
    for (int i = 1; i <= maxGrundyNumber; i++)
    {
        //cout << "grundyNumber[" << i << " ] = " << grundyNumberForPileSize(i) << endl;
        grundyNumberForPileSize(i);
    }
    for (int i = 1; i <= maxGrundyNumber; i++)
    {
        cout << "grundyNumber[" << i << " ] = " << grundyNumberForPileSize(i) << endl;
    }


    // This just actually *do* anything - it's just a simulation of the kind of computations
    // we'll have to perform in order to solve this problem, so we can see roughly how long
    // it will take to run.
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    const int blah = 600;
    vector<vector<int>> blee(blah, vector<int>(blah));
    for (int i = 1; i <= 10; i++)
    {
        vector<vector<int>> bloo(blah, vector<int>(blah));
        for (int x = 0; x < blah; x++)
        {
            for (int y = 0; y < blah; y++)
            {
                for (int z = x; z < blah; z++)
                {
                    bloo[z - x][y] = blee[x][y] ^ y;
                }
            }
        }

        blee = bloo;
    }
    std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" <<std::endl;
}

