// Simon St James (ssjgz) - 2019-07-21
#define SUBMISSION
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <utility>

#include <cassert>
#include <sys/time.h>

using namespace std;

enum Status { Unknown, Yes, No };

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));


        const int maxSumOfStringLengths = 1'000'000;
        const int maxMod = 1000;
        vector<string> strings;
        int sumOfStringLengths = 0;
        const int T = 100;
        for (int t = 0; t < T; t++)
        {
            //const int stringLength = (rand() % (maxSumOfStringLengths - sumOfStringLengths) + 1);
            int stringLength = (rand() % (30) + 2);
            //int stringLength = (rand() % (10'000) + 2);
            vector<int> digits = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

            if (t == 100)
            {
                assert(sumOfStringLengths < maxSumOfStringLengths);
                stringLength = maxSumOfStringLengths - sumOfStringLengths;
            }

            random_shuffle(digits.begin(), digits.end());
            const int numDistinctDigitsToUse = (rand() % 10) + 1;

            string numberString;
            for (int i = 0; i < stringLength; i++)
            {
                numberString.push_back('0' + rand() % numDistinctDigitsToUse);
            }
            strings.push_back(numberString);
            sumOfStringLengths += stringLength;
        }

        if (strings.size() > 10000)
            strings.resize(10000);

        cout << strings.size() << endl;
        for (int i = 0; i < strings.size(); i++)
        {
            const int mod = 800 + rand() % (maxMod - 800) + 1;
            assert(mod <= maxMod);
            cout << strings[i] << " " << mod << endl;
        }
        return 0;
    }
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {

        //cout << " t: " << t << " / " << T << endl;
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

        vector<Status> canAppendAndMake0StartingWith(modulus, Unknown);
        canAppendAndMake0StartingWith[0] = Yes;

        vector<vector<int>> canBeReachedByAppendFrom(modulus);
        for (int startValue = 0; startValue < modulus; startValue++)
        {
            for (const auto otherValue : valuesFromRemoving1Digit)
            {
                const int newValue = (startValue * powerOf10ForAppending + otherValue) % modulus;
                canBeReachedByAppendFrom[newValue].push_back(startValue);
            }
        }

        vector<int> toProcess = { 0 };
        while (!toProcess.empty())
        {
            vector<int> nextToProcess;
            for (const auto p : toProcess)
            {
                canAppendAndMake0StartingWith[p] = Yes;

                for (const auto reachedFrom : canBeReachedByAppendFrom[p])
                {
                    if (canAppendAndMake0StartingWith[reachedFrom] == Unknown)
                    {
                        canAppendAndMake0StartingWith[reachedFrom] = Yes;
                        nextToProcess.push_back(reachedFrom);
                    }
                }
            }
            toProcess = nextToProcess;
        }
        for (const auto startValue : valuesFromRemoving1Digit)
        {
            //cout << " startValue: " << startValue << endl;
            if (canAppendAndMake0StartingWith[startValue] == Yes)
            {
                //cout << " yes" << endl;
                numStartingMoves += numTimesCanMakeValueByRemoving1Digit[startValue];
            }
            else
            {
                //cout << " no" << endl;
            }
        }
        cout << numStartingMoves << endl; 
    }
}
