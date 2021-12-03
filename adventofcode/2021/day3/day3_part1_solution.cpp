#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

uint64_t binaryToDecimal(string binaryNumber)
{
    uint64_t result = 0;
    uint64_t powerOf2 = 1;
    while (!binaryNumber.empty())
    {
        if (binaryNumber.back() == '1')
            result += powerOf2;

        binaryNumber.pop_back();
        powerOf2 *= 2;
    }
   
    return result;
}

int main()
{
    string binaryNumber;
    vector<string> binaryNumbers;
    int numBinaryDigits = -1;
    while (cin >> binaryNumber)
    {
        binaryNumbers.push_back(binaryNumber);
        assert(numBinaryDigits == -1 || binaryNumber.size() == numBinaryDigits);
        numBinaryDigits = binaryNumber.size();
    }

    // MSB -> LSB.
    string gammaRateBits;
    string epsilonRateBits;
    for (int bitIndex = 0; bitIndex < numBinaryDigits; bitIndex++)
    {
        int numWithBitOn = 0;
        int numWithBitOff = 0;
        for (const auto& binaryNumber : binaryNumbers)
        {
            if (binaryNumber[bitIndex] == '0')
                numWithBitOff++;
            else
                numWithBitOn++;
        }
        if (numWithBitOn > numWithBitOff)
        {
            gammaRateBits += '1';
            epsilonRateBits += '0';
        }
        else if (numWithBitOn < numWithBitOff)
        {
            gammaRateBits += '0';
            epsilonRateBits += '1';
        }
        else
            assert(false);
    }

    cout << binaryToDecimal(epsilonRateBits) * binaryToDecimal(gammaRateBits) << endl;

}

