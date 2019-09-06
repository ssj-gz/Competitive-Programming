// Simon St James (ssjgz) - 2019-09-06
// 
// Solution to: https://www.codechef.com/SEPT19B/problems/FUZZYLIN
//
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
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

vector<int64_t> factors(int64_t number, const vector<int>& primesUpToRootMaxN, const int64_t maxFactor)
{
    //cout << "number: " << number << endl;
    vector<std::pair<int64_t, int64_t>> primeFactorisation;
    for (const auto prime : primesUpToRootMaxN)
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
        primeFactorisation.push_back({number, 1});

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
        //cout << " factor: " << factor << endl;
        //if (factor <= maxFactor)
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


#if 1
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
#endif

#if 1
vector<int64_t> solveOptimised(const vector<int64_t>& a, const vector<int>& queries)
{
    const int n = a.size();

    const int rootMaxN = sqrt(1'000'000'000UL);
    vector<bool> isPrime(rootMaxN + 1, true);

    // Sieve of Eratosthenes.
    vector<int> primesUpToRootMaxN;
    for (int factor = 2; factor <= rootMaxN; factor++)
    {
        const bool isFactorPrime = isPrime[factor];
        if (isFactorPrime)
        {
            primesUpToRootMaxN.push_back(factor);
        }
        for (int multiple = factor * 2; multiple <= rootMaxN; multiple += factor)
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
    //vector<int64_t> allFactors;
    for (int i = 0; i < n; i++)
    {
        factorsOfA[i] = factors(a[i], primesUpToRootMaxN, maxK);
        sort(factorsOfA[i].begin(), factorsOfA[i].end());
        //allFactors.insert(allFactors.end(), factorsOfA[i].begin(), factorsOfA[i].end());
    }
    //cout << "#primesUpToRootMaxN: " << primesUpToRootMaxN.size() << endl;
    //sort(allFactors.begin(), allFactors.end());
    //allFactors.erase(unique(allFactors.begin(), allFactors.end()), allFactors.end());

    vector<int64_t> numForK(maxK + 1);

    vector<int64_t>  blah(maxK + 1);
    set<int64_t> currentFactors;
    vector<int64_t> previousFactors;
    struct FactorPosInfo
    {
        int lastPosAdded = -1;
        int lastPosRemoved = -1;
    };
    map<int64_t, FactorPosInfo> factorPosInfos;

    for (int i = 0; i < n; i++)
    {
        //cout << "i: " << i << " a[i]: " << a[i] << endl;
        map<int64_t, int64_t> numSequencesWithGcd;
        for (const auto previousFactor : previousFactors)
        {
            bool previousFactorDropped = true;
            for (const auto factor : factorsOfA[i])
            {
                if (factor == previousFactor)
                {
                    previousFactorDropped = false;
                    break;
                }
            }
            if (previousFactorDropped)
            {
                factorPosInfos[previousFactor].lastPosRemoved = i;
            }
        }
        assert(is_sorted(factorsOfA[i].begin(), factorsOfA[i].end()));
        vector<int64_t> decreasingFactors = vector<int64_t>(factorsOfA[i].rbegin(), factorsOfA[i].rend());
        struct GcdRange
        {
            int left = 0;
            int right = numeric_limits<int>::max();
        };
        map<int64_t, GcdRange> rangeForGcd;
        int maxRightForLowerGcd = i;
        for (const auto gcd : decreasingFactors)
        {
            auto& rangeForThisGcd = rangeForGcd[gcd];
            auto& posInfoForThisGcd = factorPosInfos[gcd];
            int posOfLastGcd = -1;
            //cout << "i: " << i << " gcd: " << gcd << " posInfoForThisGcd.lastPosRemoved:" << posInfoForThisGcd.lastPosRemoved << " posInfoForThisGcd.lastPosAdded: " << posInfoForThisGcd.lastPosAdded << endl;
            if (posInfoForThisGcd.lastPosAdded == -1)
            {
                posOfLastGcd = i - 1;
            }
            else
            {
                if (posInfoForThisGcd.lastPosAdded > posInfoForThisGcd.lastPosRemoved)
                {
                    posOfLastGcd = posInfoForThisGcd.lastPosAdded - 1;
                }
                else
                {
                    //posOfLastGcd = posInfoForThisGcd.lastPosRemoved;
                    posOfLastGcd = i - 1;
                }
            }
            rangeForThisGcd.right = min(rangeForThisGcd.right, maxRightForLowerGcd);
#ifdef BRUTE_FORCE
            int j = i - 1;
            for (; j >= 0; j--)
            {
                if ((a[j] % gcd) != 0)
                {
                    break;
                }
            }
            cout << "i: " << i << " gcd: " << gcd << " j: " << j << " posOfLastGcd: " << posOfLastGcd << endl;
            assert(j == posOfLastGcd);
#endif
            rangeForThisGcd.right = min(rangeForThisGcd.right, i);
            rangeForThisGcd.left = posOfLastGcd + 1;
            //cout << "i: " << i << " gcd: " << gcd << " rangeForThisGcd: " << rangeForThisGcd.left << ", " << rangeForThisGcd.right << endl;
            if (rangeForThisGcd.right >= 0 && rangeForThisGcd.right >= rangeForThisGcd.left)
            {
                numSequencesWithGcd[gcd] = rangeForThisGcd.right - rangeForThisGcd.left + 1;
            }
#if 0
            for (const auto factorOfGcd : factorsOfA[i])
            {
                if (factorOfGcd < gcd 
                        //&& ((gcd % factorOfGcd) == 0)
                        )
                {
                    //if (rangeForThisGcd.left != -1)
                    {
                        //rangeForGcd[factorOfGcd].right = min(rangeForGcd[factorOfGcd].right, rangeForThisGcd.left - 1);
                    }
                }
            }
#endif
            maxRightForLowerGcd = min(maxRightForLowerGcd, rangeForThisGcd.left - 1);
        }
#ifdef BRUTE_FORCE
        int gcdForSubsequence = a[i];
        map<int64_t, int64_t> dbgNumSequencesWithGcd;
        for (int j = i; j >= 0; j--)
        {
            gcdForSubsequence = gcd(gcdForSubsequence, a[j]);
            dbgNumSequencesWithGcd[gcdForSubsequence]++;
            cout << " # with gcd: " << gcdForSubsequence << " - " << dbgNumSequencesWithGcd[gcdForSubsequence] << endl;
        }
        for (const auto factor : factorsOfA[i])
        {
            cout << " factor: " << factor << " dbgNumSequencesWithGcd[factor]: " << dbgNumSequencesWithGcd[factor] << " numSequencesWithGcd[factor]: " << numSequencesWithGcd[factor] << endl;
            assert(dbgNumSequencesWithGcd[factor] == numSequencesWithGcd[factor]);
        }
#endif
        for (const auto factor : factorsOfA[i])
        {
            if (factor < blah.size())
                blah[factor] += numSequencesWithGcd[factor];
            //cout << " factor:"  << factor << " numSequencesWithGcd: " << numSequencesWithGcd[factor] << " dbgNumSequencesWithGcd: " << dbgNumSequencesWithGcd[factor] << endl;
        }
        for (const auto factor : factorsOfA[i])
        {
            bool factorAdded = true;
            for (const auto previousFactor : previousFactors)
            {
                if (factor == previousFactor)
                {
                    factorAdded = false;
                    break;
                }
            }
            if (factorAdded)
            {
                //cout << "i: " << i << " added factor: " << factor << endl;
                factorPosInfos[factor].lastPosAdded = i;
            }
        }
        previousFactors = factorsOfA[i];
    }
    for (int factor = 1; factor <= maxK; factor++)
    {
        for (int k = factor; k <= maxK; k += factor)
        {
            numForK[k] += blah[factor];
        }
    }


    vector<int64_t> results;
    for (const auto query : queries)
    {
        //cout << " query: " << query << " result: " << numForK[query] << endl;
        results.push_back(numForK[query]);
    }
    return results;
}
#endif


int main(int argc, char* argv[])
{
#if 0
    map<int64_t, int64_t> blah;
    for (int i = 0; i < 1'000'000; i++)
    {
        blah[i]++;
    }
    for (int i = 0; i < 1'000'000; i++)
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
        //const int N = 10;
        const int maxA = rand() % 1'000'000'000ULL + 1;

        cout << N << endl;

        for (int i = 0; i < N; i++)
        {
            cout << ((rand() % maxA + 1)) << " ";
        }
        cout << endl;

        //const int Q = rand() % 1000 + 1;
        const int Q = 100'000;
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
