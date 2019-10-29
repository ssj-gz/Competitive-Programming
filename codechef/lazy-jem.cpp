// Simon St James (ssjgz) - 2019-10-29
// 
// Solution to: https://www.codechef.com/problems/TALAZY
//
#include <iostream>
#include <limits>
#include <iomanip>
#include <cmath>

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

int64_t findTimeTaken(int64_t numProblemsToSolve, int64_t breakLength, int64_t timeToSolveOneProblem)
{
    const auto timeToSolveOneProblemOrig = timeToSolveOneProblem;
    int64_t timeTaken = 0;

    int numIterations = 0;
    while (true)
    {
        const auto numToSolveBeforeBreak = ((numProblemsToSolve % 2) == 0) ? numProblemsToSolve / 2 : numProblemsToSolve / 2 + 1;
        const auto blee = numToSolveBeforeBreak * timeToSolveOneProblem;
        long int blaa = timeToSolveOneProblemOrig * pow(2, numIterations) * numToSolveBeforeBreak;
        //cout << "blee: " << blee << " blaa: " << blaa << endl;
        assert(blee == blaa);
        timeTaken += numToSolveBeforeBreak * timeToSolveOneProblem;

        numProblemsToSolve -= numToSolveBeforeBreak;

        if (numProblemsToSolve == 0)
            break;

        timeTaken += breakLength;

        timeToSolveOneProblem *= 2;
        numIterations++;

    }
    //cout << "numIterations: " << numIterations << endl;
     
    return timeTaken;
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
#if 1
    const int maxBlah = 10'000'000;
    int A = maxBlah;
    int B = maxBlah;
    int dA = -1;
    int dB = +1;
    while (true)
    {
        cout << findTimeTaken(A, 1, B) << endl;

        A += dA;
        B += dB;

        if (A >= maxBlah)
        {
            A--;
            dA = -dA;
            dB = -dB;
        }
        else if (B >= maxBlah)
        {
            B--;
            dA = -dA;
            dB = -dB;
        }
        cout << "A: " << A << " B: " << B << endl;

    }
#endif

#if 0
    while (true)
    {
        const int64_t N = rand() % 10'000'000 + 1 ;
        const int64_t B = rand() % 10'000'000 + 1 ;
        const int64_t M = rand() % 10'000'000 + 1 ;
        const auto glarp = findTimeTaken(N, B, M);
        cout << "gleep: " << glarp << endl;
    }
#endif
#if 0
    {
        while (true)
        {
            for (int i = 0; i < 10; i++)
            {
                const int64_t blee = rand() % 10'000'000 + 1;
                const int64_t bloo = rand() % 5'000'000 + 1;
                const int64_t blah = pow(2, i) * blee * bloo;
                const int64_t blaa = (static_cast<int64_t>(1) << static_cast<int64_t>(i)) * blee * bloo;
                cout << "blah: " << blah << " blaa: " << blaa << " i: " << i << endl;
                assert(blah == blaa);
            }
        }
        cout << std::setprecision (std::numeric_limits<double>::digits10 + 1);
        for (int i = 0; i < 23; i++)
        {
            cout << pow(2, i) * 10'000'000 << endl;
        }
    }
#endif
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
#if 0
            cout << rand() % 10'000'000 + 1 << " ";
            cout << rand() % 10'000'000 + 1 << " ";
            cout << rand() % 10'000'000 + 1;
#else
            cout << rand() % 100 + 1 << " ";
            cout << rand() % 100 + 1 << " ";
            cout << rand() % 100 + 1;
#endif
        }
        cout << endl;

        return 0;
    }
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto N = read<int64_t>();
        const auto B = read<int64_t>();
        const auto M = read<int64_t>();

        cout << findTimeTaken(N, B, M) << endl;
    }

    assert(cin);
}
