// Simon St James (ssjgz) - 2020-01-10
// 
// Solution to: https://www.codechef.com/HEST2020/problems/CDUTSV01
//
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

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

void findLargestAmountInSingleContainerAux(const vector<int>& amountStolenFromBank, const vector<vector<bool>>& areBanksCooperating, int bankIndex, vector<bool>& isBankIndexUsed, int64_t valueSoFar, int64_t& bestValue)
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
    findLargestAmountInSingleContainerAux(amountStolenFromBank, areBanksCooperating, bankIndex + 1, isBankIndexUsed, valueSoFar, bestValue);

    // Use this bank.
    isBankIndexUsed[bankIndex] = true;
    findLargestAmountInSingleContainerAux(amountStolenFromBank, areBanksCooperating, bankIndex + 1, isBankIndexUsed, valueSoFar + amountStolenFromBank[bankIndex], bestValue);
    isBankIndexUsed[bankIndex] = false;
}

int64_t findLargestAmountInSingleContainer(const vector<int>& amountStolenFromBank, const vector<vector<bool>>& areBanksCooperating)
{
    int64_t result = 0;

    vector<bool> isBankIndexUsed(numBanks, false);
    findLargestAmountInSingleContainerAux(amountStolenFromBank, areBanksCooperating, 0, isBankIndexUsed, 0, result);
    
    return result;
}

int main(int argc, char* argv[])
{
    // Easily solvable via Brute Force.
    // I got a string of WAs on this for an embarrassing reason: the Problem says:
    //
    //    "Everything is guaranteed to easily fit in 32 bit integer type."
    // 
    // and, by "everything", I assumed it meant the output, too XD
    //
    // Turns out that, no, there are inputs whose output is too big for an int32. 
    //
    // Sigh - wish they'd rephrased it as "each amount stolen fits easily into 32 bit integer type".
    ios::sync_with_stdio(false);
    
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

    cout << findLargestAmountInSingleContainer(amountStolenFromBank, areBanksCooperating) << endl;

    assert(cin);
}
