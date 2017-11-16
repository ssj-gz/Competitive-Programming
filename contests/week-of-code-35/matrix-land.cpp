// Simon St James (ssjgz) - 2017-11-16 
// This is a test submission - the algorithm is too slow to pass, but I want to check correctness, at least!
#define BRUTE_FORCE
//#define SUBMISSION
#ifdef SUBMISSION
#undef BRUTE_FORCE
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

int64_t maxSum(const vector<vector<int64_t>>& A)
{
    const int numRows = A.size();
    const int numCols = A[0].size();
    cout << "numRows: " << numRows << " numCols: " << numCols << endl;
    const vector<int64_t> rowOfZeros(numCols, 0);

    vector<vector<int64_t>> lookup(numRows, vector<int64_t>(numCols, -1));
    lookup.push_back(rowOfZeros); // "Sentinel" row.

    int64_t best = 0;

    for (int row = numRows - 1; row >= 0; row--)
    {
        cout << "row!" << row << endl;
        vector<int64_t> bestIfMovedRightFromAndDescended;
        {
#if 0
            vector<int64_t> bestFromRightCumulative;
            int64_t sumFromRight = 0;
            for (int startCol = numCols - 1; startCol >= 0; startCol--)
            {
                //sumFromRight += A[row][startCol];
                bestFromRightCumulative.push_back(A[row][startCol] + lookup[row + 1][startCol]);

            }
#endif
            for (const auto& blah : findMaxSubarraySumEndingAtReversed(A[row], lookup[row + 1]))
            {
                cout << " floop " << blah.sum << endl;
                bestIfMovedRightFromAndDescended.push_back(blah.sum);
            }
#ifdef BRUTE_FORCE
            {
                // Verify.
                for (int startCol = 0; startCol < numCols; startCol++)
                {
                    int64_t sum = 0;
                    int64_t best = std::numeric_limits<int64_t>::min();
                    for (int descendCol = startCol; descendCol < numCols; descendCol++)
                    {
                        sum += A[row][descendCol];
                        const auto score = sum + lookup[row + 1][descendCol];
                        best = max(best, score);
                    }
                    cout << "row: " << row << " startCol: " << startCol << " bestIfMovedRightFromAndDescended[startCol]: " << bestIfMovedRightFromAndDescended[startCol] << " best: " << best << endl;
                    assert(best == bestIfMovedRightFromAndDescended[startCol]);
                }
            }
#endif
        }
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

    int64_t best = 0;
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
                best = max(best, bestIfClearAndDescendHere); 
            }
        }
    }

    assert(lookup[startRow][startCol] == -1);
    lookup[startRow][startCol] = best;

    return best;


}

void blah(const vector<int64_t>& a, const vector<int64_t>& b)
{
    assert(a.size() == b.size());
    cout << "blah: a: " << endl;
    for (const auto x : a)
    {
        cout << x << " ";
    }
    cout << endl;
    cout << "b: " << endl;
    for (const auto x : b)
    {
        cout << x << " ";
    }
    cout << endl;
    int64_t bestSum = std::numeric_limits<int64_t>::min();
    int64_t bestB = b.front();
    int64_t cumulativeSum = 0;
    int startPoint = 0;
    //cout << "blah" << endl;
    for (int endPoint = 0; endPoint < a.size(); endPoint++)
    {
        //cout << " endPoint: " << endPoint << endl;
        if (b[endPoint] > bestSum )
        {
            //cout << " found better, apparently" << endl;
            bestSum = a[endPoint] + b[endPoint];
            bestB = b[endPoint];
            cumulativeSum = a[endPoint];
            startPoint = endPoint;
        }
        else
        {
            cumulativeSum += a[endPoint];
            bestSum = cumulativeSum + bestB;
        }
        //cout << " bestSum: " << bestSum << " cumulativeSum: " << cumulativeSum << " bestB: " << bestB << endl;
        {
            // Verify.
            int64_t bestSumDebug = std::numeric_limits<int64_t>::min();
            int cumulativeSum = 0;
            int startPointDebug = 0;
            for (int startPoint = endPoint; startPoint >= 0; startPoint--)
            {
                cumulativeSum += a[startPoint];
                if (cumulativeSum + b[startPoint] > bestSumDebug)
                {
                    bestSumDebug = cumulativeSum + b[startPoint];
                    startPointDebug = startPoint;
                }
                else if (cumulativeSum + b[startPoint] == bestSumDebug)
                {
                    startPointDebug = startPoint;
                }
            }
            //cout << " bestSumDebug: " << bestSumDebug << endl;
            assert(bestSum == bestSumDebug);
            assert(startPointDebug == startPoint);
        }
    }
}

int main()
{
#if 0

    srand(time(0));
    while (true)
    {
        const int n = (rand() % 20) + 1;
        const int range = 200;
        vector<int64_t> a;
        vector<int64_t> b;
        for (int i = 0; i < n; i++)
        {
            a.push_back(rand() % (2 * range + 1) - range);
            b.push_back(rand() % (2 * range + 1) - range);
        }
        blah(a, b);
    }
#endif

    int n, m;
    cin >> n >> m;

    vector<vector<int64_t>> A(n, vector<int64_t>(m, -1));

    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col < m; col++)
        {
            cin >> A[row][col];
        }
    }

#ifdef BRUTE_FORCE
    vector<vector<int64_t>> bruteForceLookup(n, vector<int64_t>(m, -1));
#endif
    int64_t best = 0;
    for (int startCol = 0; startCol < m; startCol++)
    {
        const auto bestStartingHere = maxSum(A);
        best = max(best, bestStartingHere);
#ifdef BRUTE_FORCE
        const auto bestStartingHereBruteForce = maxSumBruteForce(0, startCol, A, bruteForceLookup);
        cout << "startCol: " << startCol << " bestStartingHere: " << bestStartingHere << " bestStartingHereBruteForce: " << bestStartingHereBruteForce << endl;
        assert(bestStartingHere == bestStartingHereBruteForce);
#endif
    }
    cout << best << endl;
}
