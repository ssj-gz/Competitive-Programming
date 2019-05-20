// Simon St James (ssjgz) - 2017-11-16 - 2017-11-17 (Week of Code 35).
// Tidied up and re-submitted to the Practice track on 2019-05-20.
#define RANDOM
#define SUBMISSION
#ifdef SUBMISSION
#undef RANDOM
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <map>
#include <limits>
#include <algorithm>
#include <cassert>

using namespace std;

struct SubArrayInfo
{
    SubArrayInfo() = default;
    SubArrayInfo(int sum, int subArrayStartIndex, int maxDescendValue)
        : sum{sum}, subArrayStartIndex{subArrayStartIndex}, maxDescendValue{maxDescendValue}
    {
    }
    int sum = 0;
    int subArrayStartIndex = 0;
    int maxDescendValue = 0;
};

/**
 * Return an array F such that F(r) is the largest score we can obtain if we start at r column; eat
 * some number of squares to the left of r (call l the square where we stop eating); and
 * finally descend at some point d in the range we ate from (i.e. l <= d <= r).
 *
 * That is, each F(r) is the maximal value over all l, d with l <= d <= r of 
 *
 *    sum [ l <= x <= r ] {row[x] } + scoreIfDescendAt[d]
 */
vector<int> findBestIfMovedFromAndDescended(const vector<int>& row, const vector<int>& scoreIfDescendAt)
{
    // For a given index r, define:
    //
    //   bestDescend[r] = d (as defined above);
    //   bestLeft[r] = l (as defined above);
    //   bestScore[r] = F(r) (as defined above).
    //
    // bestLeft[r], bestDescend[r] and bestScore[r] are all tightly related and knowing one of them
    // often allows us to deduce the others: for example, if we know bestLeft[r], then bestDescend[d]
    // is the d (l <= d <= r) that maximises scoreIfDescendAt[r], and 
    // bestScore[r] = sum [ bestLeft[r] <= x <= r ] {row[x]} + bestDescend[d].
    //
    // Even more importantly, if we somehow know bestDescend[r], we can deduce bestLeft[r], and then bestScore[r]:
    // define bestCumulativeLeft[x] for an index x to be the index in 0, 1, ... x such that the
    // sum sum [bestCumulativeLeft[x] <= i <= x] {row[i]} (bestCumulativeLeft can be easily computed for all x using
    // Kadane's algorithm).  Let bestCumulative[r] be this sum i.e 
    // 
    //   bestCumulative[x] = sum [bestCumulativeLeft[x] <= i <= x] {row[i]}
    // 
    // We must eat at least everything in the range bestDescend[r] ... r by definition of bestDescend[r], but to maximise 
    // our score, should we eat beyond this range i.e. eat from bestDescend[r] - 1 and leftwards?
    //
    // The answer is "yes, if and only if bestCumulative[bestDescend[r] - 1] is >= 0".  If it is not i.e. if bestCumulative[bestDescend[r] - 1] < 0,
    // then by definition of bestCumulative[bestDescend[r] - 1], eating any squares starting from bestDescend[r] - 1 and moving leftward must worsen
    // our score.  Otherwise, if bestCumulative[bestDescend[r] - 1] >= 0, then we can (and indeed, must!) increase our score by also eating from
    // bestCumulativeLeft[bestDescend[r] - 1] to bestDescend[r] - 1.  So if bestCumulative[bestDescend[r] - 1] >= 0, 
    // bestLeft[r] == bestCumulative[bestDescend[r] - 1]; else bestLeft[r] = bestDescend[r].  With a little book-keeping, 
    // we can then, assuming we already know bestDescend[r], instantly (O(1)) compute bestScore[r].
    // 
    // For index r and d <= r, define then:
    //
    //    deducedFromBestDescend(d, r) = sum [ d <= x <= r ] { row[x] } + max(0, bestCumulative[d - 1]) + scoreIfDescendAt[d].
    //
    // We can then write:
    //
    //    bestScore[r] = deducedFromBestDescend[bestDescend[r], r] 
    //
    // So the problem reduces to efficiently computing bestDescend[r] for each index r.
    //
    // Here's a quick lemma:
    //
    // Lemma
    //
    // If d <= r, then
    //
    //    deducedFromBestDescend(d, r + 1) = deducedFromBestDescend(d, r) + row[r + 1].
    //
    // Proof
    //
    // Expand deducedFromBestDescend(d, r + 1):
    //
    //    deducedFromBestDescend(d, r + 1) = sum [ d <= x <= r + 1 ] { row[x] } + max(0, bestCumulative[d - 1] + scoreIfDescendAt[d])
    //                                     = sum [ d <= x <= r ] { row[x] } + row[r + 1] + max(0, bestCumulative[d - 1]) + scoreIfDescendAt[d]
    //                                     = sum [ d <= x <= r ] { row[x] } + max(0, bestCumulative[d - 1]) + scoreIfDescendAt[d] + row[r + 1]
    //
    //  Since d <= r, we can rewrite this as 
    //
    //                                     = deducedFromBestDescend(d, r) + row[r + 1]
    //
    // QED
    //
    // Theorem
    //
    // bestDescend[r + 1] is either bestDescend[r] or r + 1.
    //
    // Proof
    //
    // Assume otherwise; that is, that bestDescend[y + 1] is neither bestDescend[r] nor r + 1.
    //
    // Since bestDescend[r + 1] != bestDescend[r], it must be that descending at bestDescend[r + 1] (when trying to compute bestScore[r + 1])
    // gives a strictly better score than descending at bestDescend[r]; that is:
    //
    //    deducedFromBestDescend(bestDescend[r + 1], r + 1) > deducedFromBestDescend(bestDescend[r], r + 1)
    //
    // Since bestDescend[r + 1] <= r + 1 by definition, and bestDescend[r + 1] != r + 1 by assumption, we have that bestDescend[r + 1] <= r.
    //
    // Thus we may use the Lemma to re-write the LHS, giving:
    //  
    //    deducedFromBestDescend(bestDescend[r + 1], r) + row[r + 1] > deducedFromBestDescend(bestDescend[r], r + 1)
    //
    // and since bestDescend[r] <= r by definition, we can use the Lemma again this time to re-write the RHS, giving:
    //  
    //    deducedFromBestDescend(bestDescend[r + 1], r) + row[r + 1] > deducedFromBestDescend(bestDescend[r], r) + row[r + 1]
    //
    // i.e.
    //
    //    deducedFromBestDescend(bestDescend[r + 1], r) > deducedFromBestDescend(bestDescend[r], r)
    //
    // i.e.
    //
    //    deducedFromBestDescend(bestDescend[r + 1], r) > bestScore[r]
    //
    //  That is, if when computing bestScore[r], we descend at bestDescend[r + 1] instead of bestDescend[r], we end up with a strictly
    //  better score, contradicting the maximality of bestScore[r].  Thus, our assumption leads to a contradiction.
    //
    //  QED
    //
    //  How does this help? Well, it means that if we have computed bestDescend[x] for 0 <= x <= r, we can easily deduce bestDescend[r + 1] from 
    //  it: it is either bestDescend[r] or r + 1, whichever gives the largest value when we plug it into deducedFromBestDescend i.e.
    //
    //  bestDescend[r + 1] = r + 1 if deducedFromBestDescend(r + 1, r + 1) is greater than deducedFromBestDescend(bestDescend[r], r + 1), or 
    //                       bestDescend[r] otherwise.
    //
    // Computing deducedFromBestDescend(bestDescend[r], r + 1) is easy (O(1)): from the lemma, it is just deducedFromBestDescend(bestDescend[r], r + 1) + row[r + 1]
    // i.e. 
    //
    //    deducedFromBestDescend(bestDescend[r], r + 1) = bestScore[r] + row[r + 1]
    //
    // Computing deducedFromBestDescend(r + 1, r + 1) is also trivial:
    //
    //    deducedFromBestDescend(r + 1, r + 1) = sum [ r + 1 <= x <= r + 1 ] { row[x] } + max(0, bestCumulative[(r + 1) - 1]) + scoreIfDescendAt[r + 1]
    //                                         = row[r + 1] + max(0, bestCumulative[r]) + scoreIfDescendAt[r + 1]
    //
    // If we keep bestCumulative up to date as we go along (using Kadane's Algorithm), then this can be computed in O(1).
    // 
    // And that's it! The code looks a little different to the analysis above, but this is mainly so that we can combine several steps and bits of bookkeeping
    // into one for efficiency.
    vector<int> result(row.size());
    int bestScore = scoreIfDescendAt.front(); // Should be scoreIfDescendAt.front() + row[0], but the body of the loop adds the row[0] on the first iteration.
    int bestCumulative = numeric_limits<int>::min();
    for (int r = 0; r < row.size(); r++)
    {
        bestScore += row[r];

        if (bestCumulative < 0)
        {
            // Just as in Kadane's algorithm, if breaking with the existing
            // bestCumulative give a better result, then do so.
            bestCumulative = row[r];
        }
        else
            bestCumulative += row[r];

        if (bestCumulative + scoreIfDescendAt[r] > bestScore)
        {
            bestScore = bestCumulative + scoreIfDescendAt[r];
        }

        result[r] = bestScore;

    }
    return result;
}

vector<int> findBestIfMovedFromAndDescendedReversed(const vector<int>& A, const vector<int>& scoreIfDescendAt)
{
    const vector<int> aReversed(A.rbegin(), A.rend());
    const vector<int> scoreIfDescendAtReversed(scoreIfDescendAt.rbegin(), scoreIfDescendAt.rend());
    auto result = findBestIfMovedFromAndDescended(aReversed, scoreIfDescendAtReversed);
    reverse(result.begin(), result.end());
    assert(result.size() == A.size());
    return result;
}

vector<SubArrayInfo> findMaxSubarraySumEndingAt(const vector<int>& A, const vector<int>& scoreIfDescendAt)
{
    vector<SubArrayInfo> result(A.size());
    int bestSum = std::numeric_limits<int>::min();
    int startIndex = 0;
    int maxDescendValue = numeric_limits<int>::min();
    for (int endPoint = 0; endPoint < A.size(); endPoint++)
    {
        maxDescendValue = max(maxDescendValue, scoreIfDescendAt[endPoint]);
        if (bestSum < 0)
        {
            bestSum = A[endPoint];
            startIndex = endPoint;
            maxDescendValue = scoreIfDescendAt[endPoint];
        }
        else
        {
            bestSum += A[endPoint];
        }
        result[endPoint] = {bestSum, startIndex, maxDescendValue};

    }
    return result;
}

vector<SubArrayInfo> findMaxSubarraySumEndingAtReversed(const vector<int>& A, const vector<int>& bonusIfStartAt)
{
    const vector<int> aReversed(A.rbegin(), A.rend());
    const vector<int> bonusIfStartAtReversed(bonusIfStartAt.rbegin(), bonusIfStartAt.rend());
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

int maxSum(const vector<vector<int>>& A)
{
    // Ouch - fairly tough one (at least, one part - findBestIfMovedFromAndDescended - was), and I didn't
    // manage to complete it within the 24 hours (I first tackled it during my first Week of Code - 35) so I 
    // didn't get full marks for it.  Oh well :)
    //
    // It's actually *mostly* easy - we start at the bottom row and work our way up, and fundamentally answer
    // the question: if I am now at column x on row r, what is the best score I can achieve by eating some
    // of the cells on this row, and then descending to the next row, r + 1, and then repeating, until no rows remain.
    //
    // The question is formulated a little oddly - it equates to "if I find myself at column x on row r - perhaps because this
    // is the top row and I've been initially positioned here, or because I descended from the previous row r - 1 at column x -
    // what are the lx and rx surrounding x (i.e. lx <= x <= rx) and the descend point dx that give the highest score
    // when I eat all cells (and only the cells) on this row between lx and rx and then descend to the next row r + 1
    // at dx (lx <= dx <= rx)?"
    //
    // So, to begin with - there's always a Free Lunch Zone (or, as I like to call it, a "Gobble Zone") around x - say 
    // bestGobbleStartIndex <= x <= bestGobbleEndIndex - such that eating all cells in this range in this row is guaranteed
    // not to reduce our max score - indeed, in order to get the max score when starting at x, one *must* eat all the
    // cells in the Gobble Zone! - and where eating outside of this Gobble Zone *will* reduce our score for this row,
    // so if we stray outside of this Zone, we *must* pay for this loss by finding a better descend point (i.e.
    // a dx < bestGobbleStartIndex or dx > bestGobbleEndIndex) which is better than any bestGobbleStartIndex <= dx <= bestGobbleEndIndex
    // by at least the amount we'd lose by straying outside of the Gobble Zone for this row).
    //
    // bestGobbleStartIndex and bestGobbleEndIndex are easily computed: bestGobbleStartIndex is simply the index such that, over all
    // s <= x, the sum [s <= i <= x] {row[i]} is maximised, and is easily computed using Kadane's algorithm.  Similarly,
    // bestGobbleEndIndex is simply the index such that, over all e >= x, sum [x <= i <= e] {row[i]} is maximised.
    //
    // On the bottom row - which is the first row we compute - there is no descend point, so the maximum score we can obtain
    // if we start at column x on this row is the result of eating all cells in the Gobble Zone - straying outside of this
    // Zone, as mentioned, reduces our score, and there are no lower rows to descend to in order to recoup this loss.
    //
    // What about rows other than the last row? As mentioned, we need to eat everything in the Gobble Zone.  We could then descend 
    // inside the Gobble Zone, but what if straying outside the Gobble Zone - with the loss we'd incur by doing so -
    // enabled us to descend somewhere else, giving us a score that would more than offset this loss?
    //
    // Note that, if our descend point is to the left of bestGobbleStartIndex, then we must *not* eat to the right of bestGobbleEndIndex:
    // this would immediately reduce our score for no ultimate benefit, and similarly for if our descend point is to the right
    // of bestGobbleEndIndex; that is:
    //
    //   when starting at column x on row r, we must eat all in the Gobble Zone (and no more), then either:
    //     a) descend inside the Gobble Zone;
    //     b) eat to the left of bestGobbleStartIndex so that we may take a hit which is paid for by allowing us to descend to the
    //        left of the Gobble Zone; or
    //     c) eat to the right of bestGobbleEndIndex so that we may take a hit which is paid for by allowing us to descend to the
    //        right of the Gobble Zone
    //   whichever of the three gives us a better score.
    //
    // It might be tempting, if we decide to descend at a point dx to the left of bestGobbleStartIndex, to gobble as far as dx 
    // and no further, but in fact, having taken the hit to the score of eating to the left of bestGobbleStartIndex, it might
    // be that by ignoring cells to the left of dx, we're leaving extra points on the table (and similarly if dx > bestGobbleEndIndex).  
    // Thus, we can now phrase the recurrence relation we want to solve as follows:
    //
    //   Let Best(x, r) be the maximum score we can obtain by starting at column x on row r.  Then (for all but the bottommost row):
    //      Best(x, r) = max[ lx, rx, dx such that lx <= x <= rx and lx <= dx <= rx] { sum [lx <= i <= rx] {row[i]} + Best(dx, r + 1)}
    //
    // A pretty tall order! However, we see that computing a) is very easy with Kadane's algorithm, so the problem reduces
    // to computing b) and c), which are actually the same algorithm, just with the row reversed.  This is the bit that
    // really baked my noodle and made me miss the deadline, but it can be efficiently computed using the algorithm in
    // findBestIfMovedFromAndDescended - see that for details! 
    //
    // And that's about it - everything but findBestIfMovedFromAndDescended is pretty obvious and easy, but that part of it
    // was a bit of a pig, I found, though I think if I'd sat down with pen and paper and properly tried to figure it out
    // rather that getting stuck in some stupid trial-and-error rabbit hole I might have solved it quicker.  Who knows? :)
    //
    // One happy outcome from all this - my next Week of Code had a problem ("Cut a Strip") which had computing the equivalent
    // of findBestIfMovedFromAndDescended as a subproblem, so I ended up finding that fairly easy :)
    const int numRows = A.size();
    const int numCols = A[0].size();

    if (numCols == 1)
    {
        int sum = 0;
        for (const auto row : A)
        {
            sum += row[0];
        }
        return sum;
    }

    vector<int> maxScores;
    vector<int> rowUnderneathMaxScores(numCols, 0); // The previous row we processed (i.e. the one below the current one!).

    int best = std::numeric_limits<int>::min();

    for (int row = numRows - 1; row >= 0; row--)
    {
        maxScores.clear();
        maxScores.resize(numCols, numeric_limits<int>::min());

        const auto bestIfMovedRightFromAndDescended = findBestIfMovedFromAndDescendedReversed(A[row], rowUnderneathMaxScores);
        const auto bestIfMovedLeftFromAndDescended = findBestIfMovedFromAndDescended(A[row], rowUnderneathMaxScores);
        const auto bestGobbleToLeftFrom = findMaxSubarraySumEndingAt(A[row], rowUnderneathMaxScores);
        const auto bestGobbleToRightFrom = findMaxSubarraySumEndingAtReversed(A[row], rowUnderneathMaxScores);

        int startCol = 0;
        while (startCol < numCols)
        {
            int bestGobbleSum = 0;
            const int bestGobbleStartIndex = bestGobbleToLeftFrom[startCol].subArrayStartIndex;
            bestGobbleSum += bestGobbleToLeftFrom[startCol].sum;
            int bestGobbleEndIndex = startCol;
            int maxDescendInGobbleRange = bestGobbleToLeftFrom[startCol].maxDescendValue;
            if (startCol < numCols - 1 && bestGobbleToRightFrom[startCol + 1].sum >= 0)
            {
                bestGobbleEndIndex = bestGobbleToRightFrom[startCol + 1].subArrayStartIndex;
                bestGobbleSum += bestGobbleToRightFrom[startCol + 1].sum;
                maxDescendInGobbleRange = max(maxDescendInGobbleRange, bestGobbleToRightFrom[startCol + 1].maxDescendValue);
            }
            maxDescendInGobbleRange += bestGobbleSum;

            int bestForStartCol = std::numeric_limits<int>::min();

            // Descend in the middle of gobble-range.
            bestForStartCol = max(bestForStartCol, maxDescendInGobbleRange);

            // Descend to the left of gobble-range.
            if (bestGobbleStartIndex > 0)
            {
                const int score = bestGobbleSum + bestIfMovedLeftFromAndDescended[bestGobbleStartIndex - 1];
                bestForStartCol = max(bestForStartCol, score);
            }
            // Descend to the right of gobble-range.
            if (bestGobbleEndIndex < numCols - 1)
            {
                const int score = bestGobbleSum + bestIfMovedRightFromAndDescended[bestGobbleEndIndex + 1];
                bestForStartCol = max(bestForStartCol, score);
            }
            maxScores[startCol] = max(maxScores[startCol], bestForStartCol);

            startCol++;
        }

        if (row != 0)
        {
            // Faster than copying!
            swap(rowUnderneathMaxScores, maxScores);
        }
    }

    for (const auto bestForStartCol : maxScores)
    {
        best = max(best, bestForStartCol);
    }

    return best;
}

int main(int argc, char** argv)
{
    ios::sync_with_stdio(false);

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

    const int best = maxSum(A);
    cout << best << endl;
}
