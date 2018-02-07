// Simon St James (ssjgz) - 2018-02-07 07:27
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

const auto numRows = 8;
const auto numCols = 8;

bool isKingInCheck(const vector<string>& board, bool isBlackKing)
{
    const char kingToCheck = (isBlackKing ? 'k' : 'K');
    auto moveReachesKing = [&board, kingToCheck](int startRow, int startCol, int dRow, int dCol, int limit = 1000)
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

            if (board[row][col] == kingToCheck)
            {
                reachedKing = true;
                break;
            }
            if (board[row][col] != '#')
                break;
        }

        return reachedKing;
    };

    const bool playerPiecesToTestAreWhite = isBlackKing;
    for (int row = 0; row < numRows; row++)
    {
        for (int col = 0; col < numCols; col++)
        {
            const char piece = board[row][col];
            const char pieceUpperCase = toupper(piece);
            const bool isWhitePiece = (pieceUpperCase == piece);
            if (playerPiecesToTestAreWhite != isWhitePiece)
                continue;
            switch(pieceUpperCase)
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
                        //cout << "promotingToKnightChecks? " << promotingToKnightChecks << endl;
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

    for (int t = 0; t < T; t++)
    {
        //cout << "t: " << t << endl;
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
        //Position promotablePawnPos;
        //Position enemyKingPos;
        for (int row = 0; row < numRows; row++)
        {
            for (int col = 0; col < numCols; col++)
            {
#if 0
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
#endif
            }
        }

#if 0
        assert(promotablePawnPos.row != -1 && promotablePawnPos.col != -1);
        assert(enemyKingPos.row != -1 && enemyKingPos.col != -1);
#endif

        int numWaysToCheck = 0;
        const char piecesToPromoteTo[] = { 'Q', 'R', 'B', 'N' };
        bool foundPromotablePawn = false;
        for (int pawnCol = 0; pawnCol < numCols; pawnCol++)
        {
            const int rowFor7thRank = 1;
            const int rowFor8thRank = 0;
            const Position candidatePawnPos = {rowFor7thRank, pawnCol};
            if (boardOriginal[candidatePawnPos.row][candidatePawnPos.col] != 'P')
            {

                //cout << "Can't promote pawn at " << candidatePawnPos.row << "," << candidatePawnPos.col << " as not a pawn!" << endl;
                continue;
            }
            const Position promotedPawnPos = {rowFor8thRank, pawnCol};
            if (boardOriginal[promotedPawnPos.row][promotedPawnPos.col] != '#')
            {
                //cout << "Can't promote pawn at " << candidatePawnPos.row << "," << candidatePawnPos.col << " as it is blocked" << endl;
                continue;
            }
            for (const auto pieceToPromoteTo : piecesToPromoteTo)
            {
                vector<string> board{boardOriginal};
                board[candidatePawnPos.row][candidatePawnPos.col] = '#';
                board[promotedPawnPos.row][promotedPawnPos.col] = pieceToPromoteTo;
                if (isKingInCheck(board, false))
                {
                    // Promoting this pawn would put us in check.
                    //cout << "Can't promote pawn at " << candidatePawnPos.row << "," << candidatePawnPos.col << " as it puts our own King in check" << endl;
                    continue;
                }

                foundPromotablePawn = true;

                if (isKingInCheck(board, true))
                {
                    numWaysToCheck++;
                }
            }
        }
        assert(foundPromotablePawn);
        cout << numWaysToCheck << endl;
    }
    assert(cin);

}
