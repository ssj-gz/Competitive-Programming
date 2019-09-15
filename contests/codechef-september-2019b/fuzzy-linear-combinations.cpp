// Simon St James (ssjgz) - 2019-09-06
// 
// Solution to: https://www.codechef.com/SEPT19B/problems/FUZZYLIN
//
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
//#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <limits>
#include <algorithm>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

uint64_t gcd(uint64_t u, uint64_t v)
{
    uint64_t shift = 0;

    /* GCD(0,v) == v; GCD(u,0) == u, GCD(0,0) == 0 */
    if (u == 0) return v;
    if (v == 0) return u;

    /* Let shift := lg K, where K is the greatest power of 2
       dividing both u and v. */
    while (((u | v) & 1) == 0) {
        shift++;
        u >>= 1;
        v >>= 1;
    }

    while ((u & 1) == 0)
        u >>= 1;

    /* From here on, u is always odd. */
    do {
        /* remove all factors of 2 in v -- they are not common */
        /*   note: v is not zero, so while will terminate */
        while ((v & 1) == 0)
            v >>= 1;

        /* Now u and v are both odd. Swap if necessary so u <= v,
           then set v = v - u (which is even). For bignums, the
           swapping is just pointer movement, and the subtraction
           can be done in-place. */
        if (u > v) {
            uint64_t t = v; v = u; u = t; // Swap u and v.
        }

        v -= u; // Here v >= u.
    } while (v != 0);

    /* restore common factors of 2 */
    return u << shift;
}

const int maxK = 1'000'000;

vector<int64_t> factors(int64_t number, const vector<int>& primesUpToRootMaxN, const vector<char>& isPrime)
{
    vector<std::pair<int64_t, int64_t>> primeFactorisation;
    for (const auto prime : primesUpToRootMaxN)
    {
        if (number < isPrime.size() && isPrime[number])
        {
            break;
        }
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


vector<int64_t> solveBruteForce(const vector<int64_t>& a, const vector<int>& queries)
{
    vector<int64_t> results;
    const int n = a.size();
    vector<int64_t> numWithGcd(maxK + 1);

    for (int i = 0; i < n; i++)
    {
        int64_t subarrayGcd = a[i];
        for (int j = i; j < n; j++)
        {
            subarrayGcd = gcd(subarrayGcd, a[j]);
            if (subarrayGcd < numWithGcd.size())
            {
                numWithGcd[subarrayGcd]++;
            }
        }
    }

    vector<int64_t> numForK(maxK + 1);
    for (int factor = 1; factor <= maxK; factor++)
    {
        for (int k = factor; k <= maxK; k += factor)
        {
            numForK[k] += numWithGcd[factor];
        }
    }

    for (const auto query : queries)
    {
        results.push_back(numForK[query]);
    }

    return results;
}

vector<int64_t> solveOptimised(const vector<int64_t>& a, const vector<int>& queries)
{
    const int n = a.size();

    const int rootMaxN = sqrt(1'000'000'000UL);
    vector<char> isPrime(1'000'000 + 1, true);

    struct FactorAndPos
    {
        int64_t factor = -1;
        int pos = -1;
    };

    // Sieve of Eratosthenes.  To help with factorising numbers, compute a lookup table for
    // primes up to 1'000'000, but store a list of primes for only up to rootMaxN.
    vector<int> primesUpToRootMaxN;
    for (int64_t factor = 2; factor <= 1'000'000; factor++)
    {
        const bool isFactorPrime = isPrime[factor];
        assert(factor < isPrime.size());
        if (isFactorPrime)
        {
            if (factor <= rootMaxN)
            {
                primesUpToRootMaxN.push_back(factor);
            }
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

    vector<vector<int64_t>> factorsOfA(n);
    map<int64_t, vector<int64_t>> factorsLookup;

    map<int64_t, vector<int>, std::greater<>> positionsWithFactorDecreasing;

    vector<int> maxRightForLowerGcd(n);
    for (int i = 0; i < n; i++)
    {
        maxRightForLowerGcd[i] = i;
    }

    for (int i = 0; i < n; i++)
    {
        if (factorsLookup.find(a[i]) == factorsLookup.end())
        {
            factorsOfA[i] = factors(a[i], primesUpToRootMaxN, isPrime);
            factorsLookup[a[i]] = factorsOfA[i];
        }
        else
        {
            factorsOfA[i] = factorsLookup[a[i]];
        }
        int gcdWithPrev = -1;
        if (i > 0)
        {
            gcdWithPrev = gcd(a[i], a[i - 1]);
        }
        for (const auto factor : factorsOfA[i])
        {
            bool divisibleByGcwWithPrev = false;
            bool addPrevPos = false;
            if (factor > maxK && gcdWithPrev != -1)
            {
                if (factor > gcdWithPrev)
                {
                    maxRightForLowerGcd[i] = i - 1;
                    continue;
                }
                if ((gcdWithPrev % factor) == 0)
                {
                    addPrevPos = true;
                }

            }
            auto& positionsForFactor = positionsWithFactorDecreasing[factor];
            if (addPrevPos)
                positionsForFactor.push_back(i - 1);
            positionsForFactor.push_back(i);
        }
    }

    vector<int64_t> numSequencesWithGcd(maxK + 1);


    for (auto& gcdAndPositions : positionsWithFactorDecreasing)
    {
        const auto& gcd = gcdAndPositions.first;
        auto& positions = gcdAndPositions.second;
        // Might have accidentally added a copy of a position (addPrevPos) - remove if so.
        positions.erase(unique(positions.begin(), positions.end()), positions.end());

        int runLengthWithGcd = 1;
        int previousPosition = -1;
        for (const auto position : positions)
        {
            if (previousPosition != -1 && position == previousPosition + 1)
            {
                runLengthWithGcd++;
            }
            else
            {
                runLengthWithGcd = 1;
            }

            const int gcdRangeLeft = position - runLengthWithGcd + 1;
            const int gcdRangeRight = min(position, maxRightForLowerGcd[position]);

            if (gcdRangeRight >= 0 && gcdRangeRight >= gcdRangeLeft)
            {
                if (gcd < numSequencesWithGcd.size())
                {
                    numSequencesWithGcd[gcd] += gcdRangeRight - gcdRangeLeft + 1;
                }
            }

            maxRightForLowerGcd[position] = min(maxRightForLowerGcd[position], gcdRangeLeft - 1);

            previousPosition = position;
        }
    }

    vector<int64_t> numForK(maxK + 1);
    for (int gcd = 1; gcd <= maxK; gcd++)
    {
        if (numSequencesWithGcd[gcd] == 0)
            continue;
        for (int k = gcd; k <= maxK; k += gcd)
        {
            numForK[k] += numSequencesWithGcd[gcd];
        }
    }

    vector<int64_t> results;
    for (const auto query : queries)
    {
        results.push_back(numForK[query]);
    }
    return results;
}


int main(int argc, char* argv[])
{
#if 0
    map<int64_t, int64_t> blah;
    for (int i = 0; i < 15'000'000; i++)
    {
        blah[i]++;
    }
    for (int i = 0; i < 15'000'000; i++)
    {
        blah[i] += i + rand() % 30;
    }
    int64_t nose = 0;
    for (int i = 0; i < 1'000'000; i++)
    {
        nose += blah[i];
    }
    return 0;
#endif
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int N = rand() % 200 + 1;
        //const int N = 5;
        //const int N = 10;
        int64_t maxA = rand() % 1'000'000'000ULL + 1;
        int64_t minA = maxA - rand() % 10000;
        if (minA < 1)
            minA = 1;

        if (rand() % 3 == 0)
        {
            maxA = rand() % 1000 + 1;
            minA = 1;
        }
        assert(minA < maxA);

        cout << N << endl;

        for (int i = 0; i < N; i++)
        {
            if (rand() % 4 == 0)
            {
                cout << minA + (rand() % (maxA - minA)) << " ";
            }
            else
            {
                cout << 479001600 << " ";
            }
        }
        cout << endl;

        //const int Q = rand() % 1000 + 1;
        //const int Q = 100'000;
        const int Q = 1;
        cout << Q << endl;

        for (int i = 0; i < Q; i++)
        {
            cout << ((rand() % 1'000'000) + 1) << " ";
        }
        cout << endl;

        return 0;
    }

    const int N = read<int>();

    vector<int64_t> a(N);
    for (auto& x : a)
    {
        x = read<int64_t>();
    }

    const int Q = read<int>();
    vector<int> queries(Q);
    for (auto& x : queries)
    {
        x = read<int>();
    }

#ifdef BRUTE_FORCE
    queries.clear();
    for (int k = 1; k <= maxK; k++)
    {
        queries.push_back(k);
    }
    const auto solutionBruteForce = solveBruteForce(a, queries);
    cout << "blah: " << endl;
    for (const auto& x : solutionBruteForce)
    {
        //cout << "solutionBruteForce: " << x << endl;
    }
    const auto solutionOptimised = solveOptimised(a, queries);
    cout << "solutionOptimised:  " << endl;
    for (const auto& x : solutionOptimised)
    {
        //cout << x << endl;
    }

    assert(solutionOptimised == solutionBruteForce);
#else
    const auto solutionOptimised = solveOptimised(a, queries);
    for (const auto& x : solutionOptimised)
    {
        cout << x << endl;
    }
#endif


    assert(cin);
}


