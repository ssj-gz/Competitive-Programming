#include <iostream>
#include <cassert>

using namespace std;

int main()
{
    int64_t calibrationValsSum = 0;
    string calibrationLine;
    const std::pair<string, int> digitStringsAndValues[] = 
    {
        {"0", 0},
        {"1", 1},
        {"2", 2},
        {"3", 3},
        {"4", 4},
        {"5", 5},
        {"6", 6},
        {"7", 7},
        {"8", 8},
        {"9", 9},
        {"zero", 0},
        {"one", 1},
        {"two", 2},
        {"three", 3},
        {"four", 4},
        {"five", 5},
        {"six", 6},
        {"seven", 7},
        {"eight", 8},
        {"nine", 9},
    };
    while (getline(cin, calibrationLine))
    {
        std::cout << "calibrationLine: " << calibrationLine << std::endl;
        int firstDigitPos = calibrationLine.size();
        int firstDigitValue = -1;

        for (const auto& [digitString, value] : digitStringsAndValues)
        {
            const int firstPosOfString = calibrationLine.find(digitString);
            std::cout << " string: " << digitString << " pos: " << firstPosOfString << std::endl;
            if (firstPosOfString != std::string::npos && firstPosOfString < firstDigitPos)
            {
                firstDigitPos = firstPosOfString;
                firstDigitValue = value;
            }
        }
        assert(firstDigitPos != -1);

        int lastDigitPos = -1;
        int lastDigitValue = -1;

        for (const auto& [digitString, value] : digitStringsAndValues)
        {
            const int lastPosOfString = calibrationLine.rfind(digitString);
            if (lastPosOfString != std::string::npos && lastPosOfString > lastDigitPos)
            {
                lastDigitPos = lastPosOfString;
                lastDigitValue = value;
            }
        }
        assert(lastDigitPos != -1);

        calibrationValsSum += firstDigitValue * 10 + lastDigitValue;
    }

    std::cout << calibrationValsSum << std::endl;
}
