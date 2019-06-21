#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    vector<char> digits = { '0' };
    map<int, vector<int>> decimalsForDeciBinary;

    int decimalValue = 0;
    while (decimalValue < 100'000'000)
    {
        int digitIndex = digits.size() - 1;
        while (digitIndex >= 0 && digits[digitIndex] == '9')
        {
            digits[digitIndex] = '0';
            digitIndex--;
        }
        if (digitIndex == -1)
        {
            digits.insert(digits.begin(), '1');
        }
        else
        {
            digits[digitIndex]++;
        }
        decimalValue++;

        int decimalValueFromDecibinary = 0;
        int powerOf2 = 1;

        for (auto digitReverseIter = digits.rbegin(); digitReverseIter != digits.rend(); digitReverseIter++)
        {
            decimalValueFromDecibinary += (*digitReverseIter - '0') * powerOf2;
            powerOf2 *= 2;
        }
        decimalsForDeciBinary[decimalValueFromDecibinary].push_back(decimalValue);
    }
    for (auto& blah : decimalsForDeciBinary)
    {
        cout << blah.first << " (" << blah.second.size() << ")" << endl;
        for (const auto decimal : blah.second)
        {
            cout << blah.first << " " << decimal << endl;
            //break;
        }
    }
}



