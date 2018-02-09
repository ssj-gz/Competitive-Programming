// Simon St James (ssjgz) - 2018-02-09
// This is just a brute force solution to check correctness - it's too slow to pass!
#define BRUTE_FORCE
#include <iostream>
#include <vector>
#include <limits>
#include <cassert>
#include <sys/time.h>

using namespace std;

int findMaxSubMatrix(const vector<vector<int>>& matrix, bool ignoreFullMatrix)
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
                    if (!ignoreFullMatrix || ((r - l + 1) * (bottom - top + 1) != numRows * numCols))
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

    int result = numeric_limits<int>::min();

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

                    result = max(result, findMaxSubMatrix(matrix, false));
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

                    result = max(result, findMaxSubMatrix(matrix, false));
                }
            }
        }
    }

    return result;
}

int findMinSubRangeBruteForce(const vector<int>& row, int left, int right, int k)
{
    int result = numeric_limits<int>::max();

    for (int x1 = left; x1 <= right; x1++)
    {
        int sum = 0;
        for (int x2 = x1; x2 <= right; x2++)
        {
            if (x2 - x1 + 1 > k)
                break;
            sum += row[x2];
            result = min(result, sum);
        }
    }

    return result;
}

// Taken from Matrix Land, which is very similar - will do some renaming on it later XD
int findBestIfMovedFromAndDescended(const vector<int>& row, const vector<int>& scoreIfDescendAt)
{
    vector<int> result(row.size());
    int bestSum = scoreIfDescendAt.front(); // Should be scoreIfDescendAt.front() + row[0], but the body of the loop adds the row[0] on the first iteration.
    int bestCumulative = numeric_limits<int>::min();
    for (int startPoint = 0; startPoint < row.size(); startPoint++)
    {
        bestSum += row[startPoint];

        if (bestCumulative < 0)
        {
            // Just as in Kadane's algorithm, if breaking with the existing
            // bestCumulative give a better result, then do so.
            bestCumulative = row[startPoint];
        }
        else
            bestCumulative += row[startPoint];

        if (bestCumulative + scoreIfDescendAt[startPoint] > bestSum)
        {
            bestSum = bestCumulative + scoreIfDescendAt[startPoint];
        }

        result[startPoint] = bestSum;

    }
    return *max_element(result.begin(), result.end());
}


int blah(const vector<int>& A, const vector<int>& B)
{
    int result = numeric_limits<int>::min();
    for (int i = 0; i < A.size(); i++)
    {
        int maxB = numeric_limits<int>::min();
        int sum = 0;
        for (int j = i; j >= 0; j--)
        {
            maxB = max(maxB, B[j]);
            sum += A[j];
            result = max(result, sum + maxB);
        }
    }
    return result;

}

int findResultWithHorizontalStrip(const vector<vector<int>>& originalMatrix, int k)
{
    const int numRows = originalMatrix.size();
    const int numCols = originalMatrix[0].size();
    int result = numeric_limits<int>::min();

    vector<vector<int>> prefixSumUpToCol(numRows);
    for (int row = 0; row < numRows; row++)
    {
        prefixSumUpToCol[row].resize(numCols);
        int sum = 0;
        for (int col = 0; col < numCols; col++)
        {
            sum += originalMatrix[row][col];
            prefixSumUpToCol[row][col] = sum;
        }
    }

    for (int l = 0; l < numCols; l++)
    {
        for (int r = l; r < numCols; r++)
        {
            vector<int> rowSums(numRows);
            vector<int> negativeMinStripForRows(numRows);
            for (int row = 0; row < numRows; row++)
            {
                const int rowSum = (l > 0 ? prefixSumUpToCol[row][r] - prefixSumUpToCol[row][l - 1] : prefixSumUpToCol[row][r]);
#if 0
                for (int col = l; col <= r; col++)
                {
                    rowSum += originalMatrix[row][col];
                }
#endif
                rowSums[row] = rowSum;
                const int minStripForRow = findMinSubRangeBruteForce(originalMatrix[row], l, r, k);
                negativeMinStripForRows[row] = -minStripForRow;
            }
            result = max(result, findBestIfMovedFromAndDescended(rowSums, negativeMinStripForRows));
            //assert(blah(rowSums, negativeMinStripForRows) == findBestIfMovedFromAndDescended(rowSums, negativeMinStripForRows));
        }
    }
#if 0
    int maxSubMatrixSize = numeric_limits<int>::min();
    for (int l = 0; l < numCols; l++)
    {
        for (int r = l; r < numCols; r++)
        {
            int currentBestSubMatrix = 0;
            int currentMinSubMatrixStrip = 0;
            for (int row = 0; row < numRows; row++)
            {
                int rowSum = 0;
                for (int col = l; col <= r; col++)
                {
                    rowSum += originalMatrix[row][col];
                }
                const int minStripForRow = findMinSubRangeBruteForce(originalMatrix[row], l, r, k);
                if (row == 0)
                {
                    currentBestSubMatrix = rowSum;
                    currentMinSubMatrixStrip = minStripForRow;
                }
                if (currentBestSubMatrix + rowSum - min(currentMinSubMatrixStrip, minStripForRow) < rowSum - minStripForRow)
                {
                    cout << "Reset!" << endl;
                    // Kadane's algorithm - complete reset.
                    currentBestSubMatrix = rowSum;
                    currentMinSubMatrixStrip = minStripForRow;
                }
                else
                {
                    if (row != 0)
                    {
                    currentBestSubMatrix += rowSum;
                    currentMinSubMatrixStrip = min(currentMinSubMatrixStrip, minStripForRow);
                    }
                }
                //currentBestSubMatrix += rowSum;
                const int currentBestStrippedSubMatrix = currentBestSubMatrix - currentMinSubMatrixStrip;
                cout << "l: " << l << " r: " << r << " row: " << row << " rowSum: " << rowSum << " currentBestSubMatrix: " << currentBestSubMatrix << " currentMinSubMatrixStrip: " << currentMinSubMatrixStrip << " currentBestStrippedSubMatrix: " << currentBestStrippedSubMatrix << " minStripForRow: " << minStripForRow << endl;

                maxSubMatrixSize = max(maxSubMatrixSize, currentBestSubMatrix);
                result = max(result, currentBestStrippedSubMatrix);
            }
        }
    }
    //cout << "maxSubMatrixSize " << maxSubMatrixSize << " weeble: " << findMaxSubMatrix(originalMatrix) << endl;
    //assert(maxSubMatrixSize == findMaxSubMatrix(originalMatrix));
#endif

    const int largestSubMatrixLeavingRoomForStrip = findMaxSubMatrix(originalMatrix, true);
    cout << "largestSubMatrixLeavingRoomForStrip: " << largestSubMatrixLeavingRoomForStrip << endl;
    result = max(result, findMaxSubMatrix(originalMatrix, true));
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
    cout << "Rotated!" << endl;
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

        const int numRows = rand() % 6 + 1;
        const int numCols = rand() % 6 + 1;
        //const int numRows = 380;
        //const int numCols = 380;
        const int k = rand() % 15 + 1;
        //const int k = rand() % 500 + 1;

        cout << numRows << " " << numCols << " " << k << endl;

        const int maxValue = 10;
        const int minValue = -10;
        //const int maxValue = 5000;
        //const int minValue = -5000;

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
    assert(resultBruteForce == result);
#endif
}


