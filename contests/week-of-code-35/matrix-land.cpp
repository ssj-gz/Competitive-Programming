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

vector<BestSubarraySumUpTo> findMaxSubarraySumEndingAt(const vector<int>& A)
{
    const auto N = A.size();
    vector<BestSubarraySumUpTo> result(N);
    int64_t maxContiguousSubarraySum = 0;
    int64_t sumOfLargestSubarrayEndingHere = 0;
    int subarrayStartPos = 0;
    for (int i = 0; i < N; i++)
    {
        if (sumOfLargestSubarrayEndingHere + A[i] >= 0)
        {
            sumOfLargestSubarrayEndingHere += A[i];
        }
        else
        {
            sumOfLargestSubarrayEndingHere = 0;
            subarrayStartPos = i;
        }
        maxContiguousSubarraySum = max(maxContiguousSubarraySum, sumOfLargestSubarrayEndingHere);
        result.push_back({sumOfLargestSubarrayEndingHere, subarrayStartPos});
    }
    return result;
}

int64_t maxSum(const vector<vector<int>>& A, vector<vector<int64_t>>& lookup)
{
    const int numRows = A.size();
    const int numCols = A[0].size();

    lookup.resize(numRows, vector<int64_t>(numCols, -1));
    lookup.push_back(vector<int64_t>(numCols, 0)); // "Sentinel" row.

    int64_t best = 0;

    return best;
}

int64_t maxSumBruteForce(int startRow, int startCol, const vector<vector<int>>& A, vector<vector<int64_t>>& lookup)
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

int main()
{
    int n, m;
    cin >> n >> m;

    vector<vector<int>> A(n, vector<int>(m, -1));

    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col < m; col++)
        {
            cin >> A[row][col];
        }
    }

    vector<vector<int64_t>> lookup;
#ifdef BRUTE_FORCE
    vector<vector<int64_t>> bruteForceLookup(n, vector<int64_t>(m, -1));
#endif
    int64_t best = 0;
    for (int startCol = 0; startCol < m; startCol++)
    {
        const auto bestStartingHere = maxSum(A, lookup);
        best = max(best, bestStartingHere);
#ifdef BRUTE_FORCE
        const auto bestStartingHereBruteForce = maxSumBruteForce(0, startCol, A, bruteForceLookup);
        cout << "startCol: " << startCol << " bestStartingHere: " << bestStartingHere << " bestStartingHereBruteForce: " << bestStartingHereBruteForce << endl;
        assert(bestStartingHere == bestStartingHereBruteForce);
#endif
    }
    cout << best << endl;
}
