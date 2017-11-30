// Simon St James (ssjgz) - 2017-11-30
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    // Easy one - see the original "Tower Breakers" for how the problem can be reduced to a game
    // of Nim, where each pile of stones corresponds to a tower in the original problem, and the
    // number of stones in the pile is equal to the sum of the powers of the primes in the 
    // prime factorisation of the height of the tower.
    //
    // The solution to this Nim game is well-known: if the xor-sum of the number of stones in each
    // pile is non-zero, player 1 wins; else, player 2.
    int T;
    cin >> T;

    const auto maxHeight = 1'000'000;

    // Use modified Sieve of Erastophenes to find the sum of prime powers for all values up to maxHeight.
    vector<bool> isPrime(maxHeight + 1, true);
    vector<int> sumOfPrimePowers(maxHeight + 1);
    for (int factor = 2; factor <= maxHeight; factor++)
    {
        if (isPrime[factor])
        {
            sumOfPrimePowers[factor] = 1; // This is a prime, factor^1.
            int composite = 2 * factor;
            while (composite <= maxHeight)
            {
                isPrime[composite] = false;
                // Add the power of this particular prime factor i.e. the largest n such that
                // factor^n divides composite.
                auto compositeCopy = composite;
                while ((compositeCopy % factor) == 0)
                {
                    compositeCopy /= factor;
                    sumOfPrimePowers[composite]++;
                }
                composite += factor;
            }
        }
    }

    for (auto t = 0; t < T; t++)
    {
        int N;
        cin >> N;

        auto stonePileGameNimber = 0;
        for (auto i = 0; i < N; i++)
        {
            int towerHeight;
            cin >> towerHeight;

            const auto numStonesInPile = sumOfPrimePowers[towerHeight];
            stonePileGameNimber ^= numStonesInPile;
        }

        if (stonePileGameNimber != 0)
            cout << 1 << endl;
        else
            cout << 2 << endl;
    }

}


