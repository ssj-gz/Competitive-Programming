// Simon St James (ssjgz) - 2017-11-17
#define BRUTE_FORCE
#define SUBMISSION
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

constexpr auto modulus = 1'000'000'009ULL;
constexpr auto maxK = 1000;
constexpr auto maxnCr = maxK + 1;

int64_t quickPower(int64_t n, uint64_t k, int64_t m)
{
    int64_t result = 1;
    int64_t power = 0;
    while (k > 0)
    {
        if (k & 1)
        {
            int64_t subResult = n;
            for (int i = 0; i < power; i++)
            {
                subResult = (subResult * subResult) % m;
            }
            result = (result * subResult) % m;
        }
        k >>= 1;
        power++;
    }
    return result;
}

vector<int64_t> factorialLookup;
vector<int64_t> factorialInverseLookup;
vector<int64_t> inverseLookup;

void buildFactorialLookups(int maxN)
{
    factorialLookup.resize(maxN + 1);
    factorialLookup[0] = 1;
    factorialInverseLookup.resize(maxN + 1);
    factorialInverseLookup[0] = 1;
    int64_t factorial = 1;
    for (auto i = 1; i <= maxN; i++)
    {
        factorial = (factorial * i) % ::modulus;
        factorialLookup[i] = factorial;
        const auto factorialInverse = quickPower(factorial, ::modulus - 2, ::modulus);
        assert((factorial * factorialInverse) % ::modulus == 1);
        factorialInverseLookup[i] = factorialInverse;
    }
}

int64_t nCr(int64_t n, int64_t r, int64_t modulus)
{
    int64_t result = (factorialLookup[n] * factorialInverseLookup[r]) % modulus;
    result = (result * factorialInverseLookup[n - r]) % modulus;
    return result;
}

int64_t computePowerSum(int64_t n, int64_t k)
{
    // Recursive formula to compute; taken from https://math.stackexchange.com/a/2040334.
    vector<int64_t> answerForK(maxK + 1);

    answerForK[0] = n;

    for (int i = 1; i <= k; i++)
    {
        int64_t answer = (quickPower(n + 1, i + 1, ::modulus) + ::modulus - 1) % ::modulus;
        for (int j = 0; j <= i - 1; j++)
        {
            const int64_t multiplier = nCr(i + 1, j, ::modulus);
            const int64_t term = (multiplier * answerForK[j]) % :: modulus;
            answer = (::modulus + answer - term) % ::modulus;
        }
        answer = (answer * inverseLookup[i + 1]) % ::modulus;
        answerForK[i] = answer;
    }
    return answerForK[k];
}

int main()
{
    // Build lookup tables.
    buildFactorialLookups(maxnCr);
    inverseLookup.resize(maxnCr + 1);
    for (int64_t i = 1; i <= maxnCr; i++)
    {
        const auto inverse = quickPower(i, ::modulus - 2, ::modulus);
        assert(((inverse * i) % ::modulus) == 1);
        inverseLookup[i] = inverse;
    }

    int q;
    cin >> q;

    for (int t = 0; t < q; t++)
    {
        int64_t n, k;
        cin >> n >> k;

        // We need to compute the power sum 2^k + .... + (n-1)^k.
        // This is just 1^k + ... + n^k with the n^k and 1^k terms dropped.
        int64_t nMinusOne = (n + ::modulus - 1) % ::modulus; 
        const int64_t total = (n != 1 ? computePowerSum(nMinusOne, k) - 1 : 0);
        cout << total << endl;
#ifdef BRUTE_FORCE
        int64_t dbgTotal = 0;

        for (int i = 2; i <= n - 1; i++)
        {
            dbgTotal = (dbgTotal + quickPower(i, k, ::modulus)) % ::modulus;

        }
        cout << "total: " << total << " dbgTotal: " << dbgTotal << endl;
        assert(dbgTotal == total);
#endif
    }
}
