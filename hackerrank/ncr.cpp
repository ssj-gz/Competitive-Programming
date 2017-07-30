#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <cassert>
#include <algorithm>

using std::vector;
using std::pair;
using std::make_pair;
using std::cin;
using std::cout;
using std::endl;

const int modulus = 142857;

// The prime factors of modulus.
const vector<long> primeFactorsOfModulus = { 3, 11, 13, 37 };
const int numPrimeFactorsOfModulus = primeFactorsOfModulus.size();
const vector<long> factorsOfModulus = { 3 * 3 * 3, 11, 13, 37 };
// For each factor of modulus, a look up table for the product of the all i such that
// i is coprime to that factor of modulus.  This product is actually periodic ...
vector<vector<long>> filteredProductUpToModulusForFactor(numPrimeFactorsOfModulus);
// ... and this tells us when the filtered product starts repeating itself, and how long
// the period is.  Someone better at Number Theory would be able to compute it at runtime;
// I had to do it manually :/
const vector<pair<long, long>> filteredProductPeriodInfo = { {10, 54 }, {10, 22}, {10, 26}, {10, 74} };
// For each factorOfModulus, a lookup table of inverses mod factorOfModulus.
vector<vector<long>> inverses(numPrimeFactorsOfModulus);

long quickPower(long n, int k, int m)
{
    long result = 1;
    int power = 0;
    while (k > 0)
    {
        if (k & 1)
        {
            long subResult = n;
            for (int i = 0; i < power; i++)
            {
                subResult = (subResult * subResult) % m;
            }
            result = (result * subResult) % m;
        }
        k >>= 1;
        power++;
    }
    return result;
}

// Return a vector where the jth component is the power of primeFactorsOfModulus[j] in the prime
// factorisation of n!.
vector<int> computePowersOfPrimeFactorsOfModulus(long n)
{
    vector<int> powersOfPrimeFactorOfModulus(numPrimeFactorsOfModulus);
    for (int j = 0; j < numPrimeFactorsOfModulus; j++)
    {
        const long prime = primeFactorsOfModulus[j];
        long primePower = prime;
        while (true)
        {
            long nCopy = n;
            if ((nCopy % primePower) != 0)
            {
                nCopy = (nCopy / primePower) * primePower; 
            }
            if (nCopy < 1)
                break;
            powersOfPrimeFactorOfModulus[j] += (nCopy - 1) / primePower + 1;
            primePower *= prime;
        }
    }
    return powersOfPrimeFactorOfModulus;
}


// Returns the product (mod modulus) of all i such that 1 <= i <= n, i not divisible my primeFactorsOfModulus[primeOfModulusIndex]
// (see the "verifyResult" block for the unoptimised version of what it's doing).
// Uses the periodicity of this value in conjunction with lookup tables created in main().
long factorialExcludingP(long n, long primeOfModulusIndex, long modulus)
{
    const long repeatBegin = filteredProductPeriodInfo[primeOfModulusIndex].first;
    const long repeatPeriod = filteredProductPeriodInfo[primeOfModulusIndex].second;
    long result;
    if (n < repeatBegin)
        result = filteredProductUpToModulusForFactor[primeOfModulusIndex][n - 1];
    else
        result = filteredProductUpToModulusForFactor[primeOfModulusIndex][repeatBegin + (n - repeatBegin) % repeatPeriod - 1];

    if (false)
    {
        long verifyResult = 1;
        const long p = primeFactorsOfModulus[primeOfModulusIndex];
        for (long j = 1; j <= n; j++)
        {
            if ((j % p) == 0)
                continue;
            verifyResult = (verifyResult * j) % modulus;
        }
        assert(result == verifyResult);
    }

    return result;
}
// Returns the product (mod modulus) of all j such that j is some i, 1 <= i <= n, divided by p = primeFactorsOfModulus[primeOfModulusIndex] until
// no more divisions by p are possible.
// (see the "verifyResult" block for the unoptimised version of what it's doing).
// It's a bit tricky to  explain what's going on, but basically note that the contribution of each p^k is factorialExcludingP(n / p^k, primeOfModulusIndex, modulus).
long filteredFactorial(long n, long primeOfModulusIndex, long modulus)
{
    const long p = primeFactorsOfModulus[primeOfModulusIndex];
    long result = factorialExcludingP(n, primeOfModulusIndex, modulus);
    long powerOfP = p;
    while (powerOfP <= n)
    {
        result = (result * factorialExcludingP(n / powerOfP, primeOfModulusIndex, modulus)) % modulus;
        powerOfP *= p;
    }


    if (false)
    {
        long verifyResult = 1;
        for (long j = 1; j <= n; j++)
        {
            long copyJ = j;
            while ((copyJ % p) == 0)
            {
                copyJ = copyJ / p;
            }
            verifyResult = (verifyResult * copyJ) % modulus;
        }
        cout << "filteredFactorial: n: " << n << " p: " << p << " modulus: " << modulus << " result: " << result << " verifyResult: " << verifyResult << endl;
        assert(result == verifyResult);
    }
    return result;
}

// TODO - plenty of scope for optimisation here - mainly by using the Extended GCD algorithm
// in conjunction with the Chinese Remainder Theorem!
long solveCongruences(const vector<pair<long, long>>& valuesAndModuliOrig)
{
    vector<pair<long, long>> valuesAndModuli(valuesAndModuliOrig);
    long value = valuesAndModuliOrig.back().first;
    long modulus = valuesAndModuliOrig.back().second;

    valuesAndModuli.pop_back();

    while (!valuesAndModuli.empty())
    {
        // At the beginning of the loop, value % modulus satisfies all congruences that have been
        // removed (starting from the end) of valuesAndModuli.
        // We want to find a solution that agrees with our current solution but additionally 
        // satisfies (candidateValue % nextModulus) == nextValue.
        const long nextValue = valuesAndModuli.back().first;
        const long nextModulus = valuesAndModuli.back().second;
        valuesAndModuli.pop_back();
        const long targetModulus = modulus * nextModulus;
        long valueModTargetModulus = -1;
        for (long candidateValue = value; candidateValue < targetModulus; candidateValue += modulus)
        {
            // candidateValue increases by modulus each time as it must agree with our solutions to the 
            // congruences we've already dealt with.
            if ((candidateValue % nextModulus) == nextValue)
            {
                valueModTargetModulus = candidateValue;
                break;
            }
        }
        assert(valueModTargetModulus != -1);

        value = valueModTargetModulus;
        modulus = targetModulus;
    }
    return value;
}

long choose(long n, long r)
{
    if (r == 0)
        return 1;
    if (n == r)
        return 1;
    // Quite a tricky one - took me ages to figure out, but if I'd remembered the Chinese Remainder
    // Theorem, I would have been much quicker!
    // So: first and foremost, the modulus - 142857 - is not a prime - its prime decomposition is 
    // 3^3 * 11 * 13 * 37.  By the CRT, we need only compute nCr mod each of 3^3 and mod 11 and mod 13 and mod 37,
    // then solve the congruences.
    // So: we now only need to solve the problems of computing, for a prime p and a k >= 1, n!/r!/(n-r)! mod p^k.
    // The naive way to do this won't work: computing n! mod p^k directly will give us zero as soon as n >= p^k.
    // The only reason that n!/r!/(n-r)! mod p^k is not always zero is that sometimes, the power of p in the numerator
    // cancels with the power in the denomimator and leaves a power of p in the division that is < k.
    // So: if, as we computed the numerator and the denominator, we "filtered out" any powers of p^k as we went along,
    // and if the total power of p^l we filtered from the numerator matched that filtered from the denominator
    // (i.e. so we essentially "divided" the numerator and denominator by the same value p^l, by virtue of not 
    // multiplying it in the first place!), we'd be sorted.
    // Of course, we're not always this lucky, and the power of p we filter from the numerator will often be more than
    // that in the denominator; that is, if while filtering while computing the numerator and the denominator, p^lNum is
    // the amount filtered (i.e. "divided") from the numerator and p^lDen is that filtered from the denominator, in general
    // lNum >= lDen.  The value lNum - lDen is the "deficit" of the numerator, and to get the correct result, we need to
    // multiply the filtered numerator by it afterward.
    // As it happens, we can compute the value of p^l that would be filtered out in computing n! using computePowersOfPrimeFactorsOfModulus().
    // So: to recap, we've computed the product of i such that 1 <= j <= n and i is j after dividing j by p until we can do so no longer
    // (giving us the "filtered numerator") and done the same for r and (n-r) and multiplied the latter two results (the "filtered denominator").
    // If we then multiply by the deficit - the power of p necessary to ensure that our filtration "divided" the numerator and the denominator
    // by the same power of p - then we have successfully calculated n choose r mod p^k.
    // "Filtering" is quite tricky; doing it naively (see the "verifyResult" part of filteredFactorial()) is far too slow.  
    // Let's examine what happens when we compute:
    //  product of i such that 1 <= j <= n and i is j after dividing j by p until we can do so no longer.
    // Let's assume j is of the form k * p^l, where k is not divisible by p.  Then the corresponding value of i (the contribution of j to 
    // the filtered factorial) is k.  For some l, let's look at all ji's such that ji is of the form ki * p ^ l, ki not divisible by 
    // p.  For the given l, the contribution is then:
    //  k1 * k2 * k3 ... etc while ki * p ^ l <= n.  This is precisely the product of all n / (p ^ l) elements in the series
    //  k1, k2, k3 etc where ki is the ith number that is co-prime to p.  This product turns out to be periodic, and is easily calculated
    // from the lookup table "filteredProductUpToModulusForFactor", and this is what is done by factorialExcludingP().
    // So: if we do this for each l such that p^l divides n, we will have the "filtered factorial" we need to compute for n, n-r, and r: this 
    // is accomplished via filteredFactorial().
    // And then we just need to solve the congruences - whee!
    const vector<int> rFactorialPowersOfPrimeFactorsOfModulus = computePowersOfPrimeFactorsOfModulus(r);
    const vector<int> nMinusRPowersOfPrimeFactorsOfModulus = computePowersOfPrimeFactorsOfModulus(n - r);
    const vector<int> numeratorPowersOfPrimeFactorsOfModulus = computePowersOfPrimeFactorsOfModulus(n);

    vector<int> denominatorPowersOfPrimeFactorsOfModulus(numPrimeFactorsOfModulus);

    for (int j = 0; j < numPrimeFactorsOfModulus; j++)
    {
        denominatorPowersOfPrimeFactorsOfModulus[j] = rFactorialPowersOfPrimeFactorsOfModulus[j] + nMinusRPowersOfPrimeFactorsOfModulus[j];
    }
    vector<int> numeratorPowersOfPrimeFactorsOfModulusDeficit(numPrimeFactorsOfModulus);
    long numeratorDeficit = 1;
    for (int j = 0; j < numPrimeFactorsOfModulus; j++)
    {
        numeratorPowersOfPrimeFactorsOfModulusDeficit[j] = numeratorPowersOfPrimeFactorsOfModulus[j] - denominatorPowersOfPrimeFactorsOfModulus[j];
        numeratorDeficit = (numeratorDeficit * quickPower(primeFactorsOfModulus[j], numeratorPowersOfPrimeFactorsOfModulusDeficit[j], modulus)) % modulus;
        assert(numeratorPowersOfPrimeFactorsOfModulus[j] >= 0);
    }
    if (numeratorDeficit == 0)
    {
        // Numerator / Denominator is divisible by modulus; nCr is 0.
        return 0;
    }

    // Calculate nCr modulo each of the mutually co-prime factors of modulus (3^3, 11, 13, 37).
    vector<long> chooseModFactorOfModulus(numPrimeFactorsOfModulus);
    for (int j = 0; j < numPrimeFactorsOfModulus; j++)
    {
        const long p = primeFactorsOfModulus[j];
        const long factorOfModulus = factorsOfModulus[j];

        const long filteredNumerator = filteredFactorial(n, j, factorOfModulus);
        const long filteredDenominator = (filteredFactorial(r, j, factorOfModulus) * filteredFactorial(n - r, j, factorOfModulus)) % factorOfModulus;
        const long inverseDenominator = inverses[j][filteredDenominator];
        assert((filteredDenominator * inverseDenominator) % factorOfModulus == 1);
        const long deficientChoose = (filteredNumerator * inverseDenominator) % factorOfModulus;
        const long numeratorDeficit = quickPower(primeFactorsOfModulus[j], numeratorPowersOfPrimeFactorsOfModulusDeficit[j], factorOfModulus);
        const long choose = (deficientChoose * numeratorDeficit) % factorOfModulus;
        chooseModFactorOfModulus[j] = choose;
    }
    // We now know nCr mod each (mutually co-prime) factor of modulus: by the Chinese Remainder Theorem, there is only one 
    // solution mod modulus, and that will give us nCr mod modulus.
    vector<pair<long, long>> congruences(numPrimeFactorsOfModulus);
    for (int j = 0; j < numPrimeFactorsOfModulus; j++)
    {
        const long factorOfModulus = factorsOfModulus[j];
        congruences[j] = make_pair(chooseModFactorOfModulus[j], factorOfModulus);
    }
    return solveCongruences(congruences);
}



int main()
{

    // Lookup table for use by factorialExcludingP(...).
    for (int j = 0; j < numPrimeFactorsOfModulus; j++)
    {
        const long p = primeFactorsOfModulus[j];
        const long repeatBegin = filteredProductPeriodInfo[j].first; 
        const long repeatPeriod = filteredProductPeriodInfo[j].second; 
        long filteredProductForPrime = 1;
        const long factorOfModulus = factorsOfModulus[j];
        for (int i = 1; i <= repeatBegin + repeatPeriod; i++)
        {
            if ((i % p) != 0)
            {
                filteredProductForPrime = (filteredProductForPrime * i) % factorOfModulus;
            }
            filteredProductUpToModulusForFactor[j].push_back(filteredProductForPrime);
        }
    }


    // Calculate inverses lookup table for each factor of modulus.
    for (long j = 0; j < numPrimeFactorsOfModulus; j++)
    {
        const long factorOfModulus = factorsOfModulus[j];
        inverses[j].resize(factorOfModulus);
        for (long i = 0; i < factorOfModulus; i++)
        {
            for (long inverse = 0; inverse < factorOfModulus; inverse++)
            {
                if ((i * inverse) % factorOfModulus == 1)
                {
                    inverses[j][i] = inverse;
                    break;
                }
            }
        }
    }

    long T;
    cin >> T;
    for (long t = 0; t < T; t++)
    {
        long n, r;
        cin >> n >> r;
        if (r > n /2)
        {
            r = n - r;
        }
        long nCr = choose(n, r);
        cout << nCr << endl;
    }
}

