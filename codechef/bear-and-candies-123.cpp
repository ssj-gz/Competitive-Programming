// Simon St James (ssjgz) - 2019-XX-XX
#define SUBMISSION
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

#if 0
SolutionType solveBruteForce()
{
    SolutionType result;
    
    return result;
}
#endif

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
        cout << 1 << endl;
        cout << ((rand() % 1000) + 1) << " " << ((rand() % 1000) + 1) << endl;
        return 0;
    }
    
    const int T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int maxCandiesLimark = read<int>();
        const int maxCandiesBob = read<int>();

        int numCandiesToEat = 1;

        int numCandiesEatenByLimark = 0;
        int numCandiesEatenByBob = 0;

        bool bobIsWinner = true;
        while (true)
        {
            numCandiesEatenByLimark += numCandiesToEat;
            if (numCandiesEatenByLimark > maxCandiesLimark)
            {
                bobIsWinner = true;
                break;
            }

            numCandiesToEat++;

            numCandiesEatenByBob += numCandiesToEat;
            if (numCandiesEatenByBob > maxCandiesBob)
            {
                bobIsWinner = false;
                break;
            }

            numCandiesToEat++;
        }

        if (bobIsWinner)
            cout << "Bob";
        else
            cout << "Limak";
        cout << endl;


    }

    assert(cin);
}
