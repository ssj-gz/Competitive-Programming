// Simon St James (ssjgz) - 2017-11-10 12:09
#define BRUTE_FORCE
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#undef BRUTE_FORCE
#endif
#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <cmath>
#include <cassert>

using namespace std;

int gcd(int a, int b)
{
    while (b != 0)
    {
        int t = b; 
        b = a % b; 
        a = t; 
    }
    return a;
}

bool isPrime(int n)
{
    for (int i = 2; i <= sqrt(n) + 1; i++)
    {
        if (i < n && (n % i) == 0)
            return false;
    }
    return true;
}

vector<int> primesUpToMaxValue;
vector<vector<int>> numbersWithBasis;
vector<vector<int>> numbersWith;

struct PrimeFactor
{
    PrimeFactor(int primeFactorIndex, int primePower)
        : primeFactorIndex{primeFactorIndex}, primePower{primePower}
    {
        assert(primeFactorIndex >=0 && primeFactorIndex < primesUpToMaxValue.size());
        prime = primesUpToMaxValue[primeFactorIndex];
    }
    void updateValue()
    {
        value = 1;
        for (int i = 0; i < primePower; i++)
            value *= prime;
    }
    int primeFactorIndex = 0;
    int prime = 0;
    int primePower = 0;
    int value = -1;
};

struct PrimeFactorisation
{
    vector<PrimeFactor> primeFactors;
    void updateValue()
    {
        value = 1;
        for (auto& primeFactor : primeFactors)
        {
            primeFactor.updateValue();
            value *= primeFactor.value;
        }
    }
    int value = -1;
};

ostream& operator<<(ostream& os, const PrimeFactor& primeFactor)
{
    os << primeFactor.prime;
    if (primeFactor.primePower != 1)
        os << "^" << primeFactor.primePower;
    return os;
}

ostream& operator<<(ostream& os, const PrimeFactorisation& primeFactorisation)
{
    for (auto iter = primeFactorisation.primeFactors.begin(); iter != primeFactorisation.primeFactors.end(); iter++)
    {
        os << *iter;
        if (iter + 1 != primeFactorisation.primeFactors.end())
            os << "*";
    }
    return os;
}

vector<PrimeFactorisation> primeFactorisationOf;

void generatePrimeFactorLookups(int64_t productSoFar, int maxValue, int minPrimeIndex, PrimeFactorisation& primeFactorisationSoFar, vector<PrimeFactorisation>& lookup)
{
    if (minPrimeIndex >= primesUpToMaxValue.size())
        return;
    assert(productSoFar <= maxValue);

    for (int primeIndex = minPrimeIndex; primeIndex < primesUpToMaxValue.size(); primeIndex++)
    {
        const int64_t prime = primesUpToMaxValue[primeIndex];
        if (productSoFar * prime > maxValue)
            return;
        int64_t nextProductSoFar = productSoFar;
        primeFactorisationSoFar.primeFactors.push_back(PrimeFactor(primeIndex, 0));
        while (nextProductSoFar * prime <= maxValue)
        {
            PrimeFactor& primeFactor = primeFactorisationSoFar.primeFactors.back();
            primeFactor.primePower++;;
            nextProductSoFar *= prime;

            assert(nextProductSoFar >= 0 && nextProductSoFar < lookup.size());
            primeFactorisationSoFar.updateValue();
            assert(primeFactorisationSoFar.value == nextProductSoFar);
            lookup[nextProductSoFar] = primeFactorisationSoFar;

            generatePrimeFactorLookups(nextProductSoFar, maxValue, primeIndex + 1, primeFactorisationSoFar, lookup);

        }
        primeFactorisationSoFar.primeFactors.pop_back();
    }
}

vector<int> findAllCombinationsOfPrimeFactors(const PrimeFactorisation& primeFactorisation)
{
    vector<int> result;
    const int numDistinctPrimes = primeFactorisation.primeFactors.size();
    for (unsigned int primeFactorInclusionMask = 0; primeFactorInclusionMask < (1 << numDistinctPrimes); primeFactorInclusionMask++)
    {
        int value = 1;
        for (unsigned int i = 0; i < numDistinctPrimes; i++)
        {
            if ((primeFactorInclusionMask & (1 << i)) != 0)
            {
                assert(primeFactorisation.primeFactors[i].value != -1);
                value *= primeFactorisation.primeFactors[i].value;
            }
        }
        result.push_back(value);
    }
    return result;
}

PrimeFactorisation basis(const PrimeFactorisation& primeFactorisation)
{
    PrimeFactorisation basis;
    for (const auto& primeFactor : primeFactorisation.primeFactors)
    {
        basis.primeFactors.push_back({primeFactor.primeFactorIndex, 1});
    }
    basis.updateValue();
    return basis;
}

int gcdOfNumbersWithSameBasis(const PrimeFactorisation& primeFactorisation1, const PrimeFactorisation& primeFactorisation2)
{
    assert(primeFactorisation1.primeFactors.size() == primeFactorisation2.primeFactors.size());
    const auto numPrimeFactors = primeFactorisation1.primeFactors.size();
    int gcd = 1;
    for (int i = 0; i < numPrimeFactors; i++)
    {
        assert(primeFactorisation1.primeFactors[i].primeFactorIndex == primeFactorisation2.primeFactors[i].primeFactorIndex);
        const int prime = primesUpToMaxValue[primeFactorisation1.primeFactors[i].primeFactorIndex];
        const int primePower = min(primeFactorisation1.primeFactors[i].primePower, primeFactorisation2.primeFactors[i].primePower);
        assert(primePower > 0);
        for (int j = 0; j < primePower; j++)
            gcd *= prime;
    }
    return gcd;
}

int findNumDistinctGCDsInSubmatrix(const vector<int>& a, const vector<int>& b, int r1, int c1, int r2, int c2, int maxValue)
{
    vector<int> aSubset;
    for (int i = r1; i <= r2; i++)
        aSubset.push_back(a[i]);
    vector<int> bSubset;
    for (int j = c1; j <= c2; j++)
        bSubset.push_back(b[j]);

    sort(aSubset.begin(), aSubset.end());
    aSubset.erase(unique(aSubset.begin(), aSubset.end()), aSubset.end());

    sort(bSubset.begin(), bSubset.end());
    bSubset.erase(unique(bSubset.begin(), bSubset.end()), bSubset.end());

    vector<bool> isInASubset(maxValue + 1);
    for (const auto aElement : aSubset)
        isInASubset[aElement] = true;
    vector<bool> isInBSubset(maxValue + 1);
    for (const auto bElement : bSubset)
        isInBSubset[bElement] = true;

    vector<int64_t> numPairsWithGCD(maxValue + 1);
    vector<int64_t> numNumbersInAWith(maxValue + 1);
    vector<int64_t> numNumbersInBWith(maxValue + 1);
    for (int i = 1; i <= maxValue; i++)
    {
        for (const auto& numberWith : numbersWith[i])
        {
            if (isInASubset[numberWith])
                numNumbersInAWith[i]++;
            if (isInBSubset[numberWith])
                numNumbersInBWith[i]++;
        }
    }

    for (int basis = 1; basis <= maxValue; basis++)
    {
        if (numbersWithBasis[basis].empty())
            continue;

        for (const auto& withBasisInA : numbersWithBasis[basis])
        {
            if (numNumbersInAWith[withBasisInA] == 0)
                continue;
            for (const auto& withBasisInB : numbersWithBasis[basis])
            {
                if (numNumbersInBWith[withBasisInB] == 0)
                    continue;
                const int64_t numPairsWithThisGCD = numNumbersInAWith[withBasisInA] * numNumbersInBWith[withBasisInB];
                assert(numPairsWithThisGCD > 0);
                const auto gcd = gcdOfNumbersWithSameBasis(primeFactorisationOf[withBasisInA], primeFactorisationOf[withBasisInB]);
                const auto& gcdPrimeFactorisation = primeFactorisationOf[gcd];
                const auto numPrimeFactorsInGcd = gcdPrimeFactorisation.primeFactors.size();
                for (const auto& subsetGcd : findAllCombinationsOfPrimeFactors(gcdPrimeFactorisation))
                {
                    assert(subsetGcd >= 0 && subsetGcd <= maxValue);
                    const int numPrimeFactorsInSubsetGcd = primeFactorisationOf[subsetGcd].primeFactors.size();
                    const bool include = (((numPrimeFactorsInGcd - numPrimeFactorsInSubsetGcd) % 2) == 0);
                    numPairsWithGCD[subsetGcd] += (include ? 1 : -1) * numPairsWithThisGCD;
                }
            }
        }

    }

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
    if (argc == 2)
    {
#if 0
        const int n = 100000;
        cout << n << " " << n << " " << 1 << endl;
        for (int j = 1; j <= 2; j++)
        {
            for (int i = 0; i < n; i++)
            {
                cout << (i + 1) << " ";
            }
            cout << endl;
        }
        for (int i = 0; i < 1; i++)
        {
            cout << "0 0 " << (n - 1) << " " << (n - 1) << endl;
        }
        return 0;
#else
        srand(time(0));
        const int maxGenValue = 30000;
        const int n = rand() % 100 + 1;
        const int m = rand() % 100 + 1;
        const int q = rand() % 1000 + 1;
        cout << n << " " << m << " " << q << " " << endl;
        for (int i = 0; i < n; i++)
        {
            cout  << (rand() % maxGenValue) + 1 << " ";
        }
        cout << endl;
        for (int i = 0; i < m; i++)
        {
            cout  << (rand() % maxGenValue) + 1 << " ";
        }
        cout << endl;
        for (int i = 0; i < q; i++)
        {
            int r1 = rand() % n;
            int r2 = rand() % n;
            if (r2 < r1)
                swap(r1, r2);
            int c1 = rand() % m;
            int c2 = rand() % m;
            if (c2 < c1)
                swap(c1, c2);
            cout << r1 << " " << c1 << " " << r2 << " " << c2 << endl;
        }
        return 0;
#endif
    }
    int n, m, q;

    cin >> n >> m >> q;
    //cout << "n: " << n << " m: " << m << " q: " << q << endl;

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

    const auto maxValue = max(*max_element(a.begin(), a.end()), *max_element(b.begin(), b.end()));
    vector<bool> primeSieve(maxValue + 1, true);
    for (int factor = 2; factor <= maxValue; factor++)
    {
        if (primeSieve[factor])
        {
            assert(isPrime(factor));
            const int primeIndex = primesUpToMaxValue.size();
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

    numbersWithBasis.resize(maxValue + 1);
    for (int number = 1; number <= maxValue; number++)
    {
        const auto valueOfNumbersBasis = basis(primeFactorisationOf[number]).value;
        numbersWithBasis[valueOfNumbersBasis].push_back(number);
    }

    numbersWith.resize(maxValue + 1);
    for (const auto& primeFactorisation : primeFactorisationOf)
    {
        const auto allCombinationsOfPrimeFactorisation = findAllCombinationsOfPrimeFactors(primeFactorisation);
        for (const auto& combination : allCombinationsOfPrimeFactorisation)
        {
            numbersWith[combination].push_back(primeFactorisation.value);
        }

    }

    for (int i = 0; i < q; i++)
    {
        int r1, c1, r2, c2;

        cin >> r1 >> c1 >> r2 >> c2;

        //cout << "r1: " << r1 << " c1: " << c1 << " r2: " << r2 << " c2: " << c2 << " submatrix size: " << (c2 - c1 + 1) * (r2 - r1 + 1) << endl;

        const auto result = findNumDistinctGCDsInSubmatrix(a, b, r1, c1, r2, c2, maxValue);
        cout << result << endl;
#ifdef BRUTE_FORCE
        const auto resultBruteForce = findResultBruteForce(a, b, r1, c1, r2, c2);
        cout << "result: " << result << endl;
        cout << "resultBruteForce: " << resultBruteForce << endl;
        assert(result == resultBruteForce);
#endif
    }
}

