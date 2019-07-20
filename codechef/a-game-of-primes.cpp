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

vector<int> solveOptimised(const vector<Query>& queries, int64_t K, const vector<int>& primesThatDivideK)
{
    vector<int> queryResults;
    const int maxRangeEnd = 100'000;

    vector<RangeTracker> dbgNumWithPrimeFactorOfKTracker;

    RangeTracker blankTracker(maxRangeEnd);
    for (int i = 0; i < primesThatDivideK.size(); i++)
    {
        dbgNumWithPrimeFactorOfKTracker.emplace_back(maxRangeEnd);
    }
    for (const auto& query : queries)
    {
        if (query.queryType == '!')
        {
            const auto previousBlankSegmentsInRange = blankTracker.setRangeToOn(query.range);
            for (int primeFactorOfKIndex = 0; primeFactorOfKIndex < primesThatDivideK.size(); primeFactorOfKIndex++)
            {
                if ((query.value % primesThatDivideK[primeFactorOfKIndex]) == 0)
                {
                    for (const auto& wasBlankSegment : previousBlankSegmentsInRange)
                    {
                        dbgNumWithPrimeFactorOfKTracker[primeFactorOfKIndex].setRangeToOn(wasBlankSegment);
                    }
                }
            }
        }
        else if (query.queryType == '?')
        {
            int num = 0;
            int dbgNum = 0;
            for (int primeFactorOfKIndex = 0; primeFactorOfKIndex < primesThatDivideK.size(); primeFactorOfKIndex++)
            {
                const int dbgNumInRange = dbgNumWithPrimeFactorOfKTracker[primeFactorOfKIndex].hasOnRangeOverlapping(query.range);
                if (dbgNumInRange > 0)
                    dbgNum++;
            }
            queryResults.push_back(dbgNum);
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
    ios::sync_with_stdio(false);

    const int64_t maxXorK = 1'000'000'000ULL;
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

    int64_t K;
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

    const auto solutionOptimised = solveOptimised(queries, K, primesThatDivideK);

    for (const auto x : solutionOptimised)
    {
        cout << x << endl;
    }


}
