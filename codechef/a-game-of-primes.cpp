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
        {
        }
        // Set all values in the given range newRange to true, and return a (minimal) vector of 
        // Ranges describing the values in newRange that were false prior to this call.
        // Complexity of any one call can be is O(maxRangeEnd x log2 maxRangeEnd), but amortised over
        // a set of Q queries is O((q + maxRangeEnd) * log2 maxRangeEnd).
        vector<Range> setRangeToOn(const Range& newRange)
        {
            vector<Range> previousOffRanges;

            auto existingSegment = leftmostRangeOverlappingOrAdjacentTo(newRange.left);
            
            Range segmentToAdd = {newRange.left, newRange.right};

            Range previousOffRange = {newRange.left, -1};
            if (existingSegment != m_rangesByLeft.end() && existingSegment->left <= newRange.left)
            {
                previousOffRange.left = existingSegment->right + 1;
            }

            while (existingSegment != m_rangesByLeft.end() && existingSegment->left <= newRange.right + 1)
            {
                if (existingSegment->right >= newRange.left - 1)
                {
                    segmentToAdd.left = min(segmentToAdd.left, existingSegment->left);
                }
                segmentToAdd.right = max(segmentToAdd.right, existingSegment->right);

                if (existingSegment->left - 1 >= previousOffRange.left)
                {
                    previousOffRange.right = existingSegment->left - 1;
                    previousOffRanges.push_back(previousOffRange);
                    previousOffRange = {existingSegment->right + 1, -1};
                }

                // Erase this - it has been folded into segmentToAdd.
                existingSegment = m_rangesByLeft.erase(existingSegment);
            }

            m_rangesByLeft.insert(segmentToAdd);

            if (previousOffRange.left <= newRange.right)
            {
                previousOffRange.right = newRange.right;
                previousOffRanges.push_back(previousOffRange);
            }

            return previousOffRanges;
        }
        // Returns true if and only if rangeToSearch contains any elements that
        // have been set to on via a prior call to setRangeToOn.
        //
        // Worst case O(log2 maxRangeEnd).
        bool hasAnySetToOnInRange(const Range& rangeToSearch)
        {
            bool answer = false;
            auto rangeIter = leftmostRangeOverlappingOrAdjacentTo(rangeToSearch.left);
            // This "while" loop will only iterate at most twice.
            while (rangeIter != m_rangesByLeft.end() && rangeIter->left <= rangeToSearch.right)
            {
                if (rangeIter->left <= rangeToSearch.right && rangeIter->right >= rangeToSearch.left)
                {
                    answer = true;
                    break;
                }
                rangeIter++;
            }
            return answer;
        }
    private:
        static bool compareRangeBegins(const Range& lhs, const Range& rhs)
        {
            if (lhs.left != rhs.right)
                return lhs.left < rhs.left;
            return lhs.right < rhs.right;
        }

        set<Range, decltype(&compareRangeBegins)> m_rangesByLeft{&compareRangeBegins};

        set<Range, decltype(&compareRangeBegins)>::iterator leftmostRangeOverlappingOrAdjacentTo(int pos)
        {
            Range range{pos, -1};
            set<Range, decltype(&compareRangeBegins)>::iterator iter = m_rangesByLeft.lower_bound(range);
            if (iter != m_rangesByLeft.begin())
            {
                const auto originalIter = iter;
                iter = std::prev(iter);
                if (iter->right < pos - 1)
                {
                    iter = originalIter;
                }
            }


            return iter;

        }
};

vector<int> calcQueryResults(const vector<Query>& queries, const vector<int>& primesThatDivideK)
{
    vector<int> queryResults;
    const int maxRangeEnd = 100'000;

    vector<RangeTracker> hasPrimeFactorOfKTracker;

    RangeTracker nonBlankTracker(maxRangeEnd);
    for (int i = 0; i < primesThatDivideK.size(); i++)
    {
        hasPrimeFactorOfKTracker.emplace_back(maxRangeEnd);
    }
    for (const auto& query : queries)
    {
        if (query.queryType == '!')
        {
            const auto previouslyBlankSegmentsInRange = nonBlankTracker.setRangeToOn(query.range);
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
                const bool hasPrimeFactorOfKInRange = hasPrimeFactorOfKTracker[primeFactorOfKIndex].hasAnySetToOnInRange(query.range);
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
    // Maintain a nonBlankTracker which allows you to unset whether all elements in a range are blank
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

    auto readInt = []() { int x; cin >> x; return x; };

    int K = readInt();
    const int Q = readInt();

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

    for (auto& query : queries)
    {
        cin >> query.queryType;

        query.range.left = readInt() - 1;
        query.range.right = readInt() - 1;

        if (query.queryType == '!')
        {
            query.value = readInt();
        }
        else if (query.queryType == '?')
        {
            // Do nothing.
        }
        else
        {
            assert(false);
        }
    }

    const auto queryResults = calcQueryResults(queries, primesThatDivideK);

    for (const auto x : queryResults)
    {
        cout << x << endl;
    }


}
