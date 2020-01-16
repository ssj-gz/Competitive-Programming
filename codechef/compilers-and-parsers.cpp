// Simon St James (ssjgz) - 2020-01-16
// 
// Solution to: https://www.codechef.com/problems/COMPILER
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!
#include <algorithm> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

using namespace std;

string asBinary(int64_t n) // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!
{
    string asBinary;
    if (n == 0)
        return ">";
    while (n > 0)
    {
        if ((n & 1) == 1)
        {
            asBinary.push_back('<');
        }
        else
        {
            asBinary.push_back('>');
        }

        n >>= 1;
    }

    reverse(asBinary.begin(), asBinary.end());

    return asBinary;
}


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
    if (argc == 2 && string(argv[1]) == "--test")
    {

        for (int n = 0; n < 100'000; n++)
        {
            cout << "Q: 2 lines" << endl;
            cout << 1 << endl;
            cout << asBinary(n) << endl;
            cout << "A: 1 lines" << endl;
            cout << findLongestValidPrefix(asBinary(n)) << endl;

        }

        return 0;
    }
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto stringToParse = read<string>();
        cout << findLongestValidPrefix(stringToParse) << endl;

    }

    assert(cin);
}
