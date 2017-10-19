// Simon St James (ssjgz) 2017-10-14
#define SUBMISSION
#ifdef SUBMISSION
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

// Fill in ancestor info for each node, and also set start/endIndexInDFSArray.
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
    const auto originalHeight = node->height;
    const auto originalK = k;
    Node* ancestor = node;
    for (int exponent = log2MaxNodes - 1; exponent >= 0; exponent--)
    {
        const auto powerOf2 = (1 << exponent);
        if (k >= powerOf2) 
        {
            assert(ancestor->ancestorPowerOf2Above[exponent] && ancestor->ancestorPowerOf2Above[exponent]->height == ancestor->height - powerOf2);
            ancestor = ancestor->ancestorPowerOf2Above[exponent];
            k -= powerOf2;
        }
    }
    assert(ancestor);
    assert(ancestor->height == originalHeight - originalK);
    return ancestor;
}


Node* findLCA(Node* node1, Node* node2)
{
    // Equalise node heights.
    if (node1->height != node2->height)
    {
        if (node1->height > node2->height)
        {
            node1 = findKthAncestor(node1, node1->height - node2->height);
        }
        else
        {
            node2 = findKthAncestor(node2, node2->height - node1->height);
        }
    }
    assert(node1 && node2);
    assert(node1->height == node2->height);

    // Use a binary search to find the common ancestor: since the 
    // nodes always have the same height, if we jump both nodes up by x places
    // and they are still not equal, then the lca is still above them: if they
    // do become equal, the the lca is at or below them.
    int currentNodesHeight = node1->height;
    int minLCAHeight = 0;
    int maxLCAHeight = currentNodesHeight;
    bool found = false;
    while (true)
    {
        const int heightDecrease = (currentNodesHeight - minLCAHeight) / 2;
        assert(node1->height == node2->height);
        assert(node1->height == currentNodesHeight); 
        if (node1 != node2 && node1->parent == node2->parent && node1->parent)
            return node1->parent;

        if (heightDecrease == 0)
        {
            assert(node1 == node2);
            return node1;;
        }
        const int nodesAncestorHeight = currentNodesHeight - heightDecrease;
        auto node1Ancestor = findKthAncestor(node1, heightDecrease);
        auto node2Ancestor = findKthAncestor(node2, heightDecrease);
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
    assert(false && "Shouldn't be able to reach here!");
    return nullptr;
}

struct Query
{
    Node *node1 = nullptr;
    Node *node2 = nullptr;
    Node *lca = nullptr;
    int originalQueryIndex = -1;

    int leftIndex = -1;
    int rightIndex = -1;
};

vector<Node*> buildDFSArray(vector<Node>& nodes)
{    
    auto rootNode = &(nodes.front());
    int indexInDFSArray = 0;
    vector<Node*> ancestors;
    fillInDFSInfo(rootNode, nullptr, ancestors, indexInDFSArray);
    vector<Node*> dfsArray(indexInDFSArray);
    for (auto& node : nodes)
    {
        dfsArray[node.startIndexInDFSArray] = &node;
        dfsArray[node.endIndexInDFSArray] = &node;
    }
    return dfsArray;
}

vector<int64_t> solve(const vector<Query>& queries, vector<Node>& nodes)
{
    const int n = nodes.size();

    const auto dfsArray = buildDFSArray(nodes);

    vector<Query> rearrangedQueries = queries;
    for (auto& query : rearrangedQueries)
    {
        // Things are simpler if we can assume that node1 is first reached before node2 in the DFS.
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


    const int sqrtN = sqrt(nodes.size());
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
#ifdef RANDOM
    }
#endif

}

