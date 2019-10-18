// Simon St James (ssjgz) - 2019-10-18
// 
// Solution to: https://www.codechef.com/problems/SPAMCLAS
//
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

std::pair<int, int> solveBruteForce(const vector<int>& weights, const vector<int>& bias, int minX, int maxX)
{
    const int N = weights.size();
    

    int numSpammers = 0;
    int numNonSpammers = 0;
    for (int id = minX; id <= maxX; id++)
    {
        int64_t netResult = id;
        for (int layer = 0; layer < N; layer++)
        {
            netResult = weights[layer] * netResult + bias[layer];
        }
        if (netResult % 2 == 0)
        {
            numNonSpammers++;
        }
        else
        {
            numSpammers++;
        }

    }
    
    return {numNonSpammers, numSpammers};
}

std::pair<int, int> solveOptimised(const vector<int>& weights, const vector<int>& bias, int minX, int maxX)
{
    const int N = weights.size();

    const int numInRange = maxX - minX + 1;

    auto parityAfterApplyingNet = [N, &weights, &bias](const int id)
    {
        int netResultParity = id % 2;
        for (int layer = 0; layer < N; layer++)
        {
            netResultParity = (weights[layer] * netResultParity + bias[layer]) % 2;
        }
        return netResultParity;
    };

    const int affectOnEven = parityAfterApplyingNet(0);
    const int affectOnOdd = parityAfterApplyingNet(1);

    int numEvenInRange = -1;
    if (numInRange % 2 == 0)
    {
        numEvenInRange = numInRange / 2;
    }
    else
    {
        if ((minX % 2) == 0)
            numEvenInRange = numInRange / 2 + 1;
        else
            numEvenInRange = numInRange / 2;
    }

    int numNonSpammers = 0;
    int numSpammers = 0;
    const auto numOddInRange = numInRange - numEvenInRange;

    if (affectOnEven % 2 == 0)
    {
        numNonSpammers += numEvenInRange;
    }
    else
    {
        numSpammers += numEvenInRange;
    }


    if (affectOnOdd % 2 == 0)
    {
        numNonSpammers += numOddInRange;
    }
    else
    {
        numSpammers += numOddInRange;
    }

    
    




    return {numNonSpammers, numSpammers};
}


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
            const int N = rand() % 100 + 1;
            int minX = rand() % 100 + 1;
            int maxX = rand() % 100 + 1;
            if (maxX < minX)
                swap(minX, maxX);

            cout << N << " " << minX << " " << maxX << endl;

            for (int i = 0; i < N; i++)
            {
                cout << ((rand() % 100) + 1) << " " << ((rand() % 100) + 1) << endl;
            }

        }

        return 0;
    }
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        const int minX = read<int>();
        const int maxX = read<int>();

        vector<int> weights(N);
        vector<int> bias(N);

        for (int i = 0; i < N; i++)
        {
            cin >> weights[i];
            cin >> bias[i];
        }


#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(weights, bias, minX, maxX);
        cout << "solutionBruteForce: " << solutionBruteForce.first << " " << solutionBruteForce.second << endl;
#endif
#if 1
        const auto solutionOptimised = solveOptimised(weights, bias, minX, maxX);
        cout << "solutionOptimised:  " << solutionOptimised.first << " " << solutionOptimised.second << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised(weights, bias, minX, maxX);
        cout << solutionOptimised.first << " " << solutionOptimised.second << endl;
#endif
    }

    assert(cin);
}
