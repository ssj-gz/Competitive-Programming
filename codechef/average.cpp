// Simon St James (ssjgz) - 2019-XX-XX
// 
// Solution to: TODO - problem link here!
//
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
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

bool isAverageBruteForce(const int targetAverage, const vector<int>& a)
{
    for (int i = 0; i < a.size(); i++)
    {
        for (int j = i + 1; j < a.size(); j++)
        {
            const int sum = a[i] + a[j];
            if (sum % 2 == 1)
                continue;
            if (sum == 2 * targetAverage)
            {
                return true;
            }
        }
    }
    return false;
}

int solveBruteForce(const vector<int>& a)
{
    int result = 0;
    for (const auto& targetAverage : a)
    {
        if (isAverageBruteForce(targetAverage, a))
            result++;
    }
    
    return result;
}

bool isAverageOfPair(const int targetAverage, const vector<int>& sortedA)
{
    assert(is_sorted(sortedA.begin(), sortedA.end()));

    const int targetPairSum = 2 * targetAverage;
    int rightIndex = sortedA.size() - 1;
    for (int leftIndex = 0; leftIndex < sortedA.size(); leftIndex++)
    {
        while (rightIndex > leftIndex && sortedA[leftIndex] + sortedA[rightIndex] > targetPairSum)
        {
            rightIndex--;
        }
        if (rightIndex <= leftIndex)
            return false;

        if (sortedA[leftIndex] + sortedA[rightIndex] == targetPairSum)
            return true;
    }

    return false;
}

int solveOptimised(const vector<int>& a)
{
    int result = 0;
    vector<int> sortedA = a;
    sort(sortedA.begin(), sortedA.end());
    for (const auto& targetAverage : a)
    {
        if (isAverageOfPair(targetAverage, sortedA))
            result++;
    }
    
    return result;
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
        
        const int N = 1 + rand() % 20;
        const int maxA = 1 + rand() % 100;

        cout << N << endl;

        for (int i = 0; i < N; i++)
        {
            cout << (1 + rand() % maxA) << endl;
        }

        return 0;
    }

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
    const auto solutionOptimised = solveOptimised(a);
    cout << solutionOptimised << endl;
#endif

    assert(cin);
}
