// Simon St James (ssjgz) - 2019-08-05
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

const int64_t Mod = 1'000'000'007ULL;

class ModNum
{
    public:
        ModNum(int64_t n = 0)
            : m_n{n}
        {
            assert(n >= 0);
        }
        ModNum& operator+=(const ModNum& other)
        {
            m_n = (m_n + other.m_n) % Mod;
            return *this;
        }
        ModNum& operator-=(const ModNum& other)
        {
            m_n = (Mod + m_n - other.m_n) % Mod;
            return *this;
        }
        ModNum& operator*=(const ModNum& other)
        {
            m_n = (m_n * other.m_n) % Mod;
            return *this;
        }
        int64_t value() const { return m_n; };
    private:
        int64_t m_n;
};

ModNum operator+(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result += rhs;
    return result;
}

ModNum operator-(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result -= rhs;
    return result;
}

ModNum operator*(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result *= rhs;
    return result;
}

ostream& operator<<(ostream& os, const ModNum& toPrint)
{
    os << toPrint.value();
    return os;
}

const int maxNumberLength = 100'000;
// Lookup arrays, populated by computeMainLookupTables().
//
//   - sumOfFForNumDigitsBeginningWith[l][d] = sum [ number such that len(number) == l and number begins with d ] { f(number) }.
//   - tenToThePowerOf - just as you'd expect ;)
vector<vector<ModNum>> sumOfFForNumDigitsBeginningWith(maxNumberLength + 1, vector<ModNum>(10));
vector<ModNum> tenToThePowerOf(maxNumberLength + 1);

ModNum calcF(const string& number)
{
    const int numDigits = number.size();
    ModNum result = 0;

    int previousDigitInNumber = -1;
    for (int index = 0; index < numDigits; index++)
    {
        const int digitInNumber = number[index] - '0';

        if (digitInNumber != previousDigitInNumber)
        {
            result += digitInNumber *  tenToThePowerOf[numDigits - index - 1];
        }

        previousDigitInNumber = digitInNumber;
    }

    return result;
}

void computeMainLookupTables()
{
    // Compute sumOfFForNumDigitsBeginningWith.
    for (int digit = 0; digit <= 9; digit++)
    {
        sumOfFForNumDigitsBeginningWith[0][digit] = 0;
        sumOfFForNumDigitsBeginningWith[1][digit] = digit;
    }

    ModNum prevPowerOf10 = 1;
    ModNum powerOf10 = 10;
    for (int numberLength = 2; numberLength <= maxNumberLength; numberLength++)
    {
        for (int newFrontDigit = 0; newFrontDigit <= 9; newFrontDigit++)
        {
            ModNum& sumForLenBeginningWithThisDigit = sumOfFForNumDigitsBeginningWith[numberLength][newFrontDigit];
            ModNum numOccurrencesWithPrevFrontDigit = prevPowerOf10;
            for (int prevFrontDigit = 0; prevFrontDigit <= 9; prevFrontDigit++)
            {
                sumForLenBeginningWithThisDigit += newFrontDigit * powerOf10 * numOccurrencesWithPrevFrontDigit + sumOfFForNumDigitsBeginningWith[numberLength - 1][prevFrontDigit];
                if (prevFrontDigit == newFrontDigit)
                {
                    // Remove the contributions from numbers beginning with prevFrontDigit in the numbers of length (numberLength - 1) -
                    // they are wrong, as we have prepended another copy of prevFrontDigit to the front of the number.
                    ModNum valueOfThisDigit = prevFrontDigit * prevPowerOf10;
                    sumForLenBeginningWithThisDigit -= valueOfThisDigit * numOccurrencesWithPrevFrontDigit;
                }
            }

        }

        prevPowerOf10 = powerOf10;
        powerOf10 *= 10;
    }

    // Compute tenToThePowerOf.
    powerOf10 = 1;
    for (int i = 0; i <= maxNumberLength; i++)
    {
        tenToThePowerOf[i] = powerOf10;
        powerOf10 = powerOf10 * 10;
    }

}


ModNum sumOfFUpTo(const string& number)
{
    const int numDigits = number.size();
    ModNum result = 0;

    ModNum sumToLeft = 0;

    int previousDigitInNumber = -1;
    for (int index = 0; index < numDigits; index++)
    {
        assert(numDigits - index - 1 >= 0);

        const int digitInNumber = number[index] - '0';
        for (int digit = 0; digit < digitInNumber; digit++)
        {
            // Contribution from this index and rightwards having this digit.
            result += sumOfFForNumDigitsBeginningWith[numDigits - index][digit];

            // Contribution from previous indices from having this digit.
            const ModNum numOccurencesWithThisDigit = tenToThePowerOf[numDigits - index - 1];
            result += sumToLeft * numOccurencesWithThisDigit;

            if (digit == previousDigitInNumber)
            {
                // The contribution to the result from having this digit in this position
                // is wrong as this digit is equal to the previous digit; remove the contribution.
                // (Similar logic as in computeMainLookupTables()).
                const ModNum valueOfThisDigit = digit * tenToThePowerOf[numDigits - index - 1];
                const ModNum correctionForThisDigit = valueOfThisDigit * numOccurencesWithThisDigit;
                result -= correctionForThisDigit;
            }
        }
        if (digitInNumber != previousDigitInNumber)
        {
            // Update sumToLeft.
            sumToLeft += digitInNumber * tenToThePowerOf[numDigits - index - 1];
        }

        previousDigitInNumber = digitInNumber;
    }

    result += sumToLeft;

    return result;
}

ModNum calcSumOfFInRange(const string& L, const string& R)
{
    ModNum result =  sumOfFUpTo(R) - sumOfFUpTo(L) + calcF(L);

    return result;
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    computeMainLookupTables();

    const int T = read<int>();
    for (int t = 0; t < T; t++)
    {
        read<int>();
        const string L = read<string>();
        read<int>();
        const string R = read<string>();

        const auto sumOfFInRange = calcSumOfFInRange(L, R);
        cout << sumOfFInRange << endl;
    }

}


