// Simon St James (ssjgz) - 2019-08-05
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h>

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

bool operator==(const ModNum& lhs, const ModNum& rhs)
{
    return lhs.value() == rhs.value();
}

const int maxNumberLength = 100'000;
vector<vector<ModNum>> sumOfFForNumDigitsBeginningWith(maxNumberLength + 1, vector<ModNum>(10));
vector<ModNum> tenToThePowerOf(maxNumberLength + 1);

ModNum calcF(const string& number)
{
    const int numDigits = number.size();
    ModNum result = 0;

    int previousDigit = -1;
    for (int index = 0; index < numDigits; index++)
    {
        const int digitInNumber = number[index] - '0';

        if (digitInNumber != previousDigit)
        {
            result += digitInNumber *  tenToThePowerOf[numDigits - index - 1];
        }


        previousDigit = digitInNumber;
    }

    return result;
}

ModNum solveBruteForce(const string& L, const string& R)
{
    ModNum result = 0;
    string current = L;

    while (true)
    {
        //cout << " current: " << current << endl;
        result += calcF(current);
        if (current == R)
            break;

        int index = current.size() - 1;
        while (index >= 0 && current[index] == '9')
        {
            current[index] = '0';
            index--;
        }
        if (index == -1)
        {
            current = '1' + current;
        }
        else
        {
            current[index]++;
        }
    }

    return result;
}

vector<vector<ModNum>> computeMainLookupTable()
{
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

    powerOf10 = 1;
    for (int i = 0; i <= maxNumberLength; i++)
    {
        tenToThePowerOf[i] = powerOf10;
        powerOf10 = powerOf10 * 10;
    }

    return sumOfFForNumDigitsBeginningWith;

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
            const ModNum numOccurencesWithThisDigit = tenToThePowerOf[numDigits - index - 1];
            // Contribution from this index and rightwards having this digit.
            result += sumOfFForNumDigitsBeginningWith[numDigits - index][digit];

            // Contribution from previous indices from having this digit.
            result += sumToLeft * numOccurencesWithThisDigit;

            if (digit == previousDigitInNumber)
            {
                // The contribution to the result from having this digit in this position
                // is wrong as this digit is equal to the previous digit; remove the contribution.
                // (Similar logic as in computeMainLookupTable).
                const ModNum valueOfThisDigit = digit * tenToThePowerOf[numDigits - index - 1];
                const auto correctForThisDigit = valueOfThisDigit * numOccurencesWithThisDigit;
                result -= correctForThisDigit;
            }
        }
        const bool digitIsSameAsPrevious = (previousDigitInNumber == digitInNumber);
        if (!digitIsSameAsPrevious)
        {
            // Update sumToLeft.
            sumToLeft += digitInNumber * tenToThePowerOf[numDigits - index - 1];
        }

        previousDigitInNumber = digitInNumber;
    }

    result += sumToLeft;

    return result;
}


ModNum solveOptimised(const string& L, const string& R)
{
    ModNum result =  sumOfFUpTo(R) - sumOfFUpTo(L) + calcF(L);

    return result;
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        const int maxNumDigits = rand() % 7 + 1;

        auto generateRandomNumber = [&maxNumDigits]()
        {
            string numberAsString;
            const int originalNumDigits = (rand() % maxNumDigits) + 1;
            int numDigits = originalNumDigits;

            // First digit (must not be 0).
            numberAsString = static_cast<char>('0' + ((rand() % 9) + 1)) + string("");
            numDigits--;

            while (numDigits > 0)
            {
                numberAsString = numberAsString + static_cast<char>('0' + (rand() % 10));

                numDigits--;
            }

            assert(numberAsString.size() == originalNumDigits);
            assert(numberAsString[0] != '0');

            return numberAsString;

        };


        cout << 10 << endl;
        for (int i = 0; i < 10; i++)
        {
            auto L = generateRandomNumber();
            auto R = generateRandomNumber();
            if (L.size() > R.size())
                swap(L, R);
            else if (L.size() == R.size())
            {
                if (L > R)
                    swap(L, R);
            }
            cout << L.size() << " " << L << endl;
            cout << R.size() << " " << R << endl;
        }


        return 0;
    }

    computeMainLookupTable();

    const int T = read<int>();
    for (int t = 0; t < T; t++)
    {
        read<int>();
        const string L = read<string>();
        read<int>();
        const string R = read<string>();

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(L, R);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 1
        const auto solutionOptimised = solveOptimised(L, R);
        cout << "solutionOptimised: " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised(L, R);
        cout << solutionOptimised << endl;
#endif
    }

}


