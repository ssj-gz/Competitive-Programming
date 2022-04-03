// Simon St James (ssjgz) - 2022-03-31
// 
// Solution to: https://www.codechef.com/MARCH221D/problems/SUBSTRING
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
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto S = read<string>();
        const auto firstLetter = S.front();
        const auto finalLetter = S.back();

        // A substring P of S is "valid" if no substring of P
        // is a suffix of S or a prefix of S.
        // It's easily seen that a substring P of S is valid if
        // and only if it P contains neither the first nor last
        // letter of S.
        int currentValidSubstringLen = 0;
        int largestValidSubstringLen = 0;

        const int stringLen = static_cast<int>(S.size());

        for (int i = 0; i < stringLen; i++)
        {
            if (S[i] == firstLetter || S[i] == finalLetter)
            {
                currentValidSubstringLen = 0;
            }
            else
            {
                currentValidSubstringLen++;
                largestValidSubstringLen = max(largestValidSubstringLen,
                                               currentValidSubstringLen);
            }
        }
        if (largestValidSubstringLen == 0)
            cout << -1 << endl;
        else
            cout << largestValidSubstringLen << endl;

    }

    assert(cin);
}
