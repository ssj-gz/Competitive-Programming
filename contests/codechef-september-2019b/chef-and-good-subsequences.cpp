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

bool operator==(const ModNum& lhs, const ModNum& rhs)
{
    return lhs.value() == rhs.value();
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

vector<int> getPrimesUpTo8000()
{

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
    return primesUpTo8000;
}

ModNum solveBruteForce(int N, int K, const vector<int>& a)
{
    ModNum result;

    vector<bool> useElement(N, false);

    while (true)
    {
        vector<int> subset;
        for (int i = 0; i < N; i++)
        {
            if (useElement[i])
                subset.push_back(a[i]);
        }
        sort(subset.begin(), subset.end());
        const auto isGood = subset.size() <= K && (unique(subset.begin(), subset.end()) == subset.end());
        if (isGood)
            result = result + 1;

        int index = 0;
        while (index < N && useElement[index])
        {
            useElement[index] = false;
            index++;
        }
        if (index == N)
            break;

        useElement[index] = true;
    }
    
    return result;
}

ModNum solveOptimised(const int N, const int K, const vector<int>& aOriginal)
{
    auto a = aOriginal;
    sort(a.begin(), a.end());
    struct PrimeInfo
    {
        int prime = -1;
        int numOccurrences = 0;
    };
    vector<PrimeInfo> distinctPrimeOccurrences;

    int prevPrime = -1;
    while (!a.empty())
    {
        const int prime = a.back();
        if (prime != prevPrime)
        {
            distinctPrimeOccurrences.push_back({prime, 0});
        }
        distinctPrimeOccurrences.back().numOccurrences++;

        a.pop_back();
        prevPrime = prime;
    }
    const int numDistinctElements = distinctPrimeOccurrences.size(); 

    if (K > numDistinctElements)
    {
        return 0;
    }

#if 1
    for (const auto x : distinctPrimeOccurrences)
    {
        cout << " prime: " << x.prime << " occurs " << x.numOccurrences << " times" << endl;
    }
#endif

    vector<vector<ModNum>> dp(numDistinctElements + 1, vector<ModNum>(K + 1, 0));
    for (int i = 0; i <= numDistinctElements; i++)
    {
        dp[i][0] = 1;
    }

    for (int i = 1; i <= numDistinctElements; i++)
    {
        for (int j = 1; j <= i; j++)
        {
            assert(i - 1 >= 0);
            dp[i][j] += dp[i - 1][j]; // Don't choose this number.
            cout << " i: " << i << " j: " << j << " dp[i - 1][j]: " << dp[i - 1][j] << endl;
            if (j - 1 >= 0 && j - 1 <= i - 1)
            {
                // Do choose *one* from this bunch of same numbers.
                cout << " i: " << i << " j: " << j << " dp[i - 1][j - 1]: " << dp[i - 1][j - 1] << endl;
                dp[i][j] += dp[i - 1][j - 1] * (distinctPrimeOccurrences[j - 1].numOccurrences);
            }
            cout << "i: " << i << " j: " << j << " dp: " << dp[i][j] << endl;
        }
    }

    ModNum result;

    for (int j = 0; j <= K; j++)
    {
        result += dp[numDistinctElements][j];
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

#if 0
        cout << "primesUpTo8000: " << primesUpTo8000.size() << endl;
        for (int i = 0;i < primesUpTo8000.size(); i++)
        {
            cout << " i: " << i << " primesUpTo8000: " << primesUpTo8000[i] << endl;
        }
#endif

        const auto primesUpTo8000 = getPrimesUpTo8000();
        const int N = rand() % 20 + 1;
        const int K = rand() % (2 * N) + 1;
        //const int maxPrimeIndex = (rand() % (primesUpTo8000.size() - 1) + 1);
        const int maxPrimeIndex = 9;


        cout << N << " " << K << endl;
        for (int i = 0; i < N; i++)
        {
            cout << primesUpTo8000[rand() % maxPrimeIndex] << " ";
        }
        cout << endl;



        return 0;
    }


    const int N = read<int>();
    const int K = read<int>();

    vector<int> a(N);
    for (auto& aElement : a)
    {
        aElement = read<int>();
    }

#ifdef BRUTE_FORCE
#if 1
    const auto solutionBruteForce = solveBruteForce(N, K, a);
    cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 1
    const auto solutionOptimised = solveOptimised(N, K, a);
    cout << "solutionOptimised:  " << solutionOptimised << endl;

    assert(solutionOptimised == solutionBruteForce);
#endif
#else
    const auto solutionOptimised = solveOptimised();
    cout << solutionOptimised << endl;
#endif

    assert(cin);
}
