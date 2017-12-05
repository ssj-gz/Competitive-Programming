// Simon St James (ssjgz) 2017-12-05
#include <iostream>

using namespace std;

int main()
{
    // Decide winner using standard algorithm for Nim: https://en.wikipedia.org/wiki/Nim
    int g;
    cin >> g;

    for (int t = 0; t < g; t++)
    {
        int n;
        cin >> n;

        int xorSum = 0;
        for (int i = 0; i < n; i++)
        {
            int numInPile = 0;
            cin >> numInPile;

            xorSum ^= numInPile;
        }

        if (xorSum == 0)
            cout << "Second";
        else
            cout << "First";
        cout << endl;

    }

}
