#include <iostream>
#include <vector>
#include <set>
#include <cassert>

#include <chrono>



using namespace std;

const int maxPileSize = 600;
vector<int> grundyNumberForPileSizeLookup(maxPileSize + 1, -1);

int maxNewPilesPerMove = -1;
const uint64_t modulus = 1'000'000'007ULL;

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
    int maxGrundyNumber = 1023;
    int totalNumStones;
    cin >> totalNumStones;

    int numPiles;
    cin >> numPiles;

    cin >> maxNewPilesPerMove;

    for (int i = 1; i <= totalNumStones; i++)
    {
        //cout << "grundyNumber[" << i << " ] = " << grundyNumberForPileSize(i) << endl;
        grundyNumberForPileSize(i);
    }
    for (int i = 1; i <= totalNumStones; i++)
    {
        cout << "grundyNumber[" << i << " ] = " << grundyNumberForPileSize(i) << endl;
    }


    // This just actually *do* anything - it's just a simulation of the kind of computations
    // we'll have to perform in order to solve this problem, so we can see roughly how long
    // it will take to run.
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    vector<vector<uint64_t>> numWithGrundyNumberAndNumStones(maxGrundyNumber + 1, vector<uint64_t>(totalNumStones + 1));
    for (int i = 1; i <= totalNumStones; i++)
    {
        numWithGrundyNumberAndNumStones[grundyNumberForPileSizeLookup[i]][i] = 1;
    }
    for (int i = 1; i <= numPiles; i++)
    {
        vector<vector<uint64_t>> nextNumWithGrundyNumberAndNumStones(maxGrundyNumber + 1, vector<uint64_t>(totalNumStones + 1));
        for (int grundySoFar = 0; grundySoFar <= maxGrundyNumber; grundySoFar++)
        {
            for (int numStonesSoFar = 0; numStonesSoFar <= totalNumStones; numStonesSoFar++)
            {
                for (int numStonesNewColumn = 1; numStonesNewColumn + numStonesSoFar <= totalNumStones; numStonesNewColumn++)
                {
                    const int newGrundyNumber = grundySoFar ^ grundyNumberForPileSizeLookup[numStonesNewColumn] ;
#if 0
                    if (newGrundyNumber > maxGrundyNumber)
                    {
                        cout << "Whoops: " << newGrundyNumber << endl;
                        maxGrundyNumber = newGrundyNumber;
                    }
#endif
                    //assert(newGrundyNumber <= maxGrundyNumber);
                    nextNumWithGrundyNumberAndNumStones[newGrundyNumber][numStonesNewColumn + numStonesSoFar] = (numWithGrundyNumberAndNumStones[grundySoFar][numStonesNewColumn + numStonesSoFar] + 1) % ::modulus;
                }
            }
        }

        numWithGrundyNumberAndNumStones = nextNumWithGrundyNumberAndNumStones;
    }
    std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" <<std::endl;
    cout << "answer: " << numWithGrundyNumberAndNumStones[0][totalNumStones] << endl;
}

