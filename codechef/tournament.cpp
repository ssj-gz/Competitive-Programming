// Simon St James (ssjgz) - 2019-12-05
// 
// Solution to: https://www.codechef.com/ZCOPRAC/problems/ZCO13001/
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

int64_t solveBruteForce(const vector<int>& teamStrengths)
{
    int64_t totalRevenue = 0;
    const int N = teamStrengths.size();

    for (int team1Index = 0; team1Index < N; team1Index++)
    {
        for (int team2Index = team1Index + 1; team2Index < N; team2Index++)
        {
            totalRevenue += abs(teamStrengths[team1Index] - teamStrengths[team2Index]);
        }
    }
    
    return totalRevenue;
}

#if 0
int64_t solveOptimised()
{
    int64_t result;
    
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

        const int N = rand() % 100 + 1;
        const int maxA = rand() % 1000 + 1;

        cout << N << endl;
        for (int i = 0; i < N; i++)
        {
            cout << (1 + rand() % maxA);
            if (i != N)
                cout << " ";
        }
        cout << endl;

        return 0;
    }

    const int N = read<int>();

    vector<int> teamStrengths(N);
    for (auto& strength : teamStrengths)
        strength = read<int>();


#ifdef BRUTE_FORCE
#if 1
    const auto solutionBruteForce = solveBruteForce(teamStrengths);
    cout << "solutionBruteForce: " << solutionBruteForce << endl;
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

    assert(cin);
}
