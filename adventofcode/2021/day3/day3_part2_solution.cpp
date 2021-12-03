#include <iostream>
#include <vector>
#include <algorithm>
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

uint64_t findRating(bool isOxygenRating, vector<string> binaryNumbers)
{
    const int numBinaryDigits = binaryNumbers.front().size();
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
        char requiredBit = '\0';
        if (numWithBitOn == numWithBitOff)
        {
            requiredBit = isOxygenRating ? '1' : '0';
        }
        else if (numWithBitOn > numWithBitOff)
        {
            requiredBit = isOxygenRating ? '1' : '0';
        }
        else if (numWithBitOn < numWithBitOff)
        {
            requiredBit = isOxygenRating ? '0' : '1';
        }

        binaryNumbers.erase(remove_if(binaryNumbers.begin(), binaryNumbers.end(), [bitIndex, requiredBit](const auto& binaryNumber)
                {
                    return binaryNumber[bitIndex] != requiredBit;
                }), binaryNumbers.end());

        if (binaryNumbers.size() == 1)
        {
            return binaryToDecimal(binaryNumbers.front());
        }
    }
    assert(false);
    return 0;
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

    const uint64_t oxygenGeneratorRating = findRating(true, binaryNumbers);
    const uint64_t co2ScrubberRating = findRating(false, binaryNumbers);

    cout << oxygenGeneratorRating * co2ScrubberRating << endl;

}

