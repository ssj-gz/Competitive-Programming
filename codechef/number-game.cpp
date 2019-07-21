// Simon St James (ssjgz) - 2019-07-21
#define SUBMISSION
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        const int maxSumOfStringLengths = 1'000'000;
        const int maxMod = 1000;
        vector<string> strings;
        int sumOfStringLengths = 0;
        while (sumOfStringLengths < maxSumOfStringLengths)
        {
            const int stringLength = (rand() % (maxSumOfStringLengths - sumOfStringLengths) + 1);
            //const int stringLength = (rand() % (7) + 2);
            string numberString;
            for (int i = 0; i < stringLength; i++)
            {
                numberString.push_back('0' + rand() % 10);
            }
            strings.push_back(numberString);
            sumOfStringLengths += stringLength;
        }

        cout << strings.size() << endl;
        for (int i = 0; i < strings.size(); i++)
        {
            cout << strings[i] << " " << ((rand() % maxMod) + 1) << endl;
        }
        return 0;
    }
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {

        string A;
        cin >> A;
        //cout << "A: " << A << endl;

        int modulus;
        cin >> modulus;

        int numStartingMoves = 0;
        vector<int> firstDigitsOfAMod = { 0 };
        vector<int> lastDigitsOfAMod = { 0 };

        int numOfDigitInA[10] = {};
        int value = 0;
        for (const auto digit : A)
        {
            const int digitValue = digit - '0';
            numOfDigitInA[digitValue]++;
            value = (value * 10 + digitValue) % modulus;
            firstDigitsOfAMod.push_back(value);
        }
        value = 0;
        int powerOf10 = 1;
        for (auto digitReverseIter = A.crbegin(); digitReverseIter != A.crend(); digitReverseIter++)
        {
            const int digitValue = *digitReverseIter - '0';
            value = (value + digitValue * powerOf10) % modulus;
            lastDigitsOfAMod.push_back(value);

            powerOf10 = (powerOf10 * 10) % modulus;
        }

#if 0
        for (const auto x : lastDigitsOfAMod)
        {
            cout << " lastDigitsOfAMod: " << x << endl;
        }
        for (const auto x : firstDigitsOfAMod)
        {
            cout << " firstDigitsOfAMod: " << x << endl;
        }
#endif

        enum Status { Unknown, Yes, No };
        vector<Status> canMakeZeroWithStartingValue(modulus, Unknown);

        powerOf10 = 1;
        for (int i = A.size() - 1; i >= 0; i--)
        {

            const int aMinusDigitMod = (firstDigitsOfAMod[i] * powerOf10 + lastDigitsOfAMod[A.size() - i - 1]) % modulus;
            //cout << "Blee: " << (firstDigitsOfAMod[i] * powerOf10) << " blah: " << firstDigitsOfAMod[i] << endl;
#ifndef SUBMISSION
            {
                string aMinusDigit = A.substr(0, i) + A.substr(i + 1);
                //cout << "aMinusDigit: " << aMinusDigit << endl;
                //int powerOf10 = 1;
                int dbgAMinusDigitMod = 0;
                for (const auto digit : aMinusDigit)
                {
                    dbgAMinusDigitMod = (dbgAMinusDigitMod * 10 + (digit - '0')) % modulus;
                }
                cout << "i: " << i << " aMinusDigitMod: " << aMinusDigitMod << " dbgAMinusDigitMod: " << dbgAMinusDigitMod << endl;
                assert(aMinusDigitMod == dbgAMinusDigitMod);
            }
#endif
            if (canMakeZeroWithStartingValue[aMinusDigitMod] == Unknown)
            {
                const int removedDigitValue = (A[i] - '0');

                numOfDigitInA[removedDigitValue]--;
                vector<int> digitsInAMinusDigit;
                for (int digit = 0; digit <= 9; digit++)
                {
                    if (numOfDigitInA[digit] > 0)
                        digitsInAMinusDigit.push_back('0' + digit);
                }
                numOfDigitInA[removedDigitValue]++;

                vector<bool> processed(modulus, false);
                vector<int> toProcess = { aMinusDigitMod };

                while (!toProcess.empty())
                {
                    //cout << " # toProcess: " << toProcess.size() << endl;
                    vector<int> nextToProcess;
                    for (const auto p : toProcess)
                    {
                        processed[p] = true;
                        int pTimes10 = (p * 10) % modulus;
                        for (const auto digit : digitsInAMinusDigit)
                        {
                            const int newValue = (pTimes10 + digit) % modulus;
                            if (!processed[newValue])
                            {
                                nextToProcess.push_back(newValue);
                            }
                        }
                    }
                    toProcess = nextToProcess;
                }

                if (processed[0])
                {
                    canMakeZeroWithStartingValue[aMinusDigitMod] = Yes;
                }
                else
                {
                    canMakeZeroWithStartingValue[aMinusDigitMod] = No;
                }

            }
            if (canMakeZeroWithStartingValue[aMinusDigitMod] == Yes)
                numStartingMoves++;
            powerOf10 = (powerOf10 * 10) % modulus;

        }
        cout << numStartingMoves << endl;



    }
}
