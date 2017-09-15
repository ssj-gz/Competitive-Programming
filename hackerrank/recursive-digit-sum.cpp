// Simon St James (ssjgz) 2017-09-15 18:20
#include <iostream>

using namespace std;

int64_t digitSum(const string& digits)
{
    int64_t sum = 0;
    for (const auto letter : digits)
    {
        sum += (letter - '0');
    }
    return sum;
}

int main()
{
    string n;
    cin >> n;
    const int64_t nDigitSum = digitSum(n);
    int k;
    cin >> k;
    const int64_t pDigitSum = k * nDigitSum;

    int64_t superDigitSum = pDigitSum;
    while (true)
    {
        superDigitSum = digitSum(to_string(superDigitSum));
        if (superDigitSum <= 9)
            break;
    }
    cout << superDigitSum << endl;
}

