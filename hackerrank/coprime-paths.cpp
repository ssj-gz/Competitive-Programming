#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>

using namespace std;

int main()
{
    int n, q;
    cin >> n >> q;

    const auto maxTotalPrimeFactorsOfAllNodes = 3;
    const auto numNodePrimeFactorCombinations = (1 << maxTotalPrimeFactorsOfAllNodes) - 1;

    struct Node
    {
        int value = -1;
        vector<Node*> neighbours;
        int primeFactorBitmask = 0;
    };

    vector<Node> nodes(n);

    int maxNodeValue = 0;
    for (int i = 0; i < n; i++)
    {
        int nodeValue;
        cin >> nodeValue;
        nodes[i].value = nodeValue;
        maxNodeValue = max(maxNodeValue, nodeValue);
    }

    const int primeLimit = sqrt(maxNodeValue);
    vector<bool> isPrime(maxNodeValue + 1, true);
    for (int factor = 2; factor <= primeLimit; factor++)
    {
        int i = 2 * factor;
        while (i <= maxNodeValue)
        {
            isPrime[i] = false;
            i += factor;
        }
    }

    vector<int> primesUpToMaxValue;
    for (int i = 2; i < isPrime.size(); i++)
    {
        if (isPrime[i])
            primesUpToMaxValue.push_back(i);
    }

    vector<int> primeFactorsOfNodes;
    for (auto& node : nodes)
    {
        auto nodeValue = node.value;
        vector<int> primeFactors;
        for (auto prime : primesUpToMaxValue)
        {
            if (nodeValue == 1)
                break;
            if (prime * prime > nodeValue)
            {
                primeFactors.push_back(nodeValue);
            }
            if ((nodeValue % prime) == 0)
            {
                primeFactors.push_back(prime);
                while ((nodeValue % prime) == 0)
                {
                    nodeValue /= prime;
                }
            }
        }
        for (const auto primeFactor : primeFactors)
        {
            const auto occurrenceInPrimeFactorsOfNodes = find(primeFactorsOfNodes.begin(), primeFactorsOfNodes.end(), primeFactor);
            int indexOfPrimeFactorInPrimeFactorsOfNodes = -1;
            if (occurrenceInPrimeFactorsOfNodes == primeFactorsOfNodes.end())
            {
                indexOfPrimeFactorInPrimeFactorsOfNodes = primeFactorsOfNodes.size();
                primeFactorsOfNodes.push_back(primeFactor);
                assert(primeFactorsOfNodes.size() <= maxTotalPrimeFactorsOfAllNodes);
            }
            else
            {
                indexOfPrimeFactorInPrimeFactorsOfNodes = distance(primeFactorsOfNodes.begin(), occurrenceInPrimeFactorsOfNodes);
            }
            node.primeFactorBitmask |= (1 << indexOfPrimeFactorInPrimeFactorsOfNodes);
        }
    }

    for (int i = 0; i < n - 1; i++)
    {
        int u, v;
        cin >> u >> v;
        u--;
        v--;

        auto node1 = &(nodes[u]);
        auto node2 = &(nodes[v]);

        node1->neighbours.push_back(node2);
        node2->neighbours.push_back(node1);
    }

    for (int i = 0; i < q; i++)
    {
        int u, v;
        cin >> u >> v;
        u--;
        v--;

    }

}

