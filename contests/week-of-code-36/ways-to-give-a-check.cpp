// Simon St James (ssjgz) - 2018-02-07 07:27
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

const auto numRows = 8;
const auto numCols = 8;

bool isKingInCheck(const vector<string>& board)
{
    auto moveReachesKing = [&board](int startRow, int startCol, int dRow, int dCol, int limit = 1000)
    {
        int row = startRow;
        int col = startCol;

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

    for (int row = 0; row < numRows; row++)
    {
        for (int col = 0; col < numCols; col++)
        {
            const char piece = board[row][col];
            switch(piece)
            {
                case 'Q':
                    {
                        bool promotingToQueenChecks = false;
                        promotingToQueenChecks = promotingToQueenChecks || moveReachesKing(row, col, 0, -1); // Left
                        promotingToQueenChecks = promotingToQueenChecks || moveReachesKing(row, col, 0, 1);  // Right

                        promotingToQueenChecks = promotingToQueenChecks || moveReachesKing(row, col, 1, 0);  // Down
                        promotingToQueenChecks = promotingToQueenChecks || moveReachesKing(row, col, 1, -1);  // Down + Left
                        promotingToQueenChecks = promotingToQueenChecks || moveReachesKing(row, col, 1, 1);  // Down + Right

                        promotingToQueenChecks = promotingToQueenChecks || moveReachesKing(row, col, -1, 0);  // Up
                        promotingToQueenChecks = promotingToQueenChecks || moveReachesKing(row, col, -1, -1);  // Up + Left
                        promotingToQueenChecks = promotingToQueenChecks || moveReachesKing(row, col, -1, 1);  // Up + Right

                        if (promotingToQueenChecks)
                            return true;
                    }
                    break;
                case 'R':
                    {
                        bool promotingToRookChecks = false;
                        promotingToRookChecks = promotingToRookChecks || moveReachesKing(row, col, 0, -1); // Left
                        promotingToRookChecks = promotingToRookChecks || moveReachesKing(row, col, 0, 1); // Right
                        promotingToRookChecks = promotingToRookChecks || moveReachesKing(row, col, 1, 0); // Down
                        promotingToRookChecks = promotingToRookChecks || moveReachesKing(row, col, -1, 0); // Up
                        if (promotingToRookChecks)
                            return true;
                    };
                    break;

                case 'N':
                    {
                        bool promotingToKnightChecks = false;
                        promotingToKnightChecks = promotingToKnightChecks || moveReachesKing(row, col, 1, -2, 1); // Down + 2 Left.
                        promotingToKnightChecks = promotingToKnightChecks || moveReachesKing(row, col, 1, 2, 1);  // Down + 2 Right
                        promotingToKnightChecks = promotingToKnightChecks || moveReachesKing(row, col, 2, -1, 1); // 2 Down + Left
                        promotingToKnightChecks = promotingToKnightChecks || moveReachesKing(row, col, 2, 1, 1);  // 2 Down + Right

                        promotingToKnightChecks = promotingToKnightChecks || moveReachesKing(row, col, -1, -2, 1); // Up + 2 Left.
                        promotingToKnightChecks = promotingToKnightChecks || moveReachesKing(row, col, -1, 2, 1);  // Up + 2 Right
                        promotingToKnightChecks = promotingToKnightChecks || moveReachesKing(row, col, -2, -1, 1); // 2 Up + Left
                        promotingToKnightChecks = promotingToKnightChecks || moveReachesKing(row, col, -2, 1, 1);  // 2 Up + Right
                        if (promotingToKnightChecks)
                            return true;
                    };
                    break;

                case 'B':
                    {
                        // Bishop.
                        bool promotingToBishopChecks = false;
                        promotingToBishopChecks = promotingToBishopChecks || moveReachesKing(row, col, 1, -1); // Down + Left.
                        promotingToBishopChecks = promotingToBishopChecks || moveReachesKing(row, col, 1, 1); // Down + Right.
                        promotingToBishopChecks = promotingToBishopChecks || moveReachesKing(row, col, -1, -1); // Up + Left.
                        promotingToBishopChecks = promotingToBishopChecks || moveReachesKing(row, col, -1, 1); // Up + Right.
                        if (promotingToBishopChecks)
                            return true;
                    }
                    break;

            }
        }
    }

    return false;
}

int main()
{
    int T;
    cin >> T;

    const auto numRows = 8;
    const auto numCols = 8;
    for (int t = 0; t < T; t++)
    {
        vector<string> boardOriginal(numRows);
        for (int row = 0; row < numRows; row++)
        {
            cin >> boardOriginal[row];
            assert(boardOriginal[row].size() == numCols);
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
                if (boardOriginal[row][col] == 'P' && row == 1 && boardOriginal[row - 1][col] == '#')
                {
                    assert(promotablePawnPos.row == -1);
                    promotablePawnPos = {row, col};
                }
                if (boardOriginal[row][col] == 'k')
                {
                    assert(enemyKingPos.row == -1);
                    enemyKingPos = {row, col};
                }
            }
        }
        assert(promotablePawnPos.row != -1 && promotablePawnPos.col != -1);
        assert(enemyKingPos.row != -1 && enemyKingPos.col != -1);

        int numWaysToCheck = 0;
        const char piecesToPromoteTo[] = { 'Q', 'R', 'B', 'N' };
        for (const auto pieceToPromoteTo : piecesToPromoteTo)
        {
            vector<string> board{boardOriginal};
            const Position promotedPawnPos = {promotablePawnPos.row - 1, promotablePawnPos.col};
            board[promotablePawnPos.row][promotablePawnPos.col] = '#';
            board[promotedPawnPos.row][promotedPawnPos.col] = pieceToPromoteTo;

            if (isKingInCheck(board))
            {
                numWaysToCheck++;
            }
        }
        cout << numWaysToCheck << endl;
    }
    assert(cin);

}
