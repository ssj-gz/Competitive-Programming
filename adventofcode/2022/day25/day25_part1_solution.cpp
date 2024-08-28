#include <iostream>
#include <vector>
#include <map>
#include <cassert>

#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.

using namespace std;

int snafuDigitValue(const char snafuDigit)
{
    switch (snafuDigit)
    {
        case '0':
            return 0;
        case '1':
            return 1;
        case '2':
            return 2;
        case '-':
            return -1;
        case '=':
            return -2;
    }
    assert(false);
    return -1000;
}

int64_t snafuToDecimal(const string& snafuOrig)
{
    string snafu = snafuOrig;
    int64_t decimal = 0;
    int64_t multiplier = 1;
    while (!snafu.empty())
    {
        const auto lastDigitValue = snafuDigitValue(snafu.back());

        decimal += multiplier * lastDigitValue;
        multiplier *= 5;
        snafu.erase(std::prev(snafu.end()));

    }
    return decimal;
}

string decimalToSnafu(int64_t decimal)
{
    switch (decimal)
    {
        case 0:
            return "0";
        case 1:
            return "1";
        case 2:
            return "2";
        case -1:
            return "-";
        case -2:
            return "=";
    }
    // The recurrence relations for maxDecimalWithSnafuDigits/ minDecimalWithSnafuDigits
    // are easily proven by induction.
    vector<int64_t> maxDecimalWithSnafuDigits = {0, 2};
    vector<int64_t> minDecimalWithSnafuDigits = {0, -2};
    int64_t firstWithNumDigits = 3;
    int64_t gap = 10;
    while (maxDecimalWithSnafuDigits.back() < abs(decimal))
    {
        maxDecimalWithSnafuDigits.push_back(firstWithNumDigits + gap - 1);
        minDecimalWithSnafuDigits.push_back(-maxDecimalWithSnafuDigits.back());
        firstWithNumDigits += gap;
        gap *= 5;
    }

    const int numDigits = maxDecimalWithSnafuDigits.size() - 1;
    int64_t powerOf5 = 1;
    for (int i = 0; i < numDigits - 1; i++)
        powerOf5 *= 5;

    for (char firstDigit : {'0', '1', '2', '-', '='})
    {
        // Find the first digit by trial-and-error.
        const int digitValue = snafuDigitValue(firstDigit);

        const int64_t remainder = decimal - digitValue * powerOf5;
        if (remainder >= minDecimalWithSnafuDigits[numDigits - 1] && remainder <= maxDecimalWithSnafuDigits[numDigits - 1])
        {
            const string remainderSnafu = decimalToSnafu(remainder);
            string result = string() + firstDigit;
            while (result.size() + remainderSnafu.size() < numDigits)
            {
                // Pad final result to the required number of digits (in case remainderSnafu
                // is less than (numDigits - 1) digits long).
                result += "0";
            }
            result += remainderSnafu;
            return result;
        }
    }
    assert(false);
    return "";
}

int main()
{
    vector<string> snafuNums;
    string snafuNum;
    while (getline(cin, snafuNum))
    {
        snafuNums.push_back(snafuNum);
    }

    int64_t sumOfSnafus = 0;
    for (const auto& snafu : snafuNums)
    {
        const auto asDecimal = snafuToDecimal(snafu);
        sumOfSnafus += asDecimal;
    }

    const auto sumSnafu = decimalToSnafu(sumOfSnafus);
    std::cout << "sum: " << sumOfSnafus << std::endl;
    std::cout << "sumSnafu: " << sumSnafu << std::endl;
    std::cout << "Check: " << snafuToDecimal(sumSnafu) << std::endl;
    assert(snafuToDecimal(sumSnafu) == sumOfSnafus);
}
