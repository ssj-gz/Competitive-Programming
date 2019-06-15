// Simon St James (ssjgz) - 2019-06-15
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>

#include <sys/time.h>

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

int64_t solveBruteForce(const vector<Query>& queries, int n)
{
    vector<int64_t> a(n);
    for (const auto& query : queries)
    {
        for (int i = query.leftIndex; i <= query.rightIndex; i++)
        {
            a[i] += query.amountToAdd;
        }
    }

    return *max_element(a.begin(), a.end());
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const auto n = rand() % 1000 + 1;
        const auto m = rand() % 1000 + 1;

        cout << n << " " << m << endl;

        for (int i = 0; i < m; i++)
        {
            int left = (rand() % n) + 1;
            int right = (rand() % n) + 1;
            if (left > right)
                swap(left, right);

            cout << left << " " << right << " " << rand() % 20'000 << endl;
        }
        return 0;

    }
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

    const auto bruteForce = solveBruteForce(queries, n);
    cout << "bruteForce: " << bruteForce << " optimised: " << maxTotalToAdd << endl;
}

