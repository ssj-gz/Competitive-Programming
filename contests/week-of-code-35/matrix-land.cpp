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

class MaxTracker
{
    public:
        void add(int64_t value)
        {
            numOfValue[value]++;
            assert(size() > 0);
        }
        void remove(int64_t value)
        {
            assert(numOfValue.find(value) != numOfValue.end());
            numOfValue[value]--;
            if (numOfValue[value] == 0)
            {
                numOfValue.erase(numOfValue.find(value));
            }
        }
        void clear()
        {
            numOfValue.clear();
        }
        int64_t max() const
        {
            assert(size() > 0);
            return numOfValue.rbegin()->first;
        }
        int size() const
        {
            return numOfValue.size();
        }
        int64_t secondToMax() const
        {
            assert(size() > 1);
            auto blah = numOfValue.rbegin();
            blah++;
            return blah->first;
        };
    private:
        map<int64_t, int64_t> numOfValue;
};

vector<BestSubarraySumUpTo> findMaxSubarraySumEndingAt(const vector<int64_t>& A, const vector<int64_t>& bonusIfStartAt)
{
    vector<BestSubarraySumUpTo> result(A.size());
    for (int endPoint = 0; endPoint < A.size(); endPoint++)
    {
#if 0
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
    int64_t cumulative = 0;
    int negativePoint = -1;
    int negativePointsLastCheckedUntil = -1;
    int negativePointsStartIndex = -1;
    int upToNegativePointCumulative = 0;
    int unprocessedNegativePointIndex = 0;
    bool encounteredPositiveAfterNegativePoint = false;
    MaxTracker afterNegativePointBs;
    MaxTracker beforeNegativePointBs;
    for (int endPoint = 0; endPoint < A.size(); endPoint++)
    {
        cout << "iteration - endPoint: " << endPoint << endl;
        afterNegativePointBs.add(bonusIfStartAt[endPoint]);
        cumulative += A[endPoint];

        while (true)
        {
            cout << " iterating negative point checks" << endl;
            bool negativePointChanged = false;

            //if (negativePoint == -1)
            //afterNegativePointBs.add(bonusIfStartAt[endPoint]);


            cout << "upToNegativePointCumulative: " << upToNegativePointCumulative << " negativePoint: " << negativePoint << endl;


            if (negativePoint != -1 && negativePoint < endPoint)
            {
                cout << "There is a negative point: " << negativePoint << " upToNegativePointCumulative: " << upToNegativePointCumulative << " beforeNegativePointBs.max(): " << (beforeNegativePointBs.size() > 0 ? beforeNegativePointBs.max() : -999) << " afterNegativePointBs.max(): " << (afterNegativePointBs.size() > 0 ? afterNegativePointBs.max() : -999) << " - testing if we can remove" << endl;
                //assert(upToNegativePointCumulative < 0);
                const int64_t gainFromStrippingUpToNegativePoint = -upToNegativePointCumulative;
                int64_t lossFromStrippingUpToNegativePoint = 0;
                cout << "gainFromStrippingUpToNegativePoint: " << gainFromStrippingUpToNegativePoint << endl;
                //if (needToRemoveFromAfter)
                //{
                //afterNegativePointBs.remove(bonusIfStartAt[endPoint]);
                //needToRemoveFromAfter = false;
                //}
                const auto maxBonus = (afterNegativePointBs.size() > 0 ? afterNegativePointBs.max() : bonusIfStartAt[endPoint]);
                assert(beforeNegativePointBs.size() > 0);
                if (maxBonus < beforeNegativePointBs.max())
                {
                    //assert(afterNegativePointBs.max() < beforeNegativePointBs.max());
                    lossFromStrippingUpToNegativePoint += (beforeNegativePointBs.max() - maxBonus);
                }
                cout << "lossFromStrippingUpToNegativePoint: " << lossFromStrippingUpToNegativePoint << endl;
                if (gainFromStrippingUpToNegativePoint >= lossFromStrippingUpToNegativePoint)
                {
                    //for (int i = startPoint; i <= negativePoint; i++)
                    //{
                    //upToNegativePointCumulative -= A[i];
                    //beforeNegativePointBs.remove(bonusIfStartAt[i]);
                    //}
                    cout << " stripping" << endl;
                    cumulative -= upToNegativePointCumulative;
                    cout << " cumulative now: " << cumulative << endl;
                    upToNegativePointCumulative = 0;
                    //upToNegativePointCumulative -= upToNegativePointCumulative;
                    cout << " upToNegativePointCumulative now: " << upToNegativePointCumulative << endl;
                    beforeNegativePointBs.clear();
                    startPoint = negativePoint + 1;
                    negativePoint = -1;
                    negativePointChanged = true;
                    encounteredPositiveAfterNegativePoint = false;
                    negativePointsLastCheckedUntil = startPoint - 1;
#if 0
                    cout << "advanced start point to : " << startPoint << endl;
                    for (int i = startPoint; i <= endPoint; i++)
                    {
                        cout << " i: " << i << endl;
                        if (negativePoint == -1)
                        {
                            beforeNegativePointBs.add(bonusIfStartAt[i]);
                            upToNegativePointCumulative += A[i];
                            cout << "advancing - added A[" << i << "] = " << A[i] << " to upToNegativePointCumulative; now: " << upToNegativePointCumulative << endl;
                            if (upToNegativePointCumulative < 0)
                            {
                                negativePoint = i;
                                cout << " new negativePoint found!" << negativePoint << " with upToNegativePointCumulative: " << upToNegativePointCumulative << endl;
                                //cout << "removing " < 
                                for (int j = startPoint; j <= negativePoint; j++)
                                {
                                    cout << " removing bonusIfStartAt[" << j << "] = " << bonusIfStartAt[j] << " from afterNegativePointBs" << endl;
                                    afterNegativePointBs.remove(bonusIfStartAt[j]);
                                }
                            }
                        }
                        else
                        {
                            break;
                        }

                    }
                    cout << " afterNegativePointBs now: " << (afterNegativePointBs.size() > 0 ? afterNegativePointBs.max() : -999) << endl;
                    //needToRemoveFromAfter = false;
#endif
                }
            }
            for (int i = negativePointsLastCheckedUntil + 1; i <= endPoint; i++)
            {
                cout << "Checking for additional negative points i: " << i << " negativePoint: " << negativePoint << " encounteredPositiveAfterNegativePoint: " << encounteredPositiveAfterNegativePoint << " A[i]:  " << A[i] << endl;
                //if (negativePoint == -1/* || (!encounteredPositiveAfterNegativePoint && A[i] < 0)*/)
                if (true)
                {
                    upToNegativePointCumulative += A[i];
                    cout << " updating upToNegativePointCumulative: now " << upToNegativePointCumulative << endl;
                    beforeNegativePointBs.add(bonusIfStartAt[i]);
                    if (upToNegativePointCumulative < 0)
                    {
                        if (negativePoint == -1)
                        {
                            unprocessedNegativePointIndex = startPoint;
                            encounteredPositiveAfterNegativePoint = false;
                        }
                        cout << "Negative point found: " << i << " encounteredPositiveAfterNegativePoint: " << encounteredPositiveAfterNegativePoint << " previous negativePoint: " << negativePoint << endl;
                        negativePoint = i;
                        //needToRemoveFromAfter = true;
                        negativePointChanged = true;
                        cout << "Removing afterNegativePointBs to reflect new negative point; unprocessedNegativePointIndex: " << unprocessedNegativePointIndex << endl;
                        //if (negativePointsLastCheckedUntil == -1)
                            //negativePointsLastCheckedUntil = 0;
                        //for (int j = negativePointsLastCheckedUntil + 1; j <= negativePoint; j++)
                        for (int j = unprocessedNegativePointIndex; j <= negativePoint; j++)
                        {
                            cout << " removing bonusIfStartAt[" << j << "] = " << bonusIfStartAt[j] << " from afterNegativePointBs" << endl;
                            afterNegativePointBs.remove(bonusIfStartAt[j]);
                        }
                        unprocessedNegativePointIndex = negativePoint + 1;
                        //break;
                    }
                    else
                    {
                    }
                }
                else
                {
                    //break;
                }
                if (negativePoint != -1 && A[i] >= 0)
                {
                    //encounteredPositiveAfterNegativePoint = true;
                    cout << "Setting encounteredPositiveAfterNegativePoint to true; negativePoint: " << negativePoint << "  due to i: " << i << endl;
                }
                if (negativePoint != -1) // See if we can remove this one immediately; if not, hopefully we can loop round and get it.
                    break;
            }
            negativePointsLastCheckedUntil = endPoint;
            if (!negativePointChanged)
                break;
        }


        int64_t maxBonus = (afterNegativePointBs.size() > 0 ? afterNegativePointBs.max() : bonusIfStartAt[endPoint]);
        cout << " max bonus - beforeNegativePointBs.max() " << (beforeNegativePointBs.size() > 0 ? beforeNegativePointBs.max() : -999) << endl;
        if (beforeNegativePointBs.size() > 0)
            maxBonus = max(maxBonus, beforeNegativePointBs.max());

        cout << " maxBonus: " << maxBonus << endl;
        cout << " cumulative: " << cumulative << endl;
        bestSum = cumulative + maxBonus;

        //if (needToRemoveFromAfter)
            //afterNegativePointBs.remove(bonusIfStartAt[endPoint]);
#endif

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
            //cout << " endPoint: " << endPoint << " bestSumDebug: " << bestSumDebug << " bestSum: " << bestSum << " startPointDebug: " << startPointDebug << " startPoint:" << startPoint << endl;
            cout << " endPoint: " << endPoint << " bestSumDebug: " << bestSumDebug << " startPointDebug: " << startPointDebug << endl;
            result[endPoint] = {bestSumDebug, startPointDebug};
            //assert(bestSum == bestSumDebug);
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
        const auto bestIfMovedRightFromAndDescended = extractSums(findMaxSubarraySumEndingAtReversed(A[row], lookup[row + 1]));
        const auto bestIfMovedLeftFromAndDescended = extractSums(findMaxSubarraySumEndingAt(A[row], lookup[row + 1]));
        const auto bestGobbleToLeftFrom = findMaxSubarraySumEndingAt(A[row], rowOfZeros);
        const auto bestGobbleToRightFrom = findMaxSubarraySumEndingAtReversed(A[row], rowOfZeros);

        vector<int64_t> bleep(numCols);
        for (int i = 0; i < numCols; i++)
        {
            cout << "i: " << i << " bestIfMovedRightFromAndDescended: " << bestIfMovedRightFromAndDescended[i] << endl;
        }
        {
            int64_t maxDescent = lookup[row + 1].back();
            //int64_t cumulative = A[row].back();
            int64_t cumulative = 0;
            int64_t bestScore = cumulative + maxDescent;
            int64_t descentUsedWithBest = maxDescent;
            cout << " Initial bestScore: " << bestScore << " maxDescent: " << maxDescent << " cumulative: " << cumulative << endl;
            for (int i = numCols - 1; i >= 0; i--)
            {
                cumulative += A[row][i];
                maxDescent = max(maxDescent, lookup[row + 1][i]);
                //bestScore = cumulative + (i == numCols - 1 ? maxDescent : (maxDescent - lastMaxDescent));
                bestScore = cumulative + maxDescent;
                cout << "i: " << i << " Preliminary bestScore: " << bestScore << " maxDescent: " << maxDescent << " cumulative: " << cumulative << endl;
                if (A[row][i] + lookup[row + 1][i] > bestScore)
                {
                    bestScore = A[row][i] + lookup[row + 1][i];
                    cumulative = A[row][i];
                    maxDescent = lookup[row + 1][i];
                    descentUsedWithBest = maxDescent;
                    cout << "i: " << i << " adjusted bestScore: " << bestScore << " maxDescent: " << maxDescent << " cumulative: " << cumulative << endl;

                }
                cout << "i: " << i << " bestScore: " << bestScore << endl;
                bleep[i] = bestScore;
            }
        }
        assert(bleep == bestIfMovedRightFromAndDescended);


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
        const int n = rand() % 4 + 1;
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
        const int numRows = (rand() % 2) + 1;
        const int numCols = (rand() % 4) + 1;
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
