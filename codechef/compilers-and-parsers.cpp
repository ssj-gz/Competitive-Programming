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

int findLongestValidPrefix(const string& expression)
{
#ifdef DIAGNOSTICS
    {
        cout << "prefixLength: prefix: " + string(max<int>(0, expression.size() - string("prefix:").size()), ' ') << " is valid: " << endl;
        for (int prefixLength = 1; prefixLength <= static_cast<int>(expression.size()); prefixLength++)
        {
            const auto prefix = expression.substr(0, prefixLength);
            auto numOpenBrackets = 0;
            bool isValid = true;
            for (const auto character : prefix)
            {
                if (character == '<')
                {
                    numOpenBrackets++;
                }
                else
                {
                    numOpenBrackets--;
                }
                if (numOpenBrackets < 0)
                {
                    isValid = false;
                    break;
                }
            }
            if (numOpenBrackets != 0)
                isValid = false;

            cout << prefixLength << string(string("prefixLength: ").size() - to_string(prefixLength).size(), ' ') << " " << prefix << string(expression.size() - prefixLength, ' ') << " " << (isValid ? "true" : "false") << endl;


        }
        cout << "==========" << endl;
    }
#endif
    auto longestValidPrefix = 0;
    auto numOpenBrackets = 0;

    auto prefixLength = 0;
    for (const auto character : expression)
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
        const auto expression = read<string>();
        cout << findLongestValidPrefix(expression) << endl;

    }

    assert(cin);
}
