// Simon St James (ssjgz) - 2019-12-27
// 
// Solution to: https://www.codechef.com/problems/SAD
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

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
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int numRows = 1 + rand() % 10;
        const int numCols = 1 + rand() % 10;

        cout << numRows << " " << numCols << endl;
        for (int i = 0; i < numRows; i++)
        {
            for (int j = 0; j < numCols; j++)
            {
                cout << 1 + rand() % 100;
                if (j != numCols - 1)
                    cout << " ";
            }
            cout << endl;
        }

        return 0;
    }

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
            if (grid[rowNum][colNum] == smallestInRow[rowNum] &&
                grid[rowNum][colNum] == largestInCol[colNum])
            {
                assert(answer == -1 || answer == grid[rowNum][colNum]);
                answer = grid[rowNum][colNum];
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
