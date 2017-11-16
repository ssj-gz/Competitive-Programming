// Simon St James (ssjgz) - 2017-11-16 
// This is a test submission - the algorithm is too slow to pass, but I want to check correctness, at least!
#define BRUTE_FORCE
#define RANDOM
//#define SUBMISSION
#ifdef SUBMISSION
#undef BRUTE_FORCE
#undef RANDOM
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <map>
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
    cout << "findMaxSubarraySumEndingAt: A:" << endl;
    for (const auto x : A)
    {
        cout << x << " ";
    }
    cout << endl;
    cout << "bonusIfStartAt:" << endl;
    for (const auto x : bonusIfStartAt)
    {
        cout << x << " ";
    }
    cout << endl;
    vector<BestSubarraySumUpTo> result(A.size());
    map<int64_t, int64_t> things;
    int startPoint = 0;
    int64_t bestSum = std::numeric_limits<int64_t>::min();
    int64_t bestB = std::numeric_limits<int64_t>::min(); //bonusIfStartAt.front();
    int64_t cumulativeSum = 0;
    for (int endPoint = 0; endPoint < A.size(); endPoint++)
    {
#if 0
    int64_t bestSum = std::numeric_limits<int64_t>::min();
    int64_t bestB = std::numeric_limits<int64_t>::min(); //bonusIfStartAt.front();
    int64_t cumulativeSum = 0;
    int startPoint = 0;
    //cout << "blah" << endl;
        cout << " endPoint: " << endPoint << endl;
        if (A[endPoint] + bonusIfStartAt[endPoint] >= cumulativeSum + A[endPoint] + max(bestB, bonusIfStartAt[endPoint]))
        //if (A[endPoint] + bonusIfStartAt[endPoint] > bestSum + A[endPoint])
        {
            bestSum = A[endPoint] + bonusIfStartAt[endPoint];
            bestB = bonusIfStartAt[endPoint];
            cumulativeSum = A[endPoint];
            startPoint = endPoint;
            cout << " found better, apparently: set bestB: " << bestB << " cumulativeSum: " << cumulativeSum << " bestSum: " << bestSum << endl;

        
        else
        {
            if (bonusIfStartAt[endPoint] > bestB)
            {
                bestB = bonusIfStartAt[endPoint];
                cout << " updated bestB: " << bestB << endl;
            }
            cumulativeSum += A[endPoint];
            bestSum = cumulativeSum + bestB;
            cout << " updated cumulativeSum: " << cumulativeSum << " bestB: " << bestB << " bestB " << " bestSum: " << bestSum << endl;
        }
        result[endPoint] = {bestSum, startPoint};
        cout << " End of loop: endPoint: " << endPoint << " bestSum: " << bestSum << " cumulativeSum: " << cumulativeSum << " bestB: " << bestB << endl;
#endif
        cout << "iteration - endPoint: " << endPoint << endl;
        const auto oldBestB = bestB;
        //bestB = max(bestB, bonusIfStartAt[endPoint]);
        things[bonusIfStartAt[endPoint]]++;
        while (true)
        {
            if (startPoint == endPoint)
                break;
            //if (A[startPoint] >= 0)
                //break;
            if (cumulativeSum >= 0)
                break;
            bool eraseStartPoint = false;
            // Best sum if start point removed?
            int64_t oldBest = things.rbegin()->first;
            int64_t newBest = oldBest;
            things[bonusIfStartAt[startPoint]]--;
            assert(things[bonusIfStartAt[startPoint]] >= 0);
            if (things[bonusIfStartAt[startPoint]] == 0)
            {
                things.erase(things.find(bonusIfStartAt[startPoint]));
                newBest = things.rbegin()->first;
            }
            if (newBest < oldBest)
            {
                if (oldBest - newBest > -A[startPoint])
                    eraseStartPoint = true;
            }
            else
                eraseStartPoint = true;
            cout << "newBest if ditched startPoint: " << startPoint << " = " << newBest << endl;
            cout << "increase due to best if ditched startPoint: " << newBest - oldBest << endl;
            cout << "increase due to startCol if ditched startPoint: " << -A[startPoint] << endl;
            cout << "Ditching start point: " << startPoint << endl;
            if (!eraseStartPoint)
            {
                things[bonusIfStartAt[startPoint]]++;
                break;
            }
            else
            {
                cumulativeSum -= A[startPoint];
                bestB = newBest;
                bestSum = cumulativeSum + bestB;
                startPoint++;
            }
        }

        if (A[endPoint] + bonusIfStartAt[endPoint] >= cumulativeSum + A[endPoint] + max(bestB, bonusIfStartAt[endPoint]))
        {
            bestSum = A[endPoint] + bonusIfStartAt[endPoint];
            bestB = bonusIfStartAt[endPoint];
            cumulativeSum = A[endPoint];
            startPoint = endPoint;
            things.clear();
            things[bonusIfStartAt[startPoint]]++;
            cout << " found better, apparently: set bestB: " << bestB << " cumulativeSum: " << cumulativeSum << " bestSum: " << bestSum << endl;

        }
        else
        {
            if (bonusIfStartAt[endPoint] > bestB)
            {
                bestB = bonusIfStartAt[endPoint];
                cout << " updated bestB: " << bestB << endl;
            }
            cumulativeSum += A[endPoint];
            bestSum = cumulativeSum + bestB;
            cout << " updated cumulativeSum: " << cumulativeSum << " bestB: " << bestB << " bestB " << " bestSum: " << bestSum << endl;
        }
        result[endPoint] = {bestSum, startPoint};

#ifdef BRUTE_FORCE
        {
            // Verify.
            int64_t bestSumDebug = std::numeric_limits<int64_t>::min();
            int cumulativeSum = 0;
            int startPointDebug = 0;
            auto bestBDebug = std::numeric_limits<int64_t>::min();
            for (int startPoint = endPoint; startPoint >= 0; startPoint--)
            {
                cumulativeSum += A[startPoint];
                if (bonusIfStartAt[startPoint] > bestBDebug)
                    bestBDebug = bonusIfStartAt[startPoint];
                if (cumulativeSum + bestBDebug > bestSumDebug)
                {
                    bestSumDebug = cumulativeSum + bestBDebug;
                    startPointDebug = startPoint;
                    cout << "  endPoint: " << endPoint << " startPoint: " << startPoint << " bestSumDebug: " << bestSumDebug << endl;
                }
                else if (cumulativeSum + bestBDebug == bestSumDebug)
                {
                    startPointDebug = startPoint;
                }
                if (endPoint == 2)
                {
                    //cout << "endPoint: " << endPoint << " startPoint: " << startPoint << " cumulativeSum: " << cumulativeSum << " bestSumDebug: " << bestSumDebug << endl;
                }
            }
            cout << " endPoint: " << endPoint << " bestSumDebug: " << bestSumDebug << " bestSum: " << bestSum << " startPointDebug: " << startPointDebug << " startPoint:" << startPoint << endl;
            assert(bestSum == bestSumDebug);
            //assert(startPointDebug == startPoint);
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
    int bestScoreAfterClearing = 0;
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
                cout << "startRow: " << startRow << " bestIfClearAndDescendHere(" << descendCol << ") : " << bestIfClearAndDescendHere << endl;
                if (bestIfClearAndDescendHere > best)
                {
                    best = max(best, bestIfClearAndDescendHere); 
                    bestLeft = clearToLeftCol;
                    bestRight = clearToRightCol;
                    bestDescend = descendCol;
                    bestScoreAfterClearing = scoreAfterClearing;
                }
            }
        }
    }

    assert(lookup[startRow][startCol] == -1);
    lookup[startRow][startCol] = best;

    cout << "brute force startRow: " << startRow << " startCol: " << startCol << " best: " << best << " bestLeft: " << bestLeft << " bestRight: " << bestRight << " bestDescend: " << bestDescend << " bestScoreAfterClearing: " << bestScoreAfterClearing << endl;

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
#if 1
    while (true)
    {
        const int n = rand() % 3 + 1;
        const int range = 10;
        vector<int64_t> bloo;
        vector<int64_t> bloo2;
        for (int i = 0; i < n; i++)
        {
            bloo.push_back(rand() % (2 * range + 1) - range);
            bloo2.push_back(rand() % (2 * range + 1) - range);
        }
        findMaxSubarraySumEndingAt(bloo, vector<int64_t>(n, 0));
        findMaxSubarraySumEndingAt(bloo, bloo2);
        //findMaxSubarraySumEndingAt(vector<int64_t>{56, -93, 225}, vector<int64_t>(3, 0));
        cout << "n: " << n << endl;
    }
#endif
    while (true)
    {
        const int numRows = (rand() % 6) + 1;
        const int numCols = (rand() % 6) + 1;
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
