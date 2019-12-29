// Simon St James (ssjgz) - 2019-12-29
// 
// Solution to: https://www.codechef.com/problems/CHEALG
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

string compress(const string& originalString)
{
    string toCompress = originalString + "."; // Append a sentinel value, to simplify the code.
    string compressed;

    char previousLetter = '\0';
    int numOfLetterInRun = 0;
    for (auto letter : toCompress)
    {
        if (letter == previousLetter)
        {
            numOfLetterInRun++;
        }
        else
        {
            if (previousLetter != '\0')
            {
                compressed += previousLetter + to_string(numOfLetterInRun);
            }

                previousLetter = letter;
                numOfLetterInRun = 1;
        }
    }

    return compressed;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto toCompress = read<string>();
        const auto compressed = compress(toCompress);

        cout << (compressed.length() < toCompress.length() ? "YES" : "NO") << endl;

    }

    assert(cin);
}
