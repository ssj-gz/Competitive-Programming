// Simon St James (ssjgz) - 2019-06-15
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>

using namespace std;

struct Query
{
    int leftIndex = -1;
    int rightIndex = -1;

    int64_t amountToAdd = 0;
    int uniqueId = -1; // So we don't "lose" Queries with the same leftIndex and rightIndex!
};

bool operator<(const Query& lhs, const Query& rhs)
{
    if (lhs.leftIndex != rhs.leftIndex)
        return lhs.leftIndex < rhs.leftIndex;
    if (lhs.rightIndex != rhs.rightIndex)
        return lhs.rightIndex < rhs.rightIndex;

    return lhs.uniqueId < rhs.uniqueId;
}

int main()
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

    auto compareLeftIndices = [](const Query& lhs, const Query& rhs) 
    { 
        if (lhs.rightIndex != rhs.rightIndex)
            return lhs.rightIndex < rhs.rightIndex;

        return lhs.uniqueId < rhs.uniqueId;
    };
    auto compareRightIndices = [](const Query& lhs, const Query& rhs) 
    { 
        if (lhs.rightIndex != rhs.rightIndex)
            return lhs.rightIndex < rhs.rightIndex;

        return lhs.uniqueId < rhs.uniqueId;
    };
    using PriorityQueue = std::priority_queue<Query, std::vector<Query>, decltype(compareRightIndices) >;
    
    sort(queries.begin(), queries.end(), compareLeftIndices);

    PriorityQueue activeQueriesByRightIndex(compareRightIndices);
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

