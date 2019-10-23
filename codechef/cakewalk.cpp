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
    {
        // The prime factorisation of N does not consist solely of powers of 2 and powers
        // of 5, which would be the case if it could be formed by using the cheat codes.
        return false;
    }
    const int numCheatCodes = numPowersOf5;
    // Trial and error to split the numCheatCodes cheat codes into numFirstCheatCode & numSecondCheatCode.
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
    // Easy one.  Note that applying the First cheat code multiplies the amount by
    //
    //    2^1 x 5^1
    //
    // and the Second multiplies it by:
    //
    //    2^2 x 5^1
    //
    // We observe two things:
    //
    //    i) if N is formed from the cheat codes, then the number of cheat codes applied is
    //       the power of 5 in the prime factorisation of N; and
    //   ii) if N is formed from the cheat codes, then the prime factorisation of N
    //       consists only of a power of 2 and a power of 5.
    //
    // We can easily find numPowersOf5 (== numCheatCodes) by counting how many times we can
    // divide N by 5.  We then just need to split, by trial and error, the number of cheat
    // codes into X First and (numCheatCodes - X) second and see if we form the requisite
    // power of 2 in N.
    //
    // Hopefully the code is reasonably clear :)


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
