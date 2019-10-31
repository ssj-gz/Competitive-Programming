// Simon St James (ssjgz) - 2019-10-29
// 
// Solution to: https://www.codechef.com/problems/LAPIN
//
#include <iostream>
#include <array>
#include <algorithm>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

constexpr auto numLetters = 26;

array<int, numLetters> letterHistogram(const string& s)
{
    array<int, numLetters> letterHistogram = {};
    for (const auto letter : s)
    {
        letterHistogram[letter - 'a']++;
    }
    return letterHistogram;
}

bool isLapindrome(const string& s)
{
    const int length = s.length();
    const auto leftHalf = s.substr(0, length / 2);
    const auto rightHalf = s.substr(length - length / 2);

    return letterHistogram(leftHalf) == letterHistogram(rightHalf);
}

#if 0
SolutionType solveOptimised()
{
    SolutionType result;
    
    return result;
}
#endif


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            const int len = 1 + rand() % 100;
            const int maxChar = 1 + rand() % numLetters;

            const bool generateYES = (rand() % 2 == 0);

            string s;
            if (generateYES)
            {
                string halfString;
                for (int i = 0; i < len / 2; i++)
                {
                    halfString.push_back('a' + rand() % maxChar);
                }
                s += halfString;
                if (len % 2 == 1)
                    s += 'a' + rand() % maxChar;
                random_shuffle(halfString.begin(), halfString.end());
                s += halfString;
            }
            else
            {

                for (int i = 0; i < len; i++)
                {
                    s.push_back('a' + rand() % maxChar);
                }
            }
            cout << s << endl;
        }

        return 0;
    }
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {

        const auto s = read<string>();
        cout << (isLapindrome(s) ? "YES" : "NO") << endl;
    }

    assert(cin);
}
