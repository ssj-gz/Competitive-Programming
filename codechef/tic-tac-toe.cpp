// Simon St James (ssjgz) - 2021-05-17
// 
// Solution to: https://www.codechef.com/problems/TCTCTOE
//
#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

struct BoardState
{
    char board[3][3];
    BoardState()
    {
        for (int row = 0; row < 3; row++)
        {
            for (int col = 0; col < 3; col++)
            {
                board[row][col] = '_';
            }
        }
    }
    int uniqueId() const
    {
        int id = 0;
        for (int row = 0; row < 3; row++)
        {
            for (int col = 0; col < 3; col++)
            {
                id = id * 3;
                switch(board[row][col])
                {
                    case '_':
                        id = id + 0;
                        break;
                    case 'X':
                        id = id + 1;
                        break;
                    case 'O':
                        id = id + 2;
                        break;
                    default:
                        assert(false);
                }
            }
        }
        return id;
    }
    bool isWin() const
    {
        for (const auto playerChar : {'X', 'O'})
        {

            // Player has full row?
            for (int row = 0; row < 3; row++)
            {
                int numOfPlayerCharInRow = 0;
                for (int col = 0; col < 3; col++)
                {
                    if (board[row][col] == playerChar)
                    {
                        numOfPlayerCharInRow++;
                    }
                }
                if (numOfPlayerCharInRow == 3)
                    return true;
            }
            // Player has full col?
            for (int col = 0; col < 3; col++)
            {
                int numOfPlayerCharInCol = 0;
                for (int row = 0; row < 3; row++)
                {
                    if (board[row][col] == playerChar)
                    {
                        numOfPlayerCharInCol++;
                    }
                }
                if (numOfPlayerCharInCol == 3)
                    return true;
            }
            // Player has full diagonal (top-left to bottom right)?
            {
                int numOfPlayerCharInDiag = 0;
                for (int col = 0, row = 0; col < 3; col++, row++)
                {
                    if (board[row][col] == playerChar)
                    {
                        numOfPlayerCharInDiag++;
                    }
                }
                if (numOfPlayerCharInDiag == 3)
                    return true;
            }
            // Player has full diagonal (top-right to bottom left)?
            {
                int numOfPlayerCharInDiag = 0;
                for (int col = 0, row = 2; col < 3; col++, row--)
                {
                    if (board[row][col] == playerChar)
                    {
                        numOfPlayerCharInDiag++;
                    }
                }
                if (numOfPlayerCharInDiag == 3)
                    return true;
            }

        }
        return false;
    }
};


int main()
{
    // Build complete lookup table of all possible board states
    // (including unreachable ones).
    // There are 3 ** 9 such states.
    int maxId = 1;
    for (int i = 1; i <= 9; i++)
    {
        maxId *= 3;
    }

    vector<bool> visitedIds(maxId, false);
    vector<bool> isIdTerminal(maxId, false);

    BoardState initialBoardState;

    vector<BoardState> boardsToVisit = { initialBoardState };
    visitedIds[initialBoardState.uniqueId()] = true;

    while (!boardsToVisit.empty())
    {
        vector<BoardState> nextBoardsToVisit;
        for (const auto& boardState : boardsToVisit)
        {
            int numXs = 0;
            int numOs = 0;
            for (int row = 0; row < 3; row++)
            {
                for (int col = 0; col < 3; col++)
                {
                    switch(boardState.board[row][col])
                    {
                        case '_':
                            break;
                        case 'X':
                            numXs++;
                            break;
                        case 'O':
                            numOs++;
                            break;
                        default:
                            assert(false);
                    }

                }
            }
            const int numEmpty = 3 * 3 - (numOs + numXs);
            if (numEmpty == 0 || boardState.isWin())
            {
                isIdTerminal[boardState.uniqueId()] = true;
                continue;
            }

            const char nextMoveChar = (numOs == numXs ? 'X' : 'O');

            for (int row = 0; row < 3; row++)
            {
                for (int col = 0; col < 3; col++)
                {
                    if(boardState.board[row][col] == '_')
                    {
                        BoardState nextBoardState = boardState;
                        nextBoardState.board[row][col] = nextMoveChar;
                        if (!visitedIds[nextBoardState.uniqueId()])
                        {
                            visitedIds[nextBoardState.uniqueId()] = true;
                            nextBoardsToVisit.push_back(nextBoardState);
                        }
                    }

                }
            }
        }

        boardsToVisit = nextBoardsToVisit;
    }

    auto result  = [&isIdTerminal, &visitedIds](const auto& boardState)
    {
        const int boardUniqueId = boardState.uniqueId();
        if (!visitedIds[boardUniqueId])
            return 3;
        else
        {
            if (isIdTerminal[boardUniqueId])
                return 1;
            else
                return 2;
        }
    };

#if 1
    // Read from stdin and solve the testcases.
    const int T = read<int>();
    for (int t = 0; t < T; t++)
    {
        BoardState boardState;

        for (int row = 0; row < 3; row++)
        {
            for (int col = 0; col < 3; col++)
            {
                boardState.board[row][col] = read<char>();
            }
        }

        cout << result(boardState) << endl;
    }
#else

    // Exhaustively generate a complete testsuite, in my usual testsuite format.
    for (int id = 0; id < maxId; id++)
    {
        int tempId = id;
        BoardState boardStateWithId;
        for (int row = 2; row >= 0; row--)
        {
            for (int col = 2; col >= 0; col--)
            {
                switch(tempId % 3)
                {
                    case 0:
                        boardStateWithId.board[row][col] = '_';
                        break;
                    case 1:
                        boardStateWithId.board[row][col] = 'X';
                        break;
                    case 2:
                        boardStateWithId.board[row][col] = 'O';
                        break;
                }
                tempId = tempId / 3;
            }
        }
        assert(boardStateWithId.uniqueId() == id);
        cout << "Q: 4 lines" << endl;
        cout << 1 << endl;
        for (int row = 0; row < 3; row++)
        {
            for (int col = 0; col < 3; col++)
            {
                cout << boardStateWithId.board[row][col];
            }
            cout << endl;
        }
        cout << "A: 1 lines" << endl;
        cout << result(boardStateWithId) << endl;
    }
#endif
}
