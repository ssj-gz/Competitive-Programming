// Simon St James (ssjgz) - 2020-01-16
// 
// Solution to: https://www.codechef.com/problems/COMPILER
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

int findLongestValidPrefix(const string& string)
{
    int longestValidPrefix = 0;
    int numOpenBrackets = 0;

    int prefixLength = 0;
    for (const auto character : string)
    {
        prefixLength++;
        if (character == '<')
            numOpenBrackets++;
        else
        {
            numOpenBrackets--;
            if (numOpenBrackets < 0)
                break;
            if (numOpenBrackets == 0)
            {
                longestValidPrefix = prefixLength;
            }
        }
    }

    return longestValidPrefix;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto stringToParse = read<string>();
        cout << findLongestValidPrefix(stringToParse) << endl;

    }

    assert(cin);
}
