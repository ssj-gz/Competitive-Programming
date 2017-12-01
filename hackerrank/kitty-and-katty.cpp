// Simon St James (ssjgz) 2017-12-01
#include <iostream>
#include <array>
#include <map>
#include <cassert>

using namespace std;

enum WinState {KittyWin = 0, KattyWin = 1, Unknown};
enum Player {Kitty = 0, Katty = 1};

map<pair<array<int, 3>, Player>, WinState> blah;

WinState getWinState(const array<int, 3>& state, Player player, int indent = 0)
{
    const string indentation = string(indent, ' ');
    cout << indentation << "state: " << state[0] << "," << state[1] << "," << state[2] << " player: " << (player == Kitty ? "Kitty" : "Katty") << endl;
    if (blah.find({state, player}) != blah.end())
    {
        //cout << "returning memo-ised";
        return blah[{state, player}];
    }
    WinState winState = Unknown;
    const int numStones = state[0] + state[1] + state[2];
    if (numStones == 1)
    {
        if (state[0] == 1)
        {
            winState = static_cast<WinState>(1 - player);
        }
        else if (state[1] == 1)
        {
            winState = KittyWin;
        }
        else 
        {
            assert(state[2] == 1);
            winState = KattyWin;
        }

    }
    else
    {
        array<int, 3> stateCopy = state;
        bool haveWinningMove = false;
        for (int x = 0; x < 3; x++)
        {
            if (stateCopy[x] > 0)
            {
                stateCopy[x]--;
                for (int y = 0; y < 3; y++)
                {
                    if (stateCopy[y] > 0)
                    {
                        stateCopy[y]--;
                        const int differenceMod3 = (3 + x - y) % 3;
                        stateCopy[differenceMod3]++;
                        const auto moveWinState = getWinState(stateCopy, static_cast<Player>(1 - player), indent + 1);
                        stateCopy[differenceMod3]--;
                        stateCopy[y]++;

                        if (static_cast<int>(moveWinState) == static_cast<int>(player))
                            haveWinningMove = true;
                    }
                }
                stateCopy[x]++;
            }
        }
        if (haveWinningMove)
            winState = static_cast<WinState>(player);
        else
            winState = static_cast<WinState>(1 - player);
    }
    assert(winState != Unknown);
    blah[{state, player}] = winState;
    cout << indentation  << "state: " << state[0] << "," << state[1] << "," << state[2] << " is a winner for " << (winState == KittyWin ? "Kitty" : "Katty") << endl;
    return winState;
}

int main()
{
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        if (n == 1 || n == 2 || ((n % 2) == 0))
            cout << "Kitty";
        else
            cout << "Katty";
        cout << endl;
    }
#if 0
    for (int n = 1; n <= 50; n++)
    {
        cout << "Computing for n = " << n << endl;
        array<int, 3> state = {{0, 0, 0}};
        for (int j = 1; j <= n; j++)
        {
            state[j % 3]++;
        }
        const auto winState = getWinState(state, Kitty);
        cout << "n: " << n << " winner: " << (winState == KittyWin ? "Kitty" : "Katty") << endl;
    }
#endif
}


