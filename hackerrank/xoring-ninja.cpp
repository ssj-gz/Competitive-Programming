// Simon St James (ssjgz) 2019-04-14
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <cassert>
#include <sys/time.h>


using namespace std;

const int64_t Mod = 1'000'000'007ULL;

namespace 
{
    vector<int64_t> factorialLookup;
    vector<int64_t> factorialInverseLookup;

    int64_t factorial(int64_t n)
    {
        assert(n >= 0 && n < factorialLookup.size());
        return factorialLookup[n];
    }

    int64_t nCr(int64_t n, int64_t r, int64_t modulus)
    {
        //cout << "nCr: n: " << n << " r: " << r << endl;
        //cout << "factorialLookup.size(): " << factorialLookup.size() << endl;
        assert(n >= 0 && r >= 0);
        assert(n >= r);
        int64_t result = factorial(n);
        assert(r < factorialInverseLookup.size());
        assert(n - r < factorialInverseLookup.size());
        result = (result * factorialInverseLookup[n - r]) % modulus;
        result = (result * factorialInverseLookup[r] ) % modulus;
        return result;
    }

    int64_t quickPower(int64_t base, int64_t exponent, int64_t modulus)
    {
        // Raise base to the exponent mod modulus using as few multiplications as 
        // we can e.g. base ^ 8 ==  (((base^2)^2)^2).
        int64_t result = 1;
        int64_t power = 0;
        while (exponent > 0)
        {
            if (exponent & 1)
            {
                int64_t subResult = base;
                for (auto i = 0; i < power; i++)
                {
                    subResult = (subResult * subResult) % modulus;
                }
                result = (result * subResult) % modulus;
            }
            exponent >>= 1;
            power++;
        }
        return result;
    }

    void buildFactorialLookups(int maxN, int64_t modulus)
    {
        factorialLookup.resize(maxN + 1);
        factorialLookup[0] = 1;
        factorialInverseLookup.resize(maxN + 1);
        factorialInverseLookup[0] = 1;
        int64_t factorial = 1;
        for (auto i = 1; i <= maxN; i++)
        {
            factorial = (factorial * i) % modulus;
            factorialLookup[i] = factorial;
            const auto factorialInverse = quickPower(factorial, modulus - 2, modulus);
            assert((factorial * factorialInverse) % modulus == 1);
            factorialInverseLookup[i] = factorialInverse;
        }

    }
}


void bruteForceAux(const vector<int64_t>& a, int index, vector<int64_t>& setSoFar, int64_t& result)
{
    if (index == a.size())
    {
        int64_t xorSum = 0;
        for (const auto x : setSoFar)
        {
            xorSum ^= x;
        }
        result = (result + xorSum) % Mod;
        return;
    }
    // Without this element.
    bruteForceAux(a, index + 1, setSoFar, result);
    // With this element.
    setSoFar.push_back(a[index]);
    bruteForceAux(a, index + 1, setSoFar, result);
    setSoFar.pop_back();
}
int64_t bruteForce(const vector<int64_t>& a)
{
    int64_t result = 0;
    vector<int64_t> setSoFar;

    bruteForceAux(a, 0, setSoFar, result);

    return result;
}

int64_t optimized(const vector<int64_t>& a)
{
    int64_t result = 0;
    int64_t powerOf2 = 1;

    while (true)
    {
        bool foundHigherThanPowerOf2 = false;
        int numWithThisPower = 0;

        for (const auto x : a)
        {
            if (x >= powerOf2)
                foundHigherThanPowerOf2 = true;

            if ((x & powerOf2) != 0)
                numWithThisPower++;
        }
        //cout << "powerOf2: " << powerOf2 << " numWithThisPower: " << numWithThisPower << endl;

        int64_t thisTerm = 0;
        for (int odd = 1; odd <= numWithThisPower; odd += 2)
        {
            //cout << "odd: " << odd << " numWithThisPower: " << numWithThisPower << endl;
            int64_t numWaysToChoose = nCr(numWithThisPower, odd, Mod) % Mod;
            numWaysToChoose = (numWaysToChoose * quickPower(2, a.size() - numWithThisPower, Mod)) % Mod;
            thisTerm = (thisTerm + numWaysToChoose) % Mod;
        }
        thisTerm = (thisTerm * powerOf2) % Mod;
        result = (result + thisTerm) % Mod;

        if (!foundHigherThanPowerOf2)
            break;


        powerOf2 <<= 1;
    }
    return result;
}

int main(int argc, char* argv[])
{
#ifdef BRUTE_FORCE
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        cout << 1 << endl;
        const int n = rand() % 20 + 1;
        const int maxA = rand() % 1'000'000;
        cout << n << endl;
        for (int i = 0; i < n; i++)
        {
            cout << rand() % maxA << endl;
        }
        return 0;
    }

#endif
    int T;
    cin >> T;

    buildFactorialLookups(100'000, Mod);

    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        vector<int64_t> a(n);
        for (int i = 0; i < n; i++)
        {
            cin >> a[i];
        }
#ifdef BRUTE_FORCE
        const auto bruteForceResult = bruteForce(a);
        cout << "bruteForceResult: " << bruteForceResult << endl;
        const auto optimizedResult = optimized(a);
        cout << "optimizedResult: " << optimizedResult << endl;
        assert(bruteForceResult == optimizedResult);
#else
        cout << optimized(a) << endl;
#endif
    }

}

