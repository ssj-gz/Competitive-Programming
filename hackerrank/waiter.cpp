#include <iostream>
#include <stack>
#include <vector>

#include <cassert>

using namespace std;

int main()
{
    vector<int> primesUpToMaxValue;

    const int maxValue = 50'000;
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
        cout << "queryNum: " << queryNum << endl;
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
                // the (queryNum + 1th) prime is > maxValue, and so > n.
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

    for (int i = 1; i <= numQueries; i++)
    {
        printStackTopToBottom(b[i]);
    }
    printStackTopToBottom(a[numQueries]);


}
