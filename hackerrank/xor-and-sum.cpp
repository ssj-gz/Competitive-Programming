// Simon St James (ssjgz) 2017-10-02
#include <iostream>
#include <vector>

using namespace std;

int binaryToInt(const string& binaryString)
{
    string binaryStringLocal = binaryString;

    int integerValue = 0;
    while (!binaryStringLocal.empty())
    {
        integerValue *= 2;
        if (binaryStringLocal.front() == '1')
        {
            integerValue++;
        }
        binaryStringLocal.erase(binaryStringLocal.begin());
    }
    return integerValue;
}

int main()
{

    string a, b;
    cin >> a >> b;

    const int64_t numSums = 314159 + 1;
    //const int numSums = 6;

#if 1
    int64_t dbgSum = 0;
    const int64_t aValue = binaryToInt(a);
    const int64_t bValue = binaryToInt(b);
    //cout << "aValue: " << aValue << " bValue: " << bValue << endl;

    for (int64_t i = 0; i < numSums; i++)
    {
        dbgSum += aValue ^ (bValue << i);
    }
#endif
    int64_t sum = 0;

    const string aReversed(a.rbegin(), a.rend());
    const string bReversed(b.rbegin(), b.rend());

    const int64_t modulus = 1'000'000'007UL;

    int64_t powerOf2 = 1;
    int num1DigitsOfB = 0;

    auto digit = [](const string& binaryString, int digitNum)
    {
        if (digitNum < binaryString.size())
            return binaryString[digitNum] - '0';
        else
            return 0;
    };
    for (int digitNum = 0; digitNum < max(a.size(), b.size()) + numSums; digitNum++)
    {
        //cout << "digitNum: " << digitNum << endl;
        const int digitA = digit(aReversed, digitNum);
        const int digitB = digit(bReversed, digitNum);

        if (digitB == 1)
        {
            //cout << "bumped num1DigitsOfB" << endl;
            num1DigitsOfB++;
        }
        //else
            //num0DigitsOfB++;
        if (digitNum - numSums >= 0)
        {
            if (digit(bReversed, digitNum - numSums) == 1)
            {
                //cout << "decremented num1DigitsOfB" << endl;
                num1DigitsOfB--;
            }
                
        }
        //cout << "digitNum: " << digitNum << " num1DigitsOfB: " << num1DigitsOfB << endl;

        if (digitA == 1)
            sum = (sum + (powerOf2 * (numSums - num1DigitsOfB))) % modulus;
        else
            sum = (sum + (powerOf2 * num1DigitsOfB)) % modulus;

        //cout << "digitNum: " << digitNum << " digitA: " << digitA << " sum: " << sum << endl;

        powerOf2 = (2 * powerOf2) % modulus;


    }

    //cout << "dbgSum: " << dbgSum << endl;
    //cout << "sum: " << sum << endl;
    cout << sum << endl;
}
