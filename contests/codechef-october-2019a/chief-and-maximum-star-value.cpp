// Simon St James (ssjgz) - 2019-10-06
// 
// Solution to: https://www.codechef.com/OCT19A/problems/MSV
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

constexpr auto maxA = 1'000'000;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int solveBruteForce(const vector<int>& a)
{
    int maxStarValue = 0;
    for (int i = 0; i < a.size(); i++)
    {
        int starValue = 0;
        for (int j = 0; j < i; j++)
        {
            if ((a[j] % a[i]) == 0)
            {
                starValue++;
            }
        }
        maxStarValue = max(maxStarValue, starValue);
    }
    
    return maxStarValue;
}

vector<int> computeFactors(int number, const vector<int>& primesUpToMaxA)
{
    vector<std::pair<int64_t, int64_t>> primeFactorisation;
    for (const auto prime : primesUpToMaxA)
    {
        const auto oldNumber = number;
        int numOfPrime = 0;
        while (number % prime == 0)
        {
            numOfPrime++;
            number = number / prime;
        }
        if (numOfPrime > 0)
        {
            primeFactorisation.push_back({prime, numOfPrime});
        }
        if (prime * prime > oldNumber)
            break;
    }
    if (number > 1)
    {
        // If we ever exit the loop above with number != 1, number must
        // be prime.
        primeFactorisation.push_back({number, 1});
    }

    // Combine the powers of primes in prime factorisation to produce all factors,
    // each once and only once.
    vector<int> factors;
    vector<int> powerOfPrime(primeFactorisation.size(), 0);
    while (true)
    {
        int factor = 1;
        for (int i = 0; i < powerOfPrime.size(); i++)
        {
            int primeToPower = 1;
            for (int exponent = 0; exponent < powerOfPrime[i]; exponent++)
            {
                primeToPower = primeToPower * primeFactorisation[i].first;
            }
            factor *= primeToPower;
        }
        factors.push_back(factor);

        int index = 0;
        while (index < powerOfPrime.size() && powerOfPrime[index] == primeFactorisation[index].second)
        {
            powerOfPrime[index] = 0;
            index++;
        }

        if (index == powerOfPrime.size())
            break;

        powerOfPrime[index]++;
    }

    return factors;
}


int solveOptimised(const vector<int>& a, const vector<int>& primesUpToMaxA)
{
    vector<int> numWithFactor(maxA + 1, 0);

    int maxStarValue = 0;
    for (int i = 0; i < a.size(); i++)
    {
        const auto starValue = numWithFactor[a[i]];
        maxStarValue = max(maxStarValue, starValue);

        const auto factors = computeFactors(a[i], primesUpToMaxA);
        for (const auto factor : factors)
        {
            numWithFactor[factor]++;
        }
    }

    return maxStarValue;
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        const int T = 10;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            const int N = 100'000;
            const int maxA = 1'000'000;

            cout << N << endl;
            for (int i = 0; i < N; i++)
            {
                cout << ((rand() % maxA) + 1) << " ";
            }
            cout << endl;
        }

        return 0;
    }

    const int maxA = 1'000'000;

    // Sieve of Eratosthenes.  To help with factorising numbers, compute a lookup table for
    // primes up to maxA.
    vector<int> primesUpToMaxA;
    vector<bool> isPrime(maxA + 1, true);
    for (int64_t factor = 2; factor <= maxA; factor++)
    {
        const bool isFactorPrime = isPrime[factor];
        assert(factor < isPrime.size());
        if (isFactorPrime)
        {
            primesUpToMaxA.push_back(factor);
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


#if 0

    vector<vector<int>> factorsOf(maxA + 1);
    for (int factor = 1; factor <= maxA; factor++)
    {
        for (int multiple = factor; multiple <= maxA; multiple += factor)
        {
            factorsOf[multiple].push_back(factor);
        }
    }
#endif

    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        vector<int> a(N);
        for (auto& x : a)
            x = read<int>();

#ifdef BRUTE_FORCE
        const auto solutionBruteForce = solveBruteForce(a);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
        const auto solutionOptimised = solveOptimised(a, primesUpToMaxA);
        cout << "solutionOptimised:  " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#else
        const auto solutionOptimised = solveOptimised(a, primesUpToMaxA);
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
