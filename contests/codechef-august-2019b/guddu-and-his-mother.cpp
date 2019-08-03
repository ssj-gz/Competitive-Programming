// Simon St James (ssjgz) - 2019-XX-XX
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int64_t solveBruteForce(const vector<int>& a)
{
    const int N = a.size();
    int64_t result = 0;

    for (int i = 0; i < N; i++)
    {
        for (int j = i + 1; j < N; j++)
        {
            for (int k = j; k < N; k++)
            {
                int leftXor = 0;
                for (int m = i; m < j; m++)
                {
                    leftXor = leftXor ^ a[m];
                }
                int rightXor = 0;
                for (int m = j; m <= k; m++)
                {
                    rightXor = rightXor ^ a[m];
                }
                int iTokXor = 0;
                for (int m = i; m <= k; m++)
                {
                    iTokXor = iTokXor ^ a[m];
                }
                assert((iTokXor == 0) == (leftXor == rightXor));
                if (leftXor == rightXor)
                {
                    //cout << " found one: i: " << i << " j: " << j << " k: " << k << " leftXor: " << leftXor << " rightXor: " << rightXor << " iTokXor: " << iTokXor << endl;
                    result++;
                }
            }
        }
    }

    return result;
}

int64_t solveOptimised(const vector<int>& a)
{
    const int n = a.size();
    int64_t result = 0;

    vector<int> xorSumUpTo;
    int xorSum = 0;
    for (const auto& x : a)
    {
        xorSum ^= x;
        xorSumUpTo.push_back(xorSum);
    }

    for (int k = 0; k < n; k++)
    {
        //cout << "k: " << k << " xorSumUpTo: " << xorSumUpTo[k] << endl;
        int64_t dbgAmountToAdd = 0;
        for (int i = 0; i < k; i++)
        {
            if (xorSumUpTo[i] == xorSumUpTo[k])
            {
                dbgAmountToAdd += (k - i - 1);
            }
        }
        if (xorSumUpTo[k] == 0)
            dbgAmountToAdd += k;

        result += dbgAmountToAdd;
    }


    return result;
}

int main(int argc, char* argv[])
{
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int N = rand() % 200 + 1;
        const int maxA = rand() % 10000 + 1;

        cout << 1 << endl;
        cout << N << endl;

        for (int i = 0; i < N; i++)
        {
            cout << ((rand() % maxA) + 1) << " ";
        }

        return 0;
    }

    const int T = read<int>();
    const int N = read<int>();

    for (int t = 0; t < T; t++)
    {
        vector<int> a(N);
        for (auto& x : a)
        {
            x = read<int>();
        }
#ifdef BRUTE_FORCE
        const auto solutionBruteForce = solveBruteForce(a);
        cout << " solutionBruteForce: " << solutionBruteForce << endl;
        const auto solutionOptimised = solveOptimised(a);
        cout << " solutionOptimised: " << solutionOptimised << endl;
        assert(solutionOptimised == solutionBruteForce);
#else
        const auto solutionOptimised = solveOptimised(a);
        cout << solutionOptimised << endl;
#endif
    }
}

