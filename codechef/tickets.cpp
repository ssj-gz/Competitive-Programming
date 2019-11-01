// Simon St James (ssjgz) - 2019-11-01
// 
// Solution to: https://www.codechef.com/problems/TICKETS5
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

bool solveBruteForce(const string& ticket)
{
    if (ticket[0] == ticket[1])
        return false;
    for (int i = 2; i < ticket.size(); i++)
    {
        if (ticket[i] != ticket[i - 2])
            return false;
    }
    return true;
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
            const int N = 2 + rand() % 10;
            const int maxChar = 1 + rand() % 26;

            for (int i = 0; i < N; i++)
            {
                cout << static_cast<char>('a' + (rand() % maxChar));
            }
            cout << endl;
        }

        return 0;
    }
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto ticket = read<string>();

        const auto solutionBruteForce = solveBruteForce(ticket);
        cout << (solutionBruteForce ? "YES" : "NO") << endl;
    }

    assert(cin);
}
