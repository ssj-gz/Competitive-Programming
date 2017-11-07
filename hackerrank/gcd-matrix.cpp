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
        AAndBWithGCD()
        {
        }
        void setAAndB(const vector<int>& A, const vector<int>& B)
        {
            //cout << "setAAndB" << endl;
            //static int addedToSet = 0;
            //const auto justAddedToSet = A.size() + B.size();
            //addedToSet += justAddedToSet;
            //cout << "addedToSet: " << addedToSet << endl;
            //m_dbgA = A;
            //m_dbgB = B;
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
            addToA(A);
            addToB(B);

            verifyBIters();
        }
        void moveBsFrom(AAndBWithGCD& source)
        {
            m_BsWithPrimeFactorIndex = std::move(source.m_BsWithPrimeFactorIndex);
            m_BsPrimeIndexIterators = std::move(source.m_BsPrimeIndexIterators);
            source.m_BsWithPrimeFactorIndex.clear();
            source.m_BsPrimeIndexIterators.clear();
#ifndef NDEBUG
            m_dbgB = std::move(source.m_dbgB);
#endif
            verifyBIters();
            source.verifyBIters();
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
        void addToA(const vector<int>& as)
        {
            for (const auto a : as)
                addToA(a);
            verifyBIters();
        }
        void addToB(const vector<int>& bs)
        {
            for (const auto b : bs)
                addToB(b);
            verifyBIters();
        }
        void addToA(int a)
        {
            verifyBIters();
            cout << "addToA " << a << " m_AsPrimeIndexIterators.size(): " << m_AsPrimeIndexIterators.size() << endl;
            assert(find(m_dbgA.begin(), m_dbgA.end(), a) == m_dbgA.end());
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
            m_dbgA.push_back(a);
#endif
            verifyBIters();
        }
        void addToB(int b)
        {
            verifyBIters();
            cout << "addToB: " << b << endl;
            assert(find(m_dbgB.begin(), m_dbgB.end(), b) == m_dbgB.end());
            assert(m_BsPrimeIndexIterators[b].empty());
            for (const auto primeFactorIndex : primeFactorIndices[b])
            {
                const bool needToAddThing = (m_BsWithPrimeFactorIndex[primeFactorIndex].empty() && !m_AsWithPrimeFactorIndex[primeFactorIndex].empty());
                cout << "Bleep: " << b << " primeFactorIndex: " << primeFactorIndex << " blah: " << m_BsWithPrimeFactorIndex[primeFactorIndex].size() << endl;
                auto& glomp = m_BsWithPrimeFactorIndex[primeFactorIndex];
                const auto addedPrimeFactorIterIter = glomp.insert(glomp.end(), b);
                cout << "Adding iterator for m_BsWithPrimeFactorIndex[" << primeFactorIndex << "] at index " << m_BsPrimeIndexIterators[b].size() << " in m_BsPrimeIndexIterators[b] b = " << b << endl;
                m_BsPrimeIndexIterators[b].push_back(addedPrimeFactorIterIter);
                if (needToAddThing)
                {
                    cout << "adding b's primeFactorIndex " << primeFactorIndex << " to m_primeIndicesThatDivideAAndB" << endl;
                    assert(m_primeIndicesThatDivideAAndB.find(primeFactorIndex) == m_primeIndicesThatDivideAAndB.end());
                    m_primeIndicesThatDivideAAndB.insert(primeFactorIndex);
                }
                cout << "directly verify iter for b: " << b << endl;
                assertHasIterator(glomp, addedPrimeFactorIterIter);
                cout << "done directly verify iter for b: " << b << endl;
            }
            verifyBIters();
#ifndef NDEBUG
            m_dbgB.push_back(b);
#endif
        }
        void removeFromA(const vector<int>& as)
        {
            for (const auto a : as)
                removeFromA(a);
        }
        void removeFromB(const vector<int>& bs)
        {
            for (const auto b : bs)
                removeFromB(b);
        }
        void removeFromA(int a)
        {
            cout << "removeFromA: " << a << endl;
            verifyBIters();
            assert(find(m_dbgA.begin(), m_dbgA.end(), a) != m_dbgA.end());
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
            m_dbgA.erase(remove(m_dbgA.begin(), m_dbgA.end(), a), m_dbgA.end());
#endif
            verifyBIters();
        }
        void removeFromB(int b)
        {
            cout << "removeFromB: " << b << endl;
            assert(find(m_dbgB.begin(), m_dbgB.end(), b) != m_dbgB.end());
            int i = 0;
            for (const auto primeFactorIndex : primeFactorIndices[b])
            {
                cout << "b: " << b << " i: " << i << " primeFactorIndex: " << primeFactorIndex <<  endl;
                assert(i < m_BsPrimeIndexIterators[b].size());
                assertHasIterator(m_BsWithPrimeFactorIndex[primeFactorIndex], m_BsPrimeIndexIterators[b][i]);
                m_BsWithPrimeFactorIndex[primeFactorIndex].erase(m_BsPrimeIndexIterators[b][i]);
                if (m_BsWithPrimeFactorIndex[primeFactorIndex].empty() && !m_AsWithPrimeFactorIndex[primeFactorIndex].empty())
                {
                    assert(m_primeIndicesThatDivideAAndB.find(primeFactorIndex) != m_primeIndicesThatDivideAAndB.end());
                    m_primeIndicesThatDivideAAndB.erase(primeFactorIndex);
                }
                i++;
            }
            m_BsPrimeIndexIterators[b].clear();
#ifndef NDEBUG
            m_dbgB.erase(remove(m_dbgB.begin(), m_dbgB.end(), b), m_dbgB.end());
#endif
            verifyBIters();
        }
        void assertHasIterator(list<int>& l, list<int>::iterator it)
        {
#ifndef NDEBUG
            auto i = l.begin();
            while (i != l.end())
            {
                if (i == it)
                    return;
                i++;
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
#ifndef NDEBUG
        vector<int> as() const
        {
            return m_dbgA;
        }
        vector<int> bs() const
        {
            return m_dbgB;
        }
#endif
        const set<int>& primeIndicesThatDivideAAndB() const
        {
            return m_primeIndicesThatDivideAAndB;
        }
        void verifyBIters()
        {
#if 0
            cout << "Performing verifyBIters" << endl;
            for (const auto b : m_dbgB)
            {
                int i = 0;
                for (const auto primeFactorIndex : primeFactorIndices[b])
                {
                    cout << " verifyBIters b: " << b << " i: " << i << " primeFactorIndex: " << primeFactorIndex <<  endl;
                    assert(i < m_BsPrimeIndexIterators[b].size());
                    assertHasIterator(m_BsWithPrimeFactorIndex[primeFactorIndex], m_BsPrimeIndexIterators[b][i]);
                    i++;
                }
            }
            cout << "Done Performing verifyBIters" << endl;
#endif
        }
    private:
        set<int> m_primeIndicesThatDivideAAndB;
        map<int, list<int>> m_AsWithPrimeFactorIndex;
        map<int, list<int>> m_BsWithPrimeFactorIndex;
        map<int, vector<list<int>::iterator>> m_AsPrimeIndexIterators;
        map<int, vector<list<int>::iterator>> m_BsPrimeIndexIterators;
#ifndef NDEBUG
        vector<int> m_dbgA;
        vector<int> m_dbgB;
#endif

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
    cout << "  ";
    for (const auto primeIndex : primeIndicesThatDivideAAndB)
    {
        cout << primeIndex << " ";
    }
    cout << endl;
#endif

    if (primeIndexIter == primeIndicesThatDivideAAndB.end())
    {
        cout << "end - productSoFar: " << productSoFar << endl;
        //if (productSoFar == 1 && aAndBWithGCD
        assert(productSoFar < generatedGcds.size());
        generatedGcds[productSoFar] = true;
        return;
    }

    assert(*primeIndexIter >= minPrimeIndex);
    const int primeFactorIndex = *primeIndexIter;
    const auto prime = primesUpToMaxValue[primeFactorIndex];
    const int nextMinPrimeIndex = *primeIndexIter + 1;
    cout << "entering findResult; productSoFar: " << productSoFar << " prime: " << prime << endl;
    aAndBWithGCD.verifyBIters();
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
        aAndBWithGCD.removeFromA(asWithPrimePower[primePower]);
    }
    // Recurse - this will find some of the gcds that are not divisible by this prime.
    aAndBWithGCD.verifyBIters();
    findResult(aAndBWithGCD, productSoFar, nextMinPrimeIndex, generatedGcds);
    aAndBWithGCD.verifyBIters();

    {
        cout << "About to move: b's in source: " << endl;
        for (const auto b : aAndBWithGCD.bs())
        {
            cout << b << " ";
        }
        cout << endl;
        // Construct a AAndBWithGCD where all a's are divisible by prime, but none of 
        // the B's are.
        for (int bPrimePower = 1; bPrimePower < bsWithPrimePower.size(); bPrimePower++)
        {
            //cout << "Trying to remove bsWithPrimePower " << bPrimePower << endl;
            //for (const auto b : bsWithPrimePower[bPrimePower])
            //{
                //cout << b << " ";
            //}
            //cout << endl;
            aAndBWithGCD.removeFromB(bsWithPrimePower[bPrimePower]);
            //cout << ".. done; remaining" << endl;
            //for (const auto b : aDivisibleByPrimeBNotDivisibleByPrime.bs())
            //{
                //cout << b << " ";
            //}
            //cout << endl;
        }
        const auto dbgBs = aAndBWithGCD.bs();
        aAndBWithGCD.verifyBIters();
        AAndBWithGCD aDivisibleByPrimeBNotDivisibleByPrime;
        // Construction of aDivisibleByPrime - with all B's information intact - is O(1).
        aDivisibleByPrimeBNotDivisibleByPrime.moveBsFrom(aAndBWithGCD);
        cout << "Done move: b's in dest: " << endl;
        for (const auto b : aDivisibleByPrimeBNotDivisibleByPrime.bs())
        {
            cout << b << " ";
        }
        cout << endl;
        aDivisibleByPrimeBNotDivisibleByPrime.verifyBIters();
        for (int primePower = 1; primePower < asWithPrimePower.size(); primePower++)
        {
            aDivisibleByPrimeBNotDivisibleByPrime.addToA(asWithPrimePower[primePower]);
        }
        aDivisibleByPrimeBNotDivisibleByPrime.verifyBIters();
        // Recurse - this finds the remaining gcds not divisible by this prime.
        findResult(aDivisibleByPrimeBNotDivisibleByPrime, productSoFar, nextMinPrimeIndex, generatedGcds);

        // Restore aAndBWithGCD (constant-time).
        aDivisibleByPrimeBNotDivisibleByPrime.verifyBIters();
        aAndBWithGCD.moveBsFrom(aDivisibleByPrimeBNotDivisibleByPrime);
        aAndBWithGCD.verifyBIters();
        assert(aAndBWithGCD.bs() == dbgBs);
        for (int bPrimePower = 1; bPrimePower < bsWithPrimePower.size(); bPrimePower++)
        {
            cout << "bPrimePower: " << bPrimePower << " / " << bsWithPrimePower.size() - 1 <<  endl;
            aAndBWithGCD.addToB(bsWithPrimePower[bPrimePower]);
        }
        cout << "restored aAndBWithGCD - bs: " << endl;
        for (const auto b : aAndBWithGCD.bs())
        {
            cout << b << " ";
        }
        cout << endl;
    }

    {
        cout << "prime: " << prime << " About to do 1) and 2), perhaps" << endl;
        if (asWithPrimePower.size() > 1 && bsWithPrimePower.size() > 1)
        {
            cout << " ... yep, doing 1) and 2)" << endl;
            // TODO  - arrange for a and b divisible by prime, but 1) power of a fixed; power of b at least power of a and
            // 2) power of b fixed; power of a strictly larger than power of b.
            {
                // Do 1).
                cout << " doing 1) maxPowerOfPrime: " << maxPowerOfPrime << endl;
                AAndBWithGCD aAndBDivisibleByPrime;
                //aAndBDivisibleByPrime.addToA(asWithPrimePower[1]);
                for (int bPrimePower = 1; bPrimePower < bsWithPrimePower.size(); bPrimePower++)
                {
                    cout << "adding b's to aAndBDivisibleByPrime:" << endl;
                    for (const auto b : bsWithPrimePower[bPrimePower])
                    {
                        cout << b << " ";
                    }
                    cout << endl;
                    aAndBDivisibleByPrime.addToB(bsWithPrimePower[bPrimePower]);
                }
                int primeToPower = prime;
                for (int primePower = 1; primePower <= maxPowerOfPrime; primePower++)
                {
                    cout << "1) prime: " << prime << " primePower: " << primePower << " primeToPower: " << primeToPower << " asWithPrimePower.size() " << asWithPrimePower.size() << " bsWithPrimePower.size(): " << bsWithPrimePower.size() << endl;
                    if (primePower > 1)
                    {
                        aAndBDivisibleByPrime.removeFromA(asWithPrimePower[primePower - 1]);
                    }
                    if (primePower < asWithPrimePower.size())
                        aAndBDivisibleByPrime.addToA(asWithPrimePower[primePower]);
                    else
                        break;
                    if (primePower >= bsWithPrimePower.size())
                        break;
                    cout << " 1) Before recurse: as's in aAndBDivisibleByPrime: " << endl;
                    for (const auto a : aAndBDivisibleByPrime.as())
                    {
                        cout << a << " " << endl;
                    }
                    cout << endl;
                    cout << " 1) Before recurse: bs's in aAndBDivisibleByPrime: " << endl;
                    for (const auto b : aAndBDivisibleByPrime.bs())
                    {
                        cout << b << " " << endl;
                    }
                    cout << endl;
                    // Recurse.
                    cout << " recursing 1) " << endl;
                    findResult(aAndBDivisibleByPrime, productSoFar * primeToPower, nextMinPrimeIndex, generatedGcds);
                    cout << "After recurse: bs's in aAndBDivisibleByPrime: " << endl;
                    for (const auto b : aAndBDivisibleByPrime.bs())
                    {
                        cout << b << " " << endl;
                    }
                    cout << endl;

                    if (primePower < bsWithPrimePower.size())
                        aAndBDivisibleByPrime.removeFromB(bsWithPrimePower[primePower]);
                    else
                        break;

                    primeToPower *= prime;
                }
            }
            {
                // Do 2).
                cout << " doing 2)" << endl;
                AAndBWithGCD aAndBDivisibleByPrime;
                for (int aPrimePower = 1; aPrimePower < asWithPrimePower.size(); aPrimePower++)
                {
                    aAndBDivisibleByPrime.addToA(asWithPrimePower[aPrimePower]);
                }
                int primeToPower = prime;
                for (int primePower = 1; primePower <= maxPowerOfPrime; primePower++)
                {
                    if (primePower > 1)
                    {
                        if (primePower - 1 < bsWithPrimePower.size())
                            aAndBDivisibleByPrime.removeFromB(bsWithPrimePower[primePower - 1]);
                        else
                            break;
                    }
                    if (primePower < bsWithPrimePower.size())
                        aAndBDivisibleByPrime.addToB(bsWithPrimePower[primePower]);
                    else 
                        break;
                    if (primePower < asWithPrimePower.size())
                        aAndBDivisibleByPrime.removeFromA(asWithPrimePower[primePower]);
                    else
                        break;
                    // Recurse.
                    findResult(aAndBDivisibleByPrime, productSoFar * primeToPower, nextMinPrimeIndex, generatedGcds);

                    primeToPower *= prime;
                }
            }
        }
    }

    // Restore aAndBWithGCD back to its original state.
    for (int primePower = 1; primePower < asWithPrimePower.size(); primePower++)
    {
        aAndBWithGCD.addToA(asWithPrimePower[primePower]);
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
    cout << "Exiting findResult" << endl;
}

int findResult(int r1, int c1, int r2, int c2, int maxValue)
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
    AAndBWithGCD aAndBWithGCD;
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
        const int n = rand() % 10 + 1;
        const int m = rand() % 10 + 1;
        const int q = rand() % 1 + 1;
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

