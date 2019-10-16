// Simon St James (ssjgz) - 2019-10-14
// 
// Solution to: https://www.codechef.com/ACD2019/problems/DIV1
//
#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

constexpr int maxPrimeFactor = 1'000'000;

vector<int64_t> computeFactors(int64_t number, const vector<int>& primesUpToMaxPrimeFactor)
{
    vector<std::pair<int64_t, int64_t>> primeFactorisation;
    for (const auto prime : primesUpToMaxPrimeFactor)
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
    vector<int64_t> factors;
    vector<int> powerOfPrime(primeFactorisation.size(), 0);
    while (true)
    {
        int64_t factor = 1;
        for (int i = 0; i < powerOfPrime.size(); i++)
        {
            int64_t primeToPower = 1;
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

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int64_t findXorOfFactors(int64_t N, const vector<int>& primesUpToMaxPrimeFactor)
{
    const auto factors = computeFactors(N, primesUpToMaxPrimeFactor);

    int64_t xorOfFactors = 0;
    for (const auto factor : factors)
    {
        xorOfFactors = xorOfFactors ^ factor;
    }
    
    return xorOfFactors;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    // Sieve of Eratosthenes.  To help with factorising numbers, compute a lookup table for
    // primes up to maxPrimeFactor.
    vector<int> primesUpToMaxPrimeFactor;
    vector<bool> isPrime(maxPrimeFactor + 1, true);
    for (int64_t factor = 2; factor <= maxPrimeFactor; factor++)
    {
        const bool isFactorPrime = isPrime[factor];
        assert(factor < isPrime.size());
        if (isFactorPrime)
        {
            primesUpToMaxPrimeFactor.push_back(factor);
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
        const int64_t N = read<int64_t>();

        cout << findXorOfFactors(N, primesUpToMaxPrimeFactor) << endl;
    }

    assert(cin);
}
