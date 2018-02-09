// Simon St James (ssjgz) - 2018-02-09 12:57
#define BRUTE_FORCE
#define SUBMISSION
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <limits>
#include <cassert>
#include <algorithm>
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
    //vector<int> result(row.size());
    int bestSum = scoreIfDescendAt.front(); // Should be scoreIfDescendAt.front() + row[0], but the body of the loop adds the row[0] on the first iteration.
    int bestCumulative = numeric_limits<int>::min();
    int result = numeric_limits<int>::min();
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

        result = max(result, bestSum);

    }
    return result;
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

vector<vector<int>> computeMinSubrangeLookup(const vector<int>& row, int k)
{
    const int rowSize = row.size();
    vector<vector<int>> minSubrangeEndingAtRightIgnoringKLookup(rowSize, vector<int>(row.size(), numeric_limits<int>::max()));
    for (int l = 0; l < rowSize; l++)
    {
        int minEndingAtRIgnoringK = numeric_limits<int>::max();
        for (int r = l; r < rowSize; r++)
        {
            if (minEndingAtRIgnoringK > 0)
            {
                minEndingAtRIgnoringK = 0;
            }
            minEndingAtRIgnoringK += row[r];
            minSubrangeEndingAtRightIgnoringKLookup[l][r] = minEndingAtRIgnoringK;
        }
    }

    vector<vector<int>> minSubrangeLookup(rowSize, vector<int>(rowSize, numeric_limits<int>::max()));
    for (int l = 0; l < rowSize; l++)
    {
        int minStartingFromL = numeric_limits<int>::max();
        auto& minSubrangeLookupForL = minSubrangeLookup[l];
        for (int r = l; r < rowSize; r++)
        {
            const int leftBoundForEndingAtR = (r - l + 1 <= k ? l : r - k + 1);
            minStartingFromL = min(minStartingFromL, minSubrangeEndingAtRightIgnoringKLookup[leftBoundForEndingAtR][r]);
            minSubrangeLookupForL[r] = minStartingFromL;
        }
    }

    return minSubrangeLookup;
}

int findResultWithHorizontalStrip(const vector<vector<int>>& originalMatrix, int k)
{
    const int numRows = originalMatrix.size();
    const int numCols = originalMatrix[0].size();
    int result = numeric_limits<int>::min();

    vector<vector<int>> prefixSumUpToCol(numRows);
    vector<vector<vector<int>>> minSubrangeForRow(numRows);
    for (int row = 0; row < numRows; row++)
    {
        prefixSumUpToCol[row].resize(numCols);
        int sum = 0;
        for (int col = 0; col < numCols; col++)
        {
            sum += originalMatrix[row][col];
            prefixSumUpToCol[row][col] = sum;
        }

        minSubrangeForRow[row] = computeMinSubrangeLookup(originalMatrix[row], k);
    }

    int largestProperSubMatrixSum = numeric_limits<int>::min();
    vector<int> rowSums(numRows);
    vector<int> negativeMinStripForRows(numRows);
    for (int l = 0; l < numCols; l++)
    {
        const int leftForPrefixSum = (l > 0 ? l - 1 : 0);
        for (int r = l; r < numCols; r++)
        {
            const bool isFullWidth = (l == 0 && r == numCols - 1);
            int* rowsSumsPointer = rowSums.data();
            int* negativeMinStripForRowsPointer = negativeMinStripForRows.data();
            for (int row = 0; row < numRows; row++)
            {
                vector<int>& prefixSumsForRow = prefixSumUpToCol[row];
                const int rowSum = (l > 0 ? prefixSumsForRow[r] - prefixSumsForRow[l - 1] : prefixSumsForRow[r]);
                *rowsSumsPointer = rowSum;
                rowsSumsPointer++;
                const int minStripForRow = minSubrangeForRow[row][l][r];
                assert(minStripForRow == findMinSubRangeBruteForce(originalMatrix[row], l, r, k));
                *negativeMinStripForRowsPointer = -minStripForRow;
                negativeMinStripForRowsPointer++;
            }
            result = max(result, findBestIfMovedFromAndDescended(rowSums, negativeMinStripForRows));

            int largestSubMatrixSum = numeric_limits<int>::min();
            int largestSubMatrixTop = -1;
            for (int row = 0; row < numRows; row++)
            {
                if (largestSubMatrixSum < 0)
                {
                    largestSubMatrixSum = 0;
                    largestSubMatrixTop = row;
                }
                largestSubMatrixSum += rowSums[row];

                const bool isProper = (row != numRows - 1 || largestSubMatrixTop != 0 || !isFullWidth);
                if (isProper)
                {
                    largestProperSubMatrixSum = max(largestProperSubMatrixSum, largestSubMatrixSum);
                }
            }
            int sumFromBottom = 0;
            for (int row = numRows - 1; row >= 1; row--)
            {
                sumFromBottom += rowSums[row];
                largestProperSubMatrixSum = max(largestProperSubMatrixSum, sumFromBottom);
            }
        }
    }
    assert(largestProperSubMatrixSum == findMaxSubMatrix(originalMatrix, true));
    result = max(result, largestProperSubMatrixSum);
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
    ios::sync_with_stdio(false);
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int numRows = rand() % 5 + 1;
        const int numCols = rand() % 5 + 1;
        //const int numRows = 380;
        //const int numCols = 380;
        const int k = rand() % 15 + 1;
        //const int k = rand() % 500 + 1;

        cout << numRows << " " << numCols << " " << k << endl;

        const int maxValue = 100;
        const int minValue = -100;
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
    cout << result << endl;

#ifdef BRUTE_FORCE
    const auto resultBruteForce = findResultBruteForce(matrix, k);
    cout << "result: " << result << " resultBruteForce: " << resultBruteForce << endl;
    assert(resultBruteForce == result);
#endif
}


