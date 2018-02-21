// Editorial solution for "Move the Coins 2".
// Simon St James (ssjgz) 2018-02-21.
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

constexpr auto maxN = 100000;
constexpr int log2(int N, int exponent = 0, int powerOf2 = 1)
{
    return (powerOf2 >= N) ? exponent : log2(N, exponent + 1, powerOf2 * 2);
}
constexpr int log2MaxN = log2(maxN);

struct Node;
struct Query
{
    Node* nodeToMove = nullptr;
    Node* newParent = nullptr;
};

struct QueryForNode
{
    QueryForNode(int originalQueryIndex, int heightChange)
        : originalQueryIndex{originalQueryIndex}, heightChange{heightChange}
    {
    }
    int originalQueryIndex = -1;
    int heightChange;
    int originalCoinsThatMakeDigitOne[log2MaxN + 1] = {};
};

struct Node
{
    vector<Node*> children;
    bool hasCoin = false;
    Node* parent = nullptr;
    int originalHeight = -1;

    int grundyNumber = -1;
    vector<QueryForNode> queriesForNode;
};


void fixParentChildAndHeights(Node* node, Node* parent = nullptr, int height = 0)
{
    node->originalHeight = height;
    node->parent = parent;

    node->children.erase(remove(node->children.begin(), node->children.end(), parent), node->children.end());

    for (auto child : node->children)
    {
        fixParentChildAndHeights(child, node, height + 1);
    }
}

int findGrundyNumbersForNodes(Node* node)
{
    auto grundyNumber = 0;
    if (node->hasCoin)
        grundyNumber ^= node->originalHeight;

    for (auto child : node->children)
    {
        grundyNumber ^= findGrundyNumbersForNodes(child);
    }

    node->grundyNumber = grundyNumber;

    return grundyNumber;
}

class SegmentTree
{
    public:
        SegmentTree() = default;
        SegmentTree(int numElements)
            : m_numElements{numElements},
            m_elements(numElements)
            {
            }
        int bit_q(int i, int j)
        {
            i++;
            j++;
            auto sum = 0;
            while(j > 0)
            {
                sum += m_elements[j];
                j -= (j & (j*-1));
            }
            i--;
            while(i > 0)
            {
                sum -= m_elements[i];
                i -= (i & (i*-1));
            }
            return sum;
        }

        // bt : bit array
        // n : size of bit array
        // i is the index to be updated
        // diff is (new_val - old_val) i.e. if want to increase diff is +ive and if want to decrease -ive
        void bit_up(int i, int diff)
        {
            const int n = m_numElements;
            i++;
            while(i<=n)
            {
                m_elements[i] += diff;
                i += (i & (i*-1));
            }
        }

    private:
        int m_numElements;
        vector<int> m_elements;
};

// bt : bit array
// i and j are starting and ending index INCLUSIVE
int bit_q(int* bt, int i, int j)
{
    i++;
    j++;
    auto sum = 0;
    while(j > 0)
    {
        sum += bt[j];
        j -= (j & (j*-1));
    }
    i--;
    while(i > 0)
    {
        sum -= bt[i];
        i -= (i & (i*-1));
    }
    return sum;
}

// bt : bit array
// n : size of bit array
// i is the index to be updated
// diff is (new_val - old_val) i.e. if want to increase diff is +ive and if want to decrease -ive
void bit_up(int* bt, int n, int i, int diff)
{
    i++;
    while(i<=n)
    {
        bt[i] += diff;
        i += (i & (i*-1));
    }
}

vector<int> queryResults;
int originalTreeGrundyNumber;

//vector<int> numNodesWithHeightModuloPowerOf2[log2MaxN + 1];
SegmentTree numNodesWithHeightModuloPowerOf2[log2MaxN + 1];

int modPosOrNeg(int x, int modulus)
{
    if (x < 0)
    {
        const auto numOfModulusToAddToMakeNonNegative = (x / -modulus) + 1;
        x += numOfModulusToAddToMakeNonNegative * modulus;
    }
    assert(x >= 0);
    return x % modulus;
}

void answerQueries(Node* node)
{
    auto countCoinsThatMakeDigitOneAfterHeightChange = [](const int heightChange, int* destination)
    {
        for (auto binaryDigitNum = 0; binaryDigitNum <= log2MaxN; binaryDigitNum++)
        {
            const auto powerOf2 = (1 << (binaryDigitNum + 1));
            const auto oneThreshold = (1 << (binaryDigitNum));
            const auto begin = modPosOrNeg(oneThreshold - heightChange, powerOf2);
            const auto end = modPosOrNeg(-heightChange - 1, powerOf2);
            if (begin <= end)
            {
                //destination[binaryDigitNum] += bit_q(numNodesWithHeightModuloPowerOf2[binaryDigitNum].data(), begin, end);
                destination[binaryDigitNum] += numNodesWithHeightModuloPowerOf2[binaryDigitNum].bit_q(begin, end);
            }
            else
            {
                // Range is split in two.
                //destination[binaryDigitNum] += bit_q(numNodesWithHeightModuloPowerOf2[binaryDigitNum].data(), begin, powerOf2 - 1);
                //destination[binaryDigitNum] += bit_q(numNodesWithHeightModuloPowerOf2[binaryDigitNum].data(), 0, end);
                destination[binaryDigitNum] += numNodesWithHeightModuloPowerOf2[binaryDigitNum].bit_q(begin, powerOf2 - 1);
                destination[binaryDigitNum] += numNodesWithHeightModuloPowerOf2[binaryDigitNum].bit_q(0, end);
            }
        } 
    };
    // Store the originalCoinsThatMakeDigitOne for each query, before exploring any further.
    for (auto& queryForNode : node->queriesForNode)
    {
        countCoinsThatMakeDigitOneAfterHeightChange(queryForNode.heightChange, queryForNode.originalCoinsThatMakeDigitOne);
    }
    // Update numNodesWithHeightModuloPowerOf2 with information from this node.
    for (int binaryDigitNum = 0; binaryDigitNum <= log2MaxN; binaryDigitNum++)
    {
        if (node->hasCoin)
        {
            const auto powerOf2 = (1 << (binaryDigitNum + 1));
            const auto heightModuloPowerOf2 = node->originalHeight % powerOf2;
            //bit_up(numNodesWithHeightModuloPowerOf2[binaryDigitNum].data(), numNodesWithHeightModuloPowerOf2[binaryDigitNum].size(), heightModuloPowerOf2, 1);
            numNodesWithHeightModuloPowerOf2[binaryDigitNum].bit_up(heightModuloPowerOf2, 1);
        }
    }
    // Recurse.
    for (auto child : node->children)
    {
        answerQueries(child);
    }
    // Now we've explored all descendants, use the stored originalCoinsThatMakeDigitOne and the newly-updated numNodesWithHeightModuloPowerOf2
    // to work out relocatedSubtreeGrundyNumber for each query.
    for (auto& queryForNode : node->queriesForNode)
    {
        const auto grundyNumberMinusSubtree = originalTreeGrundyNumber ^ node->grundyNumber;

        int descendantCoinsThatMakeDigitOne[log2MaxN + 1] = {};
        countCoinsThatMakeDigitOneAfterHeightChange(queryForNode.heightChange, descendantCoinsThatMakeDigitOne);
        auto relocatedSubtreeGrundyNumber = 0;
        for (int binaryDigitNum = 0; binaryDigitNum <= log2MaxN; binaryDigitNum++)
        {
            descendantCoinsThatMakeDigitOne[binaryDigitNum] -= queryForNode.originalCoinsThatMakeDigitOne[binaryDigitNum];
            assert(descendantCoinsThatMakeDigitOne[binaryDigitNum] >= 0);
            relocatedSubtreeGrundyNumber += (1 << binaryDigitNum) * (descendantCoinsThatMakeDigitOne[binaryDigitNum] % 2);
        }

        const auto grundyNumberAfterRelocatingNode = grundyNumberMinusSubtree ^ relocatedSubtreeGrundyNumber;
        assert(queryForNode.originalQueryIndex >= 0);

        queryResults[queryForNode.originalQueryIndex] = grundyNumberAfterRelocatingNode;
    }

}

vector<int> grundyNumbersForQueries(vector<Node>& nodes, const vector<Query>& queries)
{
    for (auto binaryDigitNum = 0; binaryDigitNum <= log2MaxN; binaryDigitNum++)
    {
        //numNodesWithHeightModuloPowerOf2[binaryDigitNum].resize((1 << (binaryDigitNum + 1)) + 1);
        numNodesWithHeightModuloPowerOf2[binaryDigitNum] = SegmentTree((1 << (binaryDigitNum + 1)) + 1);
    }
    auto rootNode = &(nodes.front());
    originalTreeGrundyNumber = findGrundyNumbersForNodes(rootNode);
    // Re-order queries so that all queries that move a given node are accessible from that node.
    for (auto queryIndex = 0; queryIndex < queries.size(); queryIndex++)
    {
        const auto query = queries[queryIndex];
        query.nodeToMove->queriesForNode.push_back(QueryForNode{queryIndex, query.newParent->originalHeight - query.nodeToMove->parent->originalHeight});
    }
    queryResults.resize(queries.size());
    answerQueries(rootNode);

    return queryResults;
}

int main(int argc, char** argv)
{
    ios::sync_with_stdio(false);

    int numNodes;
    cin >> numNodes;

    vector<Node> nodes(numNodes);
    for (auto i = 0; i < numNodes; i++)
    {
        int numCoins;
        cin >> numCoins;
        nodes[i].hasCoin = ((numCoins % 2) == 1); // The Grundy number is dependent only on the parity of the number of coins.
    }
    for (auto i = 0; i < numNodes - 1; i++)
    {
        int a, b;
        cin >> a >> b;
        a--;
        b--;

        nodes[a].children.push_back(&nodes[b]);
        nodes[b].children.push_back(&nodes[a]);
    }

    int numQueries;
    cin >> numQueries;

    vector<Query> queries(numQueries);

    for (auto i = 0; i < numQueries; i++)
    {
        int u, v;
        cin >> u >> v;
        u--;
        v--;

        queries[i].nodeToMove = &(nodes[u]);
        queries[i].newParent = &(nodes[v]);
    }

    auto rootNode = &(nodes.front());
    fixParentChildAndHeights(rootNode);

    const auto result = grundyNumbersForQueries(nodes, queries);
    for (const auto queryResult : result)
    {
        cout << queryResult << " ";
    }
    cout << endl;
}
