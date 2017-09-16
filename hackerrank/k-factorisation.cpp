// Simon St James (ssjgz) 2017-09-16 18:33
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

void kFactorise(const vector<int>& destPrimeFactorisation, const vector<vector<int>>& valuesPrimeFactorisations, const vector<int>& values, int valueIndexStart, vector<int>& valuesUsed, vector<int>& bestValuesUsed)
{
    if (!bestValuesUsed.empty() && valuesUsed.size() > bestValuesUsed.size())
        return;
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
        if (bestValuesUsed.empty())
            bestValuesUsed = valuesUsed;
        else
        {
            if (bestValuesUsed.size() > valuesUsed.size())
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

int main()
{
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
                    //cout << "erasing value: " << *valueIter << endl;
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
        cout << -1 << endl;
        return 0;
    }
    int tempN = N;
    vector<int> nPrimeFactorisation(primesInA.size());
    int primeFactorIndex = 0;
    for (const auto prime : primesInA)
    {
        while ((tempN % prime) == 0)
        {
            tempN /= prime;
            nPrimeFactorisation[primeFactorIndex]++;
        }
        primeFactorIndex++;
    }
    if (tempN != 1)
    {
        cout << -1 << endl;
        return 0;
    }
    vector<vector<int>> valuePrimesFactorisations;
    for (auto value : a)
    {
        vector<int> valuePrimeFactorisation(primesInA.size());
        int primeFactorIndex = 0;
        for (const auto prime : primesInA)
        {
            while ((value % prime) == 0)
            {
                value /= prime;
                valuePrimeFactorisation[primeFactorIndex]++;
            }
            primeFactorIndex++;
        }
        valuePrimesFactorisations.push_back(valuePrimeFactorisation);
    }
    vector<int> empty;
    vector<int> best;
    kFactorise(nPrimeFactorisation, valuePrimesFactorisations, a, 0, empty, best);
    //cout << "best" << endl;
    if (best.size() == 0)
        cout << -1 << endl;
    else
    {
        int value = 1;
        cout << value << " ";
        for (const auto valueUsed : best)
        {
            value *= valueUsed;
            cout << value << " ";
        }
    }
}
