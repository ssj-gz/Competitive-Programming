// Simon St James (ssjgz) 2017-09-28 15:00
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>

using namespace std;


// Just experimenting for now.
bool isPrime(int n)
{
    if (n < 2)
        return false;
    if (n == 2)
        return true;
    for (int i = 2; i <= sqrt(n); i++)
    {
        if ((n % i) == 0)
            return false;
    }
    return true;
}

int main()
{
    const int64_t modulus = 1'000'000'007ULL;
    const int numDigits = 4;
    vector<int> digits(numDigits);
    vector<int> almostChloeNumbers;
    while (true)
    {
        bool isAlmostChloeNumber = true;
        for (int numInSequence = 3; numInSequence <= 5; numInSequence++)
        {
            for (int i = 0; i < numDigits - numInSequence + 1; i++)
            {
                int sequenceSum = 0;
                for (int j = i; j < i + numInSequence; j++)
                {
                    sequenceSum += digits[j];
                }
                if (!isPrime(sequenceSum))
                {
                    isAlmostChloeNumber = false;
                    break;
                }
            }
        }
        if (isAlmostChloeNumber)
        {
            int almostChloeNumber = 0;
            for (const auto digit : vector<int>(digits.rbegin(), digits.rend()))
            {
                almostChloeNumber *= 10;
                almostChloeNumber += digit;
            }
            almostChloeNumbers.push_back(almostChloeNumber);
        }

        int digitIndex = 0;
        while (digitIndex < numDigits && digits[digitIndex] == 9)
        {
            digits[digitIndex] = 0;
            digitIndex++;
        }
        if (digitIndex == numDigits)
            break;
        digits[digitIndex]++;
    }
    const int maxN = 400'000;

    vector<vector<int>> chloeNumberExtensionIndexLookup(almostChloeNumbers.size());
    vector<int64_t> numChloeNumbersBeginningWithAlmostChloeNumber(almostChloeNumbers.size(), 1);
    cout << "almostChloeNumbers.size()" << almostChloeNumbers.size() << endl;
    for (int i = 0; i < almostChloeNumbers.size(); i++)
    {
        const int almostChloeNumber = almostChloeNumbers[i];
        for (int digit = 0; digit <= 9; digit++)
        {
            const int extendedAlmostChloeNumber = (digit * 10000) + almostChloeNumber;
            int digitSum = 0;
            for (const auto digitChar : to_string(extendedAlmostChloeNumber))
            {
                digitSum += digitChar - '0';
            }
            if (isPrime(digitSum))
            {
                // If d is digit, then the almostChloeNumber ABCD can be extended to dABCD (extendedAlmostChloeNumber) which has a prime sum.
                // Note that dABCD is an Chloe Number (albeit possibly with leading 0) if and only if dABC is an Almost
                // Chloe Number.  dABCD is extendedAlmostChloeNumber / 10.
                const auto shiftedAlmostChloeNumberIter = find(almostChloeNumbers.begin(), almostChloeNumbers.end(), extendedAlmostChloeNumber / 10);
                if (shiftedAlmostChloeNumberIter != almostChloeNumbers.end())
                {
                    const int shiftedAlmostChloeNumberIndex = distance(almostChloeNumbers.begin(), shiftedAlmostChloeNumberIter);
                    chloeNumberExtensionIndexLookup[i].push_back(shiftedAlmostChloeNumberIndex);
                }
            }
        }
    }


    vector<int64_t> numChloeNumbersWithNDigits;
    numChloeNumbersWithNDigits.push_back(0);
    numChloeNumbersWithNDigits.push_back(0);
    numChloeNumbersWithNDigits.push_back(0);
    numChloeNumbersWithNDigits.push_back(0);

    for (int i = 0; i < maxN - 5; i++)
    {
        //cout << "i: " << i << endl;
        int64_t numChloeNumbers = 0;
        for (int j = 0; j < almostChloeNumbers.size(); j++)
        {
            const int almostChloeNumber = almostChloeNumbers[j];
            if (almostChloeNumber >= 1000) // Only count those without leading 0's.
                numChloeNumbers = (numChloeNumbers + numChloeNumbersBeginningWithAlmostChloeNumber[j]) % modulus;
        }
        numChloeNumbersWithNDigits.push_back(numChloeNumbers);
        vector<int64_t> nextNumChloeNumbersBeginningWithAlmostChloeNumber(almostChloeNumbers.size());
        for (int j = 0; j < almostChloeNumbers.size(); j++)
        {
            for (const auto k : chloeNumberExtensionIndexLookup[j])
            {
                nextNumChloeNumbersBeginningWithAlmostChloeNumber[k] = (nextNumChloeNumbersBeginningWithAlmostChloeNumber[k] + numChloeNumbersBeginningWithAlmostChloeNumber[j]) % modulus;
            }
        }
        numChloeNumbersBeginningWithAlmostChloeNumber = nextNumChloeNumbersBeginningWithAlmostChloeNumber;
    }
    //assert(numChloeNumbersWithNDigits.size() == maxN);

    int Q;
    cin >> Q;

    for (int q = 0; q < Q; q++)
    {
        int n;
        cin >> n;
        cout << numChloeNumbersWithNDigits[n] << endl;
    }


}
