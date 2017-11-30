// Simon St James (ssjgz) 2017-11-30
#include <iostream>

using namespace std;

int main()
{
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
            for (int j = 0; j < (numCoinsInSquare % 2); j++)
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
