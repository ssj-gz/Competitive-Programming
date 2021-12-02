#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

int main(int argc, char* argv[])
{
    int preambleSize = 25;
    if (argc == 2)
    {
        preambleSize = stoi(argv[1]);
    }
    cout << "Using preamble of size " << preambleSize << endl;

    vector<int64_t> numbers;
    int number;
    while (cin >> number)
    {
        numbers.push_back(number);
    }

    int64_t firstInvalidNumber = -1;
    for (int numberIndex = preambleSize; numberIndex < numbers.size(); numberIndex++)
    {
        const int64_t number = numbers[numberIndex];
        bool isSumOfPrevious = false;
        for (int i = numberIndex - preambleSize; i < numberIndex; i++)
        {
            for (int j = i + 1; j < numberIndex; j++)
            {
                if (numbers[i] + numbers[j] == number)
                {
                    isSumOfPrevious = true;
                    break;
                }
            }
        }
        if (!isSumOfPrevious)
        {
            firstInvalidNumber = number;
            break;
        }
    }
    assert(firstInvalidNumber != -1);
    bool foundContiguousRangeSummingToInvalid = false;
    for (int rangeBeginIndex = 0; rangeBeginIndex < numbers.size(); rangeBeginIndex++)
    {
        for (int rangeEndIndex = rangeBeginIndex + 1; rangeEndIndex < numbers.size(); rangeEndIndex++)
        {
            int64_t sumOfRange = 0;
            for (int i = rangeBeginIndex; i <= rangeEndIndex; i++)
            {
                sumOfRange += numbers[i];
            }
            if (sumOfRange == firstInvalidNumber)
            {
                const auto minInRange = *min_element(numbers.begin() + rangeBeginIndex, numbers.begin() + rangeEndIndex + 1);
                const auto maxInRange = *max_element(numbers.begin() + rangeBeginIndex, numbers.begin() + rangeEndIndex + 1);
                cout << "rangeBeginIndex: " << rangeBeginIndex << " rangeEndIndex: " << rangeEndIndex << endl;
                cout << minInRange + maxInRange << endl;
                assert(!foundContiguousRangeSummingToInvalid);
                foundContiguousRangeSummingToInvalid = true;
            }
        }
    }
    assert(foundContiguousRangeSummingToInvalid);

}
