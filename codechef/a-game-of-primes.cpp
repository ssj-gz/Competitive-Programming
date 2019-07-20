// Simon St James (ssjgz) - 2019-07-20
#include <iostream>
#include <vector>
#include <set>
#include <cmath>

#include <cassert>

using namespace std;

struct Range
{
    int left = -1;
    int right = -1;
};

struct Query
{
    char queryType;
    Range range = {-1, -1};
    int value = -1;
};

class RangeTracker
{
    public:
        RangeTracker(int maxRangeEnd)
#ifdef VERIFY_RANGE_TRACKER
            : m_dbgValues(maxRangeEnd + 1, false)
#endif
        {
        }
        vector<Range> setRangeToOn(const Range& newRange)
        {
            vector<Range> previousOffRanges;

            auto rangeIter = minRange(newRange.left);
            
            int rangeToAddLeft = newRange.left;
            int rangeToAddRight = newRange.right;

            Range previousOffRange = {newRange.left, -1};
            if (rangeIter != m_rangesByLeft.end() && rangeIter->left <= newRange.left)
            {
                previousOffRange.left = rangeIter->right + 1;
            }

            while (rangeIter != m_rangesByLeft.end() && rangeIter->left <= newRange.right + 1)
            {
                if (rangeIter->right >= newRange.left - 1)
                {
                    rangeToAddLeft = min(rangeToAddLeft, rangeIter->left);
                }
                rangeToAddRight = max(rangeToAddRight, rangeIter->right);

                if (rangeIter->left - 1 >= previousOffRange.left)
                {
                    previousOffRange.right = rangeIter->left - 1;
                    if (previousOffRange.left >= newRange.left && previousOffRange.right <= newRange.right)
                    {
                        previousOffRanges.push_back(previousOffRange);
                        previousOffRange = {rangeIter->right + 1, -1};
                    }
                }

                rangeIter = m_rangesByLeft.erase(rangeIter);
            }
            m_rangesByLeft.insert(Range{rangeToAddLeft, rangeToAddRight});
            if (previousOffRange.left <= newRange.right)
            {
                previousOffRange.right = newRange.right;
                previousOffRanges.push_back(previousOffRange);
            }
#ifdef VERIFY_RANGE_TRACKER
            const auto oldDbgValues = m_dbgValues;
            for (int i = newRange.left; i <= newRange.right; i++)
            {
                m_dbgValues[i] = true;
            }
            auto tempDbgValue = m_dbgValues;
            for (const auto& previousOffRange : previousOffRanges)
            {
                assert(previousOffRange.left <= previousOffRange.right);
                assert(previousOffRange.left >= newRange.left);
                assert(previousOffRange.right <= newRange.right);
                for (int i = previousOffRange.left; i <= previousOffRange.right; i++)
                {
                    tempDbgValue[i] = false;
                }
            }
            for (int i = 1; i < previousOffRanges.size(); i++)
            {
                assert(previousOffRanges[i].left > previousOffRanges[i - 1].right + 1);
            }
            assert(tempDbgValue == oldDbgValues);
            verify();
#endif

            return previousOffRanges;
        }
        bool hasOnRangeOverlapping(const Range& rangeToSearch)
        {
            bool answer = false;
            auto rangeIter = minRange(rangeToSearch.left);
            while (rangeIter != m_rangesByLeft.end() && rangeIter->left <= rangeToSearch.right)
            {
                if (rangeIter->left <= rangeToSearch.right && rangeIter->right >= rangeToSearch.left)
                {
                    answer = true;
                    break;
                }
                rangeIter++;
            }
#ifdef VERIFY_RANGE_TRACKER
            bool dbgAnswer = false;
            for (int i = rangeToSearch.left; i <= rangeToSearch.right; i++)
            {
                if (m_dbgValues[i])
                {
                    dbgAnswer = true;
                    break;
                }
            }
            assert(answer == dbgAnswer);
#endif
            return answer;
        }
#ifdef VERIFY_RANGE_TRACKER
        void verify()
        {
            if (m_rangesByLeft.empty())
                return;

            Range previousRange = *(m_rangesByLeft.begin());
            assert(previousRange.left <= previousRange.right);
            auto rangeIter = m_rangesByLeft.begin();
            rangeIter = std::next(rangeIter);

            for (; rangeIter != m_rangesByLeft.end(); rangeIter++)
            {
                assert(rangeIter->left <= rangeIter->right);
                assert(rangeIter->left > previousRange.right + 1);

                previousRange = *rangeIter;
            }
            
            vector<bool> verify(m_dbgValues.size(), false);
            for (const auto& range : m_rangesByLeft)
            {
                for (int i = range.left; i <= range.right; i++)
                {
                    verify[i] = true;
                }
            }
            assert(m_dbgValues == verify);

        }
#endif
    private:
        static bool compareRangeBegins(const Range& lhs, const Range& rhs)
        {
            if (lhs.left != rhs.right)
                return lhs.left < rhs.left;
            return lhs.right < rhs.right;
        }

        set<Range, decltype(&compareRangeBegins)> m_rangesByLeft{&compareRangeBegins};

        set<Range, decltype(&compareRangeBegins)>::iterator minRange(int left)
        {
            Range range{left, -1};
            set<Range, decltype(&compareRangeBegins)>::iterator iter = m_rangesByLeft.lower_bound(range);
            if (iter != m_rangesByLeft.begin())
            {
                const auto originalIter = iter;
                iter = std::prev(iter);
                if (iter->right < left - 1)
                {
                    iter = originalIter;
                }
            }


            return iter;

        }

#ifdef VERIFY_RANGE_TRACKER
        vector<bool> m_dbgValues;
#endif
};

vector<int> calcQueryResults(const vector<Query>& queries, int K, const vector<int>& primesThatDivideK)
{
    vector<int> queryResults;
    const int maxRangeEnd = 100'000;

    vector<RangeTracker> hasPrimeFactorOfKTracker;

    RangeTracker blankTracker(maxRangeEnd);
    for (int i = 0; i < primesThatDivideK.size(); i++)
    {
        hasPrimeFactorOfKTracker.emplace_back(maxRangeEnd);
    }
    for (const auto& query : queries)
    {
        if (query.queryType == '!')
        {
            const auto previouslyBlankSegmentsInRange = blankTracker.setRangeToOn(query.range);
            for (int primeFactorOfKIndex = 0; primeFactorOfKIndex < primesThatDivideK.size(); primeFactorOfKIndex++)
            {
                if ((query.value % primesThatDivideK[primeFactorOfKIndex]) == 0)
                {
                    for (const auto& wasBlankSegment : previouslyBlankSegmentsInRange)
                    {
                        hasPrimeFactorOfKTracker[primeFactorOfKIndex].setRangeToOn(wasBlankSegment);
                    }
                }
            }
        }
        else if (query.queryType == '?')
        {
            int numSharingPrimeFactorOfKInRange = 0;
            for (int primeFactorOfKIndex = 0; primeFactorOfKIndex < primesThatDivideK.size(); primeFactorOfKIndex++)
            {
                const bool hasPrimeFactorOfKInRange = hasPrimeFactorOfKTracker[primeFactorOfKIndex].hasOnRangeOverlapping(query.range);
                if (hasPrimeFactorOfKInRange)
                    numSharingPrimeFactorOfKInRange++;
            }
            queryResults.push_back(numSharingPrimeFactorOfKInRange);
        }
        else
        {
            assert(false);
        }
    }
    return queryResults;
}

int main(int argc, char* argv[])
{
    // Wow - severely misjudged (or rather: misread) this one - somehow ended up missing the 
    // part about the "!" only changing *blank* elements rather than all in range(!) and
    // so, solving entirely the wrong problem XD
    //
    // It's fundamentally easy, though, although with some tricky details if you don't have
    // a RangeTracker datastructure already implemented.
    //
    // Maintain a blankTracker which allows you to unset whether all elements in a range are blank
    // or not, and returns a list of non-overlapping segments describing the elements in that 
    // range that *were* blank.  These non-overlapping segments must be set to the value x corresponding
    // to this "!" query.
    //
    // For each prime factor of K, maintain a hasPrimeFactorOfKTracker for that prime factor; for the
    // x value for a given "!" query, find which of the prime factors of K also divide x: we need then
    // to set the elements of hasPrimeFactorOfKTracker for that prime factor to "true" for each previously
    // blank element.
    //
    // For a "?" query, we simply ask, for each prime factor of K, whether the corresponding hasPrimeFactorOfKTracker
    // has any elements set to true for the given range.
    //
    // The RangeTracker simply consists of an ordered (by "left"), non-overlapping set of Ranges which are kept 
    // updated (and minimal) by merging etc during each call to setRangeToOn - hopefully the code for 
    // this is self-explanatory :)
    //
    // And that's about it!
    ios::sync_with_stdio(false);

    const int maxXorK = 1'000'000'000ULL;
    const int sqrtMaxXorK = sqrt(maxXorK);

    vector<int> primesUpToSqrtMaxXorK;
    vector<bool> isPrime(sqrtMaxXorK + 1, true);

    for (int factor = 2; factor <= sqrtMaxXorK; factor++)
    {
        if (isPrime[factor])
        {
            primesUpToSqrtMaxXorK.push_back(factor);
            for (int multiple = factor; multiple < isPrime.size(); multiple += factor)
            {
                isPrime[multiple] = false;
            }
        }
    }

    int K;
    cin >> K;

    int Q;
    cin >> Q;

    vector<int> primesThatDivideK;
    for (const auto prime : primesUpToSqrtMaxXorK)
    {
        bool addedAlready = false;
        while ((K % prime) == 0)
        {
            K /= prime;
            if (!addedAlready)
            {
                primesThatDivideK.push_back(prime);
                addedAlready = true;
            }
        }
    }

    if (K != 1)
    {
        // If value of K after being divided by all its prime divisors <= sqrt(K)
        // is not 1, then the remaining value of K is prime (otherwise: it would have
        // at least two prime factors, P and Q.   But P, Q must be > sqrt(K) else
        // they would have been divided out, and so P x Q > K, a contradiction).
        primesThatDivideK.push_back(K);
    }


    vector<Query> queries(Q);

    for (int q = 0; q < Q; q++)
    {
        cin >> queries[q].queryType;

        if (queries[q].queryType == '!')
        {
            cin >> queries[q].range.left;
            cin >> queries[q].range.right;
            queries[q].range.left--;
            queries[q].range.right--;
            cin >> queries[q].value;
        }
        else if (queries[q].queryType == '?')
        {
            cin >> queries[q].range.left;
            cin >> queries[q].range.right;
            queries[q].range.left--;
            queries[q].range.right--;
        }
        else
        {
            assert(false);
        }
    }

    const auto queryResults = calcQueryResults(queries, K, primesThatDivideK);

    for (const auto x : queryResults)
    {
        cout << x << endl;
    }


}
