// Simon St James (ssjgz) - 2019-06-15
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>
#include <cassert>

using namespace std;

struct Query
{
    int leftIndex = -1;
    int rightIndex = -1;

    int64_t amountToAdd = 0;
    int uniqueId = -1; // So we don't "lose" Queries with the same leftIndex and rightIndex!
};

int main(int argc, char* argv[])
{
    // Another nice, easy one :)
    //
    // Consider the element at index i.  What is its eventual value, after all queries are processed?
    // The answer, of course, is the total amountToAdd across all queries that have i in their range.
    //
    // So, to compute each a[i], we could simply sweep from a = 0, 1, ... n - 1 and at each step
    // compute the total queries whose range contains this i, and set a[i] to the total amountToAdd
    // of this set of queries.
    //
    // But finding the set of ranges containing each i has a very well-known solution: first, sort
    // the ranges by their left index.  When we progress from i to i+1, find all queries whose
    // range's left index is i + 1, and add them to the list of active queries, activeQueriesByRightIndex,
    // and ensure activeQueriesByRightIndex is ordered by right index.
    //
    // At the same time, remove all queries from activeQueriesByRightIndex whose range no longer contains
    // i + 1 i.e. those whose right index < i + 1.  We now have precisely the set of queries whose
    // ranges contain i.  It's trivial to maintain the total amountToAdd (totalToAddFromActiveQueries) across this set of queries
    // as we add or remove them from activeQueriesByRightIndex.  In this way, we can easily find each a[i], and so
    // find the max a[i].  The algorithm is O(N) for the sweep across i, plus O(M) as each query gets added and removed
    // from activeQueriesByRightIndex at most once (each).
    //
    // However, this approach might be slightly less efficient than we need: we don't need to find each a[i]; merely
    // the max value of such an a[i].  This will simply be the largest totalToAddFromActiveQueries that we manage to 
    // form.  We don't need to consider each possible i to compute this: rather, we can just skip i's where activeQueriesByRightIndex
    // does not change i.e. just consider the i's which are a leftIndex or a rightIndex for a query.  We can
    // accomplish this by just ignoring the i's completely, and simply processing all queries in order of their left index.
    //
    // And that's it - just do this, and update maxTotalToAdd from each  totalToAddFromActiveQueries we find!
    int n;
    cin >> n;

    int m;
    cin >> m;

    vector<Query> queries;
    for (int i = 0; i < m; i++)
    {
        Query query;
        query.uniqueId = i;
        cin >> query.leftIndex;
        cin >> query.rightIndex;
        cin >> query.amountToAdd;

        // Make indices 0-relative.
        query.leftIndex--;
        query.rightIndex--;

        queries.push_back(query);
    }
    assert(cin);

    auto compareLeftIndices = [](const Query& lhs, const Query& rhs) 
    { 
        if (lhs.leftIndex != rhs.leftIndex)
            return lhs.leftIndex < rhs.leftIndex;

        return lhs.uniqueId < rhs.uniqueId;
    };
    auto compareRightIndices = [](const Query& lhs, const Query& rhs) 
    { 
        if (lhs.rightIndex != rhs.rightIndex)
            return lhs.rightIndex > rhs.rightIndex;

        return lhs.uniqueId > rhs.uniqueId;
    };
    using PriorityQueueIncreasingRightIndex = std::priority_queue<Query, std::vector<Query>, decltype(compareRightIndices) >;
    
    sort(queries.begin(), queries.end(), compareLeftIndices);

    PriorityQueueIncreasingRightIndex activeQueriesByRightIndex(compareRightIndices);
    int64_t totalToAddFromActiveQueries = 0;

    int64_t maxTotalToAdd = std::numeric_limits<int64_t>::min();
    // Process queries in order of left index, maintaining the list of currently active queries (and the total amount
    // they would add) as we go.
    for (const auto& query : queries)
    {
        while (!activeQueriesByRightIndex.empty() && activeQueriesByRightIndex.top().rightIndex < query.leftIndex)
        {
            // Remove "expired" active queries.
            totalToAddFromActiveQueries -= activeQueriesByRightIndex.top().amountToAdd;
            activeQueriesByRightIndex.pop();
        }

        activeQueriesByRightIndex.push(query);
        totalToAddFromActiveQueries += query.amountToAdd;

        maxTotalToAdd = std::max(maxTotalToAdd, totalToAddFromActiveQueries);
    }

    cout << maxTotalToAdd << endl;
}

