// Simon St James (ssjgz) - 2017-11-17
// This is just a "correctness" submission - it's too slow to pass all testcase!
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
    //cout << "nCr: n: " << n << " r: " << r << " result: " << result << endl;
    return result;
}

int64_t computePowerSum(int64_t n, int64_t k)
{
    vector<int64_t> answersForEarlierK(maxK + 1);

    answersForEarlierK[0] = n;

    for (int i = 1; i <= k; i++)
    {
        int64_t answer = (quickPower(n + 1, i + 1, ::modulus) + ::modulus - 1) % ::modulus;
        //cout << "answer quickPower: " << answer << endl;
        for (int j = 0; j <= i - 1; j++)
        {
            const int64_t multiplier = nCr(i + 1, j, ::modulus);
            //cout << "multiplier: " << multiplier << endl;
            const int64_t term = (multiplier * answersForEarlierK[j]) % :: modulus;
            //cout << "term: " << term << endl;
            answer = (::modulus + answer - term) % ::modulus;
        }
        //assert((answer % (i + 1)) == 0);
        //answer /= (i + 1);
        answer = (answer * inverseLookup[i + 1]) % ::modulus;
        answersForEarlierK[i] = answer;
        //cout << "answer for power = " << i << " : " << answer << endl;;
    }
    return answersForEarlierK[k];
}

int main()
{
#if 0
    Rational r1{3, 4};
    Rational r2{5, 4};
    Rational r3{7, 6};

    cout << r1 << " + " << r2 << " = " << (r1 + r2) << endl;
    cout << r1 << " * " << r2 << " = " << (r1 * r2) << endl;
    cout << r1 << " / " << r2 << " = " << (r1 / r2) << endl;
    cout << r1 << " / " << r3 << " = " << (r1 / r3) << endl;
    cout << r2 << " + " << r3 << " = " << (r2 + r3) << endl;
#endif
    //vector<vector<Rational>> coefficientsForK;
    //coefficientsForK.push_back(
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
        int n, k;
        cin >> n >> k;

        int64_t nMinusOne = (n + ::modulus - 1) % ::modulus; 


        //cout << "n: " << n << " k: " << k << endl;

        const int64_t total = computePowerSum(nMinusOne, k) - 1;
        cout << total << endl;
#ifdef BRUTE_FORCE
        int64_t dbgTotal = 0;

        for (int i = 2; i <= n - 1; i++)
        {
            //cout << "i : " << i << " i ^^ k: " << quickPower(i, k, ::modulus) << endl;
            //dbgTotal = (dbgTotal + (n - 1 - i) * quickPower(i, k, ::modulus)) % ::modulus;
            dbgTotal = (dbgTotal + quickPower(i, k, ::modulus)) % ::modulus;
            //cout << "i: " << i << " dbgTotal: " << dbgTotal << endl;

        }
        cout << "total: " << total << " dbgTotal: " << dbgTotal << endl;
        assert(dbgTotal == total);
#endif
    }

    //const auto blah = quickPower(5, 8, ::modulus);
    //const auto blah2 = quickPower(::modulus - 5, 8, ::modulus);
    //cout << "blah: " << blah << " blah2: " << blah2 << " sum: " << (blah + blah2) << " sum mod: " << (blah + blah2) % ::modulus << endl;

}
