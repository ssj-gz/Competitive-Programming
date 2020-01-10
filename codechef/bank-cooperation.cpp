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

void solveBruteForceAux(const vector<int>& amountStolenFromBank, const vector<vector<bool>>& areBanksCooperating, int bankIndex, vector<bool>& isBankIndexUsed, int valueSoFar, int& bestValue)
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
        bestValue = max(bestValue, valueSoFar);
        return;
    }

    // Don't use this bank.
    solveBruteForceAux(amountStolenFromBank, areBanksCooperating, bankIndex + 1, isBankIndexUsed, valueSoFar, bestValue);

    // Use this bank.
    isBankIndexUsed[bankIndex] = true;
    solveBruteForceAux(amountStolenFromBank, areBanksCooperating, bankIndex + 1, isBankIndexUsed, valueSoFar + amountStolenFromBank[bankIndex], bestValue);
    isBankIndexUsed[bankIndex] = false;
}

int solveBruteForce(const vector<int>& amountStolenFromBank, const vector<vector<bool>>& areBanksCooperating)
{
    int result = 0;

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
        const int numCooperatingBanks = rand() % 1000;

        cout << numCooperatingBanks << endl;

        for (int i = 0; i < numCooperatingBanks; i++)
        {
            cout << (1 + rand() % numBanks) << " " << (1 + rand() % numBanks) << endl;
        }

        return 0;
    }
    

    vector<int> amountStolenFromBank(numBanks);
    for (auto& amount : amountStolenFromBank)
        amount = read<int>();

    const int numCooperatingBanks = read<int>();

    vector<vector<bool>> areBanksCooperating(numBanks, vector<bool>(numBanks, false));

    for (int i = 0; i < numCooperatingBanks; i++)
    {
        const auto bank1 = read<int>() - 1;
        const auto bank2 = read<int>() - 1;

        areBanksCooperating[bank1][bank2] = true;
        areBanksCooperating[bank2][bank1] = true;
    }

    for (int i = 0; i < numBanks; i++)
    {
        for (int j = 0; j < numBanks; j++)
        {
            cout << (areBanksCooperating[i][j] ? "X" : ".");
        }
        cout << endl;

    }

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(amountStolenFromBank, areBanksCooperating);
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
