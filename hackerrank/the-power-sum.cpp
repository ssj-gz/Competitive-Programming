// Simon St James (ssjgz) 2017-09-15 18:01
#include <iostream>

using namespace std;

int numWaysOfExpressingAsPowers(int x, int baseBegin, int exponent)
{
    int numWays = 0;
    int base = baseBegin;
    while(true)
    {
        int baseToExponent = 1;
        for (int i = 1; i <= exponent; i++)
        {
            baseToExponent *= base;
        }
        if (baseToExponent > x)
        {
            // Higher values of base will also exceed x; no point exploring them.
            break;
        }
        else if (baseToExponent == x)
        {
            numWays++;
            break;
        }

        numWays += numWaysOfExpressingAsPowers(x - baseToExponent, base + 1, exponent);

        base++;
    }
    return numWays;
}

int main()
{
    int x, n;
    cin >> x >> n;
    cout << numWaysOfExpressingAsPowers(x, 1, n) << endl;
}
