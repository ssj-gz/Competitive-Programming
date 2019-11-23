// Simon St James (ssjgz) - 2019-XX-XX
// 
// Solution to: TODO - problem link here!
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <limits>
#include <set>

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
int solveBruteForce(const vector<int>& a)
{
    int minMaxDifference = std::numeric_limits<int>::max();

    set<vector<int>> toExplore = { a };
    set<vector<int>> alreadySeen = { a };

    while (!toExplore.empty())
    {

        set<vector<int>> nextToExplore;
        for (const auto& x : toExplore)
        {
            minMaxDifference = min(minMaxDifference, *max_element(x.begin(), x.end()) - *min_element(x.begin(), x.end()));

            for (int i = 0; i < x.size(); i++)
            {
                vector<int> nextX = x;
                if (nextX[i] % 2 == 0)
                {
                    nextX[i] = nextX[i] / 2;
                }
                else
                {
                    nextX[i] = nextX[i] * 2;
                }

                if (alreadySeen.find(nextX) == alreadySeen.end())
                {
                    alreadySeen.insert(nextX);
                    nextToExplore.insert(nextX);
                }
            }
        }
        toExplore = nextToExplore;
    }
    cout << "#alreadySeen: " << alreadySeen.size() << endl;

    return minMaxDifference;
}

#if 1
int solveOptimised(const vector<int>& a)
{
    int maxDifference = std::numeric_limits<int>::min();
    // The min value that can be formed by applying the transforms any number of times
    // to any of the values encountered so far.
    int minValueSoFar = std::numeric_limits<int>::max();
    // The max value that can be formed by applying the transforms any number of times
    // to any of the values encountered so far.
    int maxValueSoFar = -1;

    for (auto value : a)
    {
        const int maxTransformed = (value % 2 == 1) ? 2 * value : value;
        int minTransformed = value;
        while (value % 2 == 0)
        {
            value = value / 2;
            minTransformed = value;
        }

        if (minValueSoFar != std::numeric_limits<int>::max() && maxValueSoFar != -1)
        {
            maxDifference = max(maxDifference, abs(maxTransformed - minValueSoFar));
            maxDifference = max(maxDifference, abs(minTransformed - maxValueSoFar));
        }

        minValueSoFar = min(minValueSoFar, minTransformed);
        maxValueSoFar = max(maxValueSoFar, maxTransformed);
    }
    
    return maxDifference;
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
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            const int N = 2 + rand() % 10;
            const int maxA = rand() % 1000 + 1;

            cout << N << endl;
            for (int i = 0; i < N; i++)
            {
                cout << (1 + rand() % maxA);
                if (i != N - 1)
                    cout << " ";
            }
            cout << endl;

        }

        return 0;
    }
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        vector<int> a(N);
        for (auto& value : a)
            value = read<int>();

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(a);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 1
        const auto solutionOptimised = solveOptimised(a);
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
