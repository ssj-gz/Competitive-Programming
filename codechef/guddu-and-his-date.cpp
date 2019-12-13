// Simon St James (ssjgz) - 2019-12-13
// 
// Solution to: https://www.codechef.com/problems/CB2000
//
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

using namespace std;

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

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

#if 1
ModNum solveBruteForce(const string& N)
{
    ModNum result = 1;
    for (int i = 0; i < N.length(); i++)
    {
        for (int j = i + 1; j < N.length(); j++)
        {
            result = result * abs(N[i] - N[j]);
        }
    }
    
    return result;
}
#endif

#if 1
ModNum solveOptimised(const string& N)
{
    ModNum result = 1;
    int numOfDigit[10] = {};
    for (const auto digit : N)
    {
        numOfDigit[digit - '0']++;
    }
    for (int i = 0; i <= 9; i++)
    {
        if (numOfDigit[i] > 1)
            return 0;
    }
    for (int i = 0; i <= 9; i++)
    {
        if (numOfDigit[i] == 0)
            continue;
        for (int j = i + 1; j <= 9; j++)
        {
            if (numOfDigit[j] == 0)
                continue;
            result *= ModNum(abs(j - i)) * numOfDigit[i] * numOfDigit[j];

        }
    }
    
    return result;
}
#endif


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            const int stringLen = rand() % 20 + 1;
            string s;
            s.push_back('0' + 1 + rand() % 9);
            for (int i = 0; i < stringLen - 1; i++)
            {
                s.push_back('0' + rand() % 10);
            }
            cout << s << endl;
        }

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto N = read<string>();

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(N);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 1
        const auto solutionOptimised = solveOptimised(N);
        cout << "solutionOptimised:  " << solutionOptimised << endl;

        assert(solutionOptimised.value() == solutionBruteForce.value());
#endif
#else
        const auto solutionOptimised = solveOptimised(N);
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
