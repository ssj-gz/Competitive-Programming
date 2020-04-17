// Simon St James (ssjgz) 2018-02-21.
// Editorial solution for "Move the Coins 2".
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

constexpr auto maxN = 200'000;
constexpr int log2(int N, int exponent = 0, int powerOf2 = 1)
{
    return (powerOf2 >= N) ? exponent : log2(N, exponent + 1, powerOf2 * 2);
}
constexpr auto maxBinaryDigits = log2(maxN);

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


// Calculate the height of each node, and remove its parent from its list of "children".
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

// Calculate grundyContribForSubtree for the given node, and return the result.
int findGrundyContribForSubtree(Node* subtreeRoot)
{
    auto grundyContribForSubtree = 0;
    if (subtreeRoot->hasCoin)
        grundyContribForSubtree ^= subtreeRoot->originalHeight;

    for (auto child : subtreeRoot->children)
    {
        grundyContribForSubtree ^= findGrundyContribForSubtree(child);
    }

    subtreeRoot->grundyContribForSubtree = grundyContribForSubtree;

    return grundyContribForSubtree;
}

// Typical SegmentTree - you can find similar implementations all over the place :)
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
        // Find the number in the given range (inclusive) in O(log2(numElements)).
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

        // Increment the value at pos by one in O(log2(numElements)).
        void addOneAt(int pos)
        {
            const auto n = m_numElements;
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

void answerQueries(Node* node, const int originalTreeGrundyNumber, vector<SegmentTree>& numNodesWithHeightModuloPowerOf2, vector<int>& queryGrundyNumbers)
{
    auto countCoinsThatMakeDigitOneAfterHeightChange = [&numNodesWithHeightModuloPowerOf2](const int heightChange, int* destination)
    {
        for (auto binaryDigitNum = 0; binaryDigitNum < numNodesWithHeightModuloPowerOf2.size(); binaryDigitNum++)
        {
            const auto powerOf2 = (1 << (binaryDigitNum + 1));
            const auto oneThreshold = (1 << (binaryDigitNum));
            const auto makeDigitOneBegin = modPosOrNeg(oneThreshold - heightChange, powerOf2);
            const auto makeDigitOneEnd = modPosOrNeg(-heightChange - 1, powerOf2);
            if (makeDigitOneBegin <= makeDigitOneEnd)
            {
                destination[binaryDigitNum] += numNodesWithHeightModuloPowerOf2[binaryDigitNum].numInRange(makeDigitOneBegin, makeDigitOneEnd);
            }
            else
            {
                // Range is split in two - count the number that make digit 0 instead, and subtract from the total over the whole range.
                const auto numThatMakeDigitZero = numNodesWithHeightModuloPowerOf2[binaryDigitNum].numInRange(makeDigitOneEnd + 1, makeDigitOneBegin - 1);
                destination[binaryDigitNum] += numNodesWithHeightModuloPowerOf2[binaryDigitNum].total() - numThatMakeDigitZero;
            }
        } 
    };
    // Store the originalCoinsThatMakeDigitOneAfterHeightChange for each query, before exploring any of this node's descendants.
    for (auto& queryForNode : node->queriesForNode)
    {
        countCoinsThatMakeDigitOneAfterHeightChange(queryForNode.heightChange, queryForNode.originalCoinsThatMakeDigitOneAfterHeightChange);
    }
    // Update numNodesWithHeightModuloPowerOf2 with information from this node.
    for (auto binaryDigitNum = 0; binaryDigitNum < numNodesWithHeightModuloPowerOf2.size(); binaryDigitNum++)
    {
        if (node->hasCoin)
        {
            const auto powerOf2 = (1 << (binaryDigitNum + 1));
            const auto heightModuloPowerOf2 = node->originalHeight % powerOf2;
            numNodesWithHeightModuloPowerOf2[binaryDigitNum].addOneAt(heightModuloPowerOf2);
        }
    }
    // Recurse, and so explore this node's descendants.
    for (auto child : node->children)
    {
        answerQueries(child, originalTreeGrundyNumber, numNodesWithHeightModuloPowerOf2, queryGrundyNumbers);
    }
    // We've now explored all this node's descendants. Now for each query that moves this node: use the stored 
    // originalCoinsThatMakeDigitOneAfterHeightChange and the newly-updated numNodesWithHeightModuloPowerOf2
    // to work out descendantCoinsThatMakeDigitOneAfterHeightChange, and use that to calculate relocatedSubtreeGrundyContrib 
    // for that query.
    for (const auto& queryForNode : node->queriesForNode)
    {
        int descendantCoinsThatMakeDigitOneAfterHeightChange[maxBinaryDigits + 1] = {};
        countCoinsThatMakeDigitOneAfterHeightChange(queryForNode.heightChange, descendantCoinsThatMakeDigitOneAfterHeightChange);
        auto relocatedSubtreeGrundyContrib = 0;
        for (auto binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
        {
            descendantCoinsThatMakeDigitOneAfterHeightChange[binaryDigitNum] -= queryForNode.originalCoinsThatMakeDigitOneAfterHeightChange[binaryDigitNum];
            assert(descendantCoinsThatMakeDigitOneAfterHeightChange[binaryDigitNum] >= 0);
            const auto isRelocatedSubtreeGrundyDigitOne = ((descendantCoinsThatMakeDigitOneAfterHeightChange[binaryDigitNum] % 2) == 1);
            if (isRelocatedSubtreeGrundyDigitOne)
                relocatedSubtreeGrundyContrib += (1 << binaryDigitNum);
        }

        const auto grundyNumberMinusSubtree = originalTreeGrundyNumber ^ node->grundyContribForSubtree;
        const auto grundyNumberAfterRelocatingNode = grundyNumberMinusSubtree ^ relocatedSubtreeGrundyContrib;
        assert(queryForNode.originalQueryIndex >= 0);

        queryGrundyNumbers[queryForNode.originalQueryIndex] = grundyNumberAfterRelocatingNode;
    }
}

vector<int> calcGrundyNumbersForQueries(Node* rootNode, const int numNodes, const int numQueries)
{
    const auto maxBinaryDigitsForTestcase = log2(numNodes);
    // Initialise the SegmentTrees.
    vector<SegmentTree> numNodesWithHeightModuloPowerOf2(maxBinaryDigitsForTestcase + 1);
    for (auto binaryDigitNum = 0; binaryDigitNum < numNodesWithHeightModuloPowerOf2.size(); binaryDigitNum++)
    {
        numNodesWithHeightModuloPowerOf2[binaryDigitNum] = SegmentTree((1 << (binaryDigitNum + 1)) + 1);
    }

    const auto originalTreeGrundyNumber = findGrundyContribForSubtree(rootNode);
    vector<int> queryGrundyNumbers(numQueries);
    answerQueries(rootNode, originalTreeGrundyNumber, numNodesWithHeightModuloPowerOf2, queryGrundyNumbers);

    return queryGrundyNumbers;
}

// Lots of input to read, so use ultra-fast reader.
void scan_integer( int &x )
{
    int c = getchar_unlocked();
    x = 0;
    for( ; ((c<48 || c>57)); c = getchar_unlocked() );
    for( ;c>47 && c<58; c = getchar_unlocked() ) {
        x = (x << 1) + (x << 3) + c - 48;
    }
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    auto readInt = [](){ int x; scan_integer(x); return x; };

    const auto T = readInt();

    for (auto t = 0; t < T; t++)
    {
        const auto numNodes = readInt();

        vector<Node> nodes(numNodes);
        for (auto i = 0; i < numNodes - 1; i++)
        {
            // Make a and b 0-relative.
            const auto a = readInt() - 1;
            const auto b = readInt() - 1;

            nodes[a].children.push_back(&nodes[b]);
            nodes[b].children.push_back(&nodes[a]);
        }
        for (auto i = 0; i < numNodes; i++)
        {
            const auto numCoins = readInt();
            nodes[i].hasCoin = ((numCoins % 2) == 1); // The Grundy number is dependent only on the parity of the number of coins at each height.
        }

        auto rootNode = &(nodes.front());
        fixParentChildAndHeights(rootNode);

        const auto numQueries = readInt();

        for (auto queryIndex = 0; queryIndex < numQueries; queryIndex++)
        {
            // Make u and v 0-relative.
            const auto u = readInt() - 1;
            const auto v = readInt() - 1;

            // Re-order queries so that all queries that move a given node u are accessible from u.
            auto nodeToMove = &(nodes[u]);
            auto newParent = &(nodes[v]);
            const auto heightChange = newParent->originalHeight - nodeToMove->parent->originalHeight;
            nodeToMove->queriesForNode.push_back(QueryForNode{queryIndex, heightChange} );
        }

        const auto queryGrundyNumbers = calcGrundyNumbersForQueries(rootNode, numNodes, numQueries);
        const int64_t MOD = 1'000'000'007;
        int64_t result = 0;
        int64_t powerOf2 = 2; // 2 to the power of 1.
        for (const auto grundyNumber : queryGrundyNumbers)
        {
            if (grundyNumber == 0)
            {
                result = (result + powerOf2) % MOD;
            }
            powerOf2 = (powerOf2 * 2) % MOD;

        }
        cout << result << endl;
    }

}
