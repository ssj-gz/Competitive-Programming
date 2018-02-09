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

constexpr int maxN = 380;
// The minSubrangeForRow[row][l][r] lookup table returns the sum of the smallest subrange x1-x2 in row, where
// l <= x1 <= x2 <= r, and the length of the subrange x1-x2 is at most k.
// I use a C-style array as vectors were too slow (by a factor of approx 2!).
int minSubrangeForRow[maxN][maxN][maxN];

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

// Finds the s, t, b such that sum [ t <= r <= b ] { rowSums[r] } + negativeMinStripForRows[s] is maximised
// over all 0 <= t <= s <= b.  Just returns this sum, not s, t and b themselves.
int findBestStrippedSubMatrixForThisRange(const vector<int>& rowSums, const vector<int>& negativeMinStripForRows)
{
    // Taken from Matrix Land, which is very similar.
    int bestSum = negativeMinStripForRows.front(); // Should be negativeMinStripForRows.front() + rowSums[0], but the body of the loop adds the rowSums[0] on the first iteration.
    int bestCumulative = numeric_limits<int>::min();
    int result = numeric_limits<int>::min();
    for (int startRow = 0; startRow < rowSums.size(); startRow++)
    {
        bestSum += rowSums[startRow];

        if (bestCumulative < 0)
        {
            // Just as in Kadane's algorithm, if breaking with the existing
            // bestCumulative give a better result, then do so.
            bestCumulative = rowSums[startRow];
        }
        else
            bestCumulative += rowSums[startRow];

        if (bestCumulative + negativeMinStripForRows[startRow] > bestSum)
        {
            bestSum = bestCumulative + negativeMinStripForRows[startRow];
        }

        result = max(result, bestSum);

    }
    return result;
}

// Build minSubrangeForRow lookup table.
void computeMinSubrangeLookup(const vector<vector<int>>& originalMatrix, int k)
{
    const int numRows = originalMatrix.size();
    const int numCols = originalMatrix[0].size();
    for (int rowIndex = 0; rowIndex < numRows; rowIndex++)
    {
        const auto& row = originalMatrix[rowIndex];
        vector<vector<int>> minSubrangeEndingAtRightIgnoringKLookup(numCols, vector<int>(row.size(), numeric_limits<int>::max()));
        // In the first pass, find, for each l and r, the subrange (of any length) that ends precisely at r and whose
        // start is at least l that gives the smallest sum.
        for (int l = 0; l < numCols; l++)
        {
            int minEndingAtRIgnoringK = numeric_limits<int>::max();
            for (int r = l; r < numCols; r++)
            {
                if (minEndingAtRIgnoringK > 0)
                {
                    minEndingAtRIgnoringK = 0;
                }
                minEndingAtRIgnoringK += row[r];
                minSubrangeEndingAtRightIgnoringKLookup[l][r] = minEndingAtRIgnoringK;
            }
        }

        // Now use this result to find, for each l and r, the largest subrange of length at most k that lies in the range l-r.
        for (int l = 0; l < numCols; l++)
        {
            int minStartingFromAtLeastL = numeric_limits<int>::max();
            for (int r = l; r < numCols; r++)
            {
                const int leftBoundForEndingAtR = (r - l + 1 <= k ? l : r - k + 1); // This part enforces the "maximum of length k" part.
                minStartingFromAtLeastL = min(minStartingFromAtLeastL, minSubrangeEndingAtRightIgnoringKLookup[leftBoundForEndingAtR][r]);
                minSubrangeForRow[rowIndex][l][r] = minStartingFromAtLeastL;
            }
        }
    }
}

int findResultWithHorizontalStrip(const vector<vector<int>>& originalMatrix, int k)
{
    const int numRows = originalMatrix.size();
    const int numCols = originalMatrix[0].size();
    int result = numeric_limits<int>::min();

    // Lookup table for quickly finding the sum of all elements in a range in a row.
    int  prefixSumUpToCol[maxN][maxN];
    for (int row = 0; row < numRows; row++)
    {
        int sum = 0;
        for (int col = 0; col < numCols; col++)
        {
            sum += originalMatrix[row][col];
            prefixSumUpToCol[row][col] = sum;
        }
    }
    // Compute lookup table for quickly finding the subrange with the least sum in a range in a row.
    computeMinSubrangeLookup(originalMatrix, k);

    int largestProperSubMatrixSum = numeric_limits<int>::min();
    vector<int> rowSums(numRows);
    vector<int> negativeMinStripForRows(numRows);
    for (int l = 0; l < numCols; l++)
    {
        const int leftForPrefixSum = (l > 0 ? l - 1 : 0);
        for (int r = l; r < numCols; r++)
        {
            for (int row = 0; row < numRows; row++)
            {
                auto prefixSumsForRow = prefixSumUpToCol[row];
                const int rowSum = (l > 0 ? prefixSumsForRow[r] - prefixSumsForRow[l - 1] : prefixSumsForRow[r]);
                rowSums[row] = rowSum;
                const int minStripForRow = minSubrangeForRow[row][l][r];
                assert(minStripForRow == findMinSubRangeBruteForce(originalMatrix[row], l, r, k));
                negativeMinStripForRows[row] = -minStripForRow;
            }
            // For this range l, r, find the best stripped submatrix whose left and right edges are l and r, respectively.
            result = max(result, findBestStrippedSubMatrixForThisRange(rowSums, negativeMinStripForRows));

            // Also, while we're at it, find the proper submatrix whose left and right edges are l and r that gives
            // the largest sum.
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

                const bool isProper = !(row == numRows - 1 && largestSubMatrixTop == 0 && l == 0 && r == numCols - 1);
                if (isProper)
                {
                    largestProperSubMatrixSum = max(largestProperSubMatrixSum, largestSubMatrixSum);
                }
            }
            // Add an edge case for the largestProperSubMatrixSum - knocking off the topmost row (and possibly
            // some subsequent rows) will give us a proper submatrix with a lower sum than the "full" matrix, 
            // and it's possible for this new submatrix to still be the best proper one.
            int sumFromBottom = 0;
            for (int row = numRows - 1; row >= 1; row--)
            {
                sumFromBottom += rowSums[row];
                largestProperSubMatrixSum = max(largestProperSubMatrixSum, sumFromBottom);
            }
        }
    }
    assert(largestProperSubMatrixSum == findMaxSubMatrix(originalMatrix, true));
    // The best result might just make some trivial 1x1 strip somewhere and take a matrix that doesn't 
    // contain the strip - in this case, the result will be the largest *proper* submatrix (so we have
    // some space for our 1x1 strip!).
    result = max(result, largestProperSubMatrixSum);
    return result;
}


int findResult(const vector<vector<int>>& originalMatrix, int k)
{
    int result = numeric_limits<int>::min();
    // Best result using horizontal strips.
    result = max(result, findResultWithHorizontalStrip(originalMatrix, k));

    const int numRows = originalMatrix.size();
    const int numCols = originalMatrix[0].size();

    // Best result using vertical strips - just rotate the matrix 90 (or maybe 270??)
    // degrees!
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


