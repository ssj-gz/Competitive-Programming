// Simon St James (ssjgz) - 2019-07-21
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>

#include <cassert>

using namespace std;

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

        // Build up firstDigitsOfAMod/ lastDigitsOfAMod.
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

        powerOf10 = 1;
        vector<int> numTimesCanMakeValueByRemoving1Digit(modulus, 0);
        for (int digitIndexToRemove = A.size() - 1; digitIndexToRemove >= 0; digitIndexToRemove--)
        {
            const int aMinusDigitMod = (firstDigitsOfAMod[digitIndexToRemove] * powerOf10 + lastDigitsOfAMod[A.size() - digitIndexToRemove - 1]) % modulus;
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

        int powerOf10ForAppending = 1;
        for (int i = 0; i < A.size() - 1; i++)
        {
            powerOf10ForAppending = (powerOf10ForAppending * 10) % modulus;
        }

        vector<vector<int>> canBeReachedByAppendFrom(modulus);
        for (int startValue = 0; startValue < modulus; startValue++)
        {
            for (const auto otherValue : valuesFromRemoving1Digit)
            {
                // Appending to startValue involves multiplying it my 10^|A.size() - 1|, which is
                // powerOf10ForAppending.
                const int newValue = (startValue * powerOf10ForAppending + otherValue) % modulus;
                canBeReachedByAppendFrom[newValue].push_back(startValue);
            }
        }

        // Work backwards from 0 and use canBeReachedByAppendFrom to find all numbers that
        // can be made to reach 0 (mod modulus!) by one or more appends/ "Moves".
        vector<bool> canMake0ViaAppendsStartingWith(modulus, false);
        canMake0ViaAppendsStartingWith[0] = true;
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
        // Compute final result.
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
