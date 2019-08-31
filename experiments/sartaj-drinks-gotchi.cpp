// Simon St James (ssjgz) - 2019-XX-XX
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <limits>

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

void solutionBruteForceAux(const int N, const int P, const vector<int64_t>& a, int index, int cupNum, int64_t minCostSoFar, int64_t& best)
{
    if (index == N)
    {
        if (cupNum == P)
        {
            best = min(best, minCostSoFar);
        }
        return;
    }

    solutionBruteForceAux(N, P, a, index + 1, cupNum, minCostSoFar + cupNum * a[index], best);
    if (cupNum <= index)
        solutionBruteForceAux(N, P, a, index + 1, cupNum + 1, minCostSoFar + (cupNum + 1) * a[index], best);
}

#if 1
int64_t solveBruteForce(int N, int P, const vector<int64_t>& a)
{
    int64_t result = numeric_limits<int64_t>::max();

    solutionBruteForceAux(N, P, a, 0, 1, 0, result);
    
    return result;
}
#endif

#if 1
int64_t solveOptimised(int N, int P, const vector<int64_t>& a)
{
    vector<vector<int64_t>> minWithFirstNInPCups(N + 1, vector<int64_t>(P + 1, numeric_limits<int64_t>::max()));

    for (int i = 0; i <= N; i++)
    {
        minWithFirstNInPCups[i][0] = 0;
    }
    for (int i = 0; i <= P; i++)
    {
        minWithFirstNInPCups[0][i] = 0;
    }

    for (int i = 1; i <= N; i++)
    {
        for (int j = 1; j <= P; j++)
        {
            const auto addNewAToNewCup = (i >= j && (j > 1 || i == 1) ? minWithFirstNInPCups[i - 1][j - 1] + (j * a[i - 1]) : numeric_limits<int64_t>::max());
            const auto addNewAToOldCup = (i - 1 >= j ? minWithFirstNInPCups[i - 1][j] + (j * a[i - 1]) : numeric_limits<int64_t>::max());

            cout << "i: " << i << " j: " << j << " addNewAToOldCup: " << addNewAToOldCup << " addNewAToNewCup: " << addNewAToNewCup << endl;


            minWithFirstNInPCups[i][j] = min(addNewAToNewCup, addNewAToOldCup);

            cout << " set minWithFirstNInPCups[" << i << "][" << j << "] to " << minWithFirstNInPCups[i][j] << endl;
        }
    }

    return minWithFirstNInPCups[N][P];
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

        int N = rand() % 5 + 1;
        int P = rand() % 5 + 1;
        const int maxAbsA = rand() % 100 + 1;

        if (N < P)
            swap(P, N);

        cout << N << " " << P << endl;

        for (int i = 0; i < N; i++)
        {
            int a = rand() % maxAbsA;
            if (rand() % 2 == 0)
                a = -a;
            cout << a << " ";
        }
        cout << endl;

        return 0;
    }
    
    const int N = read<int>();
    const int P = read<int>();

    vector<int64_t> a(N);
    for (int i = 0; i < N; i++)
    {
        a[i] = read<int>();
    }


    
#ifdef BRUTE_FORCE
#if 1
    const auto solutionBruteForce = solveBruteForce(N, P, a);
    cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 1
    const auto solutionOptimised = solveOptimised(N, P, a);
    cout << "solutionOptimised:  " << solutionOptimised << endl;
    
    assert(solutionOptimised == solutionBruteForce);
#endif
#else
    const auto solutionOptimised = solveOptimised();
    cout << solutionOptimised << endl;
#endif

    assert(cin);
}

