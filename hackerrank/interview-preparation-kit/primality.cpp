// Simon St James (ssjgz) - 2019-06-15
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

int main()
{
    // Easy one: if n is composite, then it will have a prime factor <= sqrt(n).
    // To see this, assume otherwise.  Pick a prime factor p of n; then n > sqrt(n)
    // by assumption.  But then n / p is an integer <= sqrt(n), and is either prime (in
    // which case we're done) or itself has a prime factor p' < n / p <= sqrt(n) (in
    // which case we're done!).
    //
    // Thus, for a given n, we need only  check n's divisibility by primes <= sqrt(N),
    // of which there are not very many.
    const int rootMaxN = sqrt(2'000'000'000UL);
    vector<bool> isPrime(rootMaxN + 1, true);

    // Sieve of Erastophenes.
    vector<int> primesUpToRootMaxN;
    for (int factor = 2; factor <= rootMaxN; factor++)
    {
        const bool isFactorPrime = isPrime[factor];
#if 0
        if (isFactorPrime)
        {
            primesUpToRootMaxN.push_back(factor);
        }
#endif
        for (int multiple = factor * 2; multiple <= rootMaxN; multiple += factor)
        {
#if 0
            if (!isPrime[multiple] && !isFactorPrime)
            {
                // This multiple has already been marked, and since factor is not prime,
                // all subsequent multiple will already have been marked (by any of the
                // prime factors of factor!).
                break;
            }
#endif
            isPrime[multiple] = false;
        }
    }
#if 1
    for (int i = 2; i <= rootMaxN; i++)
    {
        if (isPrime[i])
        {
            primesUpToRootMaxN.push_back(i);
        }
    }
#endif

    for (const auto x : primesUpToRootMaxN)
    {
        cout << x << " is prime" << endl;
    }

#if 0
    int p;
    cin >> p;

    for (int i = 0; i < p; i++)
    {
        int n;
        cin >> n;

        bool isPrime = true;
        for (const auto prime : primesUpToRootMaxN)
        {
            if (prime >= n)
                break;
            if ((n % prime) == 0)
            {
                isPrime = false;
                break;
            }
        }
        if (isPrime)
        {
            cout << "Prime" << endl;
        }
        else
        {
            cout << "Not prime" << endl;
        }
    }

#endif
}
