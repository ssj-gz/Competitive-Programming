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

void solveBruteForceAux(const vector<int>& amountStolenFromBank, const vector<vector<bool>>& areBanksCooperating, int bankIndex, vector<bool>& isBankIndexUsed, int64_t valueSoFar, int64_t& bestValue)
{
    if (bankIndex == numBanks)
    {
        for (int i = 0; i < numBanks; i++)
        {
            for (int j = 0; j < numBanks; j++)
            {
                if (isBankIndexUsed[i] && isBankIndexUsed[j] && areBanksCooperating[i][j])
                {
                    // Invalid selection of banks.
                    return;
                }
            }
        }
        // Valid selection.
        if (valueSoFar > bestValue)
        {
            bestValue =  valueSoFar;
#if 0
            cout << "Obtained new best: " << valueSoFar << endl;
            for (int i = 0; i < numBanks; i++)
            {
                if (isBankIndexUsed[i])
                    cout << "[" << (i + 1) << ": " << amountStolenFromBank[i] << "] ";
            }
            cout << endl;
#endif
        }
        return;
    }

    // Don't use this bank.
    solveBruteForceAux(amountStolenFromBank, areBanksCooperating, bankIndex + 1, isBankIndexUsed, valueSoFar, bestValue);

    // Use this bank.
    isBankIndexUsed[bankIndex] = true;
    solveBruteForceAux(amountStolenFromBank, areBanksCooperating, bankIndex + 1, isBankIndexUsed, valueSoFar + amountStolenFromBank[bankIndex], bestValue);
    isBankIndexUsed[bankIndex] = false;
}

int64_t solveBruteForce(const vector<int>& amountStolenFromBank, const vector<vector<bool>>& areBanksCooperating)
{
    int64_t result = 0;

    vector<bool> isBankIndexUsed(numBanks, false);
    solveBruteForceAux(amountStolenFromBank, areBanksCooperating, 0, isBankIndexUsed, 0, result);
    
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

        vector<pair<int, int>> allBankPairs;

        for (int i = 0; i < numBanks; i++)
        {
            for (int j = 0; j < numBanks; j++)
            {
                if (i != j)
                    allBankPairs.push_back({i + 1, j + 1});
            }
        }

        const int numPairsCooperatingBanks = rand() % (allBankPairs.size() + 1);
        cout << numPairsCooperatingBanks << endl;

        for (int i = 0; i < numPairsCooperatingBanks; i++)
        {
            const int pairIndex = rand() % numPairsCooperatingBanks;
            cout << allBankPairs[pairIndex].first << " " << allBankPairs[pairIndex].second << endl;
        }

        return 0;
    }
    
    vector<int> amountStolenFromBank(numBanks);
    for (auto& amount : amountStolenFromBank)
        amount = read<int>();

    const int numPairsCooperatingBanks = read<int>();

    vector<vector<bool>> areBanksCooperating(numBanks, vector<bool>(numBanks, false));

    for (int i = 0; i < numPairsCooperatingBanks; i++)
    {
        const auto bank1 = read<int>() - 1;
        const auto bank2 = read<int>() - 1;

        assert(bank1 != bank2);

        areBanksCooperating[bank1][bank2] = true;
        areBanksCooperating[bank2][bank1] = true;
    }

    const auto solutionBruteForce = solveBruteForce(amountStolenFromBank, areBanksCooperating);
    cout << solutionBruteForce << endl;

    assert(cin);
}
