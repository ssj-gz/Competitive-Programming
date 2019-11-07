// Simon St James (ssjgz) - 2019-11-06
// 
// Solution to: https://www.codechef.com/NOV19B/problems/CAMC
//
#define SUBMISSION
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


#if 1
int64_t solveBruteForce(int N, int M, const vector<int64_t>& a)
{
    int64_t minDistance = std::numeric_limits<int64_t>::max();
    for (int i = 0; i < N; i++)
    {
        for (int j = i + 1; j < N; j++)
        {
            if ((i % M) != (j % M))
                minDistance = min(minDistance, abs(a[i] - a[j]));
        }
    }

    return minDistance;
}
#endif

#if 1
int64_t solveOptimised(int N, int M, const vector<int64_t>& a)
{
    // Ok - if c1, c2, ... , cm are the colours, then any
    // valid colouring is of the form:
    //
    // π(c1)π(c2)...π(cm)π(c1)π(c2)...π(cm)...π(c1)π(c2)...
    //
    // where π is a permutation of c1, c2, ... , cm i.e.
    // is it some permutation of c1, c2, ... , cm.
    //
    // Similarly, a choice is valid for such a π if and only if
    // it is valid for the colouring:
    //
    // c1c2...cmc1c2...cm...c1c2...
    //
    // The colours at i and j are different if and only if i != j mod m,
    // so we merely need to find, for each k ... 1, 2, ..., m, the 
    // maximum a[i] such that i mod m = k and the minimum a[j] such that
    // j mod m != k.
    // TODO - rest of documentation, etc.
    struct ValueAndOriginalIndex
    {
        int64_t value = -1; 
        int originalIndex = -1; 
    };
    int64_t result = std::numeric_limits<int64_t>::max();

    vector<ValueAndOriginalIndex> aSortedAndIndices;
    for (int i = 0; i < N; i++)
    {
        aSortedAndIndices.push_back({a[i], i});
    }
    sort(aSortedAndIndices.begin(), aSortedAndIndices.end(), [](const auto& lhs, const auto& rhs) { return lhs.value < rhs.value; });

    for (int i = 1; i < N; i++)
    {
        if ((aSortedAndIndices[i].originalIndex % M) != (aSortedAndIndices[i - 1].originalIndex % M))
        {
            result = min(result, abs(aSortedAndIndices[i].value - aSortedAndIndices[i - 1].value));
        }
    }

    
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
            const int N = 2 + rand() % 100;
            const int M = 2 + rand() % (N - 1);
            assert(M <= N);
            const int maxA = 1 + rand() % 100;

            cout << N << " " << M << endl;

            for (int i = 0; i < N; i++)
            {
                cout << (1 + rand() % maxA) << " ";
            }
        }

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        const int M = read<int>();

        vector<int64_t> a(N);
        for (auto& x : a)
            x = read<int64_t>();

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(N, M, a);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 1
        const auto solutionOptimised = solveOptimised(N, M, a);
        cout << "solutionOptimised:  " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised(N, M, a);
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
