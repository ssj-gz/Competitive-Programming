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
            cout << "maxValue: " << maxValue << endl;
        }
        void setAAndB(const vector<int>& A, const vector<int>& B)
        {
            //cout << "setAAndB" << endl;
            //static int addedToSet = 0;
            //const auto justAddedToSet = A.size() + B.size();
            //addedToSet += justAddedToSet;
            //cout << "addedToSet: " << addedToSet << endl;
            //m_A = A;
            //m_B = B;
            //static int addedToMaps = 0;
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
        vector<int> asWithPrimeFactor(int primeFactorIndex, int power)
        {
            cout << "in asWithPrimeFactor; primeFactorIndex: " << primeFactorIndex << " power: " << power << endl;
            return elementsWithPrimeFactor(primeFactorIndex, power, m_AsWithPrimeFactorIndex[primeFactorIndex]);
        }
        vector<int> bsWithPrimeFactor(int primeFactorIndex, int power)
        {
            cout << "in bsWithPrimeFactor; primeFactorIndex: " << primeFactorIndex << " power: " << power << endl;
            return elementsWithPrimeFactor(primeFactorIndex, power, m_BsWithPrimeFactorIndex[primeFactorIndex]);
        }
        vector<int> elementsWithPrimeFactor(int primeFactorIndex, int power, const list<int>& elementsWithPrimeFactorIndex)
        {
            const int prime = primesUpToMaxValue[primeFactorIndex];
            int primeToPower = 1;
            for (int i = 0; i < power; i++)
                primeToPower *= prime;
            vector<int> result;
            for (const auto x : elementsWithPrimeFactorIndex)
            {
                cout << " x: " << x << " primeToPower: " << primeToPower << endl;
                if ((x % primeToPower) == 0 && !((x % (primeToPower * prime)) == 0))
                {
                    cout << " adding x" << endl;
                    result.push_back(x);
                } 
            }
            return result;
        }
        void addToA(int a)
        {
            cout << "addToA " << a << " m_AsPrimeIndexIterators.size(): " << m_AsPrimeIndexIterators.size() << endl;
            assert(find(m_A.begin(), m_A.end(), a) == m_A.end());
            assert(a < m_AsPrimeIndexIterators.size());
            for (const auto primeFactorIndex : primeFactorIndices[a])
            {
                const bool needToAddThing = (m_AsWithPrimeFactorIndex[primeFactorIndex].empty() && !m_BsWithPrimeFactorIndex[primeFactorIndex].empty());
                const auto addedPrimeFactorIterIter = m_AsWithPrimeFactorIndex[primeFactorIndex].insert(m_AsWithPrimeFactorIndex[primeFactorIndex].end(), a);
                m_AsPrimeIndexIterators[a].push_back(addedPrimeFactorIterIter);
                if (needToAddThing)
                {
                    cout << "adding a's primeFactorIndex " << primeFactorIndex << " to m_primeIndicesThatDivideAAndB" << endl;
                    assert(m_primeIndicesThatDivideAAndB.find(primeFactorIndex) == m_primeIndicesThatDivideAAndB.end());
                    m_primeIndicesThatDivideAAndB.insert(primeFactorIndex);
                }
            }
#ifndef NDEBUG
            m_A.push_back(a);
#endif
        }
        void addToB(int b)
        {
            assert(find(m_B.begin(), m_B.end(), b) == m_B.end());
            for (const auto primeFactorIndex : primeFactorIndices[b])
            {
                const bool needToAddThing = (m_BsWithPrimeFactorIndex[primeFactorIndex].empty() && !m_AsWithPrimeFactorIndex[primeFactorIndex].empty());
                const auto addedPrimeFactorIterIter = m_BsWithPrimeFactorIndex[primeFactorIndex].insert(m_BsWithPrimeFactorIndex[primeFactorIndex].end(), b);
                m_BsPrimeIndexIterators[b].push_back(addedPrimeFactorIterIter);
                if (needToAddThing)
                {
                    cout << "adding b's primeFactorIndex " << primeFactorIndex << " to m_primeIndicesThatDivideAAndB" << endl;
                    assert(m_primeIndicesThatDivideAAndB.find(primeFactorIndex) == m_primeIndicesThatDivideAAndB.end());
                    m_primeIndicesThatDivideAAndB.insert(primeFactorIndex);
                }
            }
#ifndef NDEBUG
            m_B.push_back(b);
#endif
        }
        void removeFromA(int a)
        {
            cout << "removeFromA: " << a << endl;
            assert(a < m_AsPrimeIndexIterators.size());
            assert(find(m_A.begin(), m_A.end(), a) != m_A.end());
            int i = 0;
            for (const auto primeFactorIndex : primeFactorIndices[a])
            {
                cout << "removeFromA: " << a << " i: " << i << " primeFactorIndex: " << primeFactorIndex << " prime: " << primesUpToMaxValue[primeFactorIndex] << " m_AsPrimeIndexIterators[a].size(): " << m_AsPrimeIndexIterators[a].size() << endl;
                assert(i < m_AsPrimeIndexIterators[a].size());
                assertHasIterator(m_AsWithPrimeFactorIndex[primeFactorIndex], m_AsPrimeIndexIterators[a][i]);
                m_AsWithPrimeFactorIndex[primeFactorIndex].erase(m_AsPrimeIndexIterators[a][i]);
                if (m_AsWithPrimeFactorIndex[primeFactorIndex].empty() && !m_BsWithPrimeFactorIndex[primeFactorIndex].empty())
                {
                    assert(m_primeIndicesThatDivideAAndB.find(primeFactorIndex) != m_primeIndicesThatDivideAAndB.end());
                    m_primeIndicesThatDivideAAndB.erase(primeFactorIndex);
                }
                i++;
            }
            m_AsPrimeIndexIterators[a].clear();
#ifndef NDEBUG
            m_A.erase(find(m_A.begin(), m_A.end(), a), m_A.end());
#endif
        }
        void removeFromB(int b)
        {
            assert(find(m_B.begin(), m_B.end(), b) != m_B.end());
            int i = 0;
            for (const auto primeFactorIndex : primeFactorIndices[b])
            {
                m_BsWithPrimeFactorIndex[primeFactorIndex].erase(m_BsPrimeIndexIterators[b][i]);
                if (m_BsWithPrimeFactorIndex[primeFactorIndex].empty() && !m_AsWithPrimeFactorIndex[primeFactorIndex].empty())
                {
                    assert(m_primeIndicesThatDivideAAndB.find(primeFactorIndex) != m_primeIndicesThatDivideAAndB.end());
                    m_primeIndicesThatDivideAAndB.erase(primeFactorIndex);
                }
                i++;
            }
            m_AsPrimeIndexIterators[b].clear();
#ifndef NDEBUG
            m_B.erase(find(m_B.begin(), m_B.end(), b), m_B.end());
#endif
        }
        void assertHasIterator(list<int>& l, list<int>::iterator it)
        {
#ifndef NDEBUG
            auto i = l.begin();
            while (i != l.end())
            {
                if (i == it)
                    return;
            }
            assert(false && "Could not find iterator!");
#endif
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

void findResult(AAndBWithGCD& aAndBWithGCD, int productSoFar, int minPrimeIndex, vector<bool>& generatedGcds)
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
    const int primeFactorIndex = *primeIndexIter;
    const auto prime = primesUpToMaxValue[primeFactorIndex];
    const int nextMinPrimeIndex = *primeIndexIter + 1;
    //findResult(aAndBWithGCD, prime * productSoFar, nextMaxPrimeIndex, generatedGcds);

    //int primePower = 1;
    //int primeToPower = prime;
    //vector<int> asWithCurrentPrimePower = aAndBWithGCD.asWithPrimeFactorIndex(*primeIndexIter);
    //vector<int> bsWithCurrentPrimePower = aAndBWithGCD.bsWithPrimeFactorIndex(*primeIndexIter);
    //vector<int> asWithCurrentPrimePower = aAndBWithGCD.as();
    //vector<int> bsWithCurrentPrimePower = aAndBWithGCD.bs();
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
    //const int numCopied = asWithCurrentPrimePower.size() + bsWithCurrentPrimePower.size();
    //static int totalCopied = 0;
    //totalCopied += numCopied;
    ////cout << "totalCopied: " << totalCopied << endl;
    //assert(!asWithCurrentPrimePower.empty() && !bsWithCurrentPrimePower.empty());
    //int maxPowerOfPrime = primePower;

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
    vector<vector<int>> bsWithPrimePower;
    // The number of elements that aren't divisible by this prime aren't (very) efficiently
    // computed, as we won't ask for them directly anyway; set them to a dummy value (empty vector).
    asWithPrimePower.push_back(vector<int>()); 
    bsWithPrimePower.push_back(vector<int>()); 

    int primePower = 1;
    int maxPowerOfPrime = primePower;
    while (true)
    {
        const vector<int> asWithCurrentPrimePower = aAndBWithGCD.asWithPrimeFactor(primeFactorIndex, primePower);
        const vector<int> bsWithCurrentPrimePower = aAndBWithGCD.bsWithPrimeFactor(primeFactorIndex, primePower);

        cout << "prime: " << prime << " primePower: " << primePower << " asWithCurrentPrimePower: " << endl;
        for (const auto a : asWithCurrentPrimePower)
            cout << a << " ";
        cout << endl;
        cout << "prime: " << prime << " primePower: " << primePower << " bsWithCurrentPrimePower: " << endl;
        for (const auto b : bsWithCurrentPrimePower)
            cout << b << " ";
        cout << endl;

        //cout << "primeToPower: " << primeToPower << endl;

        if (!asWithCurrentPrimePower.empty())
            asWithPrimePower.push_back(asWithCurrentPrimePower);
        if (!bsWithCurrentPrimePower.empty())
            bsWithPrimePower.push_back(bsWithCurrentPrimePower);

        if (!asWithCurrentPrimePower.empty() || !bsWithCurrentPrimePower.empty())
            maxPowerOfPrime = primePower;
        else
        {
            //cout << "Neither a nor b divisble by primeToPower" << endl;
            break;
        }

        primePower++;
        //primeToPower *= prime;
    }
    assert(maxPowerOfPrime > 0);

    // Ensure that a has no elements divisible by current prime.
    for (int primePower = 1; primePower < asWithPrimePower.size(); primePower++)
    {
        for (const auto x : asWithPrimePower[primePower])
            aAndBWithGCD.removeFromA(x);
    }
    // Recurse - this will find some of the gcds that are not divisible by this prime.
    findResult(aAndBWithGCD, productSoFar, nextMinPrimeIndex, generatedGcds);
    for (int primePower = 1; primePower < min(asWithPrimePower.size(), bsWithPrimePower.size()); primePower++)
    {
        assert(!asWithPrimePower[primePower].empty() && !bsWithPrimePower[primePower].empty());
        if (primePower > 1)
        {
            for (const auto x : asWithPrimePower[primePower - 1])
                aAndBWithGCD.removeFromA(x);
        }
        for (const auto x : asWithPrimePower[primePower])
            aAndBWithGCD.addToA(x);
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

