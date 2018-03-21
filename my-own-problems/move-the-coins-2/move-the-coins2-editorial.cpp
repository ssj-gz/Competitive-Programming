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
constexpr int maxBinaryDigits = log2(maxN);

struct QueryForNode
{
    QueryForNode(int originalQueryIndex, int heightChange)
        : originalQueryIndex{originalQueryIndex}, heightChange{heightChange}
    {
    }
    int originalQueryIndex = -1;
    int heightChange;
    int originalCoinsThatMakeDigitOneAfterHeightChange[maxBinaryDigits + 1] = {};
};

struct Node
{
    vector<Node*> children;
    bool hasCoin = false;
    Node* parent = nullptr;
    int originalHeight = -1;

    int grundyContribForSubtree = -1;
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
    auto grundyContribForSubtree = 0;
    if (node->hasCoin)
        grundyContribForSubtree ^= node->originalHeight;

    for (auto child : node->children)
    {
        grundyContribForSubtree ^= findGrundyNumbersForNodes(child);
    }

    node->grundyContribForSubtree = grundyContribForSubtree;

    return grundyContribForSubtree;
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
        int total() const
        {
            return m_total;
        }
        int numInRange(int start, int end) const
        {
            start++; // Make 1-relative.  start and end are inclusive.
            end++;
            auto sum = 0;
            auto elements = m_elements.data();
            while(end > 0)
            {
                sum += elements[end];
                end -= (end & (end*-1));
            }
            start--;
            while(start > 0)
            {
                sum -= elements[start];
                start -= (start & (start*-1));
            }
            return sum;
        }

        void addOne(int pos)
        {
            const int n = m_numElements;
            auto elements = m_elements.data();
            pos++; // Make 1-relative.
            while(pos <= n)
            {
                elements[pos]++;
                pos += (pos & (pos * -1));
            }

            m_total++;
        }

    private:
        int m_numElements;
        int m_total = 0;
        vector<int> m_elements;
};

vector<int> queryResults;
int originalTreeGrundyNumber;

SegmentTree numNodesWithHeightModuloPowerOf2[maxBinaryDigits + 1];

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
        for (auto binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
        {
            const auto powerOf2 = (1 << (binaryDigitNum + 1));
            const auto oneThreshold = (1 << (binaryDigitNum));
            const auto begin = modPosOrNeg(oneThreshold - heightChange, powerOf2);
            const auto end = modPosOrNeg(-heightChange - 1, powerOf2);
            if (begin <= end)
            {
                destination[binaryDigitNum] += numNodesWithHeightModuloPowerOf2[binaryDigitNum].numInRange(begin, end);
            }
            else
            {
                // Range is split in two - count the number that make digit 0 instead, and subtract from the total over the whole range.
                const auto numThatMakeDigitZero = numNodesWithHeightModuloPowerOf2[binaryDigitNum].numInRange(end + 1, begin - 1);
                destination[binaryDigitNum] += numNodesWithHeightModuloPowerOf2[binaryDigitNum].total() - numThatMakeDigitZero;
            }
        } 
    };
    // Store the originalCoinsThatMakeDigitOneAfterHeightChange for each query, before exploring any further.
    cout << "# queries for node: " << node->queriesForNode.size() << endl;
    for (auto& queryForNode : node->queriesForNode)
    {
        countCoinsThatMakeDigitOneAfterHeightChange(queryForNode.heightChange, queryForNode.originalCoinsThatMakeDigitOneAfterHeightChange);
    }
    // Update numNodesWithHeightModuloPowerOf2 with information from this node.
    for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
    {
        if (node->hasCoin)
        {
            const auto powerOf2 = (1 << (binaryDigitNum + 1));
            const auto heightModuloPowerOf2 = node->originalHeight % powerOf2;
            numNodesWithHeightModuloPowerOf2[binaryDigitNum].addOne(heightModuloPowerOf2);
        }
    }
    // Recurse.
    for (auto child : node->children)
    {
        answerQueries(child);
    }
    // Now we've explored all descendants, use the stored originalCoinsThatMakeDigitOneAfterHeightChange and the newly-updated numNodesWithHeightModuloPowerOf2
    // to work out relocatedSubtreeGrundyNumber for each query.
    for (const auto& queryForNode : node->queriesForNode)
    {
        const auto grundyNumberMinusSubtree = originalTreeGrundyNumber ^ node->grundyContribForSubtree;

        int descendantCoinsThatMakeDigitOneAfterHeightChange[maxBinaryDigits + 1] = {};
        countCoinsThatMakeDigitOneAfterHeightChange(queryForNode.heightChange, descendantCoinsThatMakeDigitOneAfterHeightChange);
        auto relocatedSubtreeGrundyNumber = 0;
        for (auto binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
        {
            descendantCoinsThatMakeDigitOneAfterHeightChange[binaryDigitNum] -= queryForNode.originalCoinsThatMakeDigitOneAfterHeightChange[binaryDigitNum];
            assert(descendantCoinsThatMakeDigitOneAfterHeightChange[binaryDigitNum] >= 0);
            const auto isRelocatedSubtreeGrundyDigitOne = (descendantCoinsThatMakeDigitOneAfterHeightChange[binaryDigitNum] % 2);
            if (isRelocatedSubtreeGrundyDigitOne)
                relocatedSubtreeGrundyNumber += (1 << binaryDigitNum);
        }

        const auto grundyNumberAfterRelocatingNode = grundyNumberMinusSubtree ^ relocatedSubtreeGrundyNumber;
        assert(queryForNode.originalQueryIndex >= 0);

        queryResults[queryForNode.originalQueryIndex] = grundyNumberAfterRelocatingNode;
    }

}

vector<int> grundyNumbersForQueries(Node* rootNode, const int numQueries)
{
    // Initialise the SegmentTrees.
    for (auto binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
    {
        numNodesWithHeightModuloPowerOf2[binaryDigitNum] = SegmentTree((1 << (binaryDigitNum + 1)) + 1);
    }
    originalTreeGrundyNumber = findGrundyNumbersForNodes(rootNode);
    queryResults.resize(numQueries);
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
        nodes[i].hasCoin = ((numCoins % 2) == 1); // The Grundy number is dependent only on the parity of the number of coins at each height.
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

    auto rootNode = &(nodes.front());
    fixParentChildAndHeights(rootNode);

    int numQueries;
    cin >> numQueries;

    for (auto queryIndex = 0; queryIndex < numQueries; queryIndex++)
    {
        int u, v;
        cin >> u >> v;
        u--;
        v--;

        // Re-order queries so that all queries that move a given node u are accessible from u.
        auto nodeToMove = &(nodes[u]);
        auto newParent = &(nodes[v]);
        nodeToMove->queriesForNode.push_back(QueryForNode{queryIndex, newParent->originalHeight - nodeToMove->parent->originalHeight});
    }

    const auto result = grundyNumbersForQueries(rootNode, numQueries);
    int queryNumber = 1;
    for (const auto queryResult : result)
    {
        if (queryResult == 0)
            cout << queryNumber << '\n';
        queryNumber++;
    }
    cout << endl;
}
