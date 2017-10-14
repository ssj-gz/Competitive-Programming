#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>

using namespace std;

const auto maxTotalPrimeFactorsOfAllNodes = 3;
const auto numNodePrimeFactorCombinations = (1 << maxTotalPrimeFactorsOfAllNodes) - 1;

struct Node
{
    int index = -1;
    int value = -1;
    vector<Node*> neighbours;
    int primeFactorBitmask = 0;
};

vector<Node*> path(Node* root, Node* destNode, Node* parent, vector<Node*>& pathSoFar)
{
    pathSoFar.push_back(root);

    if (root == destNode)
        return pathSoFar;

    for (auto neighbour : root->neighbours)
    {
        if (neighbour == parent)
            continue;

        const auto pathToDestNode = path(neighbour, destNode, root, pathSoFar);

        if (!pathToDestNode.empty())
        {
            return pathToDestNode;
        }

    }
    pathSoFar.pop_back();

    return vector<Node*>();

}

int64_t bruteForce(Node* node1, Node* node2)
{
    vector<Node*> pathSoFar;
    const auto pathBetweenNodes = path(node1, node2, nullptr, pathSoFar);
    assert(node1 == node2 || !pathBetweenNodes.empty());

    cout << "Path between nodes with indices " << node1->index << " and " << node2->index << endl; 
    for  (const auto node : pathBetweenNodes)
    {
        cout << node->index << " ";
    }
    cout << endl;



    int64_t result = 0;
    for (auto nodeInPathIndex = 0; nodeInPathIndex < pathBetweenNodes.size(); nodeInPathIndex++)
    {
        for (auto nodeInPathIndex2 = nodeInPathIndex + 1; nodeInPathIndex2 < pathBetweenNodes.size(); nodeInPathIndex2++)
        {
            auto nodeInPath1 = pathBetweenNodes[nodeInPathIndex];
            auto nodeInPath2 = pathBetweenNodes[nodeInPathIndex2];

            if ((nodeInPath1->primeFactorBitmask & nodeInPath2->primeFactorBitmask) == 0)
                result++;
        }
    } 
    return result;
}

int main()
{
    int n, q;
    cin >> n >> q;

    vector<Node> nodes(n);

    int maxNodeValue = 0;
    for (int i = 0; i < n; i++)
    {
        int nodeValue;
        cin >> nodeValue;
        nodes[i].value = nodeValue;
        maxNodeValue = max(maxNodeValue, nodeValue);
        nodes[i].index = i + 1;
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

    vector<int> sharedNodePrimeFactors;
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
                break;
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
            const auto occurrenceInSharedNodePrimeFactors = find(sharedNodePrimeFactors.begin(), sharedNodePrimeFactors.end(), primeFactor);
            int indexInSharedNodePrimeFactors = -1;
            if (occurrenceInSharedNodePrimeFactors == sharedNodePrimeFactors.end())
            {
                indexInSharedNodePrimeFactors = sharedNodePrimeFactors.size();
                sharedNodePrimeFactors.push_back(primeFactor);
                assert(sharedNodePrimeFactors.size() <= maxTotalPrimeFactorsOfAllNodes);
            }
            else
            {
                indexInSharedNodePrimeFactors = distance(sharedNodePrimeFactors.begin(), occurrenceInSharedNodePrimeFactors);
            }
            node.primeFactorBitmask |= (1 << indexInSharedNodePrimeFactors);
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

        auto node1 = &(nodes[u]);
        auto node2 = &(nodes[v]);

        const auto bruteForceResult = bruteForce(node1, node2);
        cout << "bruteForceResult: " << bruteForceResult << endl;
    }

}

