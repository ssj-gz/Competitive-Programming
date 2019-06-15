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
    for (const auto& query : queries)
    {
        while (!activeQueriesByRightIndex.empty() && activeQueriesByRightIndex.top().rightIndex < query.leftIndex)
        {
            totalToAddFromActiveQueries -= activeQueriesByRightIndex.top().amountToAdd;
            activeQueriesByRightIndex.pop();
        }

        activeQueriesByRightIndex.push(query);
        totalToAddFromActiveQueries += query.amountToAdd;

        maxTotalToAdd = std::max(maxTotalToAdd, totalToAddFromActiveQueries);
    }

    cout << maxTotalToAdd << endl;
}

