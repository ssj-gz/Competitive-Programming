// Simon St James (ssjgz) - 2019-10-06
// 
// Solution to: https://www.codechef.com/OCT19A/problems/MSV
//
#include <iostream>
#include <vector>

#include <cassert>

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

int findMaxStarValue(const vector<int>& a, const vector<int>& primesUpToMaxA)
{
    vector<int> numWithFactor(maxA + 1, 0);

    int maxStarValue = 0;
    for (int i = 0; i < a.size(); i++)
    {
        // A previous a[j] is divisible by a[i] if and only if a[j] has a[i] as
        // a factor.
        const auto starValue = numWithFactor[a[i]];
        maxStarValue = max(maxStarValue, starValue);

        // Update count of factors of a's seen.
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

    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        vector<int> a(N);
        for (auto& x : a)
            x = read<int>();

        const auto maxStarValue = findMaxStarValue(a, primesUpToMaxA);
        cout << maxStarValue << endl;
    }

    assert(cin);
}
