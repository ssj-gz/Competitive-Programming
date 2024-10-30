#include <iostream>

#include <cassert>

using namespace std;

int main()
{
    string digitStr;
    cin >> digitStr;
    int64_t matchingDigitSum = 0;
    for (string::size_type pos = 0; pos < digitStr.size(); pos++)
    {
        const int digit = digitStr[pos] - '0';
        const int nextDigit = digitStr[(pos + 1) % digitStr.size()] - '0';
        if (digit == nextDigit)
            matchingDigitSum += digit;
    }
    std::cout << "matchingDigitSum: " << matchingDigitSum << std::endl;
    return 0;
}
