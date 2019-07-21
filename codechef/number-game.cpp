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
            //const int stringLength = (rand() % (7) + 2);
            int stringLength = (rand() % (10'000) + 2);
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
        vector<int> digitsInA;
        for (int digit = 0; digit <= 9; digit++)
        {
            if (numOfDigitInA[digit] > 0)
            {
                digitsInA.push_back(digit);
            }
        }

        enum Status { Unknown, Yes, No };
        vector<Status> canMakeZeroWithStartingValue(modulus, Unknown);

        powerOf10 = 1;
        vector<int> numTimesCanMakeValueByRemoving1Digit(modulus, 0);
        for (int i = A.size() - 1; i >= 0; i--)
        {

            const int aMinusDigitMod = (firstDigitsOfAMod[i] * powerOf10 + lastDigitsOfAMod[A.size() - i - 1]) % modulus;
            //cout << " i: " << i << " aMinusDigitMod: " << aMinusDigitMod << endl;
            numTimesCanMakeValueByRemoving1Digit[aMinusDigitMod]++;
#ifndef SUBMISSION
            {
                string aMinusDigit = A.substr(0, i) + A.substr(i + 1);
                cout << "aMinusDigit: " << aMinusDigit << endl;
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
            powerOf10 = (powerOf10 * 10) % modulus;
        }
        vector<int> valuesFromRemoving1Digit;
        for (int i = 0; i < modulus; i++)
        {
            if (numTimesCanMakeValueByRemoving1Digit[i] > 0)
            {
                //cout << "Blee; " << i << endl;
                valuesFromRemoving1Digit.push_back(i);
            }
        }

        vector<Status> canAppendAndMake0StartingWith(modulus, Unknown);
        canAppendAndMake0StartingWith[0] = Yes;

        for (const auto startValue : valuesFromRemoving1Digit)
        {
            //cout << " startValue: " << startValue << endl;
            vector<bool> processed(modulus, false);
            vector<bool> visited(modulus, false);

            vector<int> toProcess = { startValue };
            int iterationNum = 0;
            bool found = false;
            while (!toProcess.empty() && !found)
            {
                //cout << " # toProcess: " << toProcess.size() << endl;
                vector<int> nextToProcess;
                for (const auto p : toProcess)
                {
                    //cout << " p: " << p << endl;
                    if (canAppendAndMake0StartingWith[p] == Yes)
                    {
                        found = true;
                        break;

                    }
                    if (canAppendAndMake0StartingWith[p] == Unknown)
                    {
                        const int pTimes10 = (p * powerOf10) % modulus;
                        processed[p] = true;

                        for (const auto other : valuesFromRemoving1Digit)
                        {
                            const int newValue = (pTimes10 + other) % modulus;
                            if (!visited[newValue])
                            {
                                nextToProcess.push_back(newValue);
                                visited[newValue] = true;
                            }
                        }
                    }
                }
                toProcess = nextToProcess;
                iterationNum++;
            }

            if (found)
            {
                canAppendAndMake0StartingWith[startValue] = Yes;
                //cout << " found; adding: " << numTimesCanMakeValueByRemoving1Digit[startValue] << endl;
                numStartingMoves += numTimesCanMakeValueByRemoving1Digit[startValue];
            }
            else
            {
                canAppendAndMake0StartingWith[startValue] = No;
            }

        }
        cout << numStartingMoves << endl;
    }




#if 0
        if (false)
        {
            //cout << "Blee: " << (firstDigitsOfAMod[i] * powerOf10) << " blah: " << firstDigitsOfAMod[i] << endl;
            const int removedDigitValue = (A[i] - '0');

            //numOfDigitInA[removedDigitValue]--;
            //numOfDigitInA[removedDigitValue]++;
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
                //cout << "i: " << i << " aMinusDigitMod: " << aMinusDigitMod << " dbgAMinusDigitMod: " << dbgAMinusDigitMod << endl;
                assert(aMinusDigitMod == dbgAMinusDigitMod);
            }
#endif
#if 0
                vector<int> dbgDigitsInAMinusDigit;

                for (int digit = 0; digit <= 9; digit++)
                {
                    if (aMinusDigit.find(digit + '0') != string::npos)
                    {
                        dbgDigitsInAMinusDigit.push_back(digit);
                    }
                }
#endif
#if 0
                cout << "digitsInA: " << endl;
                for (const auto x : digitsInA)
                {
                    cout << " " << x;
                }
                cout << endl;
                cout << "dbgDigitsInAMinusDigit: " << endl;
                for (const auto x : dbgDigitsInAMinusDigit)
                {
                    cout << " " << x;
                }
                cout << endl;
#endif
                //assert(dbgDigitsInAMinusDigit == digitsInA);
            }
            if (canMakeZeroWithStartingValue[aMinusDigitMod] == Unknown)
            {

                vector<bool> processed(modulus, false);
                vector<bool> visited(modulus, false);
                vector<int> toProcess = { aMinusDigitMod };

                int iterationNum = 0;
                while (!toProcess.empty())
                {
                    //cout << " # toProcess: " << toProcess.size() << endl;
                    vector<int> nextToProcess;
                    for (const auto p : toProcess)
                    {
                        const int pTimes10 = (p * 10) % modulus;
                        if (iterationNum > 0)
                            processed[p] = true;

                        for (const auto digit : digitsInA)
                        {
                            const int newValue = (pTimes10 + digit) % modulus;
                            if (!visited[newValue])
                            {
                                nextToProcess.push_back(newValue);
                                visited[newValue] = true;
                            }
                        }
                    }
                    toProcess = nextToProcess;
                    iterationNum++;
                }

                if (processed[0])
                {
                    assert((canMakeZeroWithStartingValue[aMinusDigitMod] != No));
                    canMakeZeroWithStartingValue[aMinusDigitMod] = Yes;
                }
                else
                {
                    assert((canMakeZeroWithStartingValue[aMinusDigitMod] != Yes));
                    canMakeZeroWithStartingValue[aMinusDigitMod] = No;
                }

            }
            if (canMakeZeroWithStartingValue[aMinusDigitMod] == Yes)
                numStartingMoves++;
            powerOf10 = (powerOf10 * 10) % modulus;

        }
        cout << numStartingMoves << endl;



    }
#endif
}
