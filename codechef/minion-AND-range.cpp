// Simon St James (ssjgz) - 2020-01-10
// 
// Solution to: https://www.codechef.com/PLIT2020/problems/MINIAND/
//
#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

struct Query
{
    int leftIndex = -1;
    int rightIndex = -1;
};

/**
 * @return vector of bools, one for each query, where the value is true if and only if
 *         the result of the corresponding query is ODD. 
 */
vector<bool> calcQueryResults(const vector<int>& a, const vector<Query>& queries)
{
    vector<bool> queryResultIsOdd;

    vector<int> numEvensInPrefixOfLen;
    numEvensInPrefixOfLen.push_back(0);
    auto numEvens = 0;
    for (const auto value : a)
    {
        if (value % 2 == 0)
            numEvens++;
        numEvensInPrefixOfLen.push_back(numEvens);
    }

    for (const auto& query : queries)
    {
        const auto numEvensInRange = numEvensInPrefixOfLen[query.rightIndex + 1] - numEvensInPrefixOfLen[query.leftIndex];
        const auto andIsOdd = (numEvensInRange == 0);
        queryResultIsOdd.push_back(andIsOdd);
    }

    return queryResultIsOdd;
}

int main(int argc, char* argv[])
{
    // Easy one: the bitwise-and of a set of numbers is odd if and only if
    // the odd bit is set, which is true if and only if the odd bit is set
    // in all numbers, which is true if and only if none of the numbers is
    // odd.  The problem thus reduces to testing whether there are any even
    // numbers in each range, which can be easily done with a prefix-array
    // (numEvensInPrefixOfLen).
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto N = read<int>();
        vector<int> a(N);
        for (auto& value : a)
            value = read<int>();

        const auto numQueries = read<int>();
        vector<Query> queries(numQueries);

        for (auto& query : queries)
        {
            query.leftIndex = read<int>() - 1;
            query.rightIndex = read<int>() - 1;
        }

        for (const bool queryResultIsOdd : calcQueryResults(a, queries))
        {
            cout << (queryResultIsOdd ? "ODD" : "EVEN") << endl;
        }
    }

    assert(cin);
}
