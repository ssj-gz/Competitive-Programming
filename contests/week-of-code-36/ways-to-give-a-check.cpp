// Simon St James (ssjgz) - 2018-02-07 07:27
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

const auto numRows = 8;
const auto numCols = 8;

bool isKingInCheck(const vector<string>& board, bool isBlackKing)
{
    const char kingToTestChar = (isBlackKing ? 'k' : 'K');
    bool isInCheck = false;
    auto updateResultFromMove = [&isInCheck, &board, kingToTestChar](int startRow, int startCol, int dRow, int dCol, int limit = 1000)
    {
        int row = startRow;
        int col = startCol;

        for (int i = 0; i < limit; i++)
        {
            row += dRow;
            col += dCol;

            if (row < 0 || row >= 8)
                break;
            if (col < 0 || col >= 8)
                break;

            if (board[row][col] == kingToTestChar)
            {
                isInCheck = true;
                break;
            }
            if (board[row][col] != '#')
                break;
        }
    };

    const auto attackingPiecesAreWhite = isBlackKing;
    for (auto row = 0; row < numRows; row++)
    {
        for (auto col = 0; col < numCols; col++)
        {
            const auto pieceChar = board[row][col];
            const auto pieceCharUpperCase = toupper(pieceChar);
            const auto isWhitePiece = (pieceCharUpperCase == pieceChar);
            if (attackingPiecesAreWhite != isWhitePiece)
                continue;
            switch(pieceCharUpperCase)
            {
                case 'Q':
                    {
                        // Queen.
                        updateResultFromMove(row, col, 0, -1); // Left
                        updateResultFromMove(row, col, 0, 1);  // Right

                        updateResultFromMove(row, col, 1, 0);  // Down
                        updateResultFromMove(row, col, 1, -1);  // Down + Left
                        updateResultFromMove(row, col, 1, 1);  // Down + Right

                        updateResultFromMove(row, col, -1, 0);  // Up
                        updateResultFromMove(row, col, -1, -1);  // Up + Left
                        updateResultFromMove(row, col, -1, 1);  // Up + Right
                    }
                    break;
                case 'R':
                    {
                        // Rook.
                        updateResultFromMove(row, col, 0, -1); // Left
                        updateResultFromMove(row, col, 0, 1); // Right

                        updateResultFromMove(row, col, 1, 0); // Down
                        updateResultFromMove(row, col, -1, 0); // Up
                    };
                    break;

                case 'N':
                    {
                        // Knight.
                        updateResultFromMove(row, col, 1, -2, 1); // Down + 2 Left.
                        updateResultFromMove(row, col, 1, 2, 1);  // Down + 2 Right
                        updateResultFromMove(row, col, 2, -1, 1); // 2 Down + Left
                        updateResultFromMove(row, col, 2, 1, 1);  // 2 Down + Right

                        updateResultFromMove(row, col, -1, -2, 1); // Up + 2 Left.
                        updateResultFromMove(row, col, -1, 2, 1);  // Up + 2 Right
                        updateResultFromMove(row, col, -2, -1, 1); // 2 Up + Left
                        updateResultFromMove(row, col, -2, 1, 1);  // 2 Up + Right
                    };
                    break;

                case 'B':
                    {
                        // Bishop.
                        updateResultFromMove(row, col, 1, -1); // Down + Left.
                        updateResultFromMove(row, col, 1, 1); // Down + Right.
                        updateResultFromMove(row, col, -1, -1); // Up + Left.
                        updateResultFromMove(row, col, -1, 1); // Up + Right.
                    }
                    break;

            }
        }
    }

    return isInCheck;
}

int main()
{
    int T;
    cin >> T;

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

        int numWaysToCheck = 0;
        const char piecesToPromoteTo[] = { 'Q', 'R', 'B', 'N' };
        int numPromotablePawns = 0;
        for (int pawnCol = 0; pawnCol < numCols; pawnCol++)
        {
            const int rowFor7thRank = 1;
            const int rowFor8thRank = 0;
            const Position candidatePawnPos = {rowFor7thRank, pawnCol};
            bool isPawnPromotable = false;
            if (boardOriginal[candidatePawnPos.row][candidatePawnPos.col] != 'P')
                continue; // Not a Pawn!
            const Position promotedPawnPos = {rowFor8thRank, pawnCol};
            if (boardOriginal[promotedPawnPos.row][promotedPawnPos.col] != '#')
                continue; // Can't move Pawn forwards!
            for (const auto pieceToPromoteTo : piecesToPromoteTo)
            {
                vector<string> board{boardOriginal};
                board[candidatePawnPos.row][candidatePawnPos.col] = '#';
                board[promotedPawnPos.row][promotedPawnPos.col] = pieceToPromoteTo;
                if (isKingInCheck(board, false))
                    continue; // Promoting this pawn would put us in check.

                isPawnPromotable = true;

                if (isKingInCheck(board, true))
                {
                    numWaysToCheck++;
                }
            }

            if (isPawnPromotable)
                numPromotablePawns++;
        }
        assert(numPromotablePawns == 1);
        cout << numWaysToCheck << endl;
    }
    assert(cin);

}
