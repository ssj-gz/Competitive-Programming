#include <iostream>
#include <vector>
#include <map>
#include <cassert>

#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.

using namespace std;

int64_t snafuToDecimal(const string& snafuOrig)
{
    string snafu = snafuOrig;
    int64_t decimal = 0;
    int64_t multiplier = 1;
    while (!snafu.empty())
    {
        int lastDigitValue = -1;
        switch (snafu.back())
        {
            case '0':
                lastDigitValue = 0;
                break;
            case '1':
                lastDigitValue = 1;
                break;
            case '2':
                lastDigitValue = 2;
                break;
            case '-':
                lastDigitValue = -1;
                break;
            case '=':
                lastDigitValue = -2;
                break;
            default:
                assert(false);
        }
        decimal += multiplier * lastDigitValue;
        multiplier *= 5;
        snafu.erase(std::prev(snafu.end()));

    }
    return decimal;
}

string decimalToSnafu(int64_t decimal)
{
    assert(decimal >= 0);
    const int64_t origDecimal = decimal;
    std::cout << "decimalToSnafu: " << decimal << std::endl;
    if (decimal == 0)
        return "0";
    string asBase5;
    int64_t powerOf5 = 1;
    while (decimal > 0)
    {
        asBase5 = static_cast<char>('0' + (decimal % 5)) + asBase5;
        decimal /= 5;
        powerOf5 *= 5;
    }
    std::cout << "asBase5: " << asBase5 << std::endl;

    if (asBase5[0] <= '2')
    {
        std::cout << "origDecimal % (powerOf5/ 5): " << (origDecimal% (powerOf5 / 5)) << std::endl;
        std::cout << "powerOf5: " << powerOf5 << std::endl;
        if (powerOf5 >= 25)
        {
            const auto restOfResult = decimalToSnafu(origDecimal % (powerOf5 / 5));
            string result = std::string() + asBase5[0];
            std::cout << "Blee: " << asBase5[0] << " result so far: " << result << std::endl;
            while (result.size() + restOfResult.size() < asBase5.size())
                result += "0";
            result += restOfResult;
            return result;
        }
        else
        {
            std::cout << "Just returning asBase5" << std::endl;
            return asBase5;
        }
    }
    else if (asBase5[0] == '3')
    {
        string result = "1=";
        if (powerOf5 >= 25)
        {
            int64_t restOfDigitsDecimal = origDecimal% (powerOf5 / 5);
            std::cout << "restOfDigitsDecimal original: " << restOfDigitsDecimal << std::endl;
            restOfDigitsDecimal += 2 * powerOf5 / 5;
            restOfDigitsDecimal -= powerOf5;
            std::cout << "restOfDigitsDecimal final: " << restOfDigitsDecimal << std::endl;

            const auto restOfResult = decimalToSnafu(restOfDigitsDecimal);
            std::cout << "Case 3: " << asBase5[0] << " result so far: " << result << std::endl;
            while (result.size() + restOfResult.size() < asBase5.size() + 1)
                result += "0";
            result += restOfResult;
            return result;
        }
        else
        {
            std::cout << "Just returning 1=" << std::endl;
            return result;
        }
    }
    else
    {
        std::cout << "unhandled" << std::endl;
    }
    return "";
}

string decimalToSnafu2(int64_t decimal)
{
    std::cout << "decimalToSnafu2: " << decimal << std::endl; 
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
    for (int i = 1; i < maxDecimalWithSnafuDigits.size(); i++)
    {
        std::cout << "maxDecimalWithSnafuDigits[" << i << "] = " << maxDecimalWithSnafuDigits[i] << std::endl;
        std::cout << "minDecimalWithSnafuDigits[" << i << "] = " << minDecimalWithSnafuDigits[i] << std::endl;
    }
    const int numDigits = maxDecimalWithSnafuDigits.size() - 1;
    int64_t powerOf5 = 1;
    for (int i = 0; i < numDigits - 1; i++)
        powerOf5 *= 5;
    std::cout << "numDigits: " << numDigits << " powerOf5: " << powerOf5 << std::endl;

    for (char firstDigit : {'0', '1', '2', '-', '='})
    {
        int digitValue = 0;
        switch (firstDigit)
        {
            case '0':
                digitValue = 0;
                break;
            case '1':
                digitValue = 1;
                break;
            case '2':
                digitValue = 2;
                break;
            case '-':
                digitValue = -1;
                break;
            case '=':
                digitValue = -2;
                break;
            default:
                assert(false);
        }

        const int64_t remainder = decimal - digitValue * powerOf5;
        std::cout << "firstDigit: " << firstDigit << " remainder: " << remainder << std::endl;
        if (remainder < minDecimalWithSnafuDigits[numDigits - 1])
        {
            std::cout << " too negative; invalid " << std::endl;
        }
        else if (remainder > maxDecimalWithSnafuDigits[numDigits - 1])
        {
            std::cout << " can't get high enough with remaining digits; invalid " << std::endl;
        }
        else
        {
            std::cout << " this will do!" << std::endl;
            const string remainderSnafu = decimalToSnafu2(remainder);
            string result = string() + firstDigit;
            while (result.size() + remainderSnafu.size() < numDigits)
            {
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
        std::cout << "snafu: " << snafu << " decimal: " << asDecimal << std::endl;
        sumOfSnafus += asDecimal;
    }

    std::map<int, string> snafuForDecimal;
    string snafu = "0";
    for (int i = 0; i < 100'000; i++)
    {
        std::cout << "snafu: " << snafu << " decimal: " << snafuToDecimal(snafu) << std::endl;
        snafuForDecimal[snafuToDecimal(snafu)] = snafu;
        int pos = snafu.size() - 1;
        while (snafu[pos] == '=' && pos >= 0)
        {
            snafu[pos] = '0';
            pos--;
        }
        if (pos == -1)
        {
            snafu = "0" + snafu;
            pos = 0;
        }
        switch (snafu[pos])
        {
            case '0':
                snafu[pos] = '1';
                break;
            case '1':
                snafu[pos] = '2';
                break;
            case '2':
                snafu[pos] = '-';
                break;
            case '-':
                snafu[pos] = '=';
                break;
            default:
                assert(false);
        }
    }

    for (const auto& [decimal, snafu] : snafuForDecimal)
    {
        //if (decimal < 0)
            //continue;

        std::cout << "decimal: " << decimal << " snafu: " << snafu << std::endl;
    }

    std::cout << "Bloo!" << std::endl;
#if 0
    for (int decimal = 0; decimal < 100'000; decimal++)
    {
        //const auto asSnafu = decimalToSnafu(decimal);
        //std::cout << "Blah: decimal: " << decimal << " = as snafu: " << asSnafu << " should be: " << snafuForDecimal[decimal] << std::endl;

        const auto asSnafu2 = decimalToSnafu2(decimal);
        std::cout << "Blah: decimal: " << decimal << " = as snafu2: " << asSnafu2 << " should be: " << snafuForDecimal[decimal] << std::endl;
        assert(snafuToDecimal(asSnafu2) == decimal);

    }
#endif
    const auto sumSnafu = decimalToSnafu2(sumOfSnafus);
    std::cout << "sum: " << sumOfSnafus << std::endl;
    std::cout << "sumSnafu: " << sumSnafu << std::endl;
    std::cout << "Check: " << snafuToDecimal(sumSnafu) << std::endl;
    assert(snafuToDecimal(sumSnafu) == sumOfSnafus);
}
