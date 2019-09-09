// Simon St James (ssjgz) - 2019-09-09
// 
// Solution to: https://www.codechef.com/SEPT19B/problems/GDSUB
//
//#define SUBMISSION
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

ModNum solveBruteForce()
{
    ModNum result;
    
    return result;
}

#if 0
ModNum solveOptimised()
{
    ModNum result;
    
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

        const int largestPrime = 8000;
        vector<bool> isPrime(largestPrime + 1, true);
        vector<int> primesUpTo8000;
        for (int64_t factor = 2; factor <= largestPrime; factor++)
        {
            const bool isFactorPrime = isPrime[factor];
            assert(factor < isPrime.size());
            if (isFactorPrime)
            {
                primesUpTo8000.push_back(factor);
            }
            for (int64_t multiple = factor * factor; multiple < isPrime.size(); multiple += factor)
            {
                if (!isPrime[multiple] && !isFactorPrime)
                {
                    // This multiple has already been marked, and since factor is not prime,
                    // all subsequent multiples will already have been marked (by any of the
                    // prime factors of factor!), so we can stop here.
                    break;
                }
                isPrime[multiple] = false;
            }
        }

        cout << "primesUpTo8000: " << primesUpTo8000.size() << endl;
        for (int i = 0;i < primesUpTo8000.size(); i++)
        {
            cout << " i: " << i << " primesUpTo8000: " << primesUpTo8000[i] << endl;
        }


        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;

        for (int t = 0; t < T; t++)
        {
        }

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {

#ifdef BRUTE_FORCE
#if 0
        const auto solutionBruteForce = solveBruteForce();
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 0
        const auto solutionOptimised = solveOptimised();
        cout << "solutionOptimised:  " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised();
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
