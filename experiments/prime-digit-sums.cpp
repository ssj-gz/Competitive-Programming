// Simon St James (ssjgz).
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

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
    const int numDigits = 5;
    vector<int> digits(numDigits);
    vector<int> fiveDigitChloeNumbers;
    while (true)
    {

        bool satisfiesChloesRules = true;
        if (digits[numDigits - 1] == 0)
        {
            satisfiesChloesRules = false;
        }
        else
        {
            for (int numInSequence = 3; numInSequence <= 5; numInSequence++)
            {
                //cout << "numInSequence: " << numInSequence << endl;
                for (int i = 0; i < numDigits - numInSequence + 1; i++)
                {
                    //cout << " i: " << i << endl;
                    int sequenceSum = 0;
                    for (int j = i; j < i + numInSequence; j++)
                    {
                        //cout << "  j: " << j << endl;
                        sequenceSum += digits[j];
                    }
                    //cout << "sequenceSum: " << sequenceSum << " isprime: " << isPrime(sequenceSum) << endl;
                    if (!isPrime(sequenceSum))
                    {
                        satisfiesChloesRules = false;
                        //cout << " rule violation - sum: " << sequenceSum << endl;
                        goto out;
                    }
                }
            }
        }
out:
        if (satisfiesChloesRules)
        {
            cout << "Woo! ";
            int chloeNumber  = 0;
            for (const auto digit : vector<int>(digits.rbegin(), digits.rend()))
            {
                chloeNumber *= 10;
                chloeNumber += digit;
                cout << static_cast<char>(digit + '0');
            }
            cout << endl;
            cout << chloeNumber << endl;
            fiveDigitChloeNumbers.push_back(chloeNumber);
        }
        else
        {
            //cout << "does not satisfy rules" << endl;
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
#if 1
    vector<int> extensible5DigitChloeNumbers;
    for (const auto chloeNumber : fiveDigitChloeNumbers)
    {
        for (int digit = 1; digit <= 9; digit++) // No leading 0's.
        {
            const int extendedChloeNumber = (digit * 10000) + (chloeNumber / 10);
            if (find(fiveDigitChloeNumbers.begin(), fiveDigitChloeNumbers.end(), extendedChloeNumber)  != fiveDigitChloeNumbers.end())
            {
                extensible5DigitChloeNumbers.push_back(chloeNumber);
                cout << "wee: " << chloeNumber << " " << extendedChloeNumber << endl;
            }
        }
    }
#endif
    vector<vector<int>> chloeNumberExtensionIndexLookup(fiveDigitChloeNumbers.size());
    for (int i = 0; i < fiveDigitChloeNumbers.size(); i++)
    {
        const int chloeNumber = fiveDigitChloeNumbers[i];
        cout << "extensible5DigitChloeNumbers #" << i << " = " << chloeNumber << endl;
        for (int digit = 1; digit <= 9; digit++) // No leading 0's.
        {
            const int extendedChloeNumber = (digit * 10000) + (chloeNumber / 10);
            const auto extendedChloeNumberIterator = find(fiveDigitChloeNumbers.begin(), fiveDigitChloeNumbers.end(), extendedChloeNumber);
            if (extendedChloeNumberIterator == fiveDigitChloeNumbers.end())
                continue;
            const int extendedChloeNumberIndex = distance(fiveDigitChloeNumbers.begin(), extendedChloeNumberIterator);
            chloeNumberExtensionIndexLookup[i].push_back(extendedChloeNumberIndex);
            cout << " mapped to " << extendedChloeNumberIndex << endl;
        }

    }

    const int n = 12;
    vector<int> blah(fiveDigitChloeNumbers.size(), 1);
    for (int i = 0; i < n - 5; i++)
    {
        vector<int> nextBlah(fiveDigitChloeNumbers.size());
        for (int j = 0; j < fiveDigitChloeNumbers.size(); j++)
        {
            for (const auto k : chloeNumberExtensionIndexLookup[j])
            {
                nextBlah[k] += blah[j];
            }
        }
        blah = nextBlah;
        int total = 0;
        for (const auto wee : blah)
        {
            total += wee;
        }
        cout << "i: " << i << " total: " << total << endl;
    }
}
