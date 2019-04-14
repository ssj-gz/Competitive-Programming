// Simon St James (ssjgz) 2019-04-14
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <cassert>


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

int64_t calcXorSum(const vector<int64_t>& a)
{
    int64_t xorSum = 0;
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

        int64_t contributionToSumFromThisPower = 0;
        for (int oddFromNumWithThisPower = 1; oddFromNumWithThisPower <= numWithThisPower; oddFromNumWithThisPower += 2)
        {
            // Choose this odd number of elements that have this powerOf2 ...
            int64_t numWaysToChooseOdd = nCr(numWithThisPower, oddFromNumWithThisPower, Mod) % Mod;
            // ... then choose any number from remaining (i.e. - (a.size() - numWithThisPower)) numbers.
            numWaysToChooseOdd = (numWaysToChooseOdd * quickPower(2, a.size() - numWithThisPower, Mod)) % Mod;

            contributionToSumFromThisPower = (contributionToSumFromThisPower + numWaysToChooseOdd) % Mod;
        }
        contributionToSumFromThisPower = (contributionToSumFromThisPower * powerOf2) % Mod;
        xorSum = (xorSum + contributionToSumFromThisPower) % Mod;

        if (!foundHigherThanPowerOf2)
            break;

        powerOf2 <<= 1;
    }
    return xorSum;
}

int main(int argc, char* argv[])
{
    // Pretty easy one: let's take a Bit's-eye view of things!
    //
    // Take a subset S of A, and calculate the xor of its elements.
    // If the bit binaryDigitNum is set in this xor, then S will
    // contribute 2 ** binaryDigitNum to the xorSum.
    //
    // Thus:
    //
    //  xorSum = sum[ binaryDigitNum: 0 to maxBinaryDigits ]{ 
    //              sum [S is a subset of A and S has bit binaryDigitNum set in its xor ] { 2 ** binaryDigitNum }]
    //            }
    //         = sum[ binaryDigitNum: 0 to maxBinaryDigits ]
    //              { number of subsets S of A that have bit binaryDigitNum set in its xor * 2 ** binaryDigitNum }
    //            }
    //
    // When does an S have bit binaryDigitNum set in its xor? It's when the number of elements in S that have
    // their bit binaryDigitNum set is *odd* (a consequence of the fact that x ^ x == 0).
    //
    // We then have a simple combinatorics problem to solve in order to count such s - see the calculation
    // of numWaysToChooseOdd, above.  And that's pretty much it!
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
        cout << calcXorSum(a) << endl;
    }
}

