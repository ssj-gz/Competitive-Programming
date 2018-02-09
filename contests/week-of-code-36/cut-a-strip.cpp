// Simon St James (ssjgz) - 2018-02-09
// This is just a brute force solution to check correctness - it's too slow to pass!
#define BRUTE_FORCE
#include <iostream>
#include <vector>
#include <limits>
#include <sys/time.h>

using namespace std;

int findMaxSubMatrix(const vector<vector<int>>& matrix)
{
    const int numRows = matrix.size();
    const int numCols = matrix[0].size();

    int maxSum = numeric_limits<int>::min();

    for (int l = 0; l < numCols; l++)
    {
        for (int r = l; r < numCols; r++)
        {
            for (int top = 0; top < numRows; top++)
            {
                for (int bottom = top; bottom < numRows; bottom++)
                {
                    int sum = 0;
                    for (int col = l; col <= r; col++)
                    {
                        for (int row = top; row <= bottom; row++)
                        {
                            sum += matrix[row][col];
                        }
                    }
                    maxSum = max(maxSum, sum);
                }
            }
        }
    }

    return maxSum;
}

int findResultBruteForce(const vector<vector<int>>& originalMatrix, int k)
{
    const int numRows = originalMatrix.size();
    const int numCols = originalMatrix[0].size();

    int result = 0;

    for (int row = 0; row < numRows; row++)
    {
        for (int col = 0; col < numCols; col++)
        {
            {
                // Horizontal.
                vector<vector<int>> matrix{originalMatrix};
                for (int length = 1; length <= k; length++)
                {
                    const int colToZero = col - 1 + length;
                    if (colToZero >= numCols)
                        break;
                    matrix[row][colToZero] = 0;

                    result = max(result, findMaxSubMatrix(matrix));
                }
            }
            {
                // vertical.
                vector<vector<int>> matrix{originalMatrix};
                for (int length = 1; length <= k; length++)
                {
                    const int rowToZero = row - 1 + length;
                    if (rowToZero >= numRows)
                        break;
                    matrix[rowToZero][col] = 0;

                    result = max(result, findMaxSubMatrix(matrix));
                }
            }
        }
    }

    return result;
}

int findResultWithHorizontalStrip(const vector<vector<int>>& originalMatrix, int k)
{
    int result = numeric_limits<int>::min();
    return result;
}


int findResult(const vector<vector<int>>& originalMatrix, int k)
{
    int result = numeric_limits<int>::min();
    result = max(result, findResultWithHorizontalStrip(originalMatrix, k));

    const int numRows = originalMatrix.size();
    const int numCols = originalMatrix[0].size();

    vector<vector<int>> rotatedMatrix(numCols);
    for (int col = 0; col < numCols; col++)
    {
        rotatedMatrix[col].resize(numRows);
        for (int row = 0; row < numRows; row++)
        {
            rotatedMatrix[col][row] = originalMatrix[row][col];
        }
    }
    result = max(result, findResultWithHorizontalStrip(rotatedMatrix, k));
    return result;
}

int main(int argc, char** argv)
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int numRows = rand() % 10 + 1;
        const int numCols = rand() % 10 + 1;
        const int k = rand() % 15;

        cout << numRows << " " << numCols << " " << k << endl;

        const int maxValue = 10;
        const int minValue = -10;

        for (int row = 0; row < numRows; row++)
        {
            for (int col = 0; col < numCols; col++)
            {
                cout << (rand() % (maxValue - minValue + 1) + minValue) << " ";
            }
            cout << endl;
        }

        return 0;
    }

    int numRows, numCols, k;
    cin >> numRows >> numCols >> k;

    vector<vector<int>> matrix(numRows);
    for (int row = 0; row < numRows; row++)
    {
        matrix[row].resize(numCols);
        for (int col = 0; col < numCols; col++)
        {
            cin >> matrix[row][col];
        }
    }
    const auto result = findResult(matrix, k);

#ifdef BRUTE_FORCE
    const auto resultBruteForce = findResultBruteForce(matrix, k);
    cout << "result: " << result << " resultBruteForce: " << resultBruteForce << endl;
#endif
}


