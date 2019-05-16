// Simon St James (ssjgz) - 2019-05-16
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <set>
#include <cassert>

#include <chrono>

#include <sys/time.h>

using namespace std;

const int maxPileSize = 600;
vector<int> grundyNumberForPileSizeLookup(maxPileSize + 1, -1);

const uint64_t modulus = 1'000'000'007ULL;

void computeGrundyNumbersForMovesWithPileSize(int numStonesRemaining, int nextMinStones, const int maxNewPilesPerMove, vector<int>& pilesSoFar, set<int>& grundyNumbers);

int grundyNumberForPileSize(int pileSize, int maxNewPilesPerMove)
{
    if (grundyNumberForPileSizeLookup[pileSize] != -1)
        return grundyNumberForPileSizeLookup[pileSize];

    vector<int> pilesSoFar;
    set<int> grundyNumbersFromMoves;
    computeGrundyNumbersForMovesWithPileSize(pileSize, 1, maxNewPilesPerMove, pilesSoFar, grundyNumbersFromMoves);

    // Mex of all found grundy numbers.
    int grundyNumber = 0;
    while (grundyNumbersFromMoves.find(grundyNumber) != grundyNumbersFromMoves.end())
    {
        grundyNumber++;
    }

    grundyNumberForPileSizeLookup[pileSize] = grundyNumber;
    return grundyNumber;
}

void computeGrundyNumbersForMovesWithPileSize(int numStonesRemaining, int nextMinStones, const int maxNewPilesPerMove, vector<int>& pilesSoFar, set<int>& grundyNumbers)
{
    if (pilesSoFar.size() > maxNewPilesPerMove)
    {
        return;
    }
    if (numStonesRemaining == 0 && pilesSoFar.size() >= 2)
    {
        int xorSum = 0;
        for (const auto x : pilesSoFar)
        {
            xorSum ^= grundyNumberForPileSize(x, maxNewPilesPerMove);
        }
        grundyNumbers.insert(xorSum);
        return;
    }
    if (pilesSoFar.size() == maxNewPilesPerMove - 1)
    {
        if (numStonesRemaining >= 1)
        {
            pilesSoFar.push_back(numStonesRemaining);
            computeGrundyNumbersForMovesWithPileSize(0, 0, maxNewPilesPerMove, pilesSoFar, grundyNumbers);
            pilesSoFar.pop_back();
        }
        return;
    }

    for (int i = nextMinStones; numStonesRemaining - i >= 0; i++)
    {
        pilesSoFar.push_back(i);
        computeGrundyNumbersForMovesWithPileSize(numStonesRemaining - i, i, maxNewPilesPerMove, pilesSoFar, grundyNumbers);
        pilesSoFar.pop_back();
    }
}

int main(int argc, char* argv[])
{
    int totalNumStones;
    cin >> totalNumStones;

    int numPiles;
    cin >> numPiles;

    int maxNewPilesPerMove;
    cin >> maxNewPilesPerMove;

    auto calcMaxGrundyNumberWithTotalStones = [](const int totalNumStones)
    {
        // If the xth bit is the maximum bit set in totalNumStones, then
        // we can't generate a xor sum with the (x+1)th bit set.
        int maxPowerOf2 = 1;
        while (maxPowerOf2 < totalNumStones)
        {
            maxPowerOf2 <<= 1;
        }
        return maxPowerOf2 - 1;
    };

    const int maxGrundyNumber = calcMaxGrundyNumberWithTotalStones(totalNumStones);
    if (maxNewPilesPerMove <= 3)
    {
        // Spur population of grundyNumberForPileSizeLookup.
        for (int i = 1; i <= totalNumStones; i++)
        {
            grundyNumberForPileSize(i, maxNewPilesPerMove);
        }
    }
    else
    {
        // With 4 or more piles, the grundy number is easily predictable.
        grundyNumberForPileSizeLookup.resize(maxGrundyNumber + 1);
        for (int i = 1; i <= totalNumStones; i++)
        {
            grundyNumberForPileSizeLookup[i] = i - 1;
        }
    }


    vector<vector<uint64_t>> numWithGrundyNumberAndNumStones(maxGrundyNumber + 1, vector<uint64_t>(totalNumStones + 1));
    for (int i = 1; i <= totalNumStones; i++)
    {
        numWithGrundyNumberAndNumStones[grundyNumberForPileSizeLookup[i]][i] = 1;
    }
    for (int numPilesSoFar = 1; numPilesSoFar <= numPiles - 1; numPilesSoFar++)
    {
        vector<vector<uint64_t>> nextNumWithGrundyNumberAndNumStones(maxGrundyNumber + 1, vector<uint64_t>(totalNumStones + 1, 0));
        for (int numStonesSoFar = 0; numStonesSoFar <= totalNumStones; numStonesSoFar++)
        {
            const int maxGrundyForNumStones = calcMaxGrundyNumberWithTotalStones(numStonesSoFar);
            for (int grundySoFar = 0; grundySoFar <= maxGrundyForNumStones; grundySoFar++)
            {
                const auto& numWithNumStonesForGrundySoFar = numWithGrundyNumberAndNumStones[grundySoFar];
                for (int numStonesNewColumn = 1; numStonesNewColumn + numStonesSoFar <= totalNumStones; numStonesNewColumn++)
                {
                    const int newGrundyNumber = grundySoFar ^ grundyNumberForPileSizeLookup[numStonesNewColumn] ;
                    auto& itemToUpdate = nextNumWithGrundyNumberAndNumStones[newGrundyNumber][numStonesNewColumn + numStonesSoFar];
                    // NB: we don't need to take the modulus here, as it's impossible for a given iteration to add more
                    // than 600 * 600 * 600 * ::modulus to itemToUpdate, which easily fits in a uint64_t.
                    itemToUpdate = (itemToUpdate + numWithNumStonesForGrundySoFar[numStonesSoFar]);
                    assert(itemToUpdate >= 0);
                }
            }
        }
        // Take the modulus now that we have completed an iteration.
        for (int numStonesSoFar = 0; numStonesSoFar <= totalNumStones; numStonesSoFar++)
        {
            const int maxGrundyForNumStones = calcMaxGrundyNumberWithTotalStones(numStonesSoFar);
            for (int grundySoFar = 0; grundySoFar <= maxGrundyForNumStones; grundySoFar++)
            {
                nextNumWithGrundyNumberAndNumStones[grundySoFar][numStonesSoFar] %= ::modulus;
            }
        }

        numWithGrundyNumberAndNumStones = nextNumWithGrundyNumberAndNumStones;
    }
    // Add up the final result - all configs with non-zero grundy number and totalNumStones stones.
    int result = 0;
    for (int grundyNumber = 1; grundyNumber <= maxGrundyNumber; grundyNumber++)
    {
        result = (result + numWithGrundyNumberAndNumStones[grundyNumber][totalNumStones]) % ::modulus;
    }
    cout << result << endl;
}

