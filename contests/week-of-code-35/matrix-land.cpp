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
    BestSubarraySumUpTo(int64_t sum, int subArrayStartIndex, int64_t maxValue)
        : sum{sum}, subArrayStartIndex{subArrayStartIndex}, maxValue{maxValue}
    {
    }
    int64_t sum = 0;
    int subArrayStartIndex = 0;
    int64_t maxValue = 0;
};


vector<int64_t> findBestIfMovedFromAndDescended(const vector<int64_t>& row, const vector<int64_t>& scoreIfDescendAt)
{
    cout << "findBestIfMovedFromAndDescended row: " << endl;
    for (const auto x : row)
    {
        cout << x << " ";
    }
    cout << endl;
    cout << "scoreIfDescendAt: " << endl;
    for (const auto x : scoreIfDescendAt)
    {
        cout << x << " ";
    }
    cout << endl;
    vector<int64_t> result(row.size());
    int64_t bestSum = numeric_limits<int64_t>::min();
    int64_t cumulative = numeric_limits<int64_t>::min();
    cout << "original cumulative: " << cumulative << endl;
    cout << "cumulative < 0? " << (cumulative < 0) << endl;
    int64_t bestCumulative = numeric_limits<int64_t>::min();
    int64_t lowestDescentToBeatBestSum = numeric_limits<int64_t>::max();
    int64_t startIndexIfBeatBestSum = -1;
    int64_t bestSumIfBeat = -1;
    int64_t highestBonus = numeric_limits<int64_t>::min();
    int startPoint = 0;
    for (int endPoint = 0; endPoint < row.size(); endPoint++)
    {
        cout << "row[endPoint]: " << row[endPoint] << endl;
        if (bestSum == numeric_limits<int64_t>::min())
        {
            bestSum = row[endPoint] + scoreIfDescendAt[endPoint];
        }
        else
        {
            bestSum += row[endPoint];
            bestSumIfBeat += row[endPoint];
        }
        cout << "endpoint: " << endPoint << endl;
        //if (row[endPoint] > cumulative + row[endPoint])
        if (cumulative < 0)
        {
            cumulative = row[endPoint];
            cout << "New cumulative: " << cumulative << endl;
        }
        else
        {
            cumulative += row[endPoint];
            cout << "updated cumulative; now: " << cumulative << endl;
        }
        if (cumulative > bestCumulative)
        {
            bestCumulative = cumulative;
            cout << " new bestCumulative " << bestCumulative << endl;
            const int64_t deficit = bestSum - (cumulative + scoreIfDescendAt[endPoint]);
            cout << "deficit: " << deficit << " bestSum:" << bestSum << endl;
            lowestDescentToBeatBestSum = min(lowestDescentToBeatBestSum, scoreIfDescendAt[endPoint] + deficit);
            startIndexIfBeatBestSum = endPoint;
            bestSumIfBeat = cumulative;
            cout << "cumulative: " << cumulative << " lowestDescentToBeatBestSum: " << lowestDescentToBeatBestSum << endl;
        }
        if (cumulative + scoreIfDescendAt[endPoint] > bestSum)
        {
            bestSum = cumulative + scoreIfDescendAt[endPoint];
            lowestDescentToBeatBestSum = numeric_limits<int64_t>::max();
            startIndexIfBeatBestSum = endPoint;
        }
        if (scoreIfDescendAt[endPoint] >= lowestDescentToBeatBestSum)
        {
            startPoint = startIndexIfBeatBestSum;
            bestSum = bestSumIfBeat + scoreIfDescendAt[endPoint];
            cout << "Got a new favourite, I think due to scoreIfDescendAt: " << scoreIfDescendAt[endPoint] << " lowestDescentToBeatBestSum: " << lowestDescentToBeatBestSum << " bestSumIfBeat: " << bestSumIfBeat << " new bestSum: " << bestSum << endl;
            lowestDescentToBeatBestSum = numeric_limits<int64_t>::max();
        }

        result[endPoint] = bestSum;

#ifdef BRUTE_FORCE
        {
            // Verify.
            int64_t bestSumDebug = std::numeric_limits<int64_t>::min();
            int cumulativeSum = 0;
            int startPointDebug = 0;
            auto bestBDebug = std::numeric_limits<int64_t>::min();
            for (int startPoint = endPoint; startPoint >= 0; startPoint--)
            {
                cumulativeSum += row[startPoint];
                if (scoreIfDescendAt[startPoint] > bestBDebug)
                    bestBDebug = scoreIfDescendAt[startPoint];
                if (cumulativeSum + bestBDebug > bestSumDebug)
                {
                    bestSumDebug = cumulativeSum + bestBDebug;
                    startPointDebug = startPoint;
                    //cout << "  endPoint: " << endPoint << " startPoint: " << startPoint << " bestSumDebug: " << bestSumDebug << endl;
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
            //cout << " endPoint: " << endPoint << " bestSumDebug: " << bestSumDebug << " startPointDebug: " << startPointDebug << endl;
            //result[endPoint] = {bestSumDebug, startPointDebug};
            assert(bestSum == bestSumDebug);
            //assert(startPointDebug == startPoint);
        }
#endif
    }
    return result;
}

vector<BestSubarraySumUpTo> findMaxSubarraySumEndingAt(const vector<int64_t>& A, const vector<int64_t>& bonusIfStartAt)
{
    vector<BestSubarraySumUpTo> result(A.size());
    int64_t bestSum = std::numeric_limits<int64_t>::min();
    int startIndex = 0;
    for (int endPoint = 0; endPoint < A.size(); endPoint++)
    {
        if (bestSum < 0)
        {
            bestSum = A[endPoint];
            startIndex = endPoint;
        }
        else
        {
            bestSum += A[endPoint];
        }
        result[endPoint] = {bestSum, startIndex, 0};

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
                }
                else if (cumulativeSum + bestBDebug == bestSumDebug)
                {
                    startPointDebug = startPoint;
                }
            }
            assert(bestSum == bestSumDebug);
            assert(startPointDebug == startIndex);
        }
#endif
    }
    return result;
}
vector<int64_t> findBestIfMovedFromAndDescendedReversed(const vector<int64_t>& A, const vector<int64_t>& scoreIfDescendAt)
{
    const vector<int64_t> aReversed(A.rbegin(), A.rend());
    const vector<int64_t> scoreIfDescendAtReversed(scoreIfDescendAt.rbegin(), scoreIfDescendAt.rend());
    auto result = findBestIfMovedFromAndDescended(aReversed, scoreIfDescendAtReversed);
    reverse(result.begin(), result.end());
    assert(result.size() == A.size());
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
        for (const auto x : A[row])
        {
            cout << x << " ";
        }
        cout << endl;
        cout << "lookup!" << endl;
        for (const auto x : lookup[row + 1])
        {
            cout << x << " ";
        }
        cout << endl;
        const auto bestIfMovedRightFromAndDescended = findBestIfMovedFromAndDescendedReversed(A[row], lookup[row + 1]);
        const auto bestIfMovedLeftFromAndDescended = findBestIfMovedFromAndDescended(A[row], lookup[row + 1]);
        const auto bestGobbleToLeftFrom = findMaxSubarraySumEndingAt(A[row], rowOfZeros);
        const auto bestGobbleToRightFrom = findMaxSubarraySumEndingAtReversed(A[row], rowOfZeros);

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
#if 0
    vector<int64_t> k(4'000'000);
    const int range = 4'000'000 * 250;
    for (int i = 0; i < k.size(); i++)
    {
        k[i] = rand() % (2 * range + 1) - range;
    }
    sort(k.begin(), k.end());
    cout << "Sorted " << k.size() << endl;
    return 0;
#endif
#if 0
    while (true)
    {
        const int n = rand() % 8 + 1;
        const int range = 50;
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
        const int numRows = (rand() % 2) + 1;
        const int numCols = (rand() % 2) + 1;
        const int range = 10;
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
