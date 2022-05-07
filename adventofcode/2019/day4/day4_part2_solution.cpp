#include <iostream>
#include <cassert>

using namespace std;

int main()
{
    int lowerRange;
    cin >> lowerRange;
    char dash;
    cin >> dash;
    assert(dash == '-');
    int upperRange;
    cin >> upperRange;

    assert(cin);

    int numValidPasswordsInRange = 0;
    for (int value = lowerRange; value <= upperRange; value++)
    {
        const string valueAsString = to_string(value);
        bool hasValidDoubleDigit = false;
        bool hasDecreasingDigits = false;
        for (string::size_type pos = 1; pos < valueAsString.size(); pos++)
        {
            if (valueAsString[pos] == valueAsString[pos - 1])
            {
                const bool digitBeforePairIsSameAsPair = (pos >= 2 && valueAsString[pos - 2] == valueAsString[pos]);
                const bool digitAfterPairIsSameAsPair = (pos + 1 < valueAsString.size() && valueAsString[pos + 1] == valueAsString[pos]);
                if (!digitBeforePairIsSameAsPair && !digitAfterPairIsSameAsPair)
                {
                    hasValidDoubleDigit = true;
                }
            }
            else if (valueAsString[pos] < valueAsString[pos - 1])
            {
                hasDecreasingDigits = true;
            }
        }
        if (hasValidDoubleDigit && !hasDecreasingDigits)
        {
            numValidPasswordsInRange++;
        }
    }
    cout << numValidPasswordsInRange << endl;
}
