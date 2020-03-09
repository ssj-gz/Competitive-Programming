// Simon St James (ssjgz) - 2020-03-09
// 
// Solution to: https://www.codechef.com/ZCOPRAC/problems/ZCO12001/
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

int main()
{
    ios::sync_with_stdio(false);

    const auto N = read<int>();
    vector<int> brackets(N);
    for (auto i = 0; i < N; i++)
        cin >> brackets[i];
    
    vector<int> positionsOfUnmatchedOpenBrackets;
    auto nestingDepth = 0;
    auto largestNestingDepth = -1;
    auto largestNestingDepthPos = -1;
    auto largestDistBetweenMatchedBrackets = -1;
    auto largestDistBetweenMatchedBracketsPos = -1;
    for (auto pos = 0; pos < N; pos++)
    {
        if (brackets[pos] == 1) // Open bracket.
        {
            nestingDepth++;
            positionsOfUnmatchedOpenBrackets.push_back(pos);
            if (nestingDepth > largestNestingDepth)
            {
                largestNestingDepth = nestingDepth;
                largestNestingDepthPos = pos;
            }
        }
        else // Close bracket.
        {
            const auto distBetweenMatchedBrackets = pos - positionsOfUnmatchedOpenBrackets.back() + 1;
            if (distBetweenMatchedBrackets > largestDistBetweenMatchedBrackets)
            {
                largestDistBetweenMatchedBrackets = distBetweenMatchedBrackets;
                largestDistBetweenMatchedBracketsPos = positionsOfUnmatchedOpenBrackets.back();
            }
            positionsOfUnmatchedOpenBrackets.pop_back();
            nestingDepth--;
            assert(nestingDepth >= 0);
        }
    }

    cout << largestNestingDepth << " " << (largestNestingDepthPos + 1) << " " << largestDistBetweenMatchedBrackets << " " << (largestDistBetweenMatchedBracketsPos + 1) << endl;

    assert(cin);
}
