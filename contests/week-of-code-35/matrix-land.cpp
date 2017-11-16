// Simon St James (ssjgz) - 2017-11-16 
// This is a test submission - the algorithm is too slow to pass, but I want to check correctness, at least!
#define BRUTE_FORCE
//#define RANDOM
//#define SUBMISSION
#ifdef SUBMISSION
#undef BRUTE_FORCE
#undef RANDOM
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

struct BestSubarraySumUpTo
{
    BestSubarraySumUpTo() = default;
    BestSubarraySumUpTo(int64_t sum, int subArrayStartIndex)
        : sum{sum}, subArrayStartIndex{subArrayStartIndex}
    {
    }
    int64_t sum = 0;
    int subArrayStartIndex = 0;
};

struct BestSubarraySum
{
    BestSubarraySum(int64_t sum, int subArrayStartIndex, int subArrayEndIndex)
        : sum{sum}, subArrayStartIndex{subArrayStartIndex}, subArrayEndIndex{subArrayEndIndex}
    {
    }
    int64_t sum = 0;
    int subArrayStartIndex = 0;
    int subArrayEndIndex = 0;
};

vector<BestSubarraySumUpTo> findMaxSubarraySumEndingAt(const vector<int64_t>& A, const vector<int64_t>& bonusIfStartAt)
{
    vector<BestSubarraySumUpTo> result(A.size());
    int64_t bestSum = std::numeric_limits<int64_t>::min();
    int64_t bestB = bonusIfStartAt.front();
    int64_t cumulativeSum = 0;
    int startPoint = 0;
    //cout << "blah" << endl;
    for (int endPoint = 0; endPoint < A.size(); endPoint++)
    {
        //cout << " endPoint: " << endPoint << endl;
        if (bonusIfStartAt[endPoint] > bestSum )
        {
            //cout << " found better, apparently" << endl;
            bestSum = A[endPoint] + bonusIfStartAt[endPoint];
            bestB = bonusIfStartAt[endPoint];
            cumulativeSum = A[endPoint];
            startPoint = endPoint;
        }
        else
        {
            cumulativeSum += A[endPoint];
            bestSum = cumulativeSum + bestB;
        }
        result[endPoint] = {bestSum, startPoint};
        //cout << " bestSum: " << bestSum << " cumulativeSum: " << cumulativeSum << " bestB: " << bestB << endl;
#ifdef BRUTE_FORCE
        {
            // Verify.
            int64_t bestSumDebug = std::numeric_limits<int64_t>::min();
            int cumulativeSum = 0;
            int startPointDebug = 0;
            for (int startPoint = endPoint; startPoint >= 0; startPoint--)
            {
                cumulativeSum += A[startPoint];
                if (cumulativeSum + bonusIfStartAt[startPoint] > bestSumDebug)
                {
                    bestSumDebug = cumulativeSum + bonusIfStartAt[startPoint];
                    startPointDebug = startPoint;
                }
                else if (cumulativeSum + bonusIfStartAt[startPoint] == bestSumDebug)
                {
                    startPointDebug = startPoint;
                }
            }
            //cout << " bestSumDebug: " << bestSumDebug << endl;
            assert(bestSum == bestSumDebug);
            assert(startPointDebug == startPoint);
        }
#endif
    }
    return result;
}

vector<BestSubarraySumUpTo> findMaxSubarraySumEndingAtReversed(const vector<int64_t>& A, const vector<int64_t>& bonusIfStartAt)
{
    const vector<int64_t> aReversed(A.rbegin(), A.rend());
    const vector<int64_t> bonusIfStartAtReversed(bonusIfStartAt.rbegin(), bonusIfStartAt.rend());
    const auto N = A.size();
    auto result = findMaxSubarraySumEndingAt(aReversed, bonusIfStartAtReversed);
    // Need to invert indices.
    for (auto& sumEndingAt : result)
    {
        sumEndingAt.subArrayStartIndex = (N - 1) - sumEndingAt.subArrayStartIndex;
    }
    reverse(result.begin(), result.end());
    assert(result.size() == A.size());
    return result;
}

vector<int64_t> extractSums(const vector<BestSubarraySumUpTo>& sumsAndStartIndices)
{
    vector<int64_t> result;
    for (const auto sumAndStartIndex : sumsAndStartIndices)
    {
        result.push_back(sumAndStartIndex.sum);
    }
    return result;
}

int64_t maxSum(const vector<vector<int64_t>>& A)
{
    const int numRows = A.size();
    const int numCols = A[0].size();
    cout << "numRows: " << numRows << " numCols: " << numCols << endl;
    const vector<int64_t> rowOfZeros(numCols, 0);

    vector<vector<int64_t>> lookup(numRows, vector<int64_t>(numCols, std::numeric_limits<int64_t>::min()));
    lookup.push_back(rowOfZeros); // "Sentinel" row.

    int64_t best = std::numeric_limits<int64_t>::min();

    for (int row = numRows - 1; row >= 0; row--)
    {
        cout << "row!" << row << endl;
        const auto bestIfMovedRightFromAndDescended = extractSums(findMaxSubarraySumEndingAtReversed(A[row], lookup[row + 1]));
        const auto bestIfMovedLeftFromAndDescended = extractSums(findMaxSubarraySumEndingAt(A[row], lookup[row + 1]));
        const auto bestGobbleToLeftFrom = findMaxSubarraySumEndingAt(A[row], rowOfZeros);
        const auto bestGobbleToRightFrom = findMaxSubarraySumEndingAtReversed(A[row], rowOfZeros);

#if 0
        vector<int64_t> cumulativeDescendAtScore;
        {
            int64_t cumulativeScore = 0;
            for (int col = 0; col < numCols; col++)
            {
                cumulativeScore += lookup[row + 1][col];
                cumulativeDescendAtScore.push_back(cumulativeScore);
            }
        }
#endif

        int startCol = 0;
        int previousGobbleLeftIndex = -1;
        while (startCol < numCols)
        {
            cout << " startCol: " << startCol << endl;
            int64_t bestGobbleSum = 0;
            const int bestGobbleStartIndex = bestGobbleToLeftFrom[startCol].subArrayStartIndex;
            bestGobbleSum += bestGobbleToLeftFrom[startCol].sum;
            int bestGobbleEndIndex = startCol;
            if (startCol < numCols - 1 && bestGobbleToRightFrom[startCol + 1].sum >= 0)
            {
                bestGobbleEndIndex = bestGobbleToRightFrom[startCol + 1].subArrayStartIndex;
                bestGobbleSum += bestGobbleToRightFrom[startCol + 1].sum;
            }
            assert(previousGobbleLeftIndex == -1 || bestGobbleStartIndex >= previousGobbleLeftIndex);
            cout << "  bestGobbleSum: " << bestGobbleSum << " bestGobbleStartIndex: " << bestGobbleStartIndex << " bestGobbleEndIndex: " << bestGobbleEndIndex << endl;
            // Descend in the middle of gobble-range.
            int64_t bestForStartCol = std::numeric_limits<int64_t>::min();
             for (int i = bestGobbleStartIndex; i <= bestGobbleEndIndex; i++)
            {
                const int64_t scoreIfDescendHere = bestGobbleSum + lookup[row + 1][i];
                bestForStartCol = max(bestForStartCol, scoreIfDescendHere);
            }

            //const auto bestScoreIfDescendInGobbleRange = bestGobbleSum +  cumulativeDescendAtScore[bestGobbleEndIndex] - (bestGobbleStartIndex > 0 ? cumulativeDescendAtScore[bestGobbleStartIndex - 1] : 0);
            //bestForStartCol = max(bestForStartCol, bestScoreIfDescendInGobbleRange);
            cout << "  bestForStartCol after descend in gobble-range: " << bestForStartCol << endl;
            // Descend to the left of gobble-range.
            if (bestGobbleStartIndex > 0)
            {
                const int64_t score = bestGobbleSum + bestIfMovedLeftFromAndDescended[bestGobbleStartIndex - 1];
                bestForStartCol = max(bestForStartCol, score);
            }
            cout << "  bestForStartCol after descend to left of gobble-range: " << bestForStartCol << endl;
            // Descend to the right of gobble-range.
            if (bestGobbleEndIndex < numCols - 1)
            {
                const int64_t score = bestGobbleSum + bestIfMovedRightFromAndDescended[bestGobbleEndIndex + 1];
                cout << "bestIfMovedRightFromAndDescended from " << (bestGobbleEndIndex + 1) << " : " << score << endl;
                bestForStartCol = max(bestForStartCol, score);
            }
            cout << "  bestForStartCol: " << bestForStartCol << endl;
            // All columns in the gobble range have the same max attainable score if we start there TODO - no, this is wrong!
            lookup[row][startCol] = max(lookup[row][startCol], bestForStartCol);
            //for (int i = bestGobbleStartIndex; i <= bestGobbleEndIndex; i++)
            //{
                //lookup[row][i] = max(lookup[row][i], bestForStartCol);
            //}
            //startCol = bestGobbleEndIndex + 1;
            startCol++;
            previousGobbleLeftIndex = bestGobbleStartIndex;
        }
    }

    for (const auto bestForStartCol : lookup[0])
    {
        best = max(best, bestForStartCol);
    }

    return best;
}

int64_t maxSumBruteForce(int startRow, int startCol, const vector<vector<int64_t>>& A, vector<vector<int64_t>>& lookup)
{
    const int numRows = A.size();
    const int numCols = A[0].size();

    if (startRow == numRows)
        return 0;

    if (lookup[startRow][startCol] != -1)
        return lookup[startRow][startCol];

    int64_t best = std::numeric_limits<int64_t>::min();
    int bestLeft = -1;
    int bestRight = -1;
    int bestDescend = -1;
    for (int clearToLeftCol = 0; clearToLeftCol <= startCol; clearToLeftCol++)
    {
        for (int clearToRightCol = startCol; clearToRightCol < numCols; clearToRightCol++)
        {
            int64_t scoreAfterClearing = 0;
            for (int i = clearToLeftCol; i <= clearToRightCol; i++)
            {
                scoreAfterClearing += A[startRow][i];
            }
            for (int descendCol = clearToLeftCol; descendCol <= clearToRightCol; descendCol++)
            {
                const auto bestIfClearAndDescendHere = scoreAfterClearing + maxSumBruteForce(startRow + 1, descendCol, A, lookup);
                //cout << "startRow: " << startRow << " bestIfClearAndDescendHere(" << descendCol << ") : " << bestIfClearAndDescendHere << endl;
                if (bestIfClearAndDescendHere > best)
                {
                    best = max(best, bestIfClearAndDescendHere); 
                    bestLeft = clearToLeftCol;
                    bestRight = clearToRightCol;
                    bestDescend = descendCol;
                }
            }
        }
    }

    assert(lookup[startRow][startCol] == -1);
    lookup[startRow][startCol] = best;

    cout << "brute force startRow: " << startRow << " startCol: " << startCol << " best: " << best << " bestLeft: " << bestLeft << " bestRight: " << bestRight << " bestDescend: " << bestDescend << endl;

    return best;


}

int64_t maxSumBruteForce(const vector<vector<int64_t>>& A)
{
    const int numRows = A.size();
    const int numCols = A[0].size();

    vector<vector<int64_t>> bruteForceLookup(numRows, vector<int64_t>(numCols, -1));
    //const vector<int64_t> rowOfZeros(numCols, 0);
    //bruteForceLookup.push_back(rowOfZeros);
    int64_t bestBruteForce = std::numeric_limits<int64_t>::min();
    for (int startCol = 0; startCol < numCols; startCol++)
    {
        const auto bestStartingHereBruteForce = maxSumBruteForce(0, startCol, A, bruteForceLookup);
        bestBruteForce = max(bestBruteForce, bestStartingHereBruteForce);
        cout << "startCol: " << startCol << " bestStartingHereBruteForce: " << bestStartingHereBruteForce << endl;
    }
    return bestBruteForce;
}

int main()
{
#ifdef RANDOM

    srand(time(0));
    while (true)
    {
        const int numRows = (rand() % 3) + 1;
        const int numCols = (rand() % 3) + 1;
        const int range = 250;
        vector<vector<int64_t>> A(numRows, vector<int64_t>(numCols, -1));
        for (int i = 0; i < numRows; i++)
        {
            for (int j = 0; j < numCols; j++)
            {
                A[i][j] = rand() % (2 * range + 1) - range;
            }
        }
        const auto best = maxSum(A);
        const auto bestBruteForce = maxSumBruteForce(A);
        cout << "best: " << best << " bestBruteForce: " << bestBruteForce << endl;
        if (best != bestBruteForce)
        {
            cout << "Whoops: A: " << endl;
            for (int i = 0; i < numRows; i++)
            {
                for (int j = 0; j < numCols; j++)
                {
                    cout << A[i][j] << " ";
                }
                cout << endl;
            }
        }
        assert(best == bestBruteForce);
        //blah(a, b);
    }
#else

    int n, m;
    cin >> n >> m;

    vector<vector<int64_t>> A(n, vector<int64_t>(m, -1));

    cout << "A:" << endl;
    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col < m; col++)
        {
            cin >> A[row][col];
            cout << A[row][col] << " ";
        }
        cout << endl;
    }

    const int64_t best = maxSum(A);
#ifdef BRUTE_FORCE
    const auto bestBruteForce = maxSumBruteForce(A);
    cout << "best: " << best << " best brute force: " << bestBruteForce << endl;
    if (best != bestBruteForce)
    {
        cout << "Whoops: A: " << endl;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < m; j++)
            {
                cout << A[i][j] << " ";
            }
            cout << endl;
        }
    }
    assert(best == bestBruteForce);
#endif
    cout << best << endl;
#endif
}
