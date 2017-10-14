// Simon St James (ssjgz) 2017-10-14
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <cmath>
#include <cassert>

using namespace std;

const auto maxTotalPrimeFactorsOfAllNodes = 3;
const auto numNodePrimeFactorCombinations = (1 << maxTotalPrimeFactorsOfAllNodes) - 1;

constexpr int maxNodes = 25'000;
constexpr int log2(int N, int exponent = 0, int powerOf2 = 1)
{
        return (powerOf2 >= N) ? exponent : log2(N, exponent + 1, powerOf2 * 2);
}
constexpr int log2MaxNodes = log2(maxNodes);

bool isPrime(int n)
{
    for (int i = 2; i <= sqrt(n) + 1; i++)
    {
        if (i < n && (n % i) == 0)
            return false;
    }
    return true;
}

struct Node
{
    int index = -1;
    int value = -1;
    vector<Node*> neighbours;
    int primeFactorBitmask = 0;

    int height = -1;
    Node* parent = nullptr;

    array<Node*, log2MaxNodes> ancestorPowerOf2Above;
    array<int, numNodePrimeFactorCombinations> numPrimesWithCombinationToRoot;
};

void fillInAncestors(Node* node, Node* parent, const array<int, numNodePrimeFactorCombinations>& numPrimesWithCombinationToRoot, vector<Node*>& ancestors)
{
    const int height = ancestors.size();
    node->height = height;
    node->parent = parent;

    int powerOf2 = 1;
    for (int exponent = 0; exponent < log2MaxNodes; exponent++)
    {
        if (ancestors.size() >= powerOf2)
        {
            node->ancestorPowerOf2Above[exponent] = ancestors[ancestors.size() - powerOf2];
        }
        else
        {
            node->ancestorPowerOf2Above[exponent] = nullptr;
        }
        powerOf2 *= 2;
    }

    ancestors.push_back(node);
    array<int, numNodePrimeFactorCombinations> nextNumPrimesWithCombinationToRoot = numPrimesWithCombinationToRoot;
    nextNumPrimesWithCombinationToRoot[node->primeFactorBitmask]++;
    node->numPrimesWithCombinationToRoot = nextNumPrimesWithCombinationToRoot;
    for (auto neighbour : node->neighbours)
    {
        if (neighbour == parent)
            continue;
        fillInAncestors(neighbour, node, nextNumPrimesWithCombinationToRoot, ancestors);
    }
    ancestors.pop_back();
}

Node* findKthAncestor(Node* node, int k)
{
    //cout << "originalK: " << k << endl;
    const auto originalHeight = node->height;
    const auto originalK = k;
    Node* ancestor = node;
    for (int exponent = log2MaxNodes - 1; exponent >= 0; exponent--)
    {
        const auto powerOf2 = (1 << exponent);
        //cout << " exponent: " << exponent << " powerOf2: " << powerOf2 << endl;
        if (k >= powerOf2) 
        {
            assert(ancestor->ancestorPowerOf2Above[exponent] && ancestor->ancestorPowerOf2Above[exponent]->height == ancestor->height - powerOf2);
            ancestor = ancestor->ancestorPowerOf2Above[exponent];
            k -= powerOf2;
        }
    }
    //cout << "ancestor height: " << ancestor->height << " original height: " << originalHeight << " originalK: " << originalK << endl;
    assert(ancestor);
    assert(ancestor->height == originalHeight - originalK);
    return ancestor;
}

Node* findLCABruteForce(Node* node1, Node* node2)
{
    while (node1->height > node2->height)
    {
        node1 = node1->parent;
    }
    while (node2->height > node1->height)
    {
        node2 = node2->parent;
    }
    while (node1 != node2)
    {
        node1 = node1->parent;
        node2 = node2->parent;
    }
    assert(node1 && node2);
    assert(node1->height == node2->height);
    return node1;
}

Node* findLCA(Node* node1, Node* node2)
{
    //cout << "findLCA original node1 : " << node1->index << " original node2 : " << node2->index << endl;
    //cout << "findLCA original node1 height: " << node1->height << " original node2 height: " << node2->height << endl;
    if (node1->height != node2->height)
    {
        if (node1->height > node2->height)
        {
            //cout << "adjusting node1" << endl;
            node1 = findKthAncestor(node1, node1->height - node2->height);
        }
        else
        {
            //cout << "adjusting node2" << endl;
            node2 = findKthAncestor(node2, node2->height - node1->height);
            //cout << "New node2 height: " << node2->height << endl;
        }
    }
    assert(node1 && node2);
    assert(node1->height == node2->height);

    int currentNodesHeight = node1->height;
    int minLCAHeight = 0;
    int maxLCAHeight = currentNodesHeight;
    bool found = false;
    while (true)
    {
        const int heightDecrease = (currentNodesHeight - minLCAHeight) / 2;
        //cout << " minLCAHeight: " << minLCAHeight << " currentNodesHeight: " << currentNodesHeight << " heightDecrease: " << heightDecrease << endl; 
        //cout << " lca node1: " << node1->index << " node2: " << node2->index << endl;
        assert(node1->height == node2->height);
        assert(node1->height == currentNodesHeight); 
        if (node1 != node2 && node1->parent == node2->parent && node1->parent)
        {
            //cout << "wee" << endl;
            return node1->parent;
        }

        if (heightDecrease == 0)
        {
            assert(node1 == node2);
            return node1;;
        }
        const int nodesAncestorHeight = currentNodesHeight - heightDecrease;
        auto node1Ancestor = findKthAncestor(node1, heightDecrease);
        auto node2Ancestor = findKthAncestor(node2, heightDecrease);
       // cout << "node1Ancestor: " << node1Ancestor << " node2Ancestor: " << node2Ancestor << endl;
        assert(node1Ancestor && node2Ancestor);

        if (node1Ancestor == node2Ancestor)
        {
            minLCAHeight = nodesAncestorHeight;
        }
        else
        {
            currentNodesHeight = nodesAncestorHeight - 1;
            maxLCAHeight = currentNodesHeight;
            node1 = node1Ancestor->parent;
            node2 = node2Ancestor->parent;
        }
    }
    assert(false);
    return nullptr;
}

vector<Node*> path(Node* node, Node* destNode, Node* parent, vector<Node*>& pathSoFar)
{
    pathSoFar.push_back(node);

    if (node == destNode)
        return pathSoFar;

    for (auto neighbour : node->neighbours)
    {
        if (neighbour == parent)
            continue;

        const auto pathToDestNode = path(neighbour, destNode, node, pathSoFar);

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

int64_t findNumCoprimePairsAlongPath(Node* node1, Node* node2)
{
    const auto lca = findLCA(node1, node2);
    int64_t optimisedResult = 0;
    if (node1 != node2)
    {
        array<int, numNodePrimeFactorCombinations> numPrimesWithCombinationAlongPath = {};
        for (int i = 0; i < numNodePrimeFactorCombinations; i++)
        {
           // cout << "i: " << i << " node1->numPrimesWithCombinationToRoot[i]: " << node1->numPrimesWithCombinationToRoot[i] << "  node2->numPrimesWithCombinationToRoot[i]: " << node2->numPrimesWithCombinationToRoot[i] << " lca->numPrimesWithCombinationToRoot[i]: " << lca->numPrimesWithCombinationToRoot[i] << " node1 == lca? " << (node1 == lca) << " node2 == lca? " << (node2 == lca) << endl;
            if (node1 != lca && node2 != lca)
            {
                numPrimesWithCombinationAlongPath[i] += node1->numPrimesWithCombinationToRoot[i];
                numPrimesWithCombinationAlongPath[i] += node2->numPrimesWithCombinationToRoot[i];
                numPrimesWithCombinationAlongPath[i] -= lca->numPrimesWithCombinationToRoot[i];
                numPrimesWithCombinationAlongPath[i] -= (lca->parent ? lca->parent->numPrimesWithCombinationToRoot[i] : 0);
            }
            else if (node1 == lca)
            {
                numPrimesWithCombinationAlongPath[i] += node2->numPrimesWithCombinationToRoot[i] - (lca->parent ? lca->parent->numPrimesWithCombinationToRoot[i] : 0);
            }
            else if (node2 == lca)
            {
                numPrimesWithCombinationAlongPath[i] += node1->numPrimesWithCombinationToRoot[i] - (lca->parent ? lca->parent->numPrimesWithCombinationToRoot[i] : 0);
            }
            assert(numPrimesWithCombinationAlongPath[i] >= 0);
        }
        for (int i = 0; i < numNodePrimeFactorCombinations; i++)
        {
            for (int j = 0; j < numNodePrimeFactorCombinations; j++)
            {
                if ((i & j) == 0)
                {
                    optimisedResult += numPrimesWithCombinationAlongPath[i] * numPrimesWithCombinationAlongPath[j];
                }
            }
        }
        optimisedResult -= numPrimesWithCombinationAlongPath[0];
    }
    optimisedResult /= 2;
    return optimisedResult;
}

int main()
{
    int n, q;
//#define RANDOM
#ifndef RANDOM
    cin >> n >> q;

    vector<Node> nodes(n);

    for (int i = 0; i < n; i++)
    {
        int nodeValue;
        cin >> nodeValue;
        nodes[i].value = nodeValue;
        nodes[i].index = i + 1;
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
#else
    srand(time(0));
    while (true)
    {
        //n = 9;
        n = rand() % 1000 + 1;
        cout << "n: " << n << endl;
        const int numPrimes = (rand() % 3) + 1;
        vector<int> primesToUse;
        for (int i = 0; i < numPrimes; i++)
        {
            while (true)
            {
                const int prime = rand() % 10'000;
                if (isPrime(prime))
                {
                    primesToUse.push_back(prime);
                    cout << " using prime: " << prime << endl;
                    break;
                }
            }
        }
        cout << "#primesToUse: " << primesToUse.size() << endl;
        vector<Node> nodes;
        nodes.reserve(n);
        nodes.push_back(Node());
        nodes.front().index = 0;
        for (int i = 1; i < n; i++)
        {
            int parentIndex = rand() % i;
            nodes.push_back(Node());

            auto node1 = &(nodes[i]);
            auto node2 = &(nodes[parentIndex]);
            node1->index = i;


            node1->neighbours.push_back(node2);
            node2->neighbours.push_back(node1);

            cout << "node " << i << " has parent: " << parentIndex << endl;
        }
        for (auto& node : nodes)
        {
            int64_t nodeValue = 1;
            while (true)
            {
                nodeValue = 1;
                bool wentBust = false;
                for (const auto prime : primesToUse)
                {
                    int exponent = rand() % 3;
                    for (int j = 0; j < exponent; j++)
                    {
                        nodeValue *= prime;
                        if (nodeValue > 10'000'000 || nodeValue <= 0)
                        {
                            wentBust = true;
                            break;
                        }
                        assert(nodeValue > 0);
                    }
                }
                cout << "candidate nodeValue: " << nodeValue << endl;
                if (!wentBust && 1 <= nodeValue && nodeValue <= 10'000'000)
                    break;
            }
            node.value = nodeValue;
            node.value = 1;
            cout << "using " << nodeValue << " for node: " << node.index << endl;
        }
#endif

        int largestNodeValue = 0;
        for (auto& node : nodes)
        {
            largestNodeValue = max(node.value, largestNodeValue);
        }

        const int primeLimit = sqrt(largestNodeValue);
        vector<bool> isPrime(largestNodeValue + 1, true);
        for (int factor = 2; factor <= primeLimit; factor++)
        {
            int i = 2 * factor;
            while (i <= largestNodeValue)
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
            const auto originalNodeValue = nodeValue;
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
                    //cout << "Found new prime factor: " << primeFactor << " for nodeValue: " << originalNodeValue << endl;
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

        vector<Node*> ancestors;
        auto rootNode = &(nodes.front());
        //cout << " rootNode: " << rootNode << endl;
        array<int, numNodePrimeFactorCombinations> numPrimesWithCombinationToRoot = {};
        fillInAncestors(rootNode, nullptr, numPrimesWithCombinationToRoot, ancestors);

        for (int i = 0; i < q; i++)
        {
            int u, v;
            cin >> u >> v;
            u--;
            v--;

            auto node1 = &(nodes[u]);
            auto node2 = &(nodes[v]);
            const auto optimisedResult = findNumCoprimePairsAlongPath(node1, node2);
//#define BRUTE_FORCE
#ifdef BRUTE_FORCE
            const auto bruteForceResult = bruteForce(node1, node2);
            cout << "bruteForceResult: " << bruteForceResult << " optimisedResult: " << optimisedResult << endl;
            const auto lcaBruteForce = findLCABruteForce(node1, node2);
            const auto lcaOpt = findLCA(node1, node2);
            cout << "node1: " << node1->index << " node2: " << node2->index << endl;
            cout << "lcaOpt index: " << lcaOpt->index << " lcaBruteForce index: " << lcaBruteForce->index << endl;
            assert(lcaOpt == lcaBruteForce);
#endif
            cout << optimisedResult << endl;


        }
//#define EXHAUSTIVE
#ifdef EXHAUSTIVE
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                auto node1 = &(nodes[i]);
                auto node2 = &(nodes[j]);
                const auto lcaOpt = findLCA(node1, node2);
                const auto lcaBruteForce = findLCABruteForce(node1, node2);
                cout << "lcaOpt index: " << lcaOpt->index << " lcaBruteForce index: " << lcaBruteForce->index << endl;
                assert(lcaOpt == lcaBruteForce);
                const auto bruteForceResult = bruteForce(node1, node2);
                const auto optimisedResult = findNumCoprimePairsAlongPath(node1, node2);
                //assert(optimisedResult % 2 == 0);
                cout << "bruteForceResult: " << bruteForceResult << " optimisedResult: " << optimisedResult << endl;
                assert(bruteForceResult == optimisedResult);

            }
        }
#endif
#ifdef RANDOM
    }
#endif

}

