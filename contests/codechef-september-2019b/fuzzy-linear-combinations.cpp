// Simon St James (ssjgz) - 2019-09-06
// 
// Solution to: https://www.codechef.com/SEPT19B/problems/FUZZYLIN
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <cmath>

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

int gcd(int a, int b)
{
    while (true)
    {
        if (a > b)
            swap(a, b);
        if (a == 0)
            break;
        const int nextA = a;
        const int nextB = b - a;

        a = nextA;
        b = nextB;
    }
    return b;
}

vector<int64_t> factors(int64_t number, const vector<int>& primesUpToRootMaxN)
{
    cout << "number: " << number << endl;
    vector<std::pair<int64_t, int64_t>> primeFactorisation;
    for (const auto prime : primesUpToRootMaxN)
    {
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
        cout << " factor: " << factor << endl;
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

    for (const auto query : queries)
    {
        cout << "query: " << query << endl;
        int64_t queryResult = 0;
        for (int i = 0; i < n; i++)
        {
            for (int j = i; j < n; j++)
            {
                int64_t subarrayGcd = a[i];
                for (int k = i; k <= j; k++)
                {
                    subarrayGcd = gcd(subarrayGcd, a[k]);
                }
                cout << "i: " << i << " j: " << j << " gcd[i,j] = " << subarrayGcd << endl;
                if ((query % subarrayGcd) == 0)
                {
                    queryResult++;
                }
            }
        }
        results.push_back(queryResult);
    }

    return results;
}
#endif

#if 1
vector<int64_t> solveOptimised(const vector<int64_t>& a, const vector<int>& queries)
{
    vector<int64_t> results;
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
    for (int i = 0; i < n; i++)
    {
        factorsOfA[i] = factors(a[i], primesUpToRootMaxN);
    }


    return results;
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
        const int N = rand() % 100 + 1;
        const int maxA = rand() % 1000 + 1;

        cout << N << endl;

        for (int i = 0; i < N; i++)
        {
            cout << ((rand() % 1000 + 1)) << " ";
        }
        cout << endl;

        const int Q = rand() % 100 + 1;
        cout << Q << endl;

        for (int i = 0; i < Q; i++)
        {
            cout << ((rand() % (20 * maxA))) << " ";
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
#if 1
    const auto solutionBruteForce = solveBruteForce(a, queries);
    cout << "solution for query: " << endl;
    for (const auto& x : solutionBruteForce)
    {
        cout << "solutionBruteForce: " << x << endl;
    }
#endif
#if 1
    const auto solutionOptimised = solveOptimised(a, queries);
    cout << "solutionOptimised:  " << endl;
    for (const auto& x : solutionBruteForce)
    {
        cout << x << endl;
    }

    assert(solutionOptimised == solutionBruteForce);
#endif
#else
    const auto solutionOptimised = solveOptimised();
    cout << solutionOptimised << endl;
#endif

    assert(cin);
}
