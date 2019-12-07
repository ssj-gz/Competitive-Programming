// Simon St James (ssjgz) - 2019-12-07
// 
// Solution to: https://www.codechef.com/problems/RECIIDCD
//
#include <iostream>
#include <cctype>

#include <cassert>

using namespace std;

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!


template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            string s;
            const int numBlocks = rand() % 10 + 1;
            for (int i = 0; i < numBlocks; i++)
            {
                const int digit = 1 + rand() % 9;
                s += '0' + digit;
                const int stringLen = rand() % 10 + 1;
                for (int j = 0; j < stringLen; j++)
                {
                    s += 'a' + rand() % 26;
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

        for (int i = 0; i < s.size();)
        {
            assert(!isalpha(s[i]));
            const int numRepetitions = s[i] - '0';
            string toRepeat;
            i++; // Skip over the digit.
            while (i != s.size() && isalpha(s[i]))
            {
                toRepeat += s[i];
                i++;
            }
            for (int k = 1; k <= numRepetitions; k++)
            {
                cout << toRepeat;
            }
        }

        cout << endl;
    }

    assert(cin);
}
