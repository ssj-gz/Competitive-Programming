// Simon St James (ssjgz) - 2019-10-31
// 
// Solution to: https://www.codechef.com/problems/PLAYSTR
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
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

bool canConvertByAdjacentSwaps(const string& sourceBinaryString, const string& targetBinaryString)
{
    assert(sourceBinaryString.length() == targetBinaryString.length());
    return count(sourceBinaryString.begin(), sourceBinaryString.end(), '0') == 
           count(targetBinaryString.begin(), targetBinaryString.end(), '0');
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
        }

        return 0;
    }
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        read<int>(); // N (unused)

        const auto sourceBinaryString = read<string>();
        const auto targetBinaryString = read<string>();

        cout << (canConvertByAdjacentSwaps(sourceBinaryString, targetBinaryString) ? "YES" : "NO") << endl;
    }

    assert(cin);
}
