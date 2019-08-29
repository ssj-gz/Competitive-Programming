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
        // TODO - generate randomised test.
        return 0;
    }
    
    const int T = read<int>();

    for (int t = 0; t < T; t++)
    {
        int maxCandiesLimark = read<int>();
        int maxCandiesBob = read<int>();

        int numCandiesToEat = 1;

        bool bobIsWinner = true;
        while (true)
        {
            maxCandiesLimark -= numCandiesToEat;
            if (maxCandiesLimark < 0)
                break;

            numCandiesToEat++;
            
            maxCandiesBob -= numCandiesToEat;
            if (maxCandiesBob < 0)
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
