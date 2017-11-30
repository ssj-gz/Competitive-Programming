// Simon St James (ssjgz) 2017-11-30
#include <iostream>

using namespace std;

int main()
{
    // Took me a while, as it is tempting to consider this game from a "square-centric" point
    // of view, whereas you really have to take a coin-centric one!
    //
    // Take a single-coin example: this is plainly equal to a Nim-game with one pile, where
    // the pile size is equal to the square that the coin is on (moving the coin to the square
    // j squares to the left is equivalent to removing j stones from the pile).
    //
    // With multiple coins, we simply have a Nim-game with multiple piles, where each pile corresponds
    // to a coin, and has stones equal to the coin's square.
    //
    // Thus, we could use the logic, for each square i:
    //
    //  const auto pileSize = i;
    //  for (int j = 0; j < numCoinsInSquare; j++)
    //  {
    //      stonePileXorSum ^= pileSize;
    //  }
    //
    // but since numCoinsInSquare can be very large (~10^9), this will be too slow.  So we note that
    // pileSize ^ pileSize = 0; pileSize ^ pileSize ^ pileSize = pileSize; pileSize ^ pileSize ^ pileSize ^ pileSize = 0
    // etc to see that we only care about whether numCoinsInSquare is odd or even; if odd, xor stonePileXorSum with it;
    // otherwise, leave stonePileXorSum unchanged for this square.
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        int stonePileXorSum = 0;

        for (int i = 0; i < n; i++)
        {
            int numCoinsInSquare;
            cin >> numCoinsInSquare;

            const auto pileSize = i;
            const auto numPilesWithThisPileSize = numCoinsInSquare;

            if ((numPilesWithThisPileSize % 2) == 1)
            {
                stonePileXorSum ^= pileSize;
            }
        }

        if (stonePileXorSum != 0)
            cout << "First";
        else
            cout << "Second";
        cout << endl;
    }
}
