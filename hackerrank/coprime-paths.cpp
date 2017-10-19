// Simon St James (ssjgz) 2017-10-14
#define BRUTE_FORCE
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#undef BRUTE_FORCE
#endif
#include <iostream>
#include <vector>
#include <array>
#include <set>
#include <algorithm>
#include <cmath>
#include <cassert>

using namespace std;

const int maxNodeValue = 10'000'000;

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

vector<int> primeFactors(int n)
{
    bool addedPrime = false;
    vector<int> primeFactors;
    int factor = 2;
    while (factor * factor <= n)
    {
        addedPrime = false;
        while ((n % factor) == 0)
        {
            assert(isPrime(factor));
            if (!addedPrime)
            {
                primeFactors.push_back(factor);
                addedPrime = true;
            }
            n /= factor;
        }
        factor++;
    }
    if (n != 1)
    {
        assert(isPrime(n));
        primeFactors.push_back(n);
    }
    return primeFactors;
}

struct Node
{
    int value = -1;
    int numPrimeFactors = 0;
    int primeFactors[3] = {};

    int index = -1;
    vector<Node*> neighbours;
    int primeFactorBitmask = 0;

    int height = -1;
    Node* parent = nullptr;

    array<Node*, log2MaxNodes> ancestorPowerOf2Above;

    int startIndexInDFSArray = -1;
    int endIndexInDFSArray = -1;
};

void fillInDFSInfo(Node* node, Node* parent, vector<Node*>& ancestors, int& indexInDFSArray)
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
    node->startIndexInDFSArray = indexInDFSArray;
    indexInDFSArray++;
    for (auto neighbour : node->neighbours)
    {
        if (neighbour == parent)
            continue;
        fillInDFSInfo(neighbour, node, ancestors, indexInDFSArray);
    }
    node->endIndexInDFSArray = indexInDFSArray;
    indexInDFSArray++;
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
    cout << " path: node " << node->index << " " << node << endl;
    pathSoFar.push_back(node);

    if (node == destNode)
        return pathSoFar;

    for (auto neighbour : node->neighbours)
    {
        cout << "neighbour: " << neighbour << endl;
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

int64_t valueForPath(const vector<Node*>& path)
{
    int64_t result = 0;
    for (auto nodeInPathIndex = 0; nodeInPathIndex < path.size(); nodeInPathIndex++)
    {
        for (auto nodeInPathIndex2 = nodeInPathIndex + 1; nodeInPathIndex2 < path.size(); nodeInPathIndex2++)
        {
            auto nodeInPath1 = path[nodeInPathIndex];
            auto nodeInPath2 = path[nodeInPathIndex2];

            bool areCoprime = true;
            for (auto primeIndex1 = 0; primeIndex1 < nodeInPath1->numPrimeFactors; primeIndex1++)
            {
                for (auto primeIndex2 = 0; primeIndex2 < nodeInPath2->numPrimeFactors; primeIndex2++)
                {
                    if (nodeInPath1->primeFactors[primeIndex1] == nodeInPath2->primeFactors[primeIndex2])
                    {
                        areCoprime = false;
                    }
                }
            }

            cout << "pair: " << nodeInPath1->index << "[" << nodeInPath1->value << "]," << nodeInPath2->index << "[" << nodeInPath2->value << "]" << " coprime? " << areCoprime << endl;

            if (areCoprime)
                result++;
        }
    }
    return result;
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

#if 0
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
#endif

struct Query
{
    Node *node1 = nullptr;
    Node *node2 = nullptr;
    Node *lca = nullptr;
    int originalQueryIndex = -1;

    int leftIndex = -1;
    int rightIndex = -1;
};

vector<int64_t> solve(const vector<Query>& queries, vector<Node>& nodes)
{
    const int n = nodes.size();
    auto rootNode = &(nodes.front());
    int indexInDFSArray = 0;
    vector<Node*> ancestors;
    fillInDFSInfo(rootNode, nullptr, ancestors, indexInDFSArray);
    vector<Query> rearrangedQueries = queries;
    for (auto& query : rearrangedQueries)
    {
        //if (query.node1->startIndexInDFSArray > query.node2->startIndexInDFSArray)
            //swap(query.node1, query.node2);

        //query.leftIndex = query.node1->startIndexInDFSArray;
        //query.rightIndex = query.node2->endIndexInDFSArray;
        if (query.node1->startIndexInDFSArray > query.node2->startIndexInDFSArray)
            swap(query.node1, query.node2);

        query.lca = findLCA(query.node1, query.node2);
        assert((query.lca != query.node1 && query.lca != query.node2) || query.lca == query.node1);

        if (query.lca == query.node1)
        {
            query.leftIndex = query.node1->startIndexInDFSArray;
            query.rightIndex = query.node2->startIndexInDFSArray;
        }
        else
        {
            query.leftIndex = query.node1->endIndexInDFSArray;
            query.rightIndex = query.node2->startIndexInDFSArray;
        }
    }

    vector<Node*> dfsArray(indexInDFSArray);
    for (auto& node : nodes)
    {
        dfsArray[node.startIndexInDFSArray] = &node;
        dfsArray[node.endIndexInDFSArray] = &node;
    }
    //cout << "dfsArray: " << endl;
    for (const auto& node : dfsArray)
    {
        //cout << node->index << " "; 
    }
    //cout << endl;

    const int sqrtN = sqrt(n);
    auto mosAlgorithmOrdering = [sqrtN](const Query& lhs, const Query& rhs)
        {
            const int lhsBucket = lhs.leftIndex / sqrtN;
            const int rhsBucket = rhs.leftIndex / sqrtN;
            if (lhsBucket != rhsBucket)
            {
                return lhsBucket < rhsBucket;
            }
            return lhs.rightIndex < rhs.rightIndex;
        };

    sort(rearrangedQueries.begin(), rearrangedQueries.end(), mosAlgorithmOrdering);
    int leftPointer = rearrangedQueries.front().leftIndex;
    int rightPointer = rearrangedQueries.front().leftIndex;
    vector<int> nodeCountInRange(nodes.size());
    //cout << "About to do  thing" << endl;
    set<Node*> nodesInPath;
    int64_t resultForPath = 0;
    vector<int> numGeneratedByNodesInPath(maxNodeValue + 1);
    auto changeInSumIfNodeAddedToPath = [&numGeneratedByNodesInPath, &nodesInPath](const auto& node) -> int64_t 
        {
            //cout << " changeInSumIfNodeAddedToPath for node: " << node->index << endl;
            int64_t numNonCoPrimeNodes = 0;
            switch(node->numPrimeFactors)
            {
                case 1:
                    numNonCoPrimeNodes += numGeneratedByNodesInPath[node->primeFactors[0]];
                    break;
                case 2:
                    numNonCoPrimeNodes += numGeneratedByNodesInPath[node->primeFactors[0]];
                    numNonCoPrimeNodes += numGeneratedByNodesInPath[node->primeFactors[1]];

                    numNonCoPrimeNodes -= numGeneratedByNodesInPath[node->primeFactors[0] * node->primeFactors[1]];
                    break;
                case 3:
                    numNonCoPrimeNodes += numGeneratedByNodesInPath[node->primeFactors[0]];
                    numNonCoPrimeNodes += numGeneratedByNodesInPath[node->primeFactors[1]];
                    numNonCoPrimeNodes += numGeneratedByNodesInPath[node->primeFactors[2]];

                    numNonCoPrimeNodes -= numGeneratedByNodesInPath[node->primeFactors[0] * node->primeFactors[1]];
                    numNonCoPrimeNodes -= numGeneratedByNodesInPath[node->primeFactors[0] * node->primeFactors[2]];
                    numNonCoPrimeNodes -= numGeneratedByNodesInPath[node->primeFactors[1] * node->primeFactors[2]];

                    numNonCoPrimeNodes += numGeneratedByNodesInPath[node->primeFactors[0] * node->primeFactors[1] * node->primeFactors[2]];
                    break;
            }
            int64_t increaseInSum = nodesInPath.size() - numNonCoPrimeNodes;
            //cout << " #nodesInPath: " << nodesInPath.size() << endl;
            //cout << " numNonCoPrimeNodes: " << numNonCoPrimeNodes << endl;


            return increaseInSum;
        };
    auto updateNumGeneratedByNodesAlongPath = [&numGeneratedByNodesInPath](const auto& node, int numCopiesOfNodeAdded)
    {
        switch(node->numPrimeFactors)
        {
            case 1:
                numGeneratedByNodesInPath[node->primeFactors[0]] += numCopiesOfNodeAdded;
                break;
            case 2:
                numGeneratedByNodesInPath[node->primeFactors[0]] += numCopiesOfNodeAdded;
                numGeneratedByNodesInPath[node->primeFactors[1]] += numCopiesOfNodeAdded;
                numGeneratedByNodesInPath[node->primeFactors[0] * node->primeFactors[1]] += numCopiesOfNodeAdded;
                break;
            case 3:
                numGeneratedByNodesInPath[node->primeFactors[0]] += numCopiesOfNodeAdded;
                numGeneratedByNodesInPath[node->primeFactors[1]] += numCopiesOfNodeAdded;
                numGeneratedByNodesInPath[node->primeFactors[2]] += numCopiesOfNodeAdded;
                numGeneratedByNodesInPath[node->primeFactors[0] * node->primeFactors[1]] += numCopiesOfNodeAdded;
                numGeneratedByNodesInPath[node->primeFactors[0] * node->primeFactors[2]] += numCopiesOfNodeAdded;
                numGeneratedByNodesInPath[node->primeFactors[1] * node->primeFactors[2]] += numCopiesOfNodeAdded;
                numGeneratedByNodesInPath[node->primeFactors[0] * node->primeFactors[1] * node->primeFactors[2]] += numCopiesOfNodeAdded;
                break;
        }
    };
    auto onNodeAddedToPath = [&nodesInPath, &resultForPath, &changeInSumIfNodeAddedToPath, &updateNumGeneratedByNodesAlongPath](const auto& node)
    {
        resultForPath += changeInSumIfNodeAddedToPath(node);
        assert(nodesInPath.find(node) == nodesInPath.end());
        nodesInPath.insert(node);
        updateNumGeneratedByNodesAlongPath(node, 1);
    };
    auto onNodeRemovedFromPath = [&nodesInPath, &resultForPath, &changeInSumIfNodeAddedToPath, &updateNumGeneratedByNodesAlongPath](const auto& node)
    {
        assert(nodesInPath.find(node) != nodesInPath.end());
        nodesInPath.erase(nodesInPath.find(node));
        updateNumGeneratedByNodesAlongPath(node, -1);
        resultForPath -= changeInSumIfNodeAddedToPath(node);
    };

    auto addNodeCount = [&nodeCountInRange, &onNodeAddedToPath, &onNodeRemovedFromPath](const auto& node, int increase)
    {
        assert(increase != 0);
        if (nodeCountInRange[node->index] == 1)
            onNodeRemovedFromPath(node);
        nodeCountInRange[node->index] += increase;
        //assert(nodeCountInRange[node->index] <= 2);
        //cout << " count for node index  " << node->index << " now: " << nodeCountInRange[node->index] << endl;
        if (nodeCountInRange[node->index] == 1)
            onNodeAddedToPath(node);
    };
    addNodeCount(dfsArray[leftPointer], 1);
    //addNode(dfsArray[leftPointer]);
    vector<int64_t> querySolutions(queries.size());
    for (const auto query : rearrangedQueries)
    {
        //cout << " loop!" << endl;
        const int newLeftPointer = query.leftIndex;
        const int newRightPointer = query.rightIndex;
        auto lca = findLCA(query.node1, query.node2);
        //cout << " node1 index: " << query.node1->index << " node2 index: " << query.node2->index << " lca index: " << lca->index << endl;
        //cout << " leftPointer: " << leftPointer << " rightPointer: " << rightPointer << " newLeftPointer: " << newLeftPointer << " newRightPointer: " << newRightPointer << endl;
        //cout << "resultForPath: " << resultForPath << endl;
        //assert(find(nodes.begin(), nodes.end(), query.node1) != nodes.end());
        //assert(find(nodes.begin(), nodes.end(), query.node2) != nodes.end());

        while (leftPointer < newLeftPointer)
        {
            //cout << "incrementing left pointer" << endl;
            addNodeCount(dfsArray[leftPointer], -1);
            leftPointer++;
            //addNode(dfsArray[leftPointer]);
        }
        while (leftPointer > newLeftPointer)
        {
            //removeNode(dfsArray[leftPointer]);
            leftPointer--;
            //cout << "decrementing left pointer" << endl;
            addNodeCount(dfsArray[leftPointer], 1);
        }
        while (rightPointer < newRightPointer)
        {
            //removeNode(dfsArray[rightPointer]);
            //cout << "incrementing right pointer" << endl;
            rightPointer++;
            addNodeCount(dfsArray[rightPointer], 1);
        }
        while (rightPointer > newRightPointer)
        {
            addNodeCount(dfsArray[rightPointer], -1);
            rightPointer--;
            //cout << "decrementing right pointer" << endl;
            //addNode(dfsArray[rightPointer]);
        }
        //cout << "Finished pointer adjustment" << endl;

        const bool needToAddLCA = (query.lca != query.node1);
        if (needToAddLCA)
        {
            assert(nodesInPath.find(query.lca) == nodesInPath.end());
            nodesInPath.insert(lca);
        }
#ifdef BRUTE_FORCE
        vector<Node*> pathSoFar;
        const auto dbgPath = path(query.node1, query.node2, nullptr, pathSoFar);
        set<Node*> dbgPathNodes(dbgPath.begin(), dbgPath.end());
        cout << " current path nodes: " << endl;
        for (const auto& node : nodesInPath)
        {
            cout << "node" << node->index << "[" << node->value << "] ";
        }
        cout << endl;
        cout << " current dbgPath nodes: " << endl;
        for (const auto& node : dbgPathNodes)
        {
            cout << node->index << " ";
        }
        cout << endl;
        assert(dbgPathNodes == nodesInPath);
#endif
        if (needToAddLCA)
            nodesInPath.erase(nodesInPath.find(lca));
        const auto finalResultForPath = resultForPath + (!needToAddLCA ? 0 : changeInSumIfNodeAddedToPath(lca));
#ifdef BRUTE_FORCE
        cout << "finalResultForPath: " << finalResultForPath << endl;
        const auto dbgResultForPath = valueForPath(dbgPath);
        cout << "dbgResultForPath: " << dbgResultForPath << endl;
        assert(dbgResultForPath == finalResultForPath);
#endif
        assert(query.originalQueryIndex >= 0 && query.originalQueryIndex < querySolutions.size());
        querySolutions[query.originalQueryIndex] = finalResultForPath;
    }
    return querySolutions;
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
        nodes[i].index = i;
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
#error sausage
    while (true)
    {
        //n = 9;
        n = rand() % 1000 + 2;
        cout << "n: " << n << endl;
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

            cout << "node with index " << node1->index << " has parent with index: " << nodes[parentIndex].index << endl;
        }
        for (auto& node : nodes)
        {
            int64_t nodeValue = 1;
            while (true)
            {
                nodeValue = rand() % 1'000'000;
                if (primeFactors(nodeValue).size() <= 3)
                    break;
                //cout << " does not have 3 prime factors" << endl;
            }
            node.value = nodeValue;
            //cout << "using " << nodeValue << " for node: " << node.index << endl;
        }
#endif

        for (auto& node : nodes)
        {
            const auto nodeValuePrimeFactors = primeFactors(node.value);
            assert(nodeValuePrimeFactors.size() <= 3);
            node.numPrimeFactors = nodeValuePrimeFactors.size();
            std::copy(nodeValuePrimeFactors.begin(), nodeValuePrimeFactors.end(), node.primeFactors);
        }


        vector<Query> queries;
#ifndef RANDOM
        for (int i = 0; i < q; i++)
        {
            int u, v;
            cin >> u >> v;
            u--;
            v--;

            auto node1 = &(nodes[u]);
            auto node2 = &(nodes[v]);
            Query query;
            query.node1 = node1;
            query.node2 = node2;
            query.originalQueryIndex = i;
            queries.push_back(query);
//#define BRUTE_FORCE
//#ifdef BRUTE_FORCE
            //const auto bruteForceResult = bruteForce(node1, node2);
            //cout << "bruteForceResult: " << bruteForceResult << " optimisedResult: " << optimisedResult << endl;
            //const auto lcaBruteForce = findLCABruteForce(node1, node2);
            //const auto lcaOpt = findLCA(node1, node2);
            //cout << "node1: " << node1->index << " node2: " << node2->index << endl;
            //cout << "lcaOpt index: " << lcaOpt->index << " lcaBruteForce index: " << lcaBruteForce->index << endl;
            //assert(lcaOpt == lcaBruteForce);
//#endif
            //cout << optimisedResult << endl;


        }
#else
        q = n;
        for (int i = 0; i < q; i++)
        {
            while(true)
            {
                int u = rand() % n;
                int v = rand() % n;
                if (u == v)
                    continue;
                Query query;
                query.node1 = &(nodes[u]);
                query.node2 = &(nodes[v]);
                query.originalQueryIndex = i;
                queries.push_back(query);
                break;
            }
        }
#endif
        const auto solutions = solve(queries, nodes);
        for (const auto solution : solutions)
        {
            cout << solution << endl;
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

