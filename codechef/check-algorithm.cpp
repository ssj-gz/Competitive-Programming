// Simon St James (ssjgz) - 2019-12-29
// 
// Solution to: https://www.codechef.com/problems/CHEALG
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
            const int numLetters = 1 + rand() % 100;
            const int maxLetterIndex = 1 + rand() % 26;

            for (int i = 0; i < numLetters; i++)
                cout << static_cast<char>('a' + rand() % maxLetterIndex);
            cout << endl;
        }

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto toCompress = read<string>();
        const auto compressed = compress(toCompress);

        cout << (compressed.length() < toCompress.length() ? "YES" : "NO") << endl;

    }


    assert(cin);
}
