#include <iostream>
#include <vector>

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
            cout << number << endl;
            break;
        }
    }

}
