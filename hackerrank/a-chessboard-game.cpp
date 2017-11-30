// Simon St James (ssjgz) 2017-11-30
#include <iostream>
#include <vector>

using namespace std;

constexpr auto boardWidth = 15;
constexpr auto boardHeight = 15;
enum CellState { Win, Lose, Unknown };

CellState findCellState(int x, int y, vector<vector<CellState>>& boardCellStates)
{
    if (boardCellStates[x][y] != Unknown)
        return boardCellStates[x][y];

    bool canMakeMove = false;
    bool canReachLoseCell = false;

    struct Diff
    {
        int dx;
        int dy;
    };

    const Diff moveDirections[] = 
    {
        {-2, 1},
        {-2, -1},
        {1, -2},
        {-1, -2}
    };

    for (const auto moveDirection : moveDirections)
    {
        const auto newX = x + moveDirection.dx;
        const auto newY = y + moveDirection.dy;

        if (newX < 0 || newX >= boardWidth || newY < 0 || newY >= boardHeight)
            continue;

        canMakeMove = true;
        if (findCellState(newX, newY, boardCellStates) == Lose)
            canReachLoseCell = true;
    }

    const CellState cellState = canReachLoseCell ? Win : Lose;
    boardCellStates[x][y] = cellState;

    return cellState;
}

int main()
{
    // Easy one - just use retrograde analysis and a bit of Dynamic Programming to mark
    // cells of the board as either Win or Lose for the player who starts there.
    vector<vector<CellState>> boardCellStates(boardWidth, vector<CellState>(boardHeight, Unknown));

    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int x, y;
        cin >> x >> y;
        // Make x and y 0-relative.
        x--;
        y--;

        const auto player1Wins = (findCellState(x, y, boardCellStates) == Win);
        if (player1Wins)
            cout << "First";
        else
            cout << "Second";
        cout << endl;
    }

}

