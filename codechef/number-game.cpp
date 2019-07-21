#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

int main()
{
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {

        string A;
        cin >> A;
        cout << "A: " << A << endl;

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

        for (const auto x : lastDigitsOfAMod)
        {
            cout << " lastDigitsOfAMod: " << x << endl;
        }
        for (const auto x : firstDigitsOfAMod)
        {
            cout << " firstDigitsOfAMod: " << x << endl;
        }


        powerOf10 = 1;
        for (int i = A.size() - 1; i >= 0; i--)
        {
            string aMinusDigit = A.substr(0, i) + A.substr(i + 1);
            cout << "aMinusDigit: " << aMinusDigit << endl;
            const int dbgAMinusDigitMod = stoi(aMinusDigit) % modulus;

            const int aMinusDigitMod = (firstDigitsOfAMod[i] * powerOf10 + lastDigitsOfAMod[A.size() - i - 1]) % modulus;
            cout << "i: " << i << " aMinusDigitMod: " << aMinusDigitMod << " dbgAMinusDigitMod: " << dbgAMinusDigitMod << endl;
            assert(aMinusDigitMod == dbgAMinusDigitMod);

            vector<int> digitsInAMinusDigit;
            for (int digit = 0; digit <= 9; digit++)
            {
                if (A.find('0' + digit) != string::npos)
                    digitsInAMinusDigit.push_back('0' + digit);
            }

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
                numStartingMoves++;

            powerOf10 = (powerOf10 * 10) % modulus;
        }
        cout << numStartingMoves << endl;



    }
}
