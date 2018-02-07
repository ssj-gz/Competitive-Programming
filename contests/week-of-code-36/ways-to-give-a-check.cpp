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
        vector<Position> promotablePawnPositions;
        Position enemyKingPos;
        for (int row = 0; row < numRows; row++)
        {
            for (int col = 0; col < numCols; col++)
            {
                if (boardOriginal[row][col] == 'P' && row == 1 && boardOriginal[row - 1][col] == '#')
                {
                    promotablePawnPositions.push_back({row, col});
                }
                if (boardOriginal[row][col] == 'k')
                {
                    assert(enemyKingPos.row == -1);
                    enemyKingPos = {row, col};
                }
            }
        }
        assert(!promotablePawnPositions.empty());
        //cout << "t: " << t << " promotablePawnPos: " << promotablePawnPos.row << "," << promotablePawnPos.col << " enemyKingPos: " << enemyKingPos.row << "," << enemyKingPos.col << endl;
        //cout << "t: " << t << endl;

        int numWaysToCheck = 0;
        for (const auto& promotablePawnPos : promotablePawnPositions)
        {
            vector<string> board(boardOriginal);
            const Position promotedPawnPos = {promotablePawnPos.row - 1, promotablePawnPos.col};
            board[promotablePawnPos.row][promotablePawnPos.col] = '#';
            //cout << "Pawn promoted to row: " << promotedPawnPos.row << " col: " << promotedPawnPos.col << endl;

            auto moveReachesKing = [&board, promotedPawnPos](int dRow, int dCol, int limit = 1000)
            {
                //cout << "dRow: " << dRow << " dCol: " << dCol << endl;
                int row = promotedPawnPos.row;
                int col = promotedPawnPos.col;

                bool reachedKing = false;
                for (int i = 0; i < limit; i++)
                {
                    row += dRow;
                    col += dCol;

                    //cout << "i: " << i << " row: " << row << " col: " << col << endl;

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

            int numWaysToCheckWithThisPawn = 0;

            // Queen.
            bool promotingToQueenChecks = false;
            promotingToQueenChecks = promotingToQueenChecks || moveReachesKing(0, -1); // Left
            promotingToQueenChecks = promotingToQueenChecks || moveReachesKing(0, 1);  // Right
            promotingToQueenChecks = promotingToQueenChecks || moveReachesKing(1, 0);  // Down
            promotingToQueenChecks = promotingToQueenChecks || moveReachesKing(1, -1);  // Down + Left
            promotingToQueenChecks = promotingToQueenChecks || moveReachesKing(1, 1);  // Down + Right
            if (promotingToQueenChecks)
                numWaysToCheckWithThisPawn++;

            // Rook.
            bool promotingToRookChecks = false;
            promotingToRookChecks = promotingToRookChecks || moveReachesKing(0, -1); // Left
            promotingToRookChecks = promotingToRookChecks || moveReachesKing(0, 1); // Right
            promotingToRookChecks = promotingToRookChecks || moveReachesKing(1, 0); // Down
            if (promotingToRookChecks)
                numWaysToCheckWithThisPawn++;

            // Bishop.
            bool promotingToBishopChecks = false;
            promotingToBishopChecks = promotingToBishopChecks || moveReachesKing(1, -1); // Down + Left.
            promotingToBishopChecks = promotingToBishopChecks || moveReachesKing(1, 1); // Down + Right.
            if (promotingToBishopChecks)
                numWaysToCheckWithThisPawn++;

            // Knight.
            bool promotingToKnightChecks = false;
            promotingToKnightChecks = promotingToKnightChecks || moveReachesKing(1, -2, 1);
            promotingToKnightChecks = promotingToKnightChecks || moveReachesKing(1, 2, 1);
            promotingToKnightChecks = promotingToKnightChecks || moveReachesKing(2, -1, 1);
            promotingToKnightChecks = promotingToKnightChecks || moveReachesKing(2, 1, 1);
            if (promotingToKnightChecks)
                numWaysToCheckWithThisPawn++;

            //cout << "promotingToQueenChecks: " << promotingToQueenChecks << endl;
            //cout << "promotingToRookChecks: " << promotingToRookChecks << endl;
            //cout << "promotingToBishopChecks: " << promotingToBishopChecks << endl;
            //cout << "promotingToKnightChecks: " << promotingToKnightChecks << endl;

            numWaysToCheck += numWaysToCheckWithThisPawn;
        }
        cout << numWaysToCheck << endl;
    }

}
