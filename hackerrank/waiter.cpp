// Simon St James (ssjgz) - 2019-07-09
#include <iostream>
#include <stack>
#include <vector>

#include <cassert>

using namespace std;

int main()
{
    // A trivially easy 75 points(!) - just do, naively, what the Problem requests of us.
    // Constraits are small enough that we don't need any clever optimisations.

    // Compute primes up to maximum possible value using Sieve of Eratosthenes.
    vector<int> primesUpToMaxValue;
    const int maxValue = 50'000; // From the "Constraints" for the problem.
    vector<bool> primeSieve(maxValue + 1, true);
    for (int factor = 2; factor <= maxValue; factor++)
    {
        if (primeSieve[factor])
        {
            primesUpToMaxValue.push_back(factor);
            auto multiple = 2 * factor;
            while (multiple <= maxValue)
            {
                primeSieve[multiple] = false;
                multiple += factor;
            }
        }
    }

    int n;
    cin >> n;
    assert(n <= maxValue);

    int numQueries;
    cin >> numQueries;

    vector<stack<int>> a(numQueries + 1);
    vector<stack<int>> b(numQueries + 1);

    for (int i = 0; i < n; i++)
    {
        int value;
        cin >> value;
        a[0].push(value);
    }

    for (int queryNum = 0; queryNum < numQueries; queryNum++)
    {
        auto& sourceAStack = a[queryNum];
        auto& destAStack = a[queryNum + 1];
        auto& destBStack = b[queryNum + 1];
        while (!sourceAStack.empty())
        {
            const int value = sourceAStack.top();
            bool isDivisibleByIthPrime = true;
            if (queryNum < primesUpToMaxValue.size())
            {
                if (value % (primesUpToMaxValue[queryNum]))
                    isDivisibleByIthPrime = true;
                else
                    isDivisibleByIthPrime = false;
            }
            else
            {
                // the (queryNum + 1th) prime is > maxValue, and so > n, so
                // n cannot be divisible by it.
                isDivisibleByIthPrime = false;
            }

            if (isDivisibleByIthPrime)
                destAStack.push(value);
            else
                destBStack.push(value);

            sourceAStack.pop();
        }
    }

    auto printStackTopToBottom = [](stack<int>& stackToPrint)
    {
        while (!stackToPrint.empty())
        {
            cout << stackToPrint.top() << endl;
            stackToPrint.pop();
        }

    };

    // Output the requested stacks in the requested format.
    for (int i = 1; i <= numQueries; i++)
    {
        printStackTopToBottom(b[i]);
    }
    printStackTopToBottom(a[numQueries]);
}
