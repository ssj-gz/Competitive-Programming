// Simon St James (ssjgz) - 2018-02-28.
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

const int maxN = 100'000;

const int64_t modulus = 1'000'000'007ULL;

class ModNum
{
    public:
        ModNum(int64_t n)
            : m_n{n}
        {
        }
        ModNum& operator+=(const ModNum& other)
        {
            m_n = (m_n + other.m_n) % ::modulus;
            return *this;
        }
        ModNum& operator*=(const ModNum& other)
        {
            m_n = (m_n * other.m_n) % ::modulus;
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




vector<int64_t> factorialLookup;
vector<int64_t> factorialInverseLookup;

int64_t factorial(int64_t n)
{
    assert(n >= 0 && n < factorialLookup.size());
    return factorialLookup[n];
}

int64_t quickPower(int64_t base, int64_t exponent, int64_t modulus)
{
    // Raise base to the exponent mod modulus using as few multiplications as 
    // we can e.g. base ^ 8 ==  (((base^2)^2)^2).
    int64_t result = 1;
    int64_t power = 0;
    while (exponent > 0)
    {
        if (exponent & 1)
        {
            int64_t subResult = base;
            for (auto i = 0; i < power; i++)
            {
                subResult = (subResult * subResult) % modulus;
            }
            result = (result * subResult) % modulus;
        }
        exponent >>= 1;
        power++;
    }
    return result;
}

void buildFactorialLookups(int maxN)
{
    factorialLookup.resize(maxN + 1);
    factorialLookup[0] = 1;
    factorialInverseLookup.resize(maxN + 1);
    factorialInverseLookup[0] = 1;
    int64_t factorial = 1;
    for (auto i = 1; i <= maxN; i++)
    {
        factorial = (factorial * i) % ::modulus;
        factorialLookup[i] = factorial;
        const auto factorialInverse = quickPower(factorial, ::modulus - 2, ::modulus);
        assert((factorial * factorialInverse) % ::modulus == 1);
        factorialInverseLookup[i] = factorialInverse;
    }

}

int main()
{
    string s;
    cin >> s;

    int numQueries;
    cin >> numQueries;

    const int numLetters = 26;
    struct LetterHistogram
    {
        int letterCount[numLetters] = {};
        LetterHistogram operator-(const LetterHistogram& other)
        {
            LetterHistogram result(*this);
            for (int letterIndex = 0; letterIndex < numLetters; letterIndex++)
            {
                result.letterCount[letterIndex] -= other.letterCount[letterIndex];
            }
            return result;
        }
    };

    vector<LetterHistogram> letterHistogramForPrefixSize;
    LetterHistogram letterHistogram;
    letterHistogramForPrefixSize.push_back(letterHistogram);
    for (int i = 0; i < s.size(); i++)
    {
        letterHistogram.letterCount[s[i] - 'a']++;
        letterHistogramForPrefixSize.push_back(letterHistogram);
    }

    buildFactorialLookups(maxN);

    //cout << "numQueries: " << numQueries << endl;


    for (int q = 0; q < numQueries; q++)
    {
        int l, r;
        cin >> l >> r;
        // Make 0-relative.
        l--;
        r--;

        LetterHistogram letterHistogramForRange = letterHistogramForPrefixSize[r + 1] - letterHistogramForPrefixSize[l];
        int numLettersWithOddOccurrencesInRange = 0;
        ModNum denominator = 1;
        for (int letterIndex = 0; letterIndex < numLetters; letterIndex++)
        {
            const int numOccurencesOfLetterInRange = letterHistogramForRange.letterCount[letterIndex];
            if ((numOccurencesOfLetterInRange % 2) == 1)
                numLettersWithOddOccurrencesInRange++;

            denominator *= factorialInverseLookup[numOccurencesOfLetterInRange / 2];
        }
        ModNum numWithMaxPalindrome = 0;
        const int numLettersInRange = r - l + 1;
        if (numLettersWithOddOccurrencesInRange == 0)
        {
            assert((numLettersInRange % 2) == 0);
            numWithMaxPalindrome = factorialLookup[numLettersInRange / 2] * denominator;
        }
        else
        {
            ModNum numerator = 0;
            for (int letterIndex = 0; letterIndex < numLetters; letterIndex++)
            {
                const int numOccurencesOfLetterInRange = letterHistogramForRange.letterCount[letterIndex];
                if ((numOccurencesOfLetterInRange % 2) == 1)
                {
                    const int numInHalfPalindrome = (numLettersInRange - numLettersWithOddOccurrencesInRange) / 2;
                    ModNum palindromeContribToNumerator = ModNum(factorialLookup[numInHalfPalindrome]) * factorialInverseLookup[numOccurencesOfLetterInRange / 2];

                    numerator += palindromeContribToNumerator;
                }
            }
            numWithMaxPalindrome = numerator * denominator;

        }

        cout << numWithMaxPalindrome.value() << endl;
    }


}
