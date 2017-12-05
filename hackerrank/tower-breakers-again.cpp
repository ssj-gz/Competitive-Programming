// Simon St James (ssjgz) - 2017-12-05
// Computation of Mex is not optimal, so may time out!
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

const int maxN = 100'000;
vector<int> grundyForPileSizeLookup(maxN + 1, -1);

vector<int> findFactors(const int x)
{
    vector<int> factors;
    const int sqrtX = sqrt(x);
    for (int factor = 1; factor <= sqrtX; factor++)
    {
        if ((x % factor) == 0)
        {
            factors.push_back(factor);
            // For square numbers, don't add the same factor twice!
            if (factor * factor != x)
            {
                factors.push_back(x / factor);
            }
        }
    }
    return factors;
}

// Calculate x ^ x ^ .... ^ x, where "x" occurs "power" times.
int xorPower(int x, int power)
{
    return ((power % 2) == 0) ? 0 : x;
}

int findGrundyNumber(int pileSize)
{
    if (grundyForPileSizeLookup[pileSize] != -1)
        return grundyForPileSizeLookup[pileSize];

    const auto factors = findFactors(pileSize);
    vector<int> grundyNumbersForMoves;
    for (const auto factor : factors)
    {
        const auto numNewPiles = factor;
        const auto newPileSize = pileSize / factor;

        if (numNewPiles == 1)
            continue;
        
        grundyNumbersForMoves.push_back(xorPower(findGrundyNumber(newPileSize), numNewPiles));
    }

    // This is not very efficient, but seems to be efficient enough in practise!
    int mexGrundyNumbersForMoves = 0;
    while (find(grundyNumbersForMoves.begin(), grundyNumbersForMoves.end(), mexGrundyNumbersForMoves) != grundyNumbersForMoves.end())
    {
        mexGrundyNumbersForMoves++;
    }

    grundyForPileSizeLookup[pileSize] = mexGrundyNumbersForMoves;

    return mexGrundyNumbersForMoves;
}

int main()
{
    // Fundamentally easy - just find the Grundy numbers for each Pile Size.
    // For a given pile of size pileSize, the "moves" we can make correspond to
    // the factors of pileSize, with the trivial factor "1" removed.
    //
    // For a factor f of pileSize, the state we reach has f piles, each of size pileSize/f.
    // The grundy number for such a state is grundy(pileSize) ^ grundy(pileSize) ^ ... ^ grundy(pileSize), where
    // "grundy(pileSize)" appears "f" times i.e. xorPower(grundy(pileSize), f) - this is just grundy(pileSize)
    // if f is odd, and 0 otherwise.
    //
    // The grundy number for pileSize is then just the mex of the grundy numbers for the states arising from each move.
    // Recursion + memo-isation is sufficient to handle everything from then on.  The final answer is just the xor
    // sum of the grundy numbers of each pileSize in the list of piles.
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int N;
        cin >> N;

        int pileNimSum = 0;
        for (int i = 0; i < N; i++)
        {
            int pileSize;
            cin >> pileSize;

            pileNimSum ^= findGrundyNumber(pileSize);
        }

        if (pileNimSum != 0)
            cout << 1;
        else
            cout << 2;
        cout << endl;
    }
}
