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

vector<int> sorted(const vector<int>& orig)
{
    auto result = orig;
    sort(result.begin(), result.end());
    return result;
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
            addToA(A);
            addToB(B);

        }
        void moveBsFrom(AAndBWithGCD& source)
        {
            m_BsWithPrimeFactorIndex = std::move(source.m_BsWithPrimeFactorIndex);
            m_BsPrimeIndexIterators = std::move(source.m_BsPrimeIndexIterators);
            m_numBs = source.m_numBs;
            source.m_numBs = 0;
#ifndef NDEBUG
            m_dbgB = std::move(source.m_dbgB);
#endif
        }
        vector<int> asWithPrimeFactor(int primeFactorIndex, int power, bool& anyWithThisOrHigherPower)
        {
            //cout << "in asWithPrimeFactor; primeFactorIndex: " << primeFactorIndex << " power: " << power << endl;
            return elementsWithPrimeFactor(primeFactorIndex, power, m_AsWithPrimeFactorIndex[primeFactorIndex], anyWithThisOrHigherPower);
        }
        vector<int> bsWithPrimeFactor(int primeFactorIndex, int power, bool& anyWithThisOrHigherPower)
        {
            //cout << "in bsWithPrimeFactor; primeFactorIndex: " << primeFactorIndex << " power: " << power << endl;
            return elementsWithPrimeFactor(primeFactorIndex, power, m_BsWithPrimeFactorIndex[primeFactorIndex], anyWithThisOrHigherPower);
        }
        vector<int> elementsWithPrimeFactor(int primeFactorIndex, int power, const list<int>& elementsWithPrimeFactorIndex, bool& anyWithThisOrHigherPower)
        {
            const int prime = primesUpToMaxValue[primeFactorIndex];
            int primeToPower = 1;
            for (int i = 0; i < power; i++)
                primeToPower *= prime;
            vector<int> result;
            for (const auto x : elementsWithPrimeFactorIndex)
            {
                cout << " x: " << x << " primeToPower: " << primeToPower << endl;
                if ((x % primeToPower) == 0)
                {
                    anyWithThisOrHigherPower = true;
                    if (!((x % (primeToPower * prime)) == 0))
                    {
                        cout << " adding x" << endl;
                        result.push_back(x);
                    }
                } 
            }
            return result;
        }
        void addToA(const vector<int>& as)
        {
            for (const auto a : as)
                addToA(a);
        }
        void addToB(const vector<int>& bs)
        {
            for (const auto b : bs)
                addToB(b);
        }
        int numAs() const
        {
            return m_numAs;
        }
        int numBs() const
        {
            return m_numBs;
        }
        void addToA(int a)
        {
            assert(find(m_dbgA.begin(), m_dbgA.end(), a) == m_dbgA.end());
            for (const auto primeFactorIndex : primeFactorIndices[a])
            {
                const bool needToAddThing = (m_AsWithPrimeFactorIndex[primeFactorIndex].empty() && !m_BsWithPrimeFactorIndex[primeFactorIndex].empty());
                const auto addedPrimeFactorIterIter = m_AsWithPrimeFactorIndex[primeFactorIndex].insert(m_AsWithPrimeFactorIndex[primeFactorIndex].end(), a);
                cout << "adding a: " << a << " prime: " << primesUpToMaxValue[primeFactorIndex] << endl;
                m_AsPrimeIndexIterators[a].push_back(addedPrimeFactorIterIter);
                if (needToAddThing)
                {
                    assert(m_primeIndicesThatDivideAAndB.find(primeFactorIndex) == m_primeIndicesThatDivideAAndB.end());
                    m_primeIndicesThatDivideAAndB.insert(primeFactorIndex);
                }
            }
            m_numAs++;
#ifndef NDEBUG
            m_dbgA.push_back(a);
#endif
        }
        void addToB(int b)
        {
            cout << "addToB: " << b << endl;
            assert(find(m_dbgB.begin(), m_dbgB.end(), b) == m_dbgB.end());
            assert(m_BsPrimeIndexIterators[b].empty());
            for (const auto primeFactorIndex : primeFactorIndices[b])
            {
                const bool needToAddThing = (m_BsWithPrimeFactorIndex[primeFactorIndex].empty() && !m_AsWithPrimeFactorIndex[primeFactorIndex].empty());
                auto& glomp = m_BsWithPrimeFactorIndex[primeFactorIndex];
                const auto addedPrimeFactorIterIter = glomp.insert(glomp.end(), b);
                cout << "adding b: " << b << " prime: " << primesUpToMaxValue[primeFactorIndex] << endl;
                m_BsPrimeIndexIterators[b].push_back(addedPrimeFactorIterIter);
                if (needToAddThing)
                {
                    assert(m_primeIndicesThatDivideAAndB.find(primeFactorIndex) == m_primeIndicesThatDivideAAndB.end());
                    m_primeIndicesThatDivideAAndB.insert(primeFactorIndex);
                }
            }
            m_numBs++;
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
            m_numAs--;
            assert(m_numAs >= 0);
#ifndef NDEBUG
            m_dbgA.erase(remove(m_dbgA.begin(), m_dbgA.end(), a), m_dbgA.end());
#endif
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
            m_numBs--;
            assert(m_numBs >= 0);
#ifndef NDEBUG
            m_dbgB.erase(remove(m_dbgB.begin(), m_dbgB.end(), b), m_dbgB.end());
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
                i++;
            }
            assert(false && "Could not find iterator!");
#endif
        }
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
    private:
        set<int> m_primeIndicesThatDivideAAndB;
        map<int, list<int>> m_AsWithPrimeFactorIndex;
        map<int, list<int>> m_BsWithPrimeFactorIndex;
        map<int, vector<list<int>::iterator>> m_AsPrimeIndexIterators;
        map<int, vector<list<int>::iterator>> m_BsPrimeIndexIterators;
        int m_numAs = 0;
        int m_numBs = 0;
#ifndef NDEBUG
        vector<int> m_dbgA;
        vector<int> m_dbgB;
#endif

};

void findResult(AAndBWithGCD& aAndBWithGCD, int productSoFar, int minPrimeIndex, vector<bool>& generatedGcds)
{
    if (aAndBWithGCD.numAs() == 0 || aAndBWithGCD.numBs() == 0)
    {
        cout << "findResult aborting: a or b empty.  productSoFar: " << productSoFar << endl;
        return;
    }
    const auto& primeIndicesThatDivideAAndB = aAndBWithGCD.primeIndicesThatDivideAAndB();
    auto primeIndexIter = primeIndicesThatDivideAAndB.lower_bound(minPrimeIndex);

    cout << "entering findResult; productSoFar: " << productSoFar << " minPrimeIndex: " << minPrimeIndex << endl;
#ifndef NDEBUG
    const auto dbgOriginalAs = sorted(aAndBWithGCD.as());
    const auto dbgOriginalBs = sorted(aAndBWithGCD.bs());
#endif

    if (primeIndexIter == primeIndicesThatDivideAAndB.end())
    {
        cout << "end - productSoFar: " << productSoFar << endl;
        //if (productSoFar == 1 && aAndBWithGCD
        assert(productSoFar < generatedGcds.size());
        generatedGcds[productSoFar] = true;
        cout << "Exiting findResult (terminal)" << endl;
        return;
    }

    assert(*primeIndexIter >= minPrimeIndex);
    const int primeFactorIndex = *primeIndexIter;
    const auto prime = primesUpToMaxValue[primeFactorIndex];
    const int nextMinPrimeIndex = *primeIndexIter + 1;
    cout << " entered findResult; productSoFar: " << productSoFar << " prime: " << prime << endl;

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
        bool anyAWithThisOrHigherPower = false;
        bool anyBWithThisOrHigherPower = false;
        const vector<int> asWithCurrentPrimePower = aAndBWithGCD.asWithPrimeFactor(primeFactorIndex, primePower, anyAWithThisOrHigherPower);
        const vector<int> bsWithCurrentPrimePower = aAndBWithGCD.bsWithPrimeFactor(primeFactorIndex, primePower, anyBWithThisOrHigherPower);

        cout << "prime: " << prime << " primePower: " << primePower << " asWithCurrentPrimePower: " << endl;
        for (const auto a : asWithCurrentPrimePower)
            cout << a << " ";
        cout << endl;
        cout << "prime: " << prime << " primePower: " << primePower << " bsWithCurrentPrimePower: " << endl;
        for (const auto b : bsWithCurrentPrimePower)
            cout << b << " ";
        cout << endl;

        //cout << "primeToPower: " << primeToPower << endl;

        //if (!asWithCurrentPrimePower.empty())
        asWithPrimePower.push_back(asWithCurrentPrimePower);
        //if (!bsWithCurrentPrimePower.empty())
        bsWithPrimePower.push_back(bsWithCurrentPrimePower);

        if (anyAWithThisOrHigherPower || anyBWithThisOrHigherPower)
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
    findResult(aAndBWithGCD, productSoFar, nextMinPrimeIndex, generatedGcds);

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
        const auto dbgAs = sorted(aAndBWithGCD.as());
        const auto dbgBs = sorted(aAndBWithGCD.bs());
        AAndBWithGCD aDivisibleByPrimeBNotDivisibleByPrime;
        // Construction of aDivisibleByPrime - with all B's information intact - is O(1).
        aDivisibleByPrimeBNotDivisibleByPrime.moveBsFrom(aAndBWithGCD);
        cout << "Done move: b's in dest: " << endl;
        for (const auto b : aDivisibleByPrimeBNotDivisibleByPrime.bs())
        {
            cout << b << " ";
        }
        cout << endl;
        for (int primePower = 1; primePower < asWithPrimePower.size(); primePower++)
        {
            aDivisibleByPrimeBNotDivisibleByPrime.addToA(asWithPrimePower[primePower]);
        }
        // Recurse - this finds the remaining gcds not divisible by this prime.
        cout << "prime: " << prime << " productSoFar: " << productSoFar << " recursing to find B not divisible by prime; a is" << endl;
        cout << "as: " <<  endl;
        for (const auto x : aDivisibleByPrimeBNotDivisibleByPrime.as())
        {
            cout << x << " ";
        }
        cout << endl;
        cout << "bs: " <<  endl;
        for (const auto x : aDivisibleByPrimeBNotDivisibleByPrime.bs())
        {
            cout << x << " ";
        }
        cout << endl;
        findResult(aDivisibleByPrimeBNotDivisibleByPrime, productSoFar, nextMinPrimeIndex, generatedGcds);

        // Restore aAndBWithGCD (constant-time).
        aAndBWithGCD.moveBsFrom(aDivisibleByPrimeBNotDivisibleByPrime);
        assert(sorted(aAndBWithGCD.as()) == dbgAs);
        assert(sorted(aAndBWithGCD.bs()) == dbgBs);
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
                    {
                        cout << "adding as prime: " << prime << " primePower: " << primePower << endl;
                        aAndBDivisibleByPrime.addToA(asWithPrimePower[primePower]);
                        cout << "done adding as prime: " << prime << " primePower: " << primePower << endl;
                    }
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
                    cout << " 2) prime: " << prime << " primePower: " << primePower << " primeToPower: " << primeToPower << endl;
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
                    cout << " 2) prime: " << prime << " primePower: " << primePower << " primeToPower: " << primeToPower << " about to recurse" << endl;
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


#ifndef NDEBUG
    const auto dbgVerifyAs = sorted(aAndBWithGCD.as());
    const auto dbgVerifyBs = sorted(aAndBWithGCD.bs());
    cout << "dbgOriginalAs: " << endl;
    for (const auto x : dbgOriginalAs)
    {
        cout << x << " ";
    }
    cout << endl;
    cout << "dbgOriginalBs: " << endl;
    for (const auto x : dbgOriginalBs)
    {
        cout << x << " ";
    }
    cout << endl;
    cout << "as: " << endl;
    for (const auto x : dbgVerifyAs)
    {
        cout << x << " ";
    }
    cout << endl;
    cout << "bs: " << endl;
    for (const auto x : dbgVerifyBs)
    {
        cout << x << " ";
    }
    cout << endl;
    assert(dbgOriginalAs == dbgVerifyAs);
    assert(dbgOriginalBs == dbgVerifyBs);
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
        //cout << "i: " << i << " primesUpToMaxValue: " << primesUpToMaxValue[i] << endl;
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

