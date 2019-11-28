// Simon St James (ssjgz) - 2019-11-28
// 
// Solution to: https://www.codechef.com/problems/COOK82C
//
#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

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

vector<int64_t> answerQueries(const vector<int64_t>& aOriginal, const vector<int>& queries)
{

    vector<int64_t> a(aOriginal);
    sort(a.begin(), a.end());

    int currentOperationNum = 1;
    int queryIndex = 0;
    vector<int64_t> queryResults;
    deque<int64_t> newElements;

    while (queryIndex < queries.size())
    {
        bool maxIsInA = (newElements.empty() || (!a.empty() && a.back() >= newElements.back()));
        const auto maxElement = maxIsInA ? a.back() : newElements.back();
        if (currentOperationNum == queries[queryIndex])
        {
            queryResults.push_back(maxElement);
            queryIndex++;
        }
        if (maxIsInA)
        {
            a.pop_back();
        }
        else
        {
            newElements.pop_back();
        }
        if (maxElement / 2 > 0)
        {
            assert(newElements.empty() || maxElement / 2 <= newElements.front());
            newElements.push_front(maxElement / 2);
        }
        currentOperationNum++;
    }
    
    return queryResults;

}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    const int N = read<int>();
    const int M = read<int>();

    vector<int64_t> a(N);
    for (auto& x : a)
        x = read<int64_t>();

    vector<int> queries(M);
    for (auto& x : queries)
        x = read<int>();

    const auto queryResults = answerQueries(a, queries);
    for (const auto queryResult : queryResults)
    {
        cout << queryResult << '\n';
    }

    assert(cin);
}
