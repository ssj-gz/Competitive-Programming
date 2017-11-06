#define BRUTE_FORCE
//#define SUBMISSION
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

vector<int> a;
vector<int> b;
vector<vector<int>> primeFactorIndices;
vector<int> primesUpToMaxValue;

int findResultBruteForce(int r1, int c1, int r2, int c2)
{
    set<int> gcds;
    for (int i = r1; i <= r2; i++)
    {
        for (int j = c1; j <= c2; j++)
        {
            const auto gcdEntry = gcd(a[i], b[j]);
            cout << "i: " << i << " j: " << j <<  " a: " << a[i] << " b: " << b[j] << " gcdEntry: " << gcdEntry << endl;
            gcds.insert(gcdEntry);
        }
    }
    return gcds.size();
}

class AAndBWithGCD
{
    public:
        AAndBWithGCD(int maxValue)
            : maxValue{maxValue},
              m_AsPrimeIndexIterators(maxValue + 1),
              m_BsPrimeIndexIterators(maxValue + 1)
        {
        }
        void setAAndB(const vector<int>& A, const vector<int>& B)
        {
            //cout << "setAAndB" << endl;
            //static int addedToSet = 0;
            //const auto justAddedToSet = A.size() + B.size();
            //addedToSet += justAddedToSet;
            //cout << "addedToSet: " << addedToSet << endl;
            m_A = A;
            m_B = B;
            static int addedToMaps = 0;
#if 0
            for (const auto a : A)
            {
                //cout << " a: " << a << endl;
                for (const auto primeFactorOfAIndex : primeFactorIndices[a])
                {
                    //cout << "  primeFactorOfAIndex: " << primeFactorOfAIndex << endl;
                    m_AsWithPrimeFactorIndex[primeFactorOfAIndex].push_back(a);
                    m_primeIndicesThatDivideAAndB.insert(primeFactorOfAIndex);
                    addedToMaps += 2;
                }
            }
            for (const auto b : B)
            {
                //cout << " b: " << b << endl;
                for (const auto primeFactorOfBIndex : primeFactorIndices[b])
                {
                    //cout << "  primeFactorOfBIndex: " << primeFactorOfBIndex << endl;
                    m_BsWithPrimeFactorIndex[primeFactorOfBIndex].push_back(b);
                    m_primeIndicesThatDivideAAndB.insert(primeFactorOfBIndex);
                    addedToMaps += 2;
                }
            }

            //cout << "addedToMaps: " << addedToMaps << endl;

            auto primeIndexIter = m_primeIndicesThatDivideAAndB.begin();
            while (primeIndexIter != m_primeIndicesThatDivideAAndB.end())
            {
                if (m_AsWithPrimeFactorIndex[*primeIndexIter].empty() || m_BsWithPrimeFactorIndex[*primeIndexIter].empty())
                {
                    //cout << "  erasing prime index: " << *primeIndexIter << endl;
                    primeIndexIter = m_primeIndicesThatDivideAAndB.erase(primeIndexIter);
                }
                else
                {
                    //cout << "  not erasing prime index: " << *primeIndexIter << endl;
                    primeIndexIter++;
                }
            }


            //m_AsWithPrimeFactorIndex.clear();
            //m_BsWithPrimeFactorIndex.clear();
#endif
            for (const auto a : A)
                addToA(a);
            for (const auto b : B)
                addToB(b);
        }
        vector<int> asWithPrimeFactor(int prime, int power)
        {
            int primeToPower = 1;
            for (int i = 0; i < power; i++)
                primeToPower *= prime;
            vector<int> result;
            for (const auto a : m_A)
            {
                if ((a % primeToPower) == 0 && !((a % (primeToPower * prime)) == 0))
                {
                    result.push_back(a);
                } 
            }
            return result;
        }
        vector<int> bsWithPrimeFactor(int prime, int power)
        {
            int primeToPower = 1;
            for (int i = 0; i < power; i++)
                primeToPower *= prime;
            vector<int> result;
            for (const auto b : m_B)
            {
                if ((b % primeToPower) == 0 && !((b % (primeToPower * prime)) == 0))
                {
                    result.push_back(b);
                } 
            }
            return result;
        }
        void addToA(int a)
        {
            for (const auto primeFactorIndex : primeFactorIndices[a])
            {
                const bool needToAddThing = (m_AsWithPrimeFactorIndex[primeFactorIndex].empty() && !m_BsWithPrimeFactorIndex[primeFactorIndex].empty());
                const auto addedPrimeFactorIterIter = m_AsWithPrimeFactorIndex[primeFactorIndex].insert(m_AsWithPrimeFactorIndex[primeFactorIndex].end(), a);
                m_AsPrimeIndexIterators[a].push_back(addedPrimeFactorIterIter);
                if (needToAddThing)
                {
                    assert(m_primeIndicesThatDivideAAndB.find(primeFactorIndex) == m_primeIndicesThatDivideAAndB.end());
                    m_primeIndicesThatDivideAAndB.insert(primeFactorIndex);
                }
            }
        }
        void addToB(int b)
        {
        }
#if 0
        vector<int> asWithPrimeFactorIndex(int primeFactorIndex) const
        {
            const auto asIter = m_AsWithPrimeFactorIndex.find(primeFactorIndex);
            assert(asIter != m_AsWithPrimeFactorIndex.end());
            return asIter->second;
        }
        vector<int> bsWithPrimeFactorIndex(int primeFactorIndex) const
        {
            const auto bsIter = m_BsWithPrimeFactorIndex.find(primeFactorIndex);
            assert(bsIter != m_BsWithPrimeFactorIndex.end());
            return bsIter->second;
        }
#endif
        vector<int> as() const
        {
            return m_A;
        }
        vector<int> bs() const
        {
            return m_B;
        }
        const set<int>& primeIndicesThatDivideAAndB() const
        {
            return m_primeIndicesThatDivideAAndB;
        }
    private:
        int maxValue;
        set<int> m_primeIndicesThatDivideAAndB;
        map<int, list<int>> m_AsWithPrimeFactorIndex;
        map<int, list<int>> m_BsWithPrimeFactorIndex;
        vector<vector<list<int>::iterator>> m_AsPrimeIndexIterators;
        vector<vector<list<int>::iterator>> m_BsPrimeIndexIterators;
        vector<int> m_A;
        vector<int> m_B;

};

void findResult(const AAndBWithGCD& aAndBWithGCD, int productSoFar, int minPrimeIndex, vector<bool>& generatedGcds)
{
    const auto& primeIndicesThatDivideAAndB = aAndBWithGCD.primeIndicesThatDivideAAndB();
    auto primeIndexIter = primeIndicesThatDivideAAndB.lower_bound(minPrimeIndex);

    //if (primeIndexIter == primeIndicesThatDivideAAndB.end() && primeIndexIter != primeIndicesThatDivideAAndB.begin())
        //primeIndexIter--;
    //while (primeIndexIter != primeIndicesThatDivideAAndB.end() && *primeIndexIter > maxPrimeIndex)
    //{
        //cout << "Need to walk back: " << *primeIndexIter << endl;
        //if (primeIndexIter != primeIndicesThatDivideAAndB.begin())
            //primeIndexIter--;
        //else
        //{
            //primeIndexIter = primeIndicesThatDivideAAndB.end();
            //break;
        //}
        //else
        //primeIndexIter = primeIndicesThatDivideAAndB.end();
    //}

#if 0
    cout << " productSoFar: " << productSoFar << " maxPrimeIndex: " << maxPrimeIndex << " primeIndicesThatDivideAAndB: " << endl;
    cout << "  ";
    for (const auto primeIndex : primeIndicesThatDivideAAndB)
    {
        cout << primeIndex << " ";
    }
    cout << endl;
#endif

    if (primeIndexIter == primeIndicesThatDivideAAndB.end())
    {
        //cout << "end - productSoFar: " << productSoFar << endl;
        //if (productSoFar == 1 && aAndBWithGCD
        assert(productSoFar < generatedGcds.size());
        generatedGcds[productSoFar] = true;
        return;
    }

    assert(*primeIndexIter >= minPrimeIndex);
    const auto prime = primesUpToMaxValue[*primeIndexIter];
    //cout << " prime: " << prime << endl;

    // Don't include this prime.
    const int nextMinPrimeIndex = *primeIndexIter + 1;
    //findResult(aAndBWithGCD, prime * productSoFar, nextMaxPrimeIndex, generatedGcds);

    int primePower = 1;
    int primeToPower = prime;
    //vector<int> asWithCurrentPrimePower = aAndBWithGCD.asWithPrimeFactorIndex(*primeIndexIter);
    //vector<int> bsWithCurrentPrimePower = aAndBWithGCD.bsWithPrimeFactorIndex(*primeIndexIter);
    vector<int> asWithCurrentPrimePower = aAndBWithGCD.as();
    vector<int> bsWithCurrentPrimePower = aAndBWithGCD.bs();
#if 0
    for (const auto a : asWithCurrentPrimePower)
    {
        if ((a % prime) == 0)
            cout << a << " is in a and divisible by " << prime << " productSoFar: " << productSoFar << endl;
    }
    for (const auto b : bsWithCurrentPrimePower)
    {
        if ((b % prime) == 0)
            cout << b << " is in b and divisible by " << prime << " productSoFar: " << productSoFar << endl;
    }
#endif
    const int numCopied = asWithCurrentPrimePower.size() + bsWithCurrentPrimePower.size();
    static int totalCopied = 0;
    totalCopied += numCopied;
    //cout << "totalCopied: " << totalCopied << endl;
    assert(!asWithCurrentPrimePower.empty() && !bsWithCurrentPrimePower.empty());
    int maxPowerOfPrime = primePower;

#if 0
    cout << "prime: " << prime << " productSoFar: " << productSoFar << " as: " << endl;
    for (const auto aElement : asWithCurrentPrimePower)
    {
        cout << aElement << " ";
    }
    cout << endl;
    cout << " bs" << endl;
    for (const auto bElement : bsWithCurrentPrimePower)
    {
        cout << bElement << " ";
    }
    cout << endl;
#endif


    vector<vector<int>> asWithPrimePower;
    asWithPrimePower.push_back(asWithCurrentPrimePower);
    vector<vector<int>> bsWithPrimePower;
    bsWithPrimePower.push_back(bsWithCurrentPrimePower);

    while (true)
    {
        vector<int> asWithNextPrimePower;
        vector<int> bsWithNextPrimePower;

        //cout << "primeToPower: " << primeToPower << endl;

        for (const auto a : asWithCurrentPrimePower)
        {
            if ((a % primeToPower) == 0)
                asWithNextPrimePower.push_back(a);
        }
        for (const auto b : bsWithCurrentPrimePower)
        {
            if ((b % primeToPower) == 0)
                bsWithNextPrimePower.push_back(b);
        }

        if (!asWithNextPrimePower.empty())
            asWithPrimePower.push_back(asWithNextPrimePower);
        if (!bsWithNextPrimePower.empty())
            bsWithPrimePower.push_back(bsWithNextPrimePower);

        asWithCurrentPrimePower = asWithNextPrimePower;
        bsWithCurrentPrimePower = bsWithNextPrimePower;

        if (!asWithCurrentPrimePower.empty() || !bsWithCurrentPrimePower.empty())
            maxPowerOfPrime = primePower;
        else
        {
            //cout << "Neither a nor b divisble by primeToPower" << endl;
            break;
        }

        primePower++;
        primeToPower *= prime;
    }

#if 0
    auto eraseDivisibleBy = [](vector<int>& array, int divisor)
    {
        array.erase(remove_if(array.begin(), array.end(), [divisor](const int value) { return (value % divisor) == 0;}), array.end());
    };

    primeToPower = 1;
    for (int primePower = 0; primePower <= maxPowerOfPrime; primePower++)
    {
        //cout << "prime: " << prime << " primePower: " << primePower << endl;
        if (primePower < asWithPrimePower.size())
        {
            eraseDivisibleBy(asWithPrimePower[primePower], primeToPower * prime);
#if 0
            cout << "asWithPrimePower: " << endl;
            for (const auto aElement : asWithPrimePower[primePower])
            {
                cout << aElement << " ";
            }
            cout << endl;
#endif
        }
        if (primePower < bsWithPrimePower.size())
        {
            eraseDivisibleBy(bsWithPrimePower[primePower], primeToPower * prime);
#if 0
            cout << "bsWithPrimePower: " << endl;
            for (const auto bElement : bsWithPrimePower[primePower])
            {
                cout << bElement << " ";
            }
            cout << endl;
#endif
        }

        primeToPower *= prime;
    }

    primeToPower = 1;
    //cout << " computing permutations; prime: " << prime << " maxPowerOfPrime: " << maxPowerOfPrime << " productSoFar: " << productSoFar << endl;
    for (int primePower = 0; primePower <= maxPowerOfPrime; primePower++)
    {
        //cout << "prime: " << prime << "  primePower: " << primePower << " maxPowerOfPrime: " << maxPowerOfPrime << " productSoFar: " << productSoFar << endl;
        const int nextProductSoFar = productSoFar * primeToPower;
        //cout << "nextProductSoFar: " << nextProductSoFar << endl;
        if (primePower < asWithPrimePower.size())
        {
            // As with primePower; Bs with >= primePower.
            //cout << "  Power of prime for a: " << primePower << endl;
            const auto asWithCurrentPrimePower = asWithPrimePower[primePower];
            vector<int> bsWithGreaterOrEqualPrimerPower;
            for (int bPrimePower = primePower; bPrimePower < bsWithPrimePower.size(); bPrimePower++)
            {
                //cout << "   Power of prime for b: " << bPrimePower << endl; 
                //const auto bsWithCurrentPrimePower = bsWithPrimePower[bPrimePower];
                bsWithGreaterOrEqualPrimerPower.insert(bsWithGreaterOrEqualPrimerPower.end(), bsWithPrimePower[bPrimePower].begin(), bsWithPrimePower[bPrimePower].end());
            }
            if (!asWithCurrentPrimePower.empty() && !bsWithGreaterOrEqualPrimerPower.empty())
            {
                AAndBWithGCD nextAAndBWithGCD;
                nextAAndBWithGCD.setAAndB(asWithCurrentPrimePower, bsWithGreaterOrEqualPrimerPower);
                findResult(nextAAndBWithGCD, nextProductSoFar, nextMinPrimeIndex, generatedGcds);
            }
        }
        if (primePower < bsWithPrimePower.size())
        {
            // Bs with primePower; As with > primePower.
            //cout << "  Power of prime for b: " << primePower << endl;
            const auto bsWithCurrentPrimePower = bsWithPrimePower[primePower];
            vector<int> asWithGreaterPrimePower;
            for (int aPrimePower = primePower + 1; aPrimePower < asWithPrimePower.size(); aPrimePower++)
            {
                //cout << "   Power of prime for a: " << aPrimePower << endl;
                asWithGreaterPrimePower.insert(asWithGreaterPrimePower.end(), asWithPrimePower[aPrimePower].begin(), asWithPrimePower[aPrimePower].end());
                //const auto asWithCurrentPrimePower = asWithPrimePower[aPrimePower];
            }
            if (!asWithGreaterPrimePower.empty() && !bsWithCurrentPrimePower.empty())
            {
                AAndBWithGCD nextAAndBWithGCD;
                nextAAndBWithGCD.setAAndB(asWithGreaterPrimePower, bsWithCurrentPrimePower);
                findResult(nextAAndBWithGCD, nextProductSoFar, nextMinPrimeIndex, generatedGcds);
            }
        }

        primeToPower *= prime;
    }
#endif
}

int findResult(int r1, int c1, int r2, int c2, int maxValue)
{
    vector<int> aSubset;
    for (int i = r1; i <= r2; i++)
        aSubset.push_back(a[i]);
    vector<int> bSubset;
    for (int j = c1; j <= c2; j++)
        bSubset.push_back(b[j]);


    cout << "findResult:" << endl;
#if 1
    cout << "aSubset: " << endl;
    for (const auto a : aSubset)
        cout << a << " ";
    cout << endl;
    cout << "bSubset: " << endl;
    for (const auto b : bSubset)
        cout << b << " ";
    cout << endl;
#endif

    vector<bool> generatedGcds(maxValue + 1);
    AAndBWithGCD aAndBWithGCD(maxValue);
    aAndBWithGCD.setAAndB(aSubset, bSubset);
    const int maxPrimeIndex = primesUpToMaxValue.size() - 1;
    findResult(aAndBWithGCD, 1, 0, generatedGcds);

    int numDistinctGCDS = 0;
    for (const auto generatedGCD : generatedGcds)
    {
        if (generatedGCD)
        {
            //cout << "generated: " << generatedGCD << endl;
            numDistinctGCDS++;
        }
        else
        {
            //cout << "not generated " << generatedGCD << endl;
        }
    }
    return numDistinctGCDS;
}


int main(int argc, char** argv)
{
    if (argc == 2)
    {
        srand(time(0));
        const int maxGenValue = 100000;
        const int n = rand() % 5000 + 1;
        const int m = rand() % 5000 + 1;
        const int q = rand() % 100 + 1;
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
    }
    int n, m, q;

    cin >> n >> m >> q;
    cout << "n: " << n << " m: " << m << " q: " << q << endl;

    a.resize(n);
    for (int i = 0; i < n; i++)
    {
        cin >> a[i];
    }

    b.resize(m);
    for (int i = 0; i < m; i++)
    {
        cin >> b[i];
    }

    const auto maxValue = max(*max_element(a.begin(), a.end()), *max_element(b.begin(), b.end()));
    primeFactorIndices.resize(maxValue + 1);
    vector<bool> primeSieve(maxValue + 1, true);
    for (int factor = 2; factor <= maxValue; factor++)
    {
        if (primeSieve[factor])
        {
            assert(isPrime(factor));
            const int primeIndex = primesUpToMaxValue.size();
            primesUpToMaxValue.push_back(factor);
            primeFactorIndices[factor].push_back(primeIndex);
            auto multiple = 2 * factor;
            while (multiple <= maxValue)
            {
                primeSieve[multiple] = false;
                primeFactorIndices[multiple].push_back(primeIndex);
                multiple += factor;
            }
        }
    }

    for (int i = 0; i < primesUpToMaxValue.size(); i++)
    {
        cout << "i: " << i << " primesUpToMaxValue: " << primesUpToMaxValue[i] << endl;
    }

    for (int i = 0; i < q; i++)
    {
        int r1, c1, r2, c2;

        cin >> r1 >> c1 >> r2 >> c2;

        cout << "r1: " << r1 << " c1: " << c1 << " r2: " << r2 << " c2: " << c2 << endl;

        const auto result = findResult(r1, c1, r2, c2, maxValue);
        cout << "result: " << result << endl;
#ifdef BRUTE_FORCE
        const auto resultBruteForce = findResultBruteForce(r1, c1, r2, c2);
        cout << "result: " << result << endl;
        cout << "resultBruteForce: " << resultBruteForce << endl;
        assert(result == resultBruteForce);
#endif
    }
}

