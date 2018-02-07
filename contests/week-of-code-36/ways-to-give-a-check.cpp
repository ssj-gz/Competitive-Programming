// Simon St James (ssjgz) - 2018-02-07 07:27
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int main()
{
    int T;
    cin >> T;

    const auto numRows = 8;
    const auto numCols = 8;
    for (int t = 0; t < T; t++)
    {
        vector<string> board(numRows);
        for (int row = 0; row < numRows; row++)
        {
            cin >> board[row];
            assert(board[row].size() == numCols);
        }
        struct Position
        {
            int row = -1;
            int col = -1;
        }; 
        Position promotablePawnPos;
        Position enemyKingPos;
        for (int row = 0; row < numRows; row++)
        {
            for (int col = 0; col < numCols; col++)
            {
                if (board[row][col] == 'P' && row == 1)
                {
                    assert(promotablePawnPos.row == -1);
                    promotablePawnPos = {row, col};
                }
                if (board[row][col] == 'k')
                {
                    assert(enemyKingPos.row == -1);
                    enemyKingPos = {row, col};
                }
            }
        }
        assert(promotablePawnPos.row != -1 && enemyKingPos.row != -1);
        //cout << "t: " << t << " promotablePawnPos: " << promotablePawnPos.row << "," << promotablePawnPos.col << " enemyKingPos: " << enemyKingPos.row << "," << enemyKingPos.col << endl;

        const Position promotedPawnPos = {promotablePawnPos.row - 1, promotablePawnPos.col};

        auto moveReachesKing = [&board, promotedPawnPos](int dRow, int dCol, int limit = 1000)
        {
            int row = promotedPawnPos.row;
            int col = promotedPawnPos.col;

            bool reachedKing = false;
            for (int i = 0; i < limit; i++)
            {
                row += dRow;
                col += dCol;

                if (row < 0 || row >= 8)
                    break;
                if (col < 0 || col >= 8)
                    break;

                if (board[row][col] == 'k')
                {
                    reachedKing = true;
                    break;
                }
                if (board[row][col] != '#')
                    break;
            }

            return reachedKing;
        };

        int numWaysToCheck = 0;

        // Queen.
        bool promotingToQueenChecks = false;
        promotingToQueenChecks = promotingToQueenChecks || moveReachesKing(0, -1); // Left
        promotingToQueenChecks = promotingToQueenChecks || moveReachesKing(0, 1);  // Right
        promotingToQueenChecks = promotingToQueenChecks || moveReachesKing(1, 0);  // Down
        promotingToQueenChecks = promotingToQueenChecks || moveReachesKing(1, -1);  // Down + Left
        promotingToQueenChecks = promotingToQueenChecks || moveReachesKing(1, 1);  // Down + Right
        if (promotingToQueenChecks)
            numWaysToCheck++;

        // Rook.
        bool promotingToRookChecks = false;
        promotingToRookChecks = promotingToRookChecks || moveReachesKing(0, -1); // Left
        promotingToRookChecks = promotingToRookChecks || moveReachesKing(0, 1); // Right
        promotingToRookChecks = promotingToRookChecks || moveReachesKing(1, 0); // Down
        if (promotingToRookChecks)
            numWaysToCheck++;

        // Bishop.
        bool promotingToBishopChecks = false;
        promotingToBishopChecks = promotingToBishopChecks || moveReachesKing(1, -1); // Down + Left.
        promotingToBishopChecks = promotingToBishopChecks || moveReachesKing(1, 1); // Down + Left.
        if (promotingToBishopChecks)
            numWaysToCheck++;

        // Knight.
        bool promotingToKnightChecks = false;
        promotingToKnightChecks = promotingToKnightChecks || moveReachesKing(1, -2, 1);
        promotingToKnightChecks = promotingToKnightChecks || moveReachesKing(1, 2, 1);
        promotingToKnightChecks = promotingToKnightChecks || moveReachesKing(2, -1, 1);
        promotingToKnightChecks = promotingToKnightChecks || moveReachesKing(2, 1, 1);
        if (promotingToKnightChecks)
            numWaysToCheck++;

        cout << numWaysToCheck << endl;
    }

}
