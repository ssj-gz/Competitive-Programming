#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <cassert>

using namespace std;

const int64_t maxStickSize = 1'000'000'000'000ULL;

struct Factor
{
    int64_t factor;
    bool isPrime;
};

vector<Factor> findFactors(int64_t n, const vector<int64_t>& primesUpToRootMaxStickSize)
{
    //cout << "findFactors " << n << endl;
    struct PrimeFactor
    {
        int64_t prime;
        int power;
    };
    vector<PrimeFactor> primeFactors;
    for (const auto prime : primesUpToRootMaxStickSize)
    {
        if (prime * prime > n)
            break;
        //cout << "prime: " << prime << endl;
        int power = 0;
        while ((n % prime) == 0)
        {
            n /= prime;
            power++;
        }

        if (power > 0)
            primeFactors.push_back({prime, power});
    }
    if (n != 1)
        primeFactors.push_back({n, 1});

    //for (const auto primeFactor : primeFactors)
    //{
        //cout << "prime factor: " << primeFactor.prime << " power: " << primeFactor.power << endl;
    //}

    vector<int> primeToPower(primeFactors.size(), 1);
    vector<int> powerOfPrime(primeFactors.size(), 0);

    vector<Factor> factors;
    factors.push_back({1, false});

    int totalPowers = 0;
    int64_t totalFactor = 1;
    while (true)
    {
        int index = 0;
        while (index != primeFactors.size() && powerOfPrime[index] == primeFactors[index].power)
        {
            totalPowers -= powerOfPrime[index];
            powerOfPrime[index] = 0;
            totalFactor /= primeToPower[index];
            primeToPower[index] = 1;
            index++;
        }
        if (index == primeFactors.size())
            break;

        powerOfPrime[index]++;
        totalPowers++;
        totalFactor *= primeFactors[index].prime;
        primeToPower[index] *= primeFactors[index].prime;

        //cout << "Factor: " << totalFactor << " is prime? " << (totalPowers == 1) << endl; 
        factors.push_back({totalFactor, totalPowers == 1});
    }

    //cout << "No more factors" << endl;

    return factors;

}

int64_t findMaxMoves(const int64_t stickSize, map<int64_t, int64_t>& maxMovesLookup, const vector<int64_t>& primesUpToRootMaxStickSize)
{
    assert(stickSize > 0);
    if (stickSize == 1)
        return 1;
    if (maxMovesLookup.find(stickSize) != maxMovesLookup.end())
    {
        return maxMovesLookup[stickSize];
    }
    int64_t maxMoves = 0;
    const int64_t sqrtStickSize = sqrt(stickSize);
    const auto factors = findFactors(stickSize, primesUpToRootMaxStickSize);
#if 0
    for (int factor = 1; factor <= sqrtStickSize; factor++)
    {
#endif
    for (const auto factorBlah : factors)
    {
        const auto factor = factorBlah.factor;
        //if ((stickSize % factor) == 0)
        assert((stickSize % factor) == 0);
        {
            {
                const int64_t numParts = factor;
                const int64_t newStickSize = stickSize / factor;
                if (numParts != 1)
                {
                    const int64_t maxMovesFromHere = numParts * findMaxMoves(newStickSize, maxMovesLookup, primesUpToRootMaxStickSize);
                    //cout << "stickSize: " << stickSize << " numParts: " << numParts << " newStickSize: " << newStickSize << " maxMovesFromHere: " << maxMovesFromHere << endl;
                    maxMoves = max(maxMoves, maxMovesFromHere);
                }
            }
            {
                const int64_t numParts = stickSize / factor;
                const int64_t newStickSize = factor;
                if (numParts != 1)
                {
                    const int64_t maxMovesFromHere = numParts * findMaxMoves(newStickSize, maxMovesLookup, primesUpToRootMaxStickSize);
                    //cout << "stickSize: " << stickSize << " numParts: " << numParts << " newStickSize: " << newStickSize << " maxMovesFromHere: " << maxMovesFromHere << endl;
                    maxMoves = max(maxMoves, maxMovesFromHere);
                }
            }

        }
    }
   
    maxMoves++;

    maxMovesLookup[stickSize] = maxMoves;
    return maxMoves;
}

int main()
{
#if 0
    {
        const int N = 100;
        const int64_t maxStickSize = 1'000'000'000'000;
        cout << N << endl;
        for (int64_t i = maxStickSize; i >= maxStickSize - 100; i--)
        {
            cout << i << endl;
        }
        return 0;
    }
#endif
    int n;
    cin >> n;

    vector<int64_t> d(n);
    for (int i = 0; i < n; i++)
    {
        cin >> d[i];
    }

    const int64_t sqrtMaxStickSize = sqrt(maxStickSize);
    vector<int64_t> primesUpToRootMaxStickSize;
    vector<bool> isPrime(sqrtMaxStickSize + 1, true);
    for (int factor = 2; factor <= sqrtMaxStickSize; factor++)
    {
        if (isPrime[factor])
        {
            //cout << "Found prime: " << factor << endl;
            primesUpToRootMaxStickSize.push_back(factor);
            for (int multiple = factor * 2; multiple <= sqrtMaxStickSize; multiple += factor)
            {
                isPrime[multiple] = false;
            }
        }
    }

    for (int i = 1; i <= 100; i++)
    {
        findFactors(i, primesUpToRootMaxStickSize);
    }

    int64_t maxMoves = 0;
    map<int64_t, int64_t> maxMovesLookup;
    for (const auto stickSize : d)
    {
        maxMoves += findMaxMoves(stickSize, maxMovesLookup, primesUpToRootMaxStickSize);
    }
    cout << maxMoves << endl;
    
}

