// Simon St James (ssjgz) - 2019-12-07
// 
// Solution to: https://www.codechef.com/problems/RECIIDCD
//
#include <iostream>
#include <cctype>

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

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

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
