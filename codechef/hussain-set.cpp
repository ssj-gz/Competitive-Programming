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
            assert(newElements.empty() || maxElement / 2 <= newElements.front()); // i.e. newElements is always sorted.
            newElements.push_front(maxElement / 2);
        }
        currentOperationNum++;
    }
    
    return queryResults;
}

int main(int argc, char* argv[])
{
    // Fairly easy one - an asymptotically optimal solution is easy to spot:
    // we could just simulate the process using a std::multiset for a O((N + M) log N)
    // solution.  Unfortunately, the large sizes of N and M combined with the high constant
    // factor of std::multiset operations means that this would probably *just* time-out.
    //
    // We can use a more optimised (though still O(N log N)) approach - just sort
    // the array initially (very fast - std::sort has a low constant factor) and then
    // just simulate each operation with the exception that "new" elements - those that are the 
    // result of dividing the current max element by two and "re-adding" it into the
    // array - are added instead to an auxilliary deque called - appropriately enough - "newElements".
    // To decide what the current maximum element in the array would be if we were simulating the
    // process naively, we simply need to compare the current max element in
    // the array and the current max element in newElements and see which is largest.
    //
    // A moment's though tell us that each new element we add *cannot* be greater than
    // any newElements we've added so far, so if we just prepend each new element
    // to newElements, then newElements will always be in sorted order (see that assertion
    // concerning maxElement / 2 and newElements.front()).  Thus, both the array and 
    // newElements are always in sorted order, making finding (and removing) the maximum
    // element very efficient.
    //
    // With this in mind, hopefully the code now becomes self-explanatory: after the initial
    // sort, answering the queries takes a very quick O(1) per query.

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
