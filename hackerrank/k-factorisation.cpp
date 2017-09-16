// Simon St James (ssjgz) 2017-09-16 18:33
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

void kFactorise(const vector<int>& destPrimeFactorisation, const vector<vector<int>>& valuesPrimeFactorisations, const vector<int>& values, vector<int>& valuesUsed, vector<int>& bestValuesUsed)
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
        //cout << " Found!" << endl;
        for (const auto valueIndex : valuesUsed)
        {
            //cout << values[valueIndex] << " ";
        }
        //cout << endl;
        return;
    }
    for (int valueIndex = 0; valueIndex < valuesPrimeFactorisations.size(); valueIndex++)
    {
        vector<int> newDestPrimeFactorisation(destPrimeFactorisation);
        const auto& valuePrimeFactorisation = valuesPrimeFactorisations[valueIndex];
        bool canUseValue = true;
        for (int primeFactorIndex = 0; primeFactorIndex < valuePrimeFactorisation.size(); primeFactorIndex++)
        {
            newDestPrimeFactorisation[primeFactorIndex] -= valuePrimeFactorisation[primeFactorIndex];
            if (newDestPrimeFactorisation[primeFactorIndex] < 0)
            {
                canUseValue = false;
                break;
            }
        }
        if (canUseValue)
        {
            valuesUsed.push_back(values[valueIndex]);
            kFactorise(newDestPrimeFactorisation, valuesPrimeFactorisations, values, valuesUsed, bestValuesUsed);

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
    kFactorise(nPrimeFactorisation, valuePrimesFactorisations, a, empty, best);
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
