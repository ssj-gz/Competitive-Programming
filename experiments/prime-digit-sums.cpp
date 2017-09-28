// Simon St James (ssjgz).
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
    {
        int numSatisfying = 0;
        const int numDigits = 10;
        vector<int> digits(numDigits);
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
                            goto out1;
                        }
                    }
                }
            }
out1:
            if (satisfiesChloesRules)
            {
#if 0
                cout << "Woo! ";
                int chloeNumber = 0;
                for (const auto digit : vector<int>(digits.rbegin(), digits.rend()))
                {
                    chloeNumber *= 10;
                    chloeNumber += digit;
                    cout << static_cast<char>(digit + '0');
                }
                cout << endl;
#endif
                numSatisfying++;
                //cout << chloeNumber << endl;
            }
            else
            {
#if 0
                cout << "does not satisfy rules: ";
                for (const auto digit : vector<int>(digits.rbegin(), digits.rend()))
                {
                    cout << static_cast<char>(digit + '0');
                }
                cout << endl;
#endif
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
        cout << numSatisfying << endl; 
    }
    const int numDigits = 4;
    vector<int> digits(numDigits);
    vector<int> fourDigitChloeNumbers;
    while (true)
    {

        bool satisfiesChloesRules = true;
        //if (digits[numDigits - 1] == 0)
        //{
        //satisfiesChloesRules = false;
        //}
        //else
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
            int chloeNumber = 0;
            for (const auto digit : vector<int>(digits.rbegin(), digits.rend()))
            {
                chloeNumber *= 10;
                chloeNumber += digit;
                cout << static_cast<char>(digit + '0');
            }
            cout << endl;
            cout << chloeNumber << endl;
            fourDigitChloeNumbers.push_back(chloeNumber);
        }
        else
        {
            cout << "does not satisfy rules: ";
            for (const auto digit : vector<int>(digits.rbegin(), digits.rend()))
            {
                cout << static_cast<char>(digit + '0');
            }
            cout << endl;
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
#if 0
    vector<int> extensible4DigitChloeNumbers;
    for (const auto chloeNumber : fourDigitChloeNumbers)
    {
        for (int digit = 1; digit <= 9; digit++) // No leading 0's.
        {
            const int extendedChloeNumber = (digit * 1000) + (chloeNumber / 10);
            if (find(fourDigitChloeNumbers.begin(), fourDigitChloeNumbers.end(), extendedChloeNumber)  != fourDigitChloeNumbers.end())
            {
                extensible4DigitChloeNumbers.push_back(chloeNumber);
                cout << "wee: " << chloeNumber << " " << extendedChloeNumber << endl;
            }
        }
    }
#endif
#if 0
    vector<vector<int>> chloeNumberExtensionIndexLookup(fourDigitChloeNumbers.size());
    for (int i = 0; i < fourDigitChloeNumbers.size(); i++)
    {
        const int chloeNumber = fourDigitChloeNumbers[i];
        cout << "extensible4DigitChloeNumbers #" << i << " = " << chloeNumber << endl;
        for (int digit = 0; digit <= 9; digit++)
        {
            const int extendedChloeNumber = (digit * 10000) + chloeNumber;
            const auto extendedChloeNumberIterator = find(fourDigitChloeNumbers.begin(), fourDigitChloeNumbers.end(), extendedChloeNumber);
            if (extendedChloeNumberIterator == fourDigitChloeNumbers.end())
                continue;
            const int extendedChloeNumberIndex = distance(fourDigitChloeNumbers.begin(), extendedChloeNumberIterator);
            chloeNumberExtensionIndexLookup[i].push_back(extendedChloeNumberIndex);
            cout << " mapped to " << extendedChloeNumberIndex << endl;
        }

    }
#endif

    vector<vector<int>> chloeNumberExtensionIndexLookup(fourDigitChloeNumbers.size());
    const int n = 12;
    vector<int> blah(fourDigitChloeNumbers.size(), 1);
    for (int j = 0; j < fourDigitChloeNumbers.size(); j++)
    {
        const int chloeNumber = fourDigitChloeNumbers[j];
        for (int digit = 0; digit <= 9; digit++)
        {
            const int extendedChloeNumber = (digit * 10000) + chloeNumber;
            int digitSum = 0;
            for (const auto digitChar : to_string(extendedChloeNumber))
            {
                digitSum += digitChar - '0';
            }
            cout << "chloeNumber: " << chloeNumber << " extended: " << extendedChloeNumber << " digit sum:" << digitSum << endl;
            if (isPrime(digitSum))
            {
                //assert(find(fourDigitChloeNumbers.begin(), fourDigitChloeNumbers.end(), extendedChloeNumber / 10) != fourDigitChloeNumbers.end());
                const auto blahIter = find(fourDigitChloeNumbers.begin(), fourDigitChloeNumbers.end(), extendedChloeNumber / 10);
                if (blahIter != fourDigitChloeNumbers.end())
                {
                    int index = distance(fourDigitChloeNumbers.begin(), blahIter);
                    //blah[index]++;
                    cout << "waa: " << index << " " << blah[index] << endl;
                    chloeNumberExtensionIndexLookup[j].push_back(index);
                }
            }
        }
    }



    for (int i = 0; i < n - 5; i++)
    {
        int total = 0;
        for (int j = 0; j < fourDigitChloeNumbers.size(); j++)
        {
            const int wee = fourDigitChloeNumbers[j];
            if (wee >= 1000)
                total += blah[j];
        }
        cout << "i: " << i << " total: " << total << endl;
        vector<int> nextBlah(fourDigitChloeNumbers.size());
        for (int j = 0; j < fourDigitChloeNumbers.size(); j++)
        {
            for (const auto k : chloeNumberExtensionIndexLookup[j])
            {
                nextBlah[k] += blah[j];
            }
        }
        blah = nextBlah;
    }
}
