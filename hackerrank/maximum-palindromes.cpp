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
    // Fairly easy one - would have gotten it first time if I'd sat down with pen and paper, but oh well :)
    // Firstly: note that a string s can be made into a palindrome if and only if:
    //
    //  i) s contains an even number of each letter; or
    // ii) s contains an even number of each letter except for precisely one letter which occurs an odd number of times.
    //
    // In the case of ii), the palindrome is of odd length and is formed around the letter that occurs an odd number of times.
    //
    // Now, the number of such palindromes, if all the letters were distinct, would be (|s| / 2)! for case i), and again (|s| / 2)!
    // (integer division rounds down) for case ii).  Intuitively, we consider only half the palindrome, and take all the permutations
    // of letters in this half-palindrome: the other half is obviously the reverse of the first half.
    //
    // In the case where we have duplicated letters, we will overcount - if s = aaaa, there is one palindrome, not (|s| / 2)! == 2! = 4
    // palindromes! Dealing with this is easy: if the letter x occurs n(x) times in the half-palindrome, then it will  contribute a factor
    // of n(x)! to the overcount, so we must divide by this at the end; "denominatorInverse" represents the productive of these divisions
    // over all letters.
    //
    // So, anyway - if the range contains an even number of each letters, then the can form a palindrome with the same size as the size
    // of the range, and work out the number of ways of doing so using the above.
    //
    // If the range contains some letters that occur and odd number of times, then the situation is more complex: for each such letter
    // x we can form a palindrome of odd length around x by removing precisely one of each other letter y that occurs an odd number
    // of times, leading to a case ii) situation.  The length of such a palindrome is numLettersInRange - numLettersWithOddOccurrencesInRange + 1.
    // We can work out, for each x, the number of distinct palindromes of this max length centred around x fairly easily by again 
    // considering "half-palindromes", where here a half-palindrome is all letters to the left of the central x (the letters to the right of 
    // this x must then be a mirror of this half-palindrome).  As it happens, the overcount is exactly the same as in the case
    // where all letters occur an even number of times and so we must again simply multiply our result by denominatorInverse.
    // So we can easily find the number of palindromes of max length centred around x for each x that occurs and odd number of times,
    // and we just need to sum these.  And that's about it!
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

    for (int q = 0; q < numQueries; q++)
    {
        int l, r;
        cin >> l >> r;
        // Make 0-relative.
        l--;
        r--;

        LetterHistogram letterHistogramForRange = letterHistogramForPrefixSize[r + 1] - letterHistogramForPrefixSize[l];
        int numLettersWithOddOccurrencesInRange = 0;
        ModNum denominatorInverse = 1;
        for (int letterIndex = 0; letterIndex < numLetters; letterIndex++)
        {
            const int numOccurencesOfLetterInRange = letterHistogramForRange.letterCount[letterIndex];
            if ((numOccurencesOfLetterInRange % 2) == 1)
                numLettersWithOddOccurrencesInRange++;

            // Divide by the number of occurrence of the letter in the half-palindrome, factorial'd - note that this works
            // whether numOccurencesOfLetterInRange is odd or even!
            denominatorInverse *= factorialInverseLookup[numOccurencesOfLetterInRange / 2];
        }
        ModNum numWithMaxPalindrome = 0;
        const int numLettersInRange = r - l + 1;
        if (numLettersWithOddOccurrencesInRange == 0)
        {
            // Every letter occurs an even number of times; the max length of the palindrome is numLettersInRange.
            assert((numLettersInRange % 2) == 0);
            const int numInHalfPalindrome = numLettersInRange / 2;
            numWithMaxPalindrome = factorialLookup[numInHalfPalindrome] * denominatorInverse;
        }
        else
        {
            // At least one letter occurs an odd number of times; the max length of the palindrome is numLettersInRange - numLettersWithOddOccurrencesInRange + 1.
            ModNum numerator = 0;
            for (int letterIndex = 0; letterIndex < numLetters; letterIndex++)
            {
                const int numOccurencesOfLetterInRange = letterHistogramForRange.letterCount[letterIndex];
                if ((numOccurencesOfLetterInRange % 2) == 1)
                {
                    // Form an odd-length palindrome around this letter, "removing" all occurrences of the other letters that have
                    // an odd number of occurrences in this range.
                    const int numInHalfPalindrome = (numLettersInRange - numLettersWithOddOccurrencesInRange) / 2;
                    numWithMaxPalindrome += factorialLookup[numInHalfPalindrome] * denominatorInverse;
                }
            }
        }

        cout << numWithMaxPalindrome.value() << endl;
    }
}
