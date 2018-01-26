// Simon St James (ssjgz) 2018-01-26
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

constexpr auto boardWidth = 15;
constexpr auto boardHeight = 15;

int mex(const vector<int>& numbers)
{
    if (numbers.empty())
        return 0;
    auto numbersSorted = numbers;
    sort(numbersSorted.begin(), numbersSorted.end());

    int mex = 0;
    for (const auto number : numbersSorted)
    {
        if (number == mex)
            mex++;
        else if (number > mex)
            break;
    }

    return mex;
}


int findGrundyNumberForCell(int x, int y, vector<vector<int>>& grundyNumberForCellLookup)
{
    if (grundyNumberForCellLookup[x][y] != -1)
        return grundyNumberForCellLookup[x][y];

    struct Diff
    {
        int dx;
        int dy;
    };

    static const Diff moveDirections[] = 
    {
        {-2, +1},
        {-2, -1},
        {+1, -2},
        {-1, -2}
    };

    vector<int> grundyNumbersForMoves;
    for (const auto moveDirection : moveDirections)
    {
        const auto newX = x + moveDirection.dx;
        const auto newY = y + moveDirection.dy;

        if (newX < 0 || newX >= boardWidth || newY < 0 || newY >= boardHeight)
            continue;

        const auto grundyNumberForMove = findGrundyNumberForCell(newX, newY, grundyNumberForCellLookup);
        grundyNumbersForMoves.push_back(grundyNumberForMove);
    }

    const auto grundyNumberForCell = mex(grundyNumbersForMoves);
    grundyNumberForCellLookup[x][y] = grundyNumberForCell;

    //cout << "grundyNumberForCell " << x << "," << y << " = " << grundyNumberForCell << endl;

    return grundyNumberForCell;
}

int main()
{
    // Easy one - pretty much identical to "A Chessboard Game" but we need to find Grundy numbers for each cell
    // instead of a Win/ Lose value.
    vector<vector<int>> grundyNumberForCellLookup(boardWidth, vector<int>(boardHeight, -1));

    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        //cout << "t: " << t << endl;
        int k;
        cin >> k;

        int xorSum = 0;
        for (int i = 0; i < k; i++)
        {
            int coinX, coinY;
            cin >> coinX >> coinY;
            // Make 0-relative.
            coinX--;
            coinY--;

            //cout << "coinX: " << coinX << " coinY: " << coinY << endl;

            xorSum ^= findGrundyNumberForCell(coinX, coinY, grundyNumberForCellLookup);
        }

        //cout << "xorSum: " << xorSum << endl;

        const auto player1Wins = (xorSum != 0);
        if (player1Wins)
            cout << "First";
        else
            cout << "Second";
        cout << endl;
    }

}

