// Simon St James (ssjgz) - 2017-11-30
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    int T;
    cin >> T;

    const auto maxHeight = 1'000'000;
    vector<bool> isPrime(maxHeight + 1, true);
    vector<int> sumOfPrimePowers(maxHeight + 1);

    for (int factor = 2; factor <= maxHeight; factor++)
    {
        if (isPrime[factor])
        {
            sumOfPrimePowers[factor] = 1;
            int composite = 2 * factor;
            while (composite <= maxHeight)
            {
                isPrime[composite] = false;
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

    for (int t = 0; t < T; t++)
    {
        int N;
        cin >> N;

        vector<int> towerHeight(N);
        for (int i = 0; i < N; i++)
        {
            cin >> towerHeight[i];
        }
        auto stonePileGameNimber = 0;
        for (const auto height : towerHeight)
        {
            const auto numStonesInPile = sumOfPrimePowers[height];
            stonePileGameNimber ^= numStonesInPile;
        }
        if (stonePileGameNimber != 0)
            cout << 1 << endl;
        else
            cout << 2 << endl;
    }

}


