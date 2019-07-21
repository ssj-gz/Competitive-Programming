// Simon St James (ssjgz) - 2019-07-21
#define SUBMISSION
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>

#include <cassert>

using namespace std;

enum Status { Unknown, Yes, No };

int main(int argc, char* argv[])
{
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        string A;
        cin >> A;

        int modulus;
        cin >> modulus;

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
        int powerOf10ForAppending = 1;
        for (int i = 0; i < A.size() - 1; i++)
        {
            powerOf10ForAppending = (powerOf10ForAppending * 10) % modulus;
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
        vector<int> digitsInA;
        for (int digit = 0; digit <= 9; digit++)
        {
            if (numOfDigitInA[digit] > 0)
            {
                digitsInA.push_back(digit);
            }
        }


        powerOf10 = 1;
        vector<int> numTimesCanMakeValueByRemoving1Digit(modulus, 0);
        for (int i = A.size() - 1; i >= 0; i--)
        {
            const int aMinusDigitMod = (firstDigitsOfAMod[i] * powerOf10 + lastDigitsOfAMod[A.size() - i - 1]) % modulus;
            numTimesCanMakeValueByRemoving1Digit[aMinusDigitMod]++;
            powerOf10 = (powerOf10 * 10) % modulus;
        }
        vector<int> valuesFromRemoving1Digit;
        for (int i = 0; i < modulus; i++)
        {
            if (numTimesCanMakeValueByRemoving1Digit[i] > 0)
            {
                valuesFromRemoving1Digit.push_back(i);
            }
        }

        vector<bool> canMake0ViaAppendsStartingWith(modulus, Unknown);
        canMake0ViaAppendsStartingWith[0] = true;

        vector<vector<int>> canBeReachedByAppendFrom(modulus);
        for (int startValue = 0; startValue < modulus; startValue++)
        {
            for (const auto otherValue : valuesFromRemoving1Digit)
            {
                const int newValue = (startValue * powerOf10ForAppending + otherValue) % modulus;
                canBeReachedByAppendFrom[newValue].push_back(startValue);
            }
        }

        vector<bool> visited(modulus, false);
        vector<int> toProcess = { 0 };
        visited[0] = true;

        while (!toProcess.empty())
        {
            vector<int> nextToProcess;
            for (const auto p : toProcess)
            {
                canMake0ViaAppendsStartingWith[p] = true;

                for (const auto reachedFrom : canBeReachedByAppendFrom[p])
                {
                    if (!visited[reachedFrom])
                    {
                        visited[reachedFrom] = true;

                        nextToProcess.push_back(reachedFrom);
                    }
                }
            }
            toProcess = nextToProcess;
        }
        int numStartingMovesWhereWeWin = 0;
        for (const auto startValue : valuesFromRemoving1Digit)
        {
            if (canMake0ViaAppendsStartingWith[startValue])
            {
                numStartingMovesWhereWeWin += numTimesCanMakeValueByRemoving1Digit[startValue];
            }
        }
        cout << numStartingMovesWhereWeWin << endl; 
    }
}
