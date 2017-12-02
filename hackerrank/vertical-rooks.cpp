// Simon St James (ssjgz) 2017-12-02
#include <iostream>
#include <vector>

using namespace std;

int main()
{
    int T;
    cin >> T;


    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        vector<int> player1RookRowForColumn(n);
        for (int i = 0; i < n; i++)
        {
            cin >> player1RookRowForColumn[i];
        }
        vector<int> player2RookRowForColumn(n);
        for (int i = 0; i < n; i++)
        {
            cin >> player2RookRowForColumn[i];
        }

        int nimSum = 0;
        for (int i = 0; i < n; i++)
        {
            nimSum ^= (abs(player2RookRowForColumn[i] - player1RookRowForColumn[i]));
        }
        // Player 2 goes first(!) so if the nimSum is initially 0, he loses i.e.
        // Player 1 wins.
        if (nimSum == 0)
            cout << "player-1";
        else
            cout << "player-2";

        cout << endl;
    }
}
