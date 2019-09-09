// Simon St James (ssjgz) - 2019-09-09
// 
// Solution to: https://www.codechef.com/SEPT19B/problems/CHEFINSQ
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

const int maxN = 50;

vector<vector<int64_t>> nCrLookup(maxN + 1, vector<int64_t>(maxN + 1, -1));

void buildnCrLookup()
{
    for (int n = 0; n <= maxN; n++)
    {
        nCrLookup[0][n] = 0;
        nCrLookup[n][n] = 1;
        nCrLookup[n][0] = 1;
    }
    for (int n = 1; n <= maxN; n++)
    {
        for (int k = 1; k < maxN; k++)
        {
            assert(nCrLookup[n - 1][k - 1] != -1);
            assert(nCrLookup[n - 1][k] != -1);

            nCrLookup[n][k] = nCrLookup[n - 1][k - 1] + nCrLookup[n - 1][k];
            //cout << "n: " << n << " k: " << k << " nCrLookup: " << nCrLookup[n][k] << endl;
        }
    }
}

#if 1
int solveBruteForce(int N, int K, const vector<int>& a)
{
    int result = 0;
    int minimumSizeSubset = numeric_limits<int>::max();

    vector<bool> useElement(N, false);

    while (true)
    {
        vector<int> subset;
        for (int i = 0; i < N; i++)
        {
            if (useElement[i])
                subset.push_back(a[i]);
        }
        if (subset.size() == K)
        {
            int sum = 0;
            for (const auto x : subset)
            {
                sum += x;
            }
            if (sum == minimumSizeSubset)
            {
                cout << "new subset: " << endl;
                for (const auto x : subset)
                {
                    cout << " " << x;
                }
                cout << endl;
                result++;
            }
            else if (sum < minimumSizeSubset)
            {
                minimumSizeSubset = sum;
                cout << "new min!:" << minimumSizeSubset << endl;
                cout << "new subset: " << endl;
                for (const auto x : subset)
                {
                    cout << " " << x;
                }
                cout << endl;
                result = 1;
            }
        }

        int index = 0;
        while (index < N && useElement[index])
        {
            useElement[index] = false;
            index++;
        }
        if (index == N)
            break;

        useElement[index] = true;
    }

    return result;
}

#endif

#if 1
int64_t solveOptimised(int N, int K, const vector<int>& aOriginal)
{
    if (K > N)
        return 0;
    int64_t result = 0;

    auto a = aOriginal;

    sort(a.begin(), a.end());
    const int kthElement = a[K - 1];
    const int numOfKthElement = count(a.begin(), a.end(), kthElement);
    int numOfKthElementInSubset = 1;
    K--;
    while (K - 1 >= 0 && a[K - 1] == kthElement)
    {
        K--;
        numOfKthElementInSubset++;
    }
    //cout << "kthElement: " << kthElement << " numOfKthElement: " << numOfKthElement << " numOfKthElementInSubset: " << numOfKthElementInSubset << endl;
    
    return nCrLookup[numOfKthElement][numOfKthElementInSubset];
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
            const int N = rand() % 20 + 1;
            const int K = rand() % 50 + 1;
            const int maxA = rand() % 100 + 1;

            cout << N << " " << K << endl;
            for (int i = 0; i < N; i++)
            {
                cout << ((rand() % maxA) + 1) << " ";
            }
            cout << endl;
        }

        return 0;
    }
    
    buildnCrLookup();

    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        const int K = read<int>();

        vector<int> a(N);
        for (auto& aElement : a)
        {
            aElement = read<int>();
        }

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(N, K, a);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 1
        const auto solutionOptimised = solveOptimised(N, K, a);
        cout << "solutionOptimised:  " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised(N, K, a);
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
