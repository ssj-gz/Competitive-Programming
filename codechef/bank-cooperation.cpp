// Simon St James (ssjgz) - 2020-01-10
// 
// Solution to: https://www.codechef.com/HEST2020/problems/CDUTSV01
//
//#define SUBMISSION
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
constexpr auto numBanks = 8;

void solveBruteForceAux(const vector<int>& amountStolenFromBank, const vector<vector<int>>& cooperatorsWithBank, int bankIndex, vector<int>& numUsedCooperatorsWithBank, int valueSoFar, int& bestValue)
{
    bestValue = max(bestValue, valueSoFar);
    if (bankIndex == numBanks)
        return;

    // Don't use this bank.
    solveBruteForceAux(amountStolenFromBank, cooperatorsWithBank, bankIndex + 1, numUsedCooperatorsWithBank, valueSoFar, bestValue);

    if (numUsedCooperatorsWithBank[bankIndex] == 0)
    {
        // Use this bank.
        for (const auto cooperator : cooperatorsWithBank[bankIndex])
            numUsedCooperatorsWithBank[cooperator]++;

        solveBruteForceAux(amountStolenFromBank, cooperatorsWithBank, bankIndex + 1, numUsedCooperatorsWithBank, valueSoFar + amountStolenFromBank[bankIndex], bestValue);

        for (const auto cooperator : cooperatorsWithBank[bankIndex])
            numUsedCooperatorsWithBank[cooperator]--;
    }
}

int solveBruteForce(const vector<int>& amountStolenFromBank, const vector<vector<int>>& cooperatorsWithBank)
{
    int result = 0;

    vector<int> numUsedCooperatorsWithBank(numBanks, 0);
    solveBruteForceAux(amountStolenFromBank, cooperatorsWithBank, 0, numUsedCooperatorsWithBank, 0, result);
    
    return result;
}

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

        const int maxStolen = 1 + rand() % 100;
        for (int i = 0; i < numBanks; i++)
        {
            cout << (1 + rand() % maxStolen);
            if ( i != numBanks - 1)
                cout << " ";
        }
        cout << endl;
        vector<pair<int, int>> totalPairs;
        for (int i = 0; i < numBanks; i++)
        {
            for (int j = i + 1; j < numBanks; j++)
            {
                totalPairs.push_back({i + 1, j + 1});
            }
        }

        random_shuffle(totalPairs.begin(), totalPairs.end());

        const int numCooperatingBanks = rand() % (1 + totalPairs.size());

        cout << numCooperatingBanks << endl;

        for (int i = 0; i < numCooperatingBanks; i++)
        {
            cout << totalPairs[i].first << " " << totalPairs[i].second << endl;
        }

        return 0;
    }
    

    vector<int> amountStolenFromBank(numBanks);
    for (auto& amount : amountStolenFromBank)
        amount = read<int>();

    const int numCooperatingBanks = read<int>();

    vector<vector<int>> cooperatorsWithBank(numBanks);

    for (int i = 0; i < numCooperatingBanks; i++)
    {
        const auto bank1 = read<int>() - 1;
        const auto bank2 = read<int>() - 1;

        cooperatorsWithBank[bank1].push_back(bank2);
        cooperatorsWithBank[bank2].push_back(bank1);
    }

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(amountStolenFromBank, cooperatorsWithBank);
        //cout << "solutionBruteForce: " << solutionBruteForce << endl;
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

    assert(cin);
}
