// Simon St James (ssjgz) - 2017-12-05
#include <iostream>

using namespace std;

int main()
{
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int numPiles;
        cin >> numPiles;

        int numPilesOfSizeOne = 0;
        int numPilesOfSizeGreaterThanOne = 0;
        int pilesNimSum = 0;
        for (int i = 0; i < numPiles; i++)
        {
            int numInPile;
            cin >> numInPile;

            if (numInPile == 1)
                numPilesOfSizeOne++;
            else if (numInPile > 1)
                numPilesOfSizeGreaterThanOne++;

            pilesNimSum ^= numInPile;
        }

        if (numPilesOfSizeGreaterThanOne == 0)
        {
            // Every pile has size 0 or 1; the game is 
            // deterministic from here, and the winner is
            // First if the total number of stones is even, else
            // Second.
            const int numStones = numPilesOfSizeOne * 1;

            if ((numStones % 2) == 0)
                cout << "First";
            else
                cout << "Second";
        }
        else
        {
            if (pilesNimSum == 0)
                cout << "Second";
            else
                cout << "First";
        }
        cout << endl;
    }
}
