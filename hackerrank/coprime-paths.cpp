// Simon St James (ssjgz) 2017-10-14
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <cmath>
#include <cassert>

using namespace std;

const int maxNodeValue = 10'000'000;

constexpr int maxNumNodes = 25'000;
constexpr int log2(int N, int exponent = 0, int powerOf2 = 1)
{
        return (powerOf2 >= N) ? exponent : log2(N, exponent + 1, powerOf2 * 2);
}
constexpr int log2MaxNumNodes = log2(maxNumNodes);

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

    array<Node*, log2MaxNumNodes> ancestorPowerOf2Above;

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
    for (int exponent = 0; exponent < log2MaxNumNodes; exponent++)
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
    for (int exponent = log2MaxNumNodes - 1; exponent >= 0; exponent--)
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

vector<Query> rearrangedQueriesWithMosAlgorithm(const vector<Query>& queries, int numNodes)
{
    vector<Query> rearrangedQueries = queries;
    for (auto& query : rearrangedQueries)
    {
        // Things are simpler if we can assume that node1 is first reached before node2 in the DFS.
        if (query.node1->startIndexInDFSArray > query.node2->startIndexInDFSArray)
            swap(query.node1, query.node2);

        query.lca = findLCA(query.node1, query.node2);
        assert((query.lca != query.node1 && query.lca != query.node2) || query.lca == query.node1);

        // See the documentation on the codeforces article.
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


    const int sqrtN = sqrt(numNodes);
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

    return rearrangedQueries;
}

class NumCoprimePairsInPathTracker
{
    public:
        NumCoprimePairsInPathTracker(const int numNodes)
            : m_numGeneratedByNodesInPath(maxNodeValue + 1)
        {
        }
        void onNodeAddedToPath(const Node* node)
        {
            m_resultForPath += changeInSumIfNodeAddedToPath(node);
            m_numNodesInPath++;
            updateNumGeneratedByNodesAlongPath(node, 1);
        };
        void onNodeRemovedFromPath(const Node* node)
        {
            m_numNodesInPath--;
            updateNumGeneratedByNodesAlongPath(node, -1);
            m_resultForPath -= changeInSumIfNodeAddedToPath(node);
        };
        int64_t resultForPath() const
        {
            return m_resultForPath;
        }
    private:
        int m_numNodesInPath = 0;
        int64_t m_resultForPath = 0;
        // For each node N in the path, add one to m_numGeneratedByNodesInPath[x] where 
        // x is any of the numbers that can be formed by multiplying the primes in the 
        // prime factorisation of N's node-value.
        vector<int> m_numGeneratedByNodesInPath;
        int64_t changeInSumIfNodeAddedToPath(const Node* node)
        {
            int64_t numNonCoPrimeNodes = 0;
            const auto nodePrimeFactors = node->primeFactors;
            switch(node->numPrimeFactors)
            {
                case 1:
                    // We share a factor with all nodes in the path that also have this prime factor.
                    numNonCoPrimeNodes += m_numGeneratedByNodesInPath[nodePrimeFactors[0]];
                    break;
                case 2:
                    // We share a factor with all nodes in the path that also any of these two prime factors ...
                    numNonCoPrimeNodes += m_numGeneratedByNodesInPath[nodePrimeFactors[0]];
                    numNonCoPrimeNodes += m_numGeneratedByNodesInPath[nodePrimeFactors[1]];

                    // ... but we'll overcount by the number of nodes in the path that have *both* of these prime
                    // factors.
                    numNonCoPrimeNodes -= m_numGeneratedByNodesInPath[nodePrimeFactors[0] * nodePrimeFactors[1]];
                    break;
                case 3:
                    // As in case 2, but this time with 3 primes ...
                    numNonCoPrimeNodes += m_numGeneratedByNodesInPath[nodePrimeFactors[0]];
                    numNonCoPrimeNodes += m_numGeneratedByNodesInPath[nodePrimeFactors[1]];
                    numNonCoPrimeNodes += m_numGeneratedByNodesInPath[nodePrimeFactors[2]];

                    // ... correct overcount ...
                    numNonCoPrimeNodes -= m_numGeneratedByNodesInPath[nodePrimeFactors[0] * nodePrimeFactors[1]];
                    numNonCoPrimeNodes -= m_numGeneratedByNodesInPath[nodePrimeFactors[0] * nodePrimeFactors[2]];
                    numNonCoPrimeNodes -= m_numGeneratedByNodesInPath[nodePrimeFactors[1] * nodePrimeFactors[2]];

                    // ... but in subtracting our overcount, we'll *undercount* by the number of nodes in the path that
                    // share all 3 factors with node XD
                    numNonCoPrimeNodes += m_numGeneratedByNodesInPath[nodePrimeFactors[0] * nodePrimeFactors[1] * nodePrimeFactors[2]];
                    break;
            }
            int64_t increaseInSum = m_numNodesInPath - numNonCoPrimeNodes;

            return increaseInSum;
        };
        void updateNumGeneratedByNodesAlongPath(const Node* node, int numCopiesOfNodeAdded)
        {
            assert(numCopiesOfNodeAdded == 1 || numCopiesOfNodeAdded == -1);
            // See comment for m_numGeneratedByNodesInPath.
            const auto nodePrimeFactors = node->primeFactors;
            switch(node->numPrimeFactors)
            {
                case 1:
                    m_numGeneratedByNodesInPath[nodePrimeFactors[0]] += numCopiesOfNodeAdded;
                    break;
                case 2:
                    m_numGeneratedByNodesInPath[nodePrimeFactors[0]] += numCopiesOfNodeAdded;
                    m_numGeneratedByNodesInPath[nodePrimeFactors[1]] += numCopiesOfNodeAdded;
                    m_numGeneratedByNodesInPath[nodePrimeFactors[0] * nodePrimeFactors[1]] += numCopiesOfNodeAdded;
                    break;
                case 3:
                    m_numGeneratedByNodesInPath[nodePrimeFactors[0]] += numCopiesOfNodeAdded;
                    m_numGeneratedByNodesInPath[nodePrimeFactors[1]] += numCopiesOfNodeAdded;
                    m_numGeneratedByNodesInPath[nodePrimeFactors[2]] += numCopiesOfNodeAdded;
                    m_numGeneratedByNodesInPath[nodePrimeFactors[0] * nodePrimeFactors[1]] += numCopiesOfNodeAdded;
                    m_numGeneratedByNodesInPath[nodePrimeFactors[0] * nodePrimeFactors[2]] += numCopiesOfNodeAdded;
                    m_numGeneratedByNodesInPath[nodePrimeFactors[1] * nodePrimeFactors[2]] += numCopiesOfNodeAdded;
                    m_numGeneratedByNodesInPath[nodePrimeFactors[0] * nodePrimeFactors[1] * nodePrimeFactors[2]] += numCopiesOfNodeAdded;
                    break;
            }
        };
};

vector<int64_t> solve(const vector<Query>& queries, vector<Node>& nodes)
{
    const int numNodes = nodes.size();

    const auto dfsArray = buildDFSArray(nodes);
    const auto rearrangedQueries = rearrangedQueriesWithMosAlgorithm(queries, nodes.size());

    int leftPointer = rearrangedQueries.front().leftIndex;
    int rightPointer = rearrangedQueries.front().leftIndex;

    NumCoprimePairsInPathTracker resultTracker(numNodes);
    vector<int> nodeCountInRange(numNodes);

    auto addNodeCount = [&resultTracker, &nodeCountInRange](const auto& node, int increase)
    {
        // The nodes is in the path if and only if it occurs exactly once in the range leftPointer - rightPointer
        // in dfsArray.
        assert(increase != 0);
        if (nodeCountInRange[node->index] == 1)
            resultTracker.onNodeRemovedFromPath(node);
        nodeCountInRange[node->index] += increase;
        if (nodeCountInRange[node->index] == 1)
            resultTracker.onNodeAddedToPath(node);
    };
    addNodeCount(dfsArray[leftPointer], 1);
    vector<int64_t> querySolutions(queries.size());
    // Generate paths - by calling onNodeAddedToPath and onNodeRemovedFromPath on resultTracker -
    // by using the algorithm for processing queries ordered by Mo's Algorithm.
    for (const auto query : rearrangedQueries)
    {
        const int newLeftPointer = query.leftIndex;
        const int newRightPointer = query.rightIndex;
        auto lca = findLCA(query.node1, query.node2);

        while (leftPointer < newLeftPointer)
        {
            addNodeCount(dfsArray[leftPointer], -1);
            leftPointer++;
        }
        while (leftPointer > newLeftPointer)
        {
            leftPointer--;
            addNodeCount(dfsArray[leftPointer], 1);
        }
        while (rightPointer < newRightPointer)
        {
            rightPointer++;
            addNodeCount(dfsArray[rightPointer], 1);
        }
        while (rightPointer > newRightPointer)
        {
            addNodeCount(dfsArray[rightPointer], -1);
            rightPointer--;
        }

        const bool needToAddLCA = (query.lca != query.node1);
        if (needToAddLCA)
        {
            resultTracker.onNodeAddedToPath(query.lca);
        }

        const auto finalResultForPath = resultTracker.resultForPath();
        assert(query.originalQueryIndex >= 0 && query.originalQueryIndex < querySolutions.size());
        querySolutions[query.originalQueryIndex] = finalResultForPath;

        if (needToAddLCA)
            resultTracker.onNodeRemovedFromPath(query.lca);
    }
    return querySolutions;
}

int main()
{
    // http://codeforces.com/blog/entry/43230
    int n, q;
    cin >> n >> q;
    assert(n <= maxNumNodes);

    vector<Node> nodes(n);

    for (int i = 0; i < n; i++)
    {
        int nodeValue;
        cin >> nodeValue;
        assert(nodeValue <= maxNodeValue);
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

    for (auto& node : nodes)
    {
        const auto nodeValuePrimeFactors = primeFactors(node.value);
        assert(nodeValuePrimeFactors.size() <= 3);
        node.numPrimeFactors = nodeValuePrimeFactors.size();
        std::copy(nodeValuePrimeFactors.begin(), nodeValuePrimeFactors.end(), node.primeFactors);
    }

    vector<Query> queries;
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
    const auto solutions = solve(queries, nodes);
    for (const auto solution : solutions)
    {
        cout << solution << endl;
    }

}

