// Simon St James (ssjgz) - 2017-12-05
#include <iostream>

using namespace std;

int main()
{
    int T;
    cin >> T;

    for (int i = 0; i < T; i++)
    {
        int n, k;
        cin >> n >> k;

        int pilesNimSum = 0;
        for (int i = 0; i < n; i++)
        {
            int pileSize;
            cin >> pileSize;

            pilesNimSum ^= pileSize;
        }

        if (pilesNimSum == 0)
            cout << "Second";
        else
            cout << "First";
        cout << endl;
    }
}
