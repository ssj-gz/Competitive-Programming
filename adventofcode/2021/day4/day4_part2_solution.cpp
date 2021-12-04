#include <iostream>
#include <sstream>
#include <vector>
#include <cassert>

using namespace std;

const int bingoBoardWidth = 5;
const int bingoBoardHeight = 5;

void markNumberInBoard(vector<vector<int>>&  bingoBoard, int number)
{
    for (int row = 0; row < bingoBoardHeight; row++)
    {
        for (int col = 0; col < bingoBoardWidth; col++)
        {
            if (bingoBoard[row][col] == number)
                bingoBoard[row][col] = -1;
        }
    }
}

bool isBoardWinning(const vector<vector<int>>&  bingoBoard)
{
    for (int row = 0; row < bingoBoardHeight; row++)
    {
        bool rowMarked = true;
        for (int col = 0; col < bingoBoardWidth; col++)
        {
            if (bingoBoard[row][col] != -1)
                rowMarked = false;
        }
        if (rowMarked)
            return true;
    }
    for (int col = 0; col < bingoBoardWidth; col++)
    {
        bool colMarked = true;
        for (int row = 0; row < bingoBoardHeight; row++)
        {
            if (bingoBoard[row][col] != -1)
                colMarked = false;
        }
        if (colMarked)
            return true;
    }
    return false;
}

int calcSumOfUnmarked(const vector<vector<int>>&  bingoBoard)
{
    int sum = 0;
    for (int row = 0; row < bingoBoardHeight; row++)
    {
        for (int col = 0; col < bingoBoardWidth; col++)
        {
            if (bingoBoard[row][col] != -1)
                sum += bingoBoard[row][col];
        }
    }
    return sum;
}

int findResult(vector<vector<vector<int>>>&  bingoBoards, const vector<int>& drawNumbers)
{
    const int numBoards = bingoBoards.size();
    vector<bool> isBoardIndexWinning(numBoards, false);
    int activeBoardCount = numBoards;
    for (const auto drawnNumber : drawNumbers)
    {
        for (auto& bingoBoard : bingoBoards)
        {
            markNumberInBoard(bingoBoard, drawnNumber);
        }
        int boardIndex = 0;
        for (const auto& bingoBoard : bingoBoards)
        {
            if (!isBoardIndexWinning[boardIndex])
            {
                if (isBoardWinning(bingoBoard))
                {
                    isBoardIndexWinning[boardIndex] = true;
                    activeBoardCount--;
                    if (activeBoardCount == 0)
                    {
                        return calcSumOfUnmarked(bingoBoard) * drawnNumber;
                    }
                }
            }
            boardIndex++;
        }
    }

    assert(false);
    return -1;
}


int main()
{
    string drawnNumbersLine;
    getline(cin, drawnNumbersLine);

    istringstream drawnNumbersStream(drawnNumbersLine);
    vector<int> drawNumbers;
    while (drawnNumbersStream)
    {
        int number;
        drawnNumbersStream >> number;
        drawNumbers.push_back(number);

        char dummyChar;
        drawnNumbersStream >> dummyChar;

        assert(drawnNumbersStream.eof() || dummyChar == ',');
    }

    cout << "Drawn numbers: " << endl;
    for (const auto x : drawNumbers)
    {
        cout << x << " ";
    }
    cout << endl;

    vector<vector<vector<int>>> bingoBoards;

    const int bingoBoardWidth = 5;
    const int bingoBoardHeight = 5;
    while (cin)
    {
        vector<vector<int>> bingoBoard(bingoBoardHeight, vector<int>(bingoBoardWidth, -1));
        for (int row = 0; row < bingoBoardHeight; row++)
        {
            for (int col = 0; col < bingoBoardWidth; col++)
            {
                cin >> bingoBoard[row][col];
            }
        }
        if (cin)
            bingoBoards.push_back(bingoBoard);
    }

    cout << "Bingo boards: " << endl;
    for (const auto& bingoBoard : bingoBoards)
    {
        for (int row = 0; row < bingoBoardHeight; row++)
        {
            for (int col = 0; col < bingoBoardWidth; col++)
            {

                cout << bingoBoard[row][col] << " ";
            }
            cout << endl;
        }
        cout << " -- " << endl;
    }

    cout << findResult(bingoBoards, drawNumbers) << endl;




}
