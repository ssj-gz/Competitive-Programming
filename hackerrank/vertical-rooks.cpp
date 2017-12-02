// Simon St James (ssjgz) 2017-12-02
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int main()
{
    int T;
    cin >> T;


    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        //cout << "n: " << n << endl;

        vector<int> player1RookRowForColumn(n);
        for (int i = 0; i < n; i++)
        {
            cin >> player1RookRowForColumn[i];
            //cout << "player 1 row for col " << i << " = " << player1RookRowForColumn[i] << endl;
        }
        vector<int> player2RookRowForColumn(n);
        for (int i = 0; i < n; i++)
        {
            cin >> player2RookRowForColumn[i];
            //cout << "player 2 row for col " << i << " = " << player2RookRowForColumn[i] << endl;
        }

        int nimSum = 0;
        for (int i = 0; i < n; i++)
        {
            const int rookDistance = abs(player2RookRowForColumn[i] - player1RookRowForColumn[i]) - 1;
            //cout << "col " << i << " rookDistance: " << rookDistance << endl;
            nimSum ^= rookDistance;
        }
        //cout << "nimSum: " << nimSum << endl;
        // Player 2 goes first(!) so if the nimSum is initially 0, he loses i.e.
        // Player 1 wins.
        if (nimSum == 0)
            cout << "player-1";
        else
            cout << "player-2";

        cout << endl;
    }
    assert(cin);
}
