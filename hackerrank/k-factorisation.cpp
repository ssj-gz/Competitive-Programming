// Simon St James (ssjgz) 2017-09-16 18:33
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

void kFactorise(const vector<int>& destPrimeFactorisation, const vector<vector<int>>& valuesPrimeFactorisations, const vector<int>& values, int valueIndexStart, vector<int>& valuesUsed, vector<int>& bestValuesUsed)
{
    if (!bestValuesUsed.empty() && valuesUsed.size() > bestValuesUsed.size())
    {
        // We've already exceeded the length of the best; no point exploring further.
        return;
    }
    bool found = true;
    for (const auto primePower : destPrimeFactorisation)
    {
        if (primePower != 0)
        {
            found = false;
            break;
        }
    }
    if (found)
    {
        // The dest value is 1, so we're done.
        if (bestValuesUsed.empty())
            bestValuesUsed = valuesUsed;
        else
        {
            if (valuesUsed.size() < bestValuesUsed.size())
            {
                bestValuesUsed = valuesUsed;
            }
            else
            {
                if (valuesUsed < bestValuesUsed)
                    bestValuesUsed = valuesUsed;
            }
        }
        return;
    }
    for (int valueIndex = valueIndexStart; valueIndex < valuesPrimeFactorisations.size(); valueIndex++)
    {
        vector<int> newDestPrimeFactorisation(destPrimeFactorisation);
        const auto& valuePrimeFactorisation = valuesPrimeFactorisations[valueIndex];
        bool canUseValue = true;
        for (int primeFactorIndex = 0; primeFactorIndex < valuePrimeFactorisation.size(); primeFactorIndex++)
        {
            // "Divide" the destination number by value, if we can.
            newDestPrimeFactorisation[primeFactorIndex] -= valuePrimeFactorisation[primeFactorIndex];
            if (newDestPrimeFactorisation[primeFactorIndex] < 0)
            {
                // This value doesn't divide into the value represented  by destPrimeFactorisation.
                canUseValue = false;
                break;
            }
        }
        if (canUseValue)
        {
            valuesUsed.push_back(values[valueIndex]);
            // We can assume that the best solution will have the values in order (otherwise, the solution where the
            // values are sorted will be lexically less than it), so don't allow value indices less than valueIndex
            // to be explored subsequently.
            kFactorise(newDestPrimeFactorisation, valuesPrimeFactorisations, values, valueIndex, valuesUsed, bestValuesUsed);

            valuesUsed.pop_back();
        }
    }
}

vector<int> primeFactorisation(int x, const vector<int>& primes, int* xWithoutPrimes = nullptr)
{
    vector<int> primeFactorisation(primes.size());
    int primeFactorIndex = 0;
    for (const auto prime : primes)
    {
        while ((x % prime) == 0)
        {
            x /= prime;
            primeFactorisation[primeFactorIndex]++;
        }
        primeFactorIndex++;
    }
    if (xWithoutPrimes)
        *xWithoutPrimes = x;

    return primeFactorisation;
}

int main()
{
    // Fairly easy one: essentially, gather a list of primes that appear in values of a, and call them
    // p1, p2 ... pm.
    // A prime factorisation of a number x is then the m-vector l1, l2 ... lm such that p1^l1 * p2^l2 * ... * pm^lm = x.
    // v1 * v2 * ... * vp = x if and only if the sums of prime factorisations of the vi's is equal to the prime factorisation
    // of x.  Similarly, we can "divide" v1 by v2 by subtracting the prime factorisation of v2 from v1; if any of the li's of 
    // the resulting prime factorisation are < 0, then v1 was not divisible by v2.
    // We then basically just do a trial-and-error search for a product of numbers in a that equals N, but instead of 
    // actually multiplying, we "divide" N by numbers in a, aiming for "1": this way, we can detect an impossible state a little easier
    // i.e. if the current prime factorisation (representing N divided by the product of some numbers in a) has any li < 0,
    // we can stop: we've overshot this the power of pi in N.
    // That's about it: there's also a few tricks to immediately filter out certain kinds of impossibility or values of a that can't 
    // possibly be part of a solution, etc.
    int N, K;
    cin >> N >> K;
    vector<int> a(K);
    for (int i = 0; i < K; i++)
    {
        cin >> a[i];
    }
    sort(a.begin(), a.end());

    const vector<int> primes = { 2, 3, 5, 7, 11, 13, 17, 19 };
    vector<int> primesInA;
    for (const auto prime : primes)
    {
        auto valueIter = a.begin();
        while (valueIter != a.end())
        {
            bool erasedValue = false;
            if ((*valueIter % prime) == 0)
            {
                if ((N % prime) != 0)
                {
                    // This value contains a prime that N does not, so cannot be part of a solution; remove it.
                    valueIter = a.erase(valueIter);
                    erasedValue = true;
                }
                else
                {
                    primesInA.push_back(*valueIter);
                }
            }
            if (!erasedValue)
                valueIter++;
        }
    }
    if (primesInA.empty())
    {
        // All values contain primes that N does not; can't do it.
        cout << -1 << endl;
        return 0;
    }
    int nWithoutPrimesInA = N;
    const vector<int> nPrimeFactorisation = primeFactorisation(N, primesInA, &nWithoutPrimesInA);
    if (nWithoutPrimesInA != 1)
    {
        // N contains primes that do not appear amongst the a's, so we cannot form N from the a's.
        cout << -1 << endl;
        return 0;
    }
    vector<vector<int>> valuePrimesFactorisations;
    for (auto value : a)
    {
        valuePrimesFactorisations.push_back(primeFactorisation(value, primesInA));
    }
    vector<int> empty;
    vector<int> bestValues;
    kFactorise(nPrimeFactorisation, valuePrimesFactorisations, a, 0, empty, bestValues);
    if (bestValues.empty())
        cout << -1 << endl;
    else
    {
        int value = 1;
        cout << value << " ";
        for (const auto valueUsed : bestValues)
        {
            value *= valueUsed;
            cout << value << " ";
        }
    }
}
