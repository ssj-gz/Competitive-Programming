// Simon St James (ssjgz) - 2017-11-10 12:09
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <array>
#include <list>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <cmath>
#include <cassert>

using namespace std;

vector<int> primesUpToMaxValue;
vector<vector<int>> numbersWithBasis;
vector<vector<int>> numbersContaining;

struct PrimeFactor
{
    PrimeFactor()
        : primeFactorIndex{-1}, primePower{-1}
    {
    }
    PrimeFactor(int primeFactorIndex, int primePower)
        : primeFactorIndex{primeFactorIndex}, primePower{primePower}
    {
        assert(primeFactorIndex >=0 && primeFactorIndex < primesUpToMaxValue.size());
        prime = primesUpToMaxValue[primeFactorIndex];
    }
    void updateValue()
    {
        assert(primeFactorIndex != -1 && primePower != -1);
        value = 1;
        for (auto i = 0; i < primePower; i++)
            value *= prime;
    }
    int primeFactorIndex = 0;
    int prime = 0;
    int primePower = 0;
    int value = -1;
};

constexpr auto maxPrimeFactors = 6;

struct PrimeFactorisation
{
    void updateValue()
    {
        value = 1;
        for (int i = 0; i < numPrimeFactors; i++)
        {
            PrimeFactor& primeFactor = primeFactors[i];
            primeFactor.updateValue();
            value *= primeFactor.value;
        }
    }
    int value = -1;
    void push_back(const PrimeFactor& primeFactor)
    {
        primeFactors[numPrimeFactors] = primeFactor;
        numPrimeFactors++;

    }
    void pop_back()
    {
        assert(numPrimeFactors > 0);
        numPrimeFactors--;
    }
    PrimeFactor& back()
    {
        return primeFactors[numPrimeFactors - 1];
    }
    int numFactors() const
    {
        return numPrimeFactors;
    }
    const PrimeFactor& operator[](int i) const
    {
        return primeFactors[i];
    };
    PrimeFactor& operator[](int i)
    {
        return primeFactors[i];
    };
    private:
        array<PrimeFactor, maxPrimeFactors> primeFactors;
        int numPrimeFactors = 0;
};

vector<PrimeFactorisation> primeFactorisationOf;
vector<vector<int>> allCombinationsOfPrimeFactors;

// Populate the primeFactorisationOf lookup - for each value i in 1 ... maxValue + 1, set primeFactorisationOf[i] to i's prime factorisation1
void generatePrimeFactorLookups(int64_t productSoFar, int maxValue, int minPrimeIndex, PrimeFactorisation& primeFactorisationSoFar, vector<PrimeFactorisation>& lookup)
{
    if (minPrimeIndex >= primesUpToMaxValue.size())
        return;
    assert(productSoFar <= maxValue);

    for (auto primeIndex = minPrimeIndex; primeIndex < primesUpToMaxValue.size(); primeIndex++)
    {
        const auto prime = primesUpToMaxValue[primeIndex];
        if (productSoFar * prime > maxValue)
            return;
        auto nextProductSoFar = productSoFar;
        primeFactorisationSoFar.push_back(PrimeFactor(primeIndex, 0));
        while (nextProductSoFar * prime <= maxValue)
        {
            PrimeFactor& primeFactor = primeFactorisationSoFar.back();
            primeFactor.primePower++;;
            nextProductSoFar *= prime;

            assert(nextProductSoFar >= 0 && nextProductSoFar < lookup.size());
            primeFactorisationSoFar.updateValue();
            assert(primeFactorisationSoFar.value == nextProductSoFar);

            lookup[nextProductSoFar] = primeFactorisationSoFar;

            generatePrimeFactorLookups(nextProductSoFar, maxValue, primeIndex + 1, primeFactorisationSoFar, lookup);

        }
        primeFactorisationSoFar.pop_back();
    }
}

// If there are k prime factors in prime factorisation of number, then compute all 2^k values we can get by using all subsets of its prime factors.
vector<int> findAllCombinationsOfPrimeFactors(int number)
{
    if (!allCombinationsOfPrimeFactors[number].empty())
        return allCombinationsOfPrimeFactors[number];

    const PrimeFactorisation& primeFactorisation = primeFactorisationOf[number];
    //cout << "findAllCombinationsOfPrimeFactors: " << primeFactorisation << endl;
    vector<int> result;
    const auto numDistinctPrimes = primeFactorisation.numFactors();
    result.reserve((1 << numDistinctPrimes));
    for (uint32_t primeFactorInclusionMask = 0; primeFactorInclusionMask < (1 << numDistinctPrimes); primeFactorInclusionMask++)
    {
        int value = 1;
        for (uint32_t i = 0; i < numDistinctPrimes; i++)
        {
            if ((primeFactorInclusionMask & (1 << i)) != 0)
            {
                assert(primeFactorisation[i].value != -1);
                value *= primeFactorisation[i].value;
            }
        }
        result.push_back(value);
    }
    // Cache result.
    allCombinationsOfPrimeFactors[number] = result;
    return result;
}

// The "basis" of a prime factorisation is just the value obtained by multiplying each distinct prime in primeFactorisation together.
PrimeFactorisation basis(const PrimeFactorisation& primeFactorisation)
{
    PrimeFactorisation basis;
    for (int i = 0; i < primeFactorisation.numFactors(); i++)
    {
        basis.push_back({primeFactorisation[i].primeFactorIndex, 1});
    }
    basis.updateValue();
    return basis;
}

int gcdOfNumbersWithSameBasis(const PrimeFactorisation& primeFactorisation1, const PrimeFactorisation& primeFactorisation2)
{
    assert(primeFactorisation1.numFactors() == primeFactorisation2.numFactors());
    const auto numPrimeFactors = primeFactorisation1.numFactors();
    auto gcd = 1;
    for (auto i = 0; i < numPrimeFactors; i++)
    {
        assert(primeFactorisation1[i].primeFactorIndex == primeFactorisation2[i].primeFactorIndex);
        const int prime = primesUpToMaxValue[primeFactorisation1[i].primeFactorIndex];
        const int primePower = min(primeFactorisation1[i].primePower, primeFactorisation2[i].primePower);
        assert(primePower > 0);
        for (auto j = 0; j < primePower; j++)
            gcd *= prime;
    }
    return gcd;
}

int findNumDistinctGCDsInSubmatrix(const vector<int>& a, const vector<int>& b, int r1, int c1, int r2, int c2, int maxValue)
{
    // Create the isInASubset and isInBSubsets.
    vector<bool> isInASubset(maxValue + 1, false);
    for (auto i = r1; i <= r2; i++)
    {
        isInASubset[a[i]] = true;
    }
    vector<bool> isInBSubset(maxValue + 1, false);
    for (auto j = c1; j <= c2; j++)
    {
        isInBSubset[b[j]] = true;
    }

    vector<int64_t> numNumbersInAContaining(maxValue + 1);
    vector<int64_t> numNumbersInBContaining(maxValue + 1);
    for (auto value = 1; value <= maxValue; value++)
    {
        for (const auto& numberContainingValue : numbersContaining[value])
        {
            if (isInASubset[numberContainingValue])
                numNumbersInAContaining[value]++;
            if (isInBSubset[numberContainingValue])
                numNumbersInBContaining[value]++;
        }
    }

    vector<int64_t> numPairsWithGCD(maxValue + 1);
    for (auto basis = 1; basis <= maxValue; basis++)
    {
        if (numbersWithBasis[basis].empty())
            continue;

        // Iterate over all pairs of numbers each of which has basis "basis".
        for (const auto& withBasisInA : numbersWithBasis[basis])
        {
            if (numNumbersInAContaining[withBasisInA] == 0)
                continue;
            for (const auto& withBasisInB : numbersWithBasis[basis])
            {
                if (numNumbersInBContaining[withBasisInB] == 0)
                    continue;
                const auto gcd = gcdOfNumbersWithSameBasis(primeFactorisationOf[withBasisInA], primeFactorisationOf[withBasisInB]);
                const auto numPairsWithThisGCD = numNumbersInAContaining[withBasisInA] * numNumbersInBContaining[withBasisInB];
                assert(numPairsWithThisGCD > 0);
                const auto& gcdPrimeFactorisation = primeFactorisationOf[gcd];
                const auto numPrimeFactorsInGcd = gcdPrimeFactorisation.numFactors();
                for (const auto& subsetGcd : findAllCombinationsOfPrimeFactors(gcd))
                {
                    assert(subsetGcd >= 0 && subsetGcd <= maxValue);
                    const int numPrimeFactorsInSubsetGcd = primeFactorisationOf[subsetGcd].numFactors();
                    // If gcd has an odd number more primes in its factorisation than subsetGcd, then exclude it - else include it
                    // (Inclusion-Exclusion principle).
                    const bool include = (((numPrimeFactorsInGcd - numPrimeFactorsInSubsetGcd) % 2) == 0);
                    numPairsWithGCD[subsetGcd] += (include ? 1 : -1) * numPairsWithThisGCD;
                }
            }
        }

    }

    // Finally compute the numDistinctGcds!
    int numDistinctGcds = 0;
    for (int gcd = 1; gcd <= maxValue; gcd++)
    {
        if (numPairsWithGCD[gcd] > 0)
        {
            numDistinctGcds++;
        }
    }

    return numDistinctGcds;
}


int main(int argc, char** argv)
{
    int n, m, q;

    cin >> n >> m >> q;

    vector<int> a(n);
    for (int i = 0; i < n; i++)
    {
        cin >> a[i];
    }

    vector<int> b(m);
    for (int i = 0; i < m; i++)
    {
        cin >> b[i];
    }

    // Compute list of primesUpToMaxValue, using Sieve of Erastophenes.
    const auto maxValue = max(*max_element(a.begin(), a.end()), *max_element(b.begin(), b.end()));
    vector<bool> primeSieve(maxValue + 1, true);
    for (int factor = 2; factor <= maxValue; factor++)
    {
        if (primeSieve[factor])
        {
            primesUpToMaxValue.push_back(factor);
            auto multiple = 2 * factor;
            while (multiple <= maxValue)
            {
                primeSieve[multiple] = false;
                multiple += factor;
            }
        }
    }

    primeFactorisationOf.resize(maxValue + 1);
    primeFactorisationOf[0].updateValue();
    primeFactorisationOf[1].updateValue();
    PrimeFactorisation primeFactorisationSoFar;
    generatePrimeFactorLookups(1, maxValue, 0, primeFactorisationSoFar, primeFactorisationOf);

    allCombinationsOfPrimeFactors.resize(maxValue + 1);

    numbersWithBasis.resize(maxValue + 1);
    for (int number = 1; number <= maxValue; number++)
    {
        const auto valueOfNumbersBasis = basis(primeFactorisationOf[number]).value;
        numbersWithBasis[valueOfNumbersBasis].push_back(number);
    }

    numbersContaining.resize(maxValue + 1);
    for (const auto& primeFactorisation : primeFactorisationOf)
    {
        const auto allCombinationsOfPrimeFactorisation = findAllCombinationsOfPrimeFactors(primeFactorisation.value);
        for (const auto& combination : allCombinationsOfPrimeFactorisation)
        {
            numbersContaining[combination].push_back(primeFactorisation.value);
        }

    }

    for (int i = 0; i < q; i++)
    {
        int r1, c1, r2, c2;

        cin >> r1 >> c1 >> r2 >> c2;

        const auto result = findNumDistinctGCDsInSubmatrix(a, b, r1, c1, r2, c2, maxValue);
        cout << result << endl;
    }
}

