// Simon St James (ssjgz) - 2019-05-16
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <set>
#include <cassert>

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
    // Another one of the "tried this a couple of years ago and couldn't solve it, tried again
    // recently and solved it fairly quickly" problems.  I think in the original attempt
    // I was trying to Logic my way to a solution, whereas this time around I was happier to 
    // pattern-match (as the Editorial seems to do).
    //
    // So, anyway: the state of the game can be represented as a vector (s1, s2, ... sx) for
    // some x.  The initial state is subject to x = M, but a general state is only restricted
    // by x <= N (with the x = N case being the state where there are N piles, each with exactly one of the original N stones in them).
    //
    // We immediately recognise the one-pile case as an impartial game, and any state (s1, s2, ... sx)
    // as x impartial games played concurrently (the problem description is not quite as unambiguous as 
    // it could be - the intention is that when we make a move with a given pile, we create up to K *new piles*, and leave the
    // other piles unchanged - thus, the parallel games don't affect each other at all), so we almost
    // certainly want to be able to compute the grundy number for a given state!
    //
    // Thus, by the Sprague-Grundy Theorem, the grundy number for a state (s1, s2, ... sx) is
    //
    //     grundy((s1, s2, ... sx)) = grundy(s1) ^ grundy(s2) ^ ... ^ grundy(sx) (*)
    //
    // where grundy(S) for an integer S is the grundy number for a state consisting of one pile of size
    // S.
    //
    // Again by Sprague Grundy, we have that the grundy number for a pile-size S is the mex of 
    // the grundy numbers for all states reachable in one move from a state consisting of just one pile, whose size is 
    // S; that is:
    //
    //     grundy(S) = mex[(s1, s2, ... sx) such that each x <= K, si >= 1, and s1 + s2 + ... + sx = S] {grundy((s1, s2, ... sx))}
    //
    // Finding all such vectors for, say, S = 600, K = 600 is computationally intractable, and that's where the pattern-recognition
    // comes in :/ As it happens, for K >= 4, we have a very simple result which I guessed at by brute-force
    // computing g(S) for small-ish S:
    //   
    //      For K >= 4, grundy(S) = S - 1
    //
    // There's actually a good reason for this, though I'm missing a proof for one of the last steps (the Editorial doesn't attempt
    // any kind of justification for it whatsoever!)
    //
    // Theorem
    //
    // For K >= 4, grundy(S) = S - 1
    //
    // Proof
    //
    // Proof is by induction on S.  The S = 1 case is trivial: no moves can be made from a state consisting of just one pile with just
    // one stone, so grundy(1) = 0 = 1 - 1.
    //
    // Assume true for 0, 1, .... S - 1 i.e. grundy(T) = T - 1 for T = 0, 1, ... S - 1.
    //
    // What moves can we make from a single pile of S stones?
    //
    // We could arrange them (since K >= 2) this way:
    //
    //     (S - 1, 1)
    //
    // From (*), we see that 
    //
    //     grundy((S - 1, 1)) = grundy(S - 1) ^ grundy(1)
    //                        = S - 2 ^ 0 (by induction hypothesis)
    //                        = S - 2
    //
    // We could also arrange them (since K >= 3) as, for some l >= 1 with S - 2l >= 1:
    //
    //     (S - 2l, l, l)
    //
    // From (*), we see that 
    //
    //     grundy((S - 2l, l, l)) = grundy(S - 2l) ^ grundy(l) ^ grundy(l)
    //                            = grundy(S - 2l) ^ (grundy(l) ^ grundy(l))
    //                            = grundy(S - 2l) ^ (0)
    //                            = grundy(S - 2l)
    //                            = S - 2l - 1 (by induction hypothesis)
    //
    // We could also arrange them (since K >= 4) as, for some l >= 1 with S - 2l - 1 >= 1:
    //
    //    (S - 2l - 1, l, l, 1)
    //
    // From (*), we see that 
    //
    //     grundy((S - 2l - 1, l, l, 1)) = grundy(S - 2l - 1) ^ grundy(l) ^ grundy(l) ^ grundy(1)
    //                                   = grundy(S - 2l - 1) ^ (grundy(l) ^ grundy(l)) ^ grundy(1)
    //                                   = grundy(S - 2l - 1) ^ (0) ^ grundy(1)
    //                                   = S - 2l - 2 ^ (0) ^ 0 (by induction hypothesis)
    //                                   = S - 2l - 2
    //
    // Now, any number 0 <= T <= S - 2 can be written as one of S - 2, S - 2l - 1, S - 2l - 1 for some l, so we
    // see that mex[(s1, s2, ... sx) such that each x <= K, si >= 1, and s1 + s2 + ... + sx = S] {grundy((s1, s2, ... sx))}
    // is at least S - 1.  In order to complete the proof, we'd now have to show that, given a state consisting of just
    // one pile, which of size S, there is no move that leads to a state with grundy number S - 1, but unfortunately
    // I don't know how to do that :/ But it seems to be the case, so grundy(S) = S - 1, so there!
    //
    // QED
    //
    // For K == 2 and K == 3, we can compute grundy(S) for all 1 <= S <= N by "brute-force", with a couple of optimisations:
    // while for the purpose of enumerating games the order of the piles is important, for just computing grundy numbers,
    // the order is unimportant (xor is commutative), so we can restrict our search to all states 
    // (s1, s2, ... sx) such that each si >= 1, and s1 + s2 + ... + sx = S *and* s_i+1 >= si, with the latter restriction
    // reducing the complexity considerably. The functions computeGrundyNumbersForMovesWithPileSize
    // plus grundyNumberForPileSize use this to compute grundyNumberForPileSizeLookup in acceptable time for K == 2 and K == 3. 
    //
    // So: we can efficiently compute the grundy number for any pile size, and so for any state.  It now remains to compute the number
    // of initial states (those with N stones spread across exactly M piles) that give a non-zero grundy number.
    //
    // This is conceptually very easy: let 
    //
    //    numWithGrundyNumberAndNumStones[grundyNum][numStones][numPiles]
    //
    // be the number of states with exactly numStones spread across exactly numPiles whose
    // grundy number is grundyNum.  We imagine adding a new pile (to give numPiles + 1) with
    // some number of stones >= 1 to it, and see that:
    //
    //    numWithGrundyNumberAndNumStones[grundyNum ^ grundy(numStonesNewColumn)][numStones + numStonesNewColumn][numPiles+1] +=
    //       sum[1 <= numStonesNewColumn <= N,
    //           0 <= grundyNum maxGrundyNumber(?),
    //           1 <= numStones <=N - 1 ] {numWithGrundyNumberAndNumStones[grundyNum][numStones][numPiles]}
    //
    // (with some adjustments to make sure none of the indices are out of bounds).  
    // After filling out this table for the M values of numPiles, the final answer is simply:
    //
    //    sum [grundyNum != 0] {numWithGrundyNumberAndNumStones[grundyNum][N][M]}
    //
    //  Unfortunately, summing over all approx M * N * maxGrundyNumber * N is prohibitively expensive (for N = 600, maxGrundyNumber = 1023, giving
    //  approx 10 * 600 * 1023 * 600  = ~3'682'800'000 computations, which is just a little too large - perhaps about 3x more than we can
    //  compute in the time limit.)
    //
    //  Luckily, we can reduce this somewhat: if all initial states counted so far have used numStones stones in total, then we can
    //  put a limit on the maximum grundy numbers such states can have: if l is the maximum bit set in numStones, then the 
    //  grundy number for any state - which will be the xor of one or more numbers, all <= numStones - cannot have its l+1th bit set,
    //  so the maximum possible grundy number is (1 << (l+1)) - 1.  This maxGrundyForNumStones allows us to fairly drastically reduce the number
    //  of computations, just about bringing us under the time limit (at least, after adding one more micro-optimisation - 
    //  we don't need to take the modulus after *every* calculation if we use a large enough underlying type - uint64_t, say!).
    //
    //  And that's it!
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
        // Fill out numWithGrundyNumberAndNumStones for single pile case.
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
    // Add up the final result - all states with non-zero grundy number and totalNumStones stones.
    int numWinningInitialStates = 0;
    for (int grundyNumber = 1; grundyNumber <= maxGrundyNumber; grundyNumber++)
    {
        numWinningInitialStates = (numWinningInitialStates + numWithGrundyNumberAndNumStones[grundyNumber][totalNumStones]) % ::modulus;
    }
    cout << numWinningInitialStates << endl;
}

