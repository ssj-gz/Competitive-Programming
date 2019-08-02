// Simon St James (ssjgz) - 2019-XX-XX
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
void solveBruteForce(int N, const int K, int64_t M, int distFromX0, vector<int>& distsOfImpactFromX0SoFar, bool& hasSolution)
{
    //cout << "solveBruteForce: N: " << N << " K: " << K << " M: " << M << " distFromX0: " << distFromX0 <<endl;
    if (N == 0 && M == 0)
    {
        hasSolution = true;
        cout << "Found solution: " << endl;
        for (const auto x : distsOfImpactFromX0SoFar)
        {
            cout << x << " ";
        }
        cout << endl;
        return;
    }
    if (N == 0)
        return;

    if (M < 0)
        return;

    int64_t powerOfK = 1;
    for (int i = 0; i < distFromX0; i++)
    {
        powerOfK *= K;
    }
    if (powerOfK > M)
        return;

    // Use this dist.
    distsOfImpactFromX0SoFar.push_back(distFromX0);
    solveBruteForce(N - 1, K, M - powerOfK, distFromX0 + 1, distsOfImpactFromX0SoFar, hasSolution);
    distsOfImpactFromX0SoFar.pop_back();

    // Don't use this dist.
    solveBruteForce(N, K, M, distFromX0 + 1, distsOfImpactFromX0SoFar, hasSolution);
}

bool solveBruteForce(int N, int K, int64_t M, int64_t X0)
{
    bool hasSolution = false;
    vector<int> distsOfImpactFromX0SoFar;

    solveBruteForce(N, K, M, 0, distsOfImpactFromX0SoFar, hasSolution);

    return hasSolution;
}


bool solveOptimised(int N, int K, int64_t M, int64_t X0)
{
    if (K == 1)
        return M == N;

    int64_t powerOfK = 1;
    while (powerOfK * K <= M)
    {
        powerOfK *= K;
    }

    int num1DigitsInBaseK = 0;
    while (powerOfK >= 1)
    {
        const int digit = M / powerOfK;
        if (digit != 0 && digit != 1)
            return false;
        if (digit == 1)
            num1DigitsInBaseK++;

        M -= digit * powerOfK;
        powerOfK /= K;
    }

    return (num1DigitsInBaseK == N);
}
int main(int argc, char* argv[])
{
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        while (true)
        {
            const int N = rand() % 10 + 1;
            const int K = rand() % 20 + 3;
            const int64_t maxM = rand() % 1'000'000'000 + 1;
            const int X0 = rand() % 200 - 100;

            if (rand() % 4 == 0)
            {
                vector<int> possibleImpactSites;
                int impactSite = 0;
                int64_t powerOfK = 1;
                vector<int> powersOfK;
                while( powerOfK <= maxM)
                {
                    powersOfK.push_back(powerOfK);
                    possibleImpactSites.push_back(impactSite);

                    powerOfK = powerOfK * K;
                    impactSite++;
                }
                if (possibleImpactSites.size() < N)
                    continue;

                random_shuffle(possibleImpactSites.begin(), possibleImpactSites.end());

                int64_t M = 0;
                for (int i = 0; i < N; i++)
                {
                    M += powersOfK[possibleImpactSites[i]];
                }

                cout << 1 << endl;
                cout << N << " " << K << " " << M << " " << X0 << endl;
                break;
            }
            else
            {
                const auto M = maxM;
                cout << 1 << endl;
                cout << N << " " << K << " " << M << " " << X0 << endl;
            }
        }

        return 0;
    }

    const int T = read<int>();
    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        const int K = read<int>();
        const int64_t M = read<int64_t>();
        const int64_t X0 = read<int64_t>();

        const auto hasSolutionBruteForce = solveBruteForce(N, K, M, X0);
        cout << "solution brute force: " << (hasSolutionBruteForce ? "yes" : "no") << endl;
        const auto hasSolutionOptimised = solveOptimised(N, K, M, X0);
        cout << "solution Optimised: " << (hasSolutionOptimised ? "yes" : "no") << endl;

        assert(hasSolutionBruteForce == hasSolutionOptimised);
    }
}

