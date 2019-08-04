// Simon St James (ssjgz) - 2019-XX-XX
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



ModNum calcF(const string& s)
{
    ModNum result = 0;

    ModNum powerOf10 = 1;
    ModNum toAdd = 0;
    for (int i = s.size() - 1; i >= 0; i--)
    {
        if (i + 1 < s.size() && s[i] != s[i + 1])
        {
            result += toAdd;
        }

        toAdd = powerOf10 * (s[i] - '0');
        powerOf10 *= 10;
    }
    result += toAdd;

    return result;
}

ModNum solveBruteForce(const string& L, const string& R)
{
    ModNum result = 0;
    string current = L;

    while (true)
    {
        //cout << " current: " << current << endl;
        result = calcF(current);
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

ModNum solveOptimised(const string& L, const string& R)
{
    const int numDigits = 10;
    vector<vector<ModNum>> sumOfFForNumDigitsBeginningWith(100'000, vector<ModNum>(numDigits));
    for (int i = 0; i < numDigits; i++)
    {
        sumOfFForNumDigitsBeginningWith[0][i] = 0;
        sumOfFForNumDigitsBeginningWith[1][i] = i;
    }

    ModNum prevPowerOf10 = 1;
    ModNum powerOf10 = 10;
    for (int numberLength = 2; numberLength < 100; numberLength++)
    {
        for (int newFrontDigit = 0; newFrontDigit <= 9; newFrontDigit++)
        {
            ModNum blah;
            ModNum numWithPrevFrontDigit = prevPowerOf10;
            for (int prevFrontDigit = 0; prevFrontDigit <= 9; prevFrontDigit++)
            {
                if (prevFrontDigit != newFrontDigit)
                {
                    blah += newFrontDigit * powerOf10 * numWithPrevFrontDigit + sumOfFForNumDigitsBeginningWith[numberLength - 1][prevFrontDigit];
                }
                else
                {
                    blah += newFrontDigit * powerOf10 * numWithPrevFrontDigit + sumOfFForNumDigitsBeginningWith[numberLength - 1][prevFrontDigit] - prevFrontDigit * prevPowerOf10 * numWithPrevFrontDigit;
                }
            }
            sumOfFForNumDigitsBeginningWith[numberLength][newFrontDigit] = blah;

        }

        prevPowerOf10 = powerOf10;
        powerOf10 *= 10;
    }

    {
        vector<vector<ModNum>> dbgSumOfFForNumDigitsBeginningWith(100'000, vector<ModNum>(numDigits));
        string current = "0";
        const int maxLen = 8;

        while (current.size() < maxLen)
        {
            dbgSumOfFForNumDigitsBeginningWith[current.size()][current[0] - '0'] += calcF(current);
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

        for (int len = 1; len < maxLen; len++)
        {
            for (int frontDigit = 0; frontDigit <= 9; frontDigit++)
            {
                cout << " len: " << len << " frontDigit: " << frontDigit << " sumOfFForNumDigitsBeginningWith: " << sumOfFForNumDigitsBeginningWith[len][frontDigit] << " dbgSumOfFForNumDigitsBeginningWith: " << dbgSumOfFForNumDigitsBeginningWith[len][frontDigit] << endl;
                if (frontDigit != 0)
                {
                    assert(sumOfFForNumDigitsBeginningWith[len][frontDigit] == dbgSumOfFForNumDigitsBeginningWith[len][frontDigit]);
                }
            }
        }
    }

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


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        const int maxValue = rand() % 100'000'000;
        int L = rand() % maxValue + 1;
        int R = rand() % maxValue + 1;
        if (R < L)
            swap(L, R);
        cout << 1 << endl;
        cout << to_string(L).size() << " " << L << endl;
        cout << to_string(R).size() << " " << R << endl;
        return 0;
    }

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


