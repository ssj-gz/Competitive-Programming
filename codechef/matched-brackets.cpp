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
    auto largestNestingDepthBeginPos = -1;
    auto largestNestingDepthPos = -1;
    auto largestDistBetweenMatchedBrackets = -1;
    auto largestDistBetweenMatchedBracketsPos = -1;
    auto lastBalancePos = -1;
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
                largestNestingDepthBeginPos = lastBalancePos + 1;
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
            if (nestingDepth == 0)
                lastBalancePos = pos;
        }
    }
#ifdef DIAGNOSTICS
    {
        string bracketString;
        for (const auto bracketAsNum : brackets)
        {
            if (bracketAsNum == 1)
                bracketString += "(";
            else if (bracketAsNum == 2)
                bracketString += ")";
            else
                assert(false);
        }
        auto repeatedString = [](const string& stringToRepeat, int numRepetitions)
        {
            string repeatedString;
            for (int i = 0; i < numRepetitions; i++)
                repeatedString += stringToRepeat;
            return repeatedString;
        };
        cout << "Largest Nesting Depth: " << largestNestingDepth << ", occurring for the first time in below range:" << endl;
        cout << bracketString << endl;
        cout << repeatedString(" ", largestNestingDepthBeginPos) << u8"└" << repeatedString(u8"─", largestNestingDepthPos - largestNestingDepthBeginPos - 1) << u8"┘" << endl;
        cout << "Largest dist between matched brackets: " << largestDistBetweenMatchedBrackets << ", occurring for the first time in below range:" << endl;
        cout << bracketString << endl;
        cout << repeatedString(" ", largestDistBetweenMatchedBracketsPos) << u8"└" << repeatedString(u8"─", largestDistBetweenMatchedBrackets - 2) << u8"┘" << endl;
    }
#endif

    cout << largestNestingDepth << " " << (largestNestingDepthPos + 1) << " " << largestDistBetweenMatchedBrackets << " " << (largestDistBetweenMatchedBracketsPos + 1) << endl;

    assert(cin);
}
