#include <iostream>
#include <vector>
#include <sstream>
#include <limits>

#include <cassert>

using namespace std;

int main()
{
    string rowNumbersString;
    int64_t checkSum = 0;
    while (getline(cin, rowNumbersString))
    {
        vector<int64_t> numbers;
        istringstream rowNumbersStream(rowNumbersString);
        int64_t number;
        while (rowNumbersStream >> number)
        {
            numbers.push_back(number);
        }

        for (const auto& firstNumber : numbers)
        {
        for (const auto& secondNumber : numbers)
        {
            if (&firstNumber == &secondNumber)
                continue;
            if (firstNumber % secondNumber == 0)
                checkSum += (firstNumber / secondNumber);
        }
        }

    }
    std::cout << "checkSum: " << checkSum << std::endl;
    return 0;
}
