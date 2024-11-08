#include <iostream>

#include <cassert>

using namespace std;

int main()
{
    string digits;
    cin >> digits;

    for (int i = 1; i <= 50; i++)
    {
        string nextDigits;
        int prevDigit = -1;
        int numInRun = 0;

        auto flushRun = [&prevDigit, &numInRun, &nextDigits]()
        {
            if(prevDigit != -1)
            {
                nextDigits += std::to_string(numInRun);
                nextDigits.push_back(prevDigit + '0');
            }
        };

        for (const auto digitChar : digits)
        {
            const int digit = digitChar - '0';
            if (digit != prevDigit)
            {
                flushRun();
                prevDigit = digit;
                numInRun = 1;
            }
            else
            {
                numInRun++;
            }
        }
        flushRun();

        digits = nextDigits;
        std::cout << "After iteration " << i << " digits: " << nextDigits << std::endl;
    }
    std::cout << "result: " << digits.size() << std::endl;
    return 0;
}
