// Simon St James (ssjgz) - 2019-12-27
// 
// Solution to: https://www.codechef.com/problems/SAD
//
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

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

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    const int numRows = read<int>();
    const int numCols = read<int>();

    vector<vector<int>> grid(numRows, vector<int>(numCols, -1));

    for (int rowNum = 0; rowNum < numRows; rowNum++)
    {
        for (int colNum = 0; colNum < numCols; colNum++)
        {
            grid[rowNum][colNum] = read<int>();
        }
    }

    vector<int> smallestInRow(numRows);
    for (int rowNum = 0; rowNum < numRows; rowNum++)
    {
        smallestInRow[rowNum] = *min_element(grid[rowNum].begin(), grid[rowNum].end());
    }

    vector<int> largestInCol(numCols, std::numeric_limits<int>::min());

    for (int colNum = 0; colNum < numCols; colNum++)
    {
        for (int rowNum = 0; rowNum < numRows; rowNum++)
        {
            largestInCol[colNum] = max(largestInCol[colNum], grid[rowNum][colNum]);
        }
    }

    int answer = -1;
    for (int rowNum = 0; rowNum < numRows; rowNum++)
    {
        for (int colNum = 0; colNum < numCols; colNum++)
        {
            const int cellValue = grid[rowNum][colNum];
            if (cellValue == smallestInRow[rowNum] &&
                cellValue == largestInCol[colNum])
            {
                assert(answer == -1 || answer == cellValue); // The answer should be unique.
                answer = cellValue;
            }

        }
    }

    if (answer == -1)
        cout << "GUESS";
    else
        cout << answer;
    cout << endl;

    assert(cin);
}
