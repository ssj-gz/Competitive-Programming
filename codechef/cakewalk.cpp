// Simon St James (ssjgz) - 2019-10-23
// 
// Solution to: https://www.codechef.com/problems/CKWLK
//
#include <iostream>

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

bool calcCanMakeAmountFromCheatCodes(int64_t N)
{
    int numPowersOf5 = 0;
    while ((N % 5) == 0)
    {
        numPowersOf5++;
        N /= 5;
    }
    int numPowersOf2 = 0;
    while ((N % 2) == 0)
    {
        numPowersOf2++;
        N /= 2;
    }
    if (N != 1)
        return false;
    const int numCheatCodes = numPowersOf5;
    for (int numFirstCheatCode = 0; numFirstCheatCode <= numCheatCodes; numFirstCheatCode++)
    {
        const int numSecondCheatCode = numCheatCodes - numFirstCheatCode;
        const int numPowersOf2FromCheatCodes = numFirstCheatCode + 2 * numSecondCheatCode;
        if (numPowersOf2FromCheatCodes == numPowersOf2)
            return true;
    }
    return false;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int64_t N = read<int64_t>();


        const auto canMakeAmount = calcCanMakeAmountFromCheatCodes(N);
        cout << (canMakeAmount ? "Yes" : "No") << endl;
    }

    assert(cin);
}
