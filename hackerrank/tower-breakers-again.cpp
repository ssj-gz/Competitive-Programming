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
            //cout << "x: " << x << " adding factor " << factor << endl;
            factors.push_back(factor);
            if (factor * factor != x)
            {
                //cout << "x: " << x << " adding factor " << x / factor << endl;
                factors.push_back(x / factor);
            }
        }
    }
#if 0
    cout << "x: " << x << " factors: ";
    for (const auto factor : factors)
    {
        cout << factor << " ";
    }
    cout << " -- " << endl;
#endif
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

    const auto properFactors = findFactors(pileSize);
    vector<int> grundyNumbersForMoves;
    for (const auto factor : properFactors)
    {
        const auto numNewPiles = factor;
        const auto newPileSize = pileSize / factor;

        if (numNewPiles == 1)
            continue;
        
        //cout << "pileSize: " << pileSize << " move factor: " << factor << " newPileSize: " << newPileSize << " numNewPiles: " << numNewPiles << endl;
        grundyNumbersForMoves.push_back(xorPower(findGrundyNumber(newPileSize), numNewPiles));
    }

    int mexGrundyNumbersForMoves = 0;
    while (find(grundyNumbersForMoves.begin(), grundyNumbersForMoves.end(), mexGrundyNumbersForMoves) != grundyNumbersForMoves.end())
    {
        mexGrundyNumbersForMoves++;
    }

    grundyForPileSizeLookup[pileSize] = mexGrundyNumbersForMoves;

    //cout << "Grundy number for pileSize: " << pileSize << " : " << mexGrundyNumbersForMoves << endl;

    return mexGrundyNumbersForMoves;
}

int main()
{
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
