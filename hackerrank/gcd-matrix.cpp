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
    // *Ouch* - this was a really tough one, and I spent *10s of hours* pursuing dead ends before I finally figured out
    // the correct approach.  The 20% completion rate gave me some solace, but after successfully completing it I looked
    // at the Editorial briefly (I didn't examine it in detail, but it looked far simpler than what I have here)
    // and it turns out that this challenge was part of an *Hourrank* challenge - i.e. those 20% of people have solved this,
    // plus about 4 other challenges, *within an hour*.  Felt very, very stupid after that!
    //
    // Anyway, the basic problem is, given too lists of numbers A and B, of size ~100000 and where each number in the list
    // is less than 100000, to find the number of distinct values of gcd(a, b) (a in A, b in B) that can be produced.
    // The basic strategy is, for each possible gcd (since the numbers in A and B are bounded by 100000, so are the possible
    // gcds), to see if we can generate that gcd using a pair from A and B.  More specifically, we use a concept that I'm
    // calling the "basis" of a number to home-in on subsets of A and B in which pairs from these subsets are not co-prime and whose
    // gcd can be predicted to a degree, and then count the number of ways of picking from A and B to generate this gcd using
    // a counting argument that takes into account over and under counting using the Inclusion-Exclusion Principle.  All
    // clear as mud, so let's describe this all more fully!
    //
    // Let maxValue be the maximum value of an element on A or B, and find all the primes up to maxValue.  Let's say there are
    // NP such primes, and number them p_1, p_2, ... p_NP.  Then any number x in A or B can be expressed as an NP-vector
    // (k_1, k_2, ... , k_NP) where x = p_1^k_1 + p_2^k_2 + ... + p_NP^k_NP, where each k_i >= 0.  Let pvec(x) be this (unique) NP-vector
    // representing x.  Sometimes I'll be using x and pvec(x) interchangeably; it's hopefully clear when I'm doing this from context.
    //
    // Given a y in A or B, we say that y *contains* x if and only if for each non-zero entry in pvec(x), the corresponding entry in
    // pvec(y) is equal to that entry in pvec(x) i.e. y contains x if and only if, for all i such that pvec(x)[i] != 0, pvec(y)[i] = pvec(x)[i].
    // Thus, if x = 3^2*5*7^3, then 2*3^2*5*7^3*11 contains x, but 2*3*7^3 and 3^2*5*7^4 do not.
    //
    // The *basis* of x is the set of p_i's such that the ith entry in pvec(x) is non-zero (essentially corresponding to the set of indices of primes
    // that make up x).  Thus .e.g the basis of 3^2 * 11 is {3, 11} since x is divisible by (and only by) the primes 3 and 11.
    //
    // Given x and y, it is easily seen that the NP-vector v representing gcd(x, y) satisfies v[i] = min(pvec(x)[i], pvec(y)[i]) for all i = 1 ... NP,
    // so is easily computed if we know pvec(x) and pvec(y).
    //
    // For each x in 1 ... maxValue, we'd like to know whether or not there is a pair a in A, b in B such that gcd(a,b) = x.  Imagine x = 3^3*5*11^2;
    // then restricting our attention to pairs a, b such that a and b *contain* the basis of x i.e. a and b that contain {5, 7, 11}.  We can narrow it 
    // down further: using the formula for gcd above, we want to consider the pairs satisfying the following:
    //
    //  i) a contains 3^3 * 5 * 11^2, as does b.
    //  ii) a contains 3^4 * 5 * 11^2, b contains 3^3 * 5 * 11^2 (and vice-versa).
    //  iii) a contains 3^3 * 5^2 * 11^2, b contains 3^3 * 5 * 11^2 (and vice-versa)
    //
    // and that's all - since a and b must be less than maxValue = 100'000, there's not much wiggle-room for changing powers of the 3, 5 and 11 without
    // exceeding maxValue.  If we pick a and b satisfying any of these five (condition i) or condition ii) or condition iii) or reverse of condition ii),
    // or reverse of condition iii)) conditions, then in their gcd the power of 3 will be 3, the power of 5, 1, and the power of 11, 2 i.e. the gcd of 
    // a, b will *contain* the required x.  So it seems that perhaps counting the number of pairs of a and b which satisfy any of the five conditions
    // will give us the number of pairs with gcd(a, b) = x.
    //
    // Unfortunately, of course, just because a gcd contains x, does not mean that the gcd equals x!
    //
    // Imagine A = 3^3*5*11^2, 2*3^4*5*11^2, 3^4*5*7*11^2, 2*3^3*5*7*11^2 and
    //         B = 2*3^3*5*11^2, 3^4*5*11^2, 3^3*5*7*11^2, 2*3^3*5*7*11^2.
    //
    // (not a very realistic example, since some of the elements in A and B exceed maxValue, but it will do to illustrate the point!).
    //
    // If we choose all a, b satisfying i), we get
    //
    //   A' = 3^3*5*11^2, 2*3^3*5*7*11^2
    //   B' = 2*3^3*5*11^2, 3^3*5*7*11^2, 2*3^3*5*7*11^2.
    //
    // and we see that if we pick a = 2*3^3*5*7*11^2 and b = 2*3^3*5*11^2, we get gcd(a, b) = 2*3^3*5*11^2, which is not the desired x = 3^3*5*11^2.
    // Picking a = 2*3^3*5*7*11^2 and b = 2*3^3*5*7*11^2 gives gcd(a, b) = 2*3^3*5*7*11^2, which again is not the desired x.
    //
    // Likewise, if we choose all a,b satisfying ii), then we get
    //
    //   A' = 2*3^4*5*11^2, 3^4*5*7*11^2
    //   B = 2*3^3*5*11^2, 3^3*5*7*11^2, 2*3^3*5*7*11^2
    //
    // and picking a = 3^4*5*7*11^2 and b = 3^3*5*7*11^2 gives gcd(a, b) = 3^3*5*7*11^2.
    //
    // I actually picked A, B especially so that most pairs satisfy one of the conditions; let's make a table of those that do!
    //
    //        a              b             gcd(a, b)        condition satisfied
    //  3^3*5*11^2      2*3^3*5*11^2       3^3*5*11^2             i)
    //  3^3*5*11^2      3^4*5*11^2         3^3*5*11^2             ii)-reversed
    //  3^3*5*11^2      3^3*5*7*11^2       3^3*5*11^2             i)
    //  3^3*5*11^2      3^3*5*7*11^2       3^3*5*11^2             i)
    //  2*3^4*5*11^2    2*3^3*5*11^2       2*3^3*5*11^2           ii)
    //  2*3^4*5*11^2    3^3*5*7*11^2       3^3*5*11^2             ii)
    //  2*3^4*5*11^2    2*3^3*5*7*11^2     2*3^3*5*11^2           ii)
    //  3^4*5*7*11^2    2*3^3*5*11^2       3^3*5*11^2             ii)
    //  3^4*5*7*11^2    3^3*5*7*11^2       3^3*5*7*11^2           ii)
    //  3^4*5*7*11^2    2*3^3*5*7*11^2     3^3*5*7*11^2           ii)
    //  2*3^3*5*7*11^2  2*3^3*5*11^2       2*3^3*5*11^2           i)
    //  2*3^3*5*7*11^2  3^4*5*11^2         3^3*5*11^2             ii)-reversed
    //  2*3^3*5*7*11^2  3^3*5*7*11^2       3^3*5*7*11^2           i)
    //  2*3^3*5*7*11^2  2*3^3*5*7*11^2     2*3^3*5*7*11^2         i)
    //
    // The number of pairs of a and b that satisfy any of the conditions is 14, and naively, we might have expected that
    // the number of pairs that generated x = 3^3*5*11^2 was 14.  But instead, it is 7 - the other pairs make
    // 2*3^3*5*11^2 (3 pairs), 3^3*5*7*11^2 (3 pairs) and 2*3^3*5*7*11^2 (1 pair).
    //
    // Actually, I'm just going to stop documenting, now - this whole solution is really stupid and the Editorial
    // one is vastly better.
    //
    // Just ignore this whole thing XD
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

    // Compute list of primesUpToMaxValue, using Sieve of Eratosthenes.
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

