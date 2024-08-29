#include <iostream>

using namespace std;

int main()
{
    int64_t calibrationValsSum = 0;
    string calibrationLine;
    while (getline(cin, calibrationLine))
    {
        const auto firstDigitChar = calibrationLine[calibrationLine.find_first_of("0123456789")];
        const auto lastDigitChar = calibrationLine[calibrationLine.find_last_of("0123456789")];
        std::cout << "calibrationLine: " << calibrationLine << " firstDigitChar: " << firstDigitChar << " lastDigitChar: " << lastDigitChar << std::endl;
        calibrationValsSum += (firstDigitChar - '0') * 10 + (lastDigitChar - '0');
    }

    std::cout << calibrationValsSum << std::endl;
}
