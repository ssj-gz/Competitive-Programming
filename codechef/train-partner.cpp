// Simon St James (ssjgz) - 2020-01-21
// 
// Solution to: https://www.codechef.com/problems/ANKTRAIN
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

#if 1
string solveBruteForce(int berthNumber)
{
    string result;

    static const string berthNames[] = 
    {
        "LB", // 1
        "MB", // 2
        "UB", // 3
        "LB", // 4
        "MB", // 5,
        "UB", // 6
        "SL", // 7
        "SU", // 8
    };

    static const int berthPartner[] = 
    {
        4, // 1
        5, // 2
        6, // 3
        1, // 4
        2, // 5
        3, // 6
        8, // 7
        7, // 8
    };

    const int carriageNum = (berthNumber - 1) / 8;
    const int berthNumberReduced = ((berthNumber - 1) % 8); // 0-relative.
    const int berthPartnerIndex = berthPartner[berthNumberReduced] - 1; // 0-relative.
    const int berthPartnerNumber = carriageNum * 8 + berthPartnerIndex + 1;
    const string berthPartnerName = berthNames[berthPartnerIndex];
    
    return to_string(berthPartnerNumber) + berthPartnerName;
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
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            const int n = 1 + rand() % 1000;
            cout << n << endl;
        }

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int berthNumber = read<int>();

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(berthNumber);
        cout << solutionBruteForce << endl;
#endif
#if 0
        const auto solutionOptimised = solveOptimised();
        cout << "solutionOptimised:  " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised();
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
