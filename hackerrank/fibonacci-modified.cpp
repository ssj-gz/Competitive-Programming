#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <sstream>
#include <iomanip>

using namespace std;

namespace
{
    constexpr long calcPower(long base, long power)
    {
        long result = base;    
        for (long i = 0; i < power - 1; i++)
        {
            assert(std::numeric_limits<decltype(result)>::max() / result >= base);
            result *= base;
        }
        return result;
    }
}

class BigNum
{
    public:
        BigNum(const string& decimalDigits)
        {
            stringstream decimalParser;
            string::size_type decimalDigitsForBigDigitBegin = decimalDigits.size();
            const long numDecimalDigitsInBigDigit = power;
            string::size_type digitsToRead = numDecimalDigitsInBigDigit;
            while (decimalDigitsForBigDigitBegin != 0) 
            {
                if (decimalDigitsForBigDigitBegin >= numDecimalDigitsInBigDigit)
                {
                    decimalDigitsForBigDigitBegin -= numDecimalDigitsInBigDigit;
                }
                else
                {
                    digitsToRead = decimalDigitsForBigDigitBegin;
                    decimalDigitsForBigDigitBegin = 0;
                }
                decimalParser.clear();
                decimalParser.str(decimalDigits.substr(decimalDigitsForBigDigitBegin, digitsToRead));
                BigDigit bigDigit;
                decimalParser >> bigDigit;
                m_digits.push_back(bigDigit);
            }
        }
        string toString() const
        {
            stringstream decimalWriter;
            decimalWriter << setfill('0');
            for (auto bigDigitIter = m_digits.rbegin(); bigDigitIter != m_digits.rend(); bigDigitIter++)
            {
                decimalWriter << *bigDigitIter;
                // After the most significant digit, need to left-pad with '0'.
                decimalWriter << setw(power);
            }
            return decimalWriter.str();
        }
        BigNum& operator+=(const BigNum& other) &
        {
            add(&other != this ? other : BigNum(other));
            return *this;
        }
        BigNum& operator*=(const BigNum& originalOther) &
        {
            const BigNum& other = (&originalOther != this ? originalOther : BigNum(originalOther));
            const BigNum originalValue(*this);
            int shiftBy = 0;
            BigNum result("0");
            for (auto otherDigit : other.m_digits)
            {
                BigNum multipliedByOtherDigit(originalValue);
                multipliedByOtherDigit.multiplyBy(otherDigit);
                result.add(multipliedByOtherDigit, shiftBy);
                shiftBy++;
            }
            *this = result;
            return *this;
        }
    private:
        // Pick a power of 10 as a base; this aids with converting
        // to and from decimal strings.
        static const long power = 9;
        static constexpr long digitBase = calcPower(10, power);
        using BigDigit = uint64_t;
        static_assert(numeric_limits<BigDigit>::max() / digitBase >= digitBase, "digitBase too large or BigDigit too small!");
       
        // Digits are stored "backwards" i.e. least significant at the front. 
        vector<BigDigit> m_digits;
        using DigitIndex = decltype(m_digits)::size_type;

    public:
        void multiplyBy(BigDigit singleDigit)
        {
            assert(singleDigit >= 0 && singleDigit < digitBase);
            BigDigit carry = 0;
            for (auto& digit : m_digits)
            {
                digit *= singleDigit;
                digit += carry;
                carry = 0;
                if (digit >= digitBase)
                {
                    carry = digit / digitBase;
                    digit %= digitBase;
                }
            }
            if (carry > 0)
            {
                m_digits.push_back(carry);
            }
        }

        void add(const BigNum& other, int shiftOtherBy = 0)
        {
            const auto otherNumDigits = other.m_digits.size() + shiftOtherBy;
            if (otherNumDigits > m_digits.size())
                m_digits.reserve(otherNumDigits + 1);
            bool carry = false;
            for (DigitIndex digitIndex = 0; digitIndex < otherNumDigits || carry; digitIndex++)
            {
                BigDigit toAdd = (carry ? 1 : 0);
                carry = false;
                if (digitIndex < otherNumDigits)
                {
                    if (digitIndex >= shiftOtherBy)
                    {
                        const BigDigit otherDigit = other.m_digits[digitIndex - shiftOtherBy];
                        assert(otherDigit < digitBase);
                        toAdd += otherDigit;
                    }
                }

                if (digitIndex >= m_digits.size())
                {
                    m_digits.push_back(0);
                }
                assert(digitIndex < m_digits.size());

                m_digits[digitIndex] += toAdd;
                if (m_digits[digitIndex] >= digitBase)
                {
                    carry = true;
                    m_digits[digitIndex] -= digitBase;
                }
                assert(m_digits[digitIndex] >= 0 && m_digits[digitIndex] < digitBase);
            }
        }

};

BigNum operator+(const BigNum& lhs, const BigNum& rhs)
{
    BigNum result(lhs);
    result += rhs;
    return result;
}

BigNum operator*(const BigNum& lhs, const BigNum& rhs)
{
    BigNum result(lhs);
    result *= rhs;
    return result;
}

int main() {
    string t1, t2;
    cin >> t1 >> t2;
    long n;
    cin >> n;

    BigNum tnMinus2(t1);
    BigNum tnMinus1(t2);
    BigNum result("0");
    for (int i = 2; i < n; i++)
    {
        BigNum tn = (tnMinus1 * tnMinus1) + tnMinus2;
        result = tn;
        tnMinus2 = tnMinus1;
        tnMinus1 = tn;
    }
    cout << result.toString() << endl;


#if 0
    BigNum testNum1("999999999");
    BigNum testNum2("382543543");

    cout << "testNum1: " << testNum1.toString() << endl;

    //BigNum sum = testNum1;
    //sum.multiplyBy(5);

    //BigNum sum = testNum1;
    //sum.add(testNum2, 1);

    //BigNum product = testNum1;
    //product.multiplyBy(3);

    BigNum product = testNum1;
    product *= testNum2;
    cout << product.toString() << endl;
#endif
#if 0
    long sumsTried = 0;
    while (true)
    {
        const long n1 = rand();
        const long n2 = rand();
        BigNum testNum1(to_string(n1));
        BigNum testNum2(to_string(n2));


        assert((testNum1 + testNum2).toString() == to_string(n1 + n2));

        sumsTried++;
        if ((sumsTried % 100000) == 0)
            cout << sumsTried << endl;
    }
#endif

    return 0;
}
