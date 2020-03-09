#define BRUTE_FORCE
//#define VERIFY_SEGMENT_TREE
//#define VERIFY_SUBSTEPS
#define FIND_ZERO_GRUNDYS
//#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#undef BRUTE_FORCE
#undef VERIFY_SEGMENT_TREE
#undef VERIFY_SUBSTEPS
#undef FIND_ZERO_GRUNDYS
#endif

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <thread>
#include <future>
#include <cassert>
#include <sys/time.h>

using namespace std;

constexpr auto maxN = 100'000;
constexpr int log2(int N, int exponent = 0, int powerOf2 = 1)
{
            return (powerOf2 >= N) ? exponent : log2(N, exponent + 1, powerOf2 * 2);
}
constexpr int log2MaxN = log2(maxN);
constexpr int maxBinaryDigits = log2(maxN);
constexpr int maxHeight = maxN * 2;

class HeightTracker
{
    public:
        HeightTracker()
        {
            auto powerOf2 = 2;
            for (auto binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                m_heightsModPowerOf2[binaryDigitNum] = vector<VersionedValue>(powerOf2);
                powerOf2 <<= 1;
            }
            clear();
        }
        void insertHeight(const int newHeight)
        {
            doPendingAdjustAllHeights();
            const auto newHeightAdjusted = newHeight - m_cumulativeHeightAdjustment
                // Add a number guarantees makes newHeightAdjusted >= 0, but does not affect its value modulo the powers of 2 we care about.
                + (1 << (maxBinaryDigits + 1)); 
            assert(newHeightAdjusted >= 0);
            auto powerOf2 = 2;
            for (auto binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                const auto heightModPowerOf2 = newHeightAdjusted & (powerOf2 - 1);
                numHeightsModPowerOf2(binaryDigitNum, heightModPowerOf2)++;
                if (m_makesDigitOneBegin[binaryDigitNum] <= m_makesDigitOneEnd[binaryDigitNum])
                {
                    if (heightModPowerOf2 >= m_makesDigitOneBegin[binaryDigitNum] && heightModPowerOf2 <= m_makesDigitOneEnd[binaryDigitNum])
                        m_grundyNumber ^= (powerOf2 >> 1);
                }
                else
                {
                    const auto makeDigitZeroBegin = m_makesDigitOneEnd[binaryDigitNum] + 1;
                    const auto makeDigitZeroEnd = m_makesDigitOneBegin[binaryDigitNum] - 1;
                    assert(makeDigitZeroBegin <= makeDigitZeroEnd);
                    assert(0 <= makeDigitZeroEnd < powerOf2);
                    assert(0 <= makeDigitZeroBegin < powerOf2);
                    if (!(heightModPowerOf2 >= makeDigitZeroBegin && heightModPowerOf2 <= makeDigitZeroEnd))
                        m_grundyNumber ^= (powerOf2 >> 1);
                }

                powerOf2 <<= 1;
            }
        };

        void adjustAllHeights(int heightDiff)
        {
            m_pendingHeightAdjustment += heightDiff;
        }
        void doPendingAdjustAllHeights()
        {
            int heightDiff = m_pendingHeightAdjustment;
            if (heightDiff == 0)
                return;
            //assert(heightDiff == 1 || heightDiff == -1);
            m_pendingHeightAdjustment = 0;
            m_cumulativeHeightAdjustment += heightDiff;

            auto powerOf2 = 2;
            if (heightDiff > 0)
            {
                for (auto binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
                {
                    // Scroll the begin/ end of the "makes digit one" zone to the left, updating m_grundyNumber
                    // on-the-fly.
                    auto parityChangeToNumberOfHeightsThatMakeDigitsOne = 0;
                    for (int i = 0; i < heightDiff; i++)
                    {
                        parityChangeToNumberOfHeightsThatMakeDigitsOne += numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneEnd[binaryDigitNum]);
                        m_makesDigitOneEnd[binaryDigitNum] = (powerOf2 + m_makesDigitOneEnd[binaryDigitNum] - 1) & (powerOf2 - 1);

                        m_makesDigitOneBegin[binaryDigitNum] = (powerOf2 + m_makesDigitOneBegin[binaryDigitNum] - 1) & (powerOf2 - 1);
                        parityChangeToNumberOfHeightsThatMakeDigitsOne += numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneBegin[binaryDigitNum]);
                    }

                    if ((parityChangeToNumberOfHeightsThatMakeDigitsOne & 1) == 1)
                        m_grundyNumber ^= (powerOf2 >> 1);

                    powerOf2 <<= 1;
                } 
            }
            else
            {
                heightDiff = -heightDiff;
                assert(heightDiff > 0);
                for (auto binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
                {
                    // As above, but scroll the "makes digit one" zone to the right.
                    auto parityChangeToNumberOfHeightsThatMakeDigitsOne = 0;
                    for (int i = 0; i < heightDiff; i++)
                    {
                        parityChangeToNumberOfHeightsThatMakeDigitsOne += numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneBegin[binaryDigitNum]);
                        m_makesDigitOneBegin[binaryDigitNum] = (m_makesDigitOneBegin[binaryDigitNum] + 1) & (powerOf2 - 1);

                        m_makesDigitOneEnd[binaryDigitNum] = (m_makesDigitOneEnd[binaryDigitNum] + 1) & (powerOf2 - 1);
                        parityChangeToNumberOfHeightsThatMakeDigitsOne += numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneEnd[binaryDigitNum]);
                    }

                    if ((parityChangeToNumberOfHeightsThatMakeDigitsOne & 1) == 1)
                        m_grundyNumber ^= (powerOf2 >> 1);

                    powerOf2 <<= 1;
                } 
            }
        }
        int& numHeightsModPowerOf2(int binaryDigitNum, int modPowerOf2)
        {
            auto& numHeights = m_heightsModPowerOf2[binaryDigitNum][modPowerOf2];
            if (numHeights.versionNumber != m_versionNumber)
            {
                numHeights.value = 0;
                numHeights.versionNumber = m_versionNumber;
            }
            return numHeights.value;
        }
        int grundyNumber()
        {
            if (m_pendingHeightAdjustment != 0)
            {
                doPendingAdjustAllHeights();
            }
            return m_grundyNumber;
        }
        void clear()
        {
            m_versionNumber++;
            auto powerOf2 = 2;
            for (auto binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                m_makesDigitOneBegin[binaryDigitNum] = powerOf2 >> 1;
                m_makesDigitOneEnd[binaryDigitNum] = powerOf2 - 1;

                powerOf2 <<= 1;
            }
            m_grundyNumber = 0;
            m_cumulativeHeightAdjustment = 0;
        }
    private:
        struct VersionedValue
        {
            int value = 0;
            int versionNumber = -1;
        };
        vector<VersionedValue> m_heightsModPowerOf2[maxBinaryDigits + 1];
        int m_makesDigitOneBegin[maxBinaryDigits + 1];
        int m_makesDigitOneEnd[maxBinaryDigits + 1];

        int m_cumulativeHeightAdjustment = 0;
        int m_grundyNumber = 0;

        int m_versionNumber = 0;

        int m_pendingHeightAdjustment = 0;
};

struct Node;
struct Query
{
    Node* nodeToMove = nullptr;
    Node* newParent = nullptr;
};

struct ReorderedQuery
{
    int originalQueryIndex = -1;
    Query originalQuery;
    int originalNodesThatMakeDigitOne[log2MaxN + 1];
};


struct Node
{
    vector<Node*> children;
    int numCoins = -1;
    Node* parent = nullptr;
    int nodeId = -1;
    bool usable = true;
    int height = -1;

    int grundyNumber = -1;
    vector<ReorderedQuery> queriesForNode;
};


void grundyNumberForTreeBruteForce(Node* node, const int depth, int& grundyNumber, int& numNodes)
{
    numNodes++;
    for (int i = 0; i < node->numCoins; i++)
    {
        grundyNumber ^= depth;
    }
    for (Node* child : node->children)
    {
        grundyNumberForTreeBruteForce(child, depth + 1, grundyNumber, numNodes);
    }
}

int grundyNumberForTreeBruteForce(Node* node, int& numNodes)
{
    int grundyNumber = 0;
    grundyNumberForTreeBruteForce(node, 0, grundyNumber, numNodes);
    //cout << "grundyNumberForTreeBruteForce node: " << node->nodeId << " = " << grundyNumber << endl;
    return grundyNumber;
}

void markDescendentsAsUsable(Node* node,  bool usable)
{
    node->usable = usable;

    for (Node* child : node->children)
    {
        markDescendentsAsUsable(child, usable);
    }
}

void reparentNode(Node* nodeToMove, Node* newParent)
{
    auto oldParent = nodeToMove->parent;
    if (oldParent)
    {
        assert(find(oldParent->children.begin(), oldParent->children.end(), nodeToMove) != oldParent->children.end());
        oldParent->children.erase(remove(oldParent->children.begin(), oldParent->children.end(), nodeToMove), oldParent->children.end());
        assert(find(oldParent->children.begin(), oldParent->children.end(), nodeToMove) == oldParent->children.end());
    }
    nodeToMove->parent = newParent;
    if (newParent)
    {
        assert(find(newParent->children.begin(), newParent->children.end(), nodeToMove) == newParent->children.end());
        newParent->children.push_back(nodeToMove);
    }
}

void fixParentChildAndHeights(Node* node, Node* parent = nullptr, int height = 0)
{
    node->height = height;
    node->parent = parent;

    node->children.erase(remove(node->children.begin(), node->children.end(), parent), node->children.end());

    for (auto child : node->children)
    {
        fixParentChildAndHeights(child, node, height + 1);
    }
}

int findGrundyNumberForNodes(Node* node, const int depth)
{
    int grundyNumber = 0;
    if ((node->numCoins % 2) == 1)
        grundyNumber ^= depth;

    for (auto child : node->children)
    {
        grundyNumber ^= findGrundyNumberForNodes(child, depth + 1);
    }

    node->grundyNumber = grundyNumber;


    return grundyNumber;
}
int findGrundyNumberForNodes(Node* node)
{
    return findGrundyNumberForNodes(node, 0);
}

vector<int> grundyNumbersForQueriesBruteForce(vector<Node>& nodes, const vector<Query>& queries)
{
    //cout << "grundyNumbersForQueriesBruteForce" << endl;
    auto rootNode = &(nodes.front());
    vector<int> grundyNumbersForQueries;
    int queryNum = 0;
    int unused = 0;
    const auto originalTreeGrundyNumber = grundyNumberForTreeBruteForce(rootNode, unused);
    //cout << "Original tree grundy number: " << originalTreeGrundyNumber << endl;
    for (const auto& query : queries)
    {
        //cout << " grundy number for node to move: " << query.nodeToMove->grundyNumber << " new parent height: " << query.newParent->height << " grundyNumberMinusSubtree: " << (originalTreeGrundyNumber ^ query.nodeToMove->grundyNumber) << endl;
        auto originalParent = query.nodeToMove->parent;
        reparentNode(query.nodeToMove, query.newParent);
        int numNodesInModifiedTree = 0;
        grundyNumbersForQueries.push_back(grundyNumberForTreeBruteForce(rootNode, numNodesInModifiedTree));
        assert(numNodesInModifiedTree == nodes.size());
        reparentNode(query.nodeToMove, originalParent);
        queryNum++;
        if (queryNum % 100 == 0)
            cerr << "Brute force queryNum: " << queryNum << " out of " << queries.size() << endl;
    }

    return grundyNumbersForQueries;
}

//bt : bit array
//i and j are starting and ending index INCLUSIVE
long long bit_q(long long * bt,int i,int j)
{
    i++;
    j++;
    long long sum=0ll;
    while(j>0)
    {
        sum+=bt[j];
        j -= (j & (j*-1));
    }
    i--;
    while(i>0)
    {
        sum-=bt[i];
        i -= (i & (i*-1));
    }
    return sum;
}
//bt : bit array
//n : size of bit array
//i is the index to be updated
//diff is (new_val - old_val) i.e. if want to increase diff is +ive and if want to decrease -ive
void bit_up(long long * bt,int n,int i,long long diff)
{
    i++;
    while(i<=n)
    {
        bt[i] += diff;
        i += (i & (i*-1));
    }
}

#ifdef VERIFY_SUBSTEPS
vector<int> numNodesWithHeight;
#endif
vector<int> queryResults;
int originalTreeGrundyNumber;
int largestHeight = -1;
int log2LargestHeight = -1;

vector<long long> numNodesWithHeightModuloPowerOf2[log2MaxN + 1];

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


void buildHeightHistogram(Node* node, vector<int>& numWithHeight)
{
    if ((node->numCoins % 2) == 1)
        numWithHeight[node->height]++;

    for (auto child : node->children)
    {
        buildHeightHistogram(child, numWithHeight);
    }
}

auto countCoinsThatMakeDigitOneAfterHeightChange = [](const int heightChange, int* destination)
{
    for (int binaryDigitNum = 0; binaryDigitNum <= log2LargestHeight; binaryDigitNum++)
    {
        const int powerOf2 = (1 << (binaryDigitNum + 1));
        const int oneThreshold = (1 << (binaryDigitNum));
        const int begin = modPosOrNeg(oneThreshold - heightChange, powerOf2);
        const int end = modPosOrNeg(-heightChange - 1, powerOf2);
        if (begin <= end)
        {
            destination[binaryDigitNum] += bit_q(numNodesWithHeightModuloPowerOf2[binaryDigitNum].data(), begin, end);
        }
        else
        {
            destination[binaryDigitNum] += bit_q(numNodesWithHeightModuloPowerOf2[binaryDigitNum].data(), begin, powerOf2 - 1);
            destination[binaryDigitNum] += bit_q(numNodesWithHeightModuloPowerOf2[binaryDigitNum].data(), 0, end);
        }
#ifdef VERIFY_SUBSTEPS
#if 0
        {
            int verify = 0;
            for (int height = 0; height < numNodesWithHeight.size(); height++)
            {
                if (((height + heightChange) & (1 << binaryDigitNum)) != 0)
                {
                    verify += numNodesWithHeight[height];
                }
            }
            assert(destination[binaryDigitNum] == verify);
        }
#endif
#endif
    } 
};

void solve(Node* node)
{
    static int numZeroGrundys = 0;
    for (auto& reorderedQuery : node->queriesForNode)
    {
        auto nodeToMove = reorderedQuery.originalQuery.nodeToMove;
        auto newParent = reorderedQuery.originalQuery.newParent;
        const int heightChange = newParent->height - nodeToMove->parent->height;
        countCoinsThatMakeDigitOneAfterHeightChange(heightChange, reorderedQuery.originalNodesThatMakeDigitOne);
    }
#ifdef VERIFY_SUBSTEPS
    const auto originalNumNodesWithHeight = numNodesWithHeight;
    if ((node->numCoins) % 2 == 1)
        numNodesWithHeight[node->height]++;
#endif
    for (int binaryDigitNum = 0; binaryDigitNum <= log2LargestHeight; binaryDigitNum++)
    {
        if ((node->numCoins % 2) == 1)
        {
            const int powerOf2 = (1 << (binaryDigitNum + 1));
            const int heightModuloPowerOf2 = node->height % powerOf2;
            //cout << "About to applyOperatorToAllInRange - binaryDigitNum: " << binaryDigitNum << " heightModuloPowerOf2: " << heightModuloPowerOf2 << endl;
            bit_up(numNodesWithHeightModuloPowerOf2[binaryDigitNum].data(), numNodesWithHeightModuloPowerOf2[binaryDigitNum].size(), heightModuloPowerOf2, 1);
        }
    }
    for (auto child : node->children)
    {
        solve(child);
    }
#ifdef VERIFY_SUBSTEPS
    vector<int> numDescendendantNodesWithHeight(numNodesWithHeight.size());
    for (int height = 0; height < numDescendendantNodesWithHeight.size(); height++)
    {
        numDescendendantNodesWithHeight[height] = numNodesWithHeight[height] - originalNumNodesWithHeight[height];
    }
#endif
    int queryNum = 0;
    for (auto& reorderedQuery : node->queriesForNode)
    {
        //cout << "reorderedQuery" << endl;
        auto nodeToMove = reorderedQuery.originalQuery.nodeToMove;
        auto newParent = reorderedQuery.originalQuery.newParent;
        const int heightChange = newParent->height - nodeToMove->parent->height;

        const int grundyNumberMinusSubtree = originalTreeGrundyNumber ^ nodeToMove->grundyNumber;
        //int newGrundyNumber = grundyNumberMinusSubtree;
        //cout << "originalTreeGrundyNumber: " << originalTreeGrundyNumber << endl;

        int relocatedSubtreeGrundyDigits[log2MaxN + 1] = {};
        countCoinsThatMakeDigitOneAfterHeightChange(heightChange, relocatedSubtreeGrundyDigits);
        int relocatedSubtreeGrundyNumber = 0;
        for (int binaryDigitNum = 0; binaryDigitNum <= log2LargestHeight; binaryDigitNum++)
        {
            relocatedSubtreeGrundyDigits[binaryDigitNum] -= reorderedQuery.originalNodesThatMakeDigitOne[binaryDigitNum];
            assert(relocatedSubtreeGrundyDigits[binaryDigitNum] >= 0);
            relocatedSubtreeGrundyNumber += (1 << binaryDigitNum) * (relocatedSubtreeGrundyDigits[binaryDigitNum] % 2);
        }

#ifdef VERIFY_SUBSTEPS
        {
            int verifyRelocatedSubtreeGrundyNumber = 0;
            for (int binaryDigitNum = 0; binaryDigitNum <= log2LargestHeight; binaryDigitNum++)
            {
                //cout << "binaryDigitNum: " << binaryDigitNum << endl;
                int digit = -reorderedQuery.originalNodesThatMakeDigitOne[binaryDigitNum];
                relocatedSubtreeGrundyDigits[binaryDigitNum] -= reorderedQuery.originalNodesThatMakeDigitOne[binaryDigitNum];
                for (int height = 0; height < numNodesWithHeight.size(); height++)
                {
                    if (((height + heightChange) & (1 << binaryDigitNum)) != 0)
                    {
                        digit += numNodesWithHeight[height];
                    }
                }
                assert(digit >= 0);
                digit %= 2;
                verifyRelocatedSubtreeGrundyNumber = verifyRelocatedSubtreeGrundyNumber + (1 << binaryDigitNum) * digit;
                //cout << " relocatedSubtreeGrundyDigits[" << binaryDigitNum << "] = " << relocatedSubtreeGrundyDigits[binaryDigitNum] << endl;
            } 

            //assert(verifyRelocatedSubtreeGrundyNumber == grundyNumberWithHeightChange(nodeToMove, heightChange));
        }
#endif
        int newGrundyNumber = grundyNumberMinusSubtree;
        newGrundyNumber ^= relocatedSubtreeGrundyNumber;
        queryResults[reorderedQuery.originalQueryIndex] = newGrundyNumber;
        if (newGrundyNumber == 0)
        {
            numZeroGrundys++;
            //cout << "numZeroGrundys: " << numZeroGrundys << " node original height: " << nodeToMove->height << " node new height: " << newParent->height << endl;
        }
        //cout << " relocatedSubtreeGrundyNumber: " << relocatedSubtreeGrundyNumber << endl;
        //cout << " relocatedSubtreeGrundyNumber: " << blee << endl;
#ifdef VERIFY_SUBSTEPS
        {
            int verifyRelocatedSubtreeGrundyNumber = 0;
            for (int height = 0; height < numDescendendantNodesWithHeight.size(); height++)
            {
                if ((numDescendendantNodesWithHeight[height] % 2) == 1)
                {
                    verifyRelocatedSubtreeGrundyNumber ^= (height + heightChange);
                    cout << " height: " << height << " contributes " << (height + heightChange) << " to relocatedSubtreeGrundyNumber!" << endl;
                }
            }
            assert(relocatedSubtreeGrundyNumber == verifyRelocatedSubtreeGrundyNumber);
        }
#endif
        queryNum++;
        if (queryNum % 100 == 0)
            cerr << "queryNum: " << queryNum << " out of " << queryResults.size() << endl;
    }

}

vector<int> grundyNumbersForQueries(vector<Node>& nodes, const vector<Query>& queries)
{
    for (int binaryDigitNum = 0; binaryDigitNum <= log2LargestHeight; binaryDigitNum++)
    {
        numNodesWithHeightModuloPowerOf2[binaryDigitNum].resize((1 << (binaryDigitNum + 1)) + 1);
    }
#ifdef VERIFY_SUBSTEPS
    numNodesWithHeight.resize(nodes.size() + 1);
#endif
    auto rootNode = &(nodes.front());
    originalTreeGrundyNumber = findGrundyNumberForNodes(rootNode);
    int unused = 0;
    assert(rootNode->grundyNumber == grundyNumberForTreeBruteForce(rootNode, unused));
    for (int queryIndex = 0; queryIndex < queries.size(); queryIndex++)
    {
        const auto query = queries[queryIndex];
        query.nodeToMove->queriesForNode.push_back({queryIndex, query});
    }
#ifdef VERIFY_SUBSTEPS
    vector<int> grundyNumbersForQueries;
    for (int queryIndex = 0; queryIndex < queries.size(); queryIndex++)
    {
        const auto query = queries[queryIndex];
        auto originalParent = query.nodeToMove->parent;
        {
            reparentNode(query.nodeToMove, nullptr);
            assert(grundyNumberForTreeBruteForce(rootNode, unused) == (originalTreeGrundyNumber ^ query.nodeToMove->grundyNumber));
        }
        reparentNode(query.nodeToMove, query.newParent);
        grundyNumbersForQueries.push_back(grundyNumberForTreeBruteForce(rootNode, unused));
        reparentNode(query.nodeToMove, originalParent);
    }
#endif
    queryResults.resize(queries.size());
    //cout << "Calling solve" << endl;
    solve(rootNode);
#ifdef VERIFY_SUBSTEPS
    for (int queryIndex = 0; queryIndex < queries.size(); queryIndex++)
    {
        cout << " queryIndex: " << queryIndex << " queryResults: " << queryResults[queryIndex] << " grundyNumbersForQueries: " << grundyNumbersForQueries[queryIndex] << endl;
    }
    assert(queryResults == grundyNumbersForQueries);
#endif

    return queryResults;
}

#include <set> 
#include <memory> 
struct TestNode;
struct NodeData
{
    int numCoins = -1;
    bool usable = true;
    int grundyNumber = -1;
    vector<int> newParentHeightsThatGiveZeroGrundy;
    int maxHeightOfNonDescendant = -1;
};
struct TestEdge;
struct TestNode
{
    vector<TestEdge*> neighbours;
    int originalId = -1;
    int scrambledId = -1;
    int id() const
    {
        return (scrambledId == -1) ? originalId : scrambledId;
    }
    NodeData data;

    // Filled in my doGenericInfoDFS.
    TestNode* parent = nullptr;
    int visitNum = -1;
    int endVisitNum = -1;
    int height = -1;
};
struct TestEdge
{
    TestNode* nodeA = nullptr;
    TestNode* nodeB = nullptr;
    TestNode* otherNode(TestNode* node)
    {
        if (node == nodeA)
            return nodeB;
        if (node == nodeB)
            return nodeA;
        assert(false);
        return nullptr;
    }
};
class TreeGenerator
{
    public:
        TestNode* createNode();
        TestNode* createNode(TestNode* attachedTo);
        TestNode* createNodeWithRandomParent();
        vector<TestNode*> createNodesWithRandomParent(int numNewNodes);
        vector<TestNode*> createNodesWithRandomParentPreferringLeafNodes(int numNewNodes, double leafNodePreferencePercent);
        vector<TestNode*> createNodesWithRandomParentPreferringFromSet(const set<TestNode*>& preferredSet, int numNewNodes, double preferencePercent, std::function<void(TestNode* newNode, TestNode* parent, const bool parentWasPreferred, bool& addNewNodeToSet, bool& removeParentFromSet)> onCreateNode);

        int numNodes() const;
        TestNode* firstNode() const;
        TestEdge* addEdge(TestNode* nodeA, TestNode* nodeB);
        vector<TestNode*> addNodeChain(TestNode* chainStart, int numInChain);
        void scrambleNodeOrder();
        void scrambleEdgeOrder();
        // Scrambles the order of the nodes, then re-ids them, in new order, with the first
        // node in the new order having scrambledId 1, then next 2, etc.
        void scrambleNodeIdsAndReorder(TestNode* forceAsRootNode);
        vector<TestNode*> nodes() const;
        vector<TestEdge*> edges() const;
        void printEdges() const;
    private:
        vector<unique_ptr<TestNode>> m_nodes;
        vector<unique_ptr<TestEdge>> m_edges;
};

void doDFS(TestNode* node, TestNode* parent, int depth, std::function<void(TestNode* node, int depth)> onVisitNode, std::function<void(TestNode*, int depth)> onEndVisitNode = std::function<void(TestNode*, int depth)>())
{
    onVisitNode(node, depth);
    for (auto edge : node->neighbours)
    {
        auto child = edge->otherNode(node);
        if (child == parent)
            continue;

        doDFS(child, node, depth + 1, onVisitNode, onEndVisitNode);
    }
    if (onEndVisitNode)
        onEndVisitNode(node, depth);
}

void doGenericInfoDFS(TestNode* rootNode)
{
    int visitNum = 0;
    vector<TestNode*> ancestors = {nullptr};
    doDFS(rootNode, nullptr, 0, [&ancestors, &visitNum](TestNode* node, int depth) 
            { 
                node->parent = ancestors.back(); 
                node->height = depth; 
                node->visitNum = visitNum;
                visitNum++;
                ancestors.push_back(node);
            },
            [&ancestors, &visitNum](TestNode* node, int depth)
            { 
                ancestors.pop_back();
                node->endVisitNum = visitNum;
                visitNum++;
            });

}

void markDescendentsAsUsable(TestNode* node, bool usable)
{
    doDFS(node, node->parent, 0, [usable](TestNode* node, int depth) { node->data.usable = usable; });
}

int findGrundyNumberForNodes(TestNode* node, const int depth)
{
    int grundyNumber = 0;
    if ((node->data.numCoins % 2) == 1)
        grundyNumber ^= depth;

    for (auto childEdge : node->neighbours)
    {
        auto child = childEdge->otherNode(node);
        if (child == node->parent)
            continue;
        grundyNumber ^= findGrundyNumberForNodes(child, depth + 1);
    }

    node->data.grundyNumber = grundyNumber;

    return grundyNumber;
}
int findGrundyNumberForNodes(TestNode* node)
{
    return findGrundyNumberForNodes(node, 0);
}

TestNode* pickNodeAtHeightNotDescendantOf(int height, TestNode* forbidDescendantsOf, const vector<vector<TestNode*>>& nodesWithHeight)
{
#if 0
    cout << "pickNodeAtHeightNotDescendantOf: " << height << " forbidDescendantsOf: " << forbidDescendantsOf->id() << " visitNum:" << forbidDescendantsOf->visitNum << " endVisitNum: " << forbidDescendantsOf->endVisitNum << endl;
    cout << "nodesWithHeight: " << endl;
#endif
    const vector<TestNode*>& nodesAtGivenHeight = nodesWithHeight[height];
#if 0
    for (auto node : nodesAtGivenHeight)
    {
        cout << "id: " << node->id() << " visitNum: " << node->visitNum << " ";
    }
    cout << endl;
#endif
    const int descendantVisitNumBegin = forbidDescendantsOf->visitNum;
    const int descendantVisitNumEnd = forbidDescendantsOf->endVisitNum;
    auto compareNodesByVisitNum = [](const TestNode* lhs, const TestNode* rhs) { return lhs->visitNum < rhs->visitNum;};
    TestNode dummyNode; 
    dummyNode.visitNum = descendantVisitNumBegin;
    auto endOfBeforeNodeIter = lower_bound(nodesAtGivenHeight.begin(), nodesAtGivenHeight.end(), &dummyNode, compareNodesByVisitNum);
    //assert(endOfBeforeNodeIter != nodesAtGivenHeight.end());
    //cout << "Blarp: " << (*endOfBeforeNodeIter)->visitNum << endl;
    if (endOfBeforeNodeIter != nodesAtGivenHeight.begin())
    {
        //cout << "decrementing" << endl;
        endOfBeforeNodeIter--;
    }
    if(endOfBeforeNodeIter == nodesAtGivenHeight.end())
    {
        //endOfBeforeNodeIter = nodesAtGivenHeight.begin();
        //cout << "is end!" << endl;
    }
    else
    {
        //cout << "Blee: " << (*endOfBeforeNodeIter)->visitNum << " bloo: " << descendantVisitNumBegin << endl;
        if((*endOfBeforeNodeIter)->visitNum >= descendantVisitNumBegin)
            endOfBeforeNodeIter = nodesAtGivenHeight.end();
    }
    dummyNode.visitNum = descendantVisitNumEnd;
    auto beginOfAfterNodeIter = upper_bound(nodesAtGivenHeight.cbegin(), nodesAtGivenHeight.cend(), &dummyNode, compareNodesByVisitNum);
    const int numBefore = (endOfBeforeNodeIter == nodesAtGivenHeight.end() ? 0 : endOfBeforeNodeIter - nodesAtGivenHeight.begin() + 1);
    const int numAfter = nodesAtGivenHeight.end() - beginOfAfterNodeIter;
    const int total = numBefore + numAfter;
    TestNode* retval = nullptr;
    const int randomIndex = (total == 0 ? -1 : rand() % total);
    if (total > 0)
    {
    if (randomIndex < numBefore)
        retval =  *(nodesAtGivenHeight.begin() + randomIndex);
    else
        retval =  *(beginOfAfterNodeIter + (randomIndex - numBefore));
    }

#if 0
    vector<TestNode*> dbgValidNodes;
    int dbgNumBefore = 0;
    int dbgNumAfter = 0;
    for (const auto& node : nodesAtGivenHeight)
    {
        if (node->visitNum < forbidDescendantsOf->visitNum)
        {
            dbgValidNodes.push_back(node);
            dbgNumBefore++;
        }

        if (node->visitNum > forbidDescendantsOf->endVisitNum)
        {
            dbgValidNodes.push_back(node);
            dbgNumAfter++;
        }
    }
    //cout << "numBefore: " << numBefore << " dbgNumBefore: " << dbgNumBefore << " numAfter: " << numAfter << " dbgNumAfter: " << dbgNumAfter << " blah: " << forbidDescendantsOf->id() << endl;
    assert(dbgNumAfter == numAfter);
    assert(dbgNumBefore == numBefore);
    assert((total == 0 && dbgValidNodes.empty()) || (total != 0 && !dbgValidNodes.empty()));
    if (total != 0)
    {
        assert(dbgValidNodes[randomIndex] == retval);
    }
#endif
    return retval;
}

struct TestQuery
{
    TestNode* nodeToReparent = nullptr;
    TestNode* newParent = nullptr;
};

bool operator<(const TestQuery& lhs, const TestQuery& rhs)
{
    if (lhs.nodeToReparent != rhs.nodeToReparent)
        return lhs.nodeToReparent < rhs.nodeToReparent;
    return lhs.newParent < rhs.newParent;
}

int grundyNumberWithHeightChange(TestNode* node, const int heightChange)
{
    int grundyNumber = 0;
    if ((node->data.numCoins % 2) == 1)
    {
        const int newHeight = node->height + heightChange;
        assert(newHeight >= 0);
        grundyNumber ^= newHeight;
    }
    for (auto childEdge : node->neighbours)
    {
        auto child = childEdge->otherNode(node);
        if (child == node->parent)
            continue;
        grundyNumber ^= grundyNumberWithHeightChange(child, heightChange);
    }
    return grundyNumber;
}

void findZeroGrundies(TreeGenerator&  treeGenerator, const vector<vector<TestNode*>> nodesWithHeight, int nodeBeginIndex, int nodeIndexStep, mutex& printMutex)
{
    cerr << "findZeroGrundies" << endl;
    auto rootNode = treeGenerator.nodes().front();
    const auto numNodes = treeGenerator.numNodes();
    auto originalTreeGrundyNumber = rootNode->data.grundyNumber;
    //assert(rootNode->grundyNumber == grundyNumberForTreeBruteForce(rootNode));
    int numZeroGrundies = 0;
    int numNodesProcessed = 0;
    HeightTracker heightTracker;
    auto nodes = treeGenerator.nodes();
    vector<int> numDescendantsWithHeight(numNodes + 1);
    for (int nodeIndex = nodeBeginIndex; nodeIndex < numNodes; nodeIndex += nodeIndexStep)
    {
        auto node = nodes[nodeIndex];
        if ((nodeIndex - nodeBeginIndex) % 100 == 0)
        {
            lock_guard<mutex> lock(printMutex);
            cerr << "node: " << node->id() << " thread id: " << nodeBeginIndex << endl;
        }
        numDescendantsWithHeight.clear();
        numDescendantsWithHeight.resize(numNodes + 1);
        doDFS(node, node->parent, 0, [&numDescendantsWithHeight](auto node, int depth) 
                {
                    if ((node->data.numCoins % 2) == 1)
                        numDescendantsWithHeight[node->height]++;
                });
        vector<int> descendentHeights;
        heightTracker.clear();
        for (int height = 0; height < numDescendantsWithHeight.size(); height++)
        {
            if ((numDescendantsWithHeight[height] % 2) == 1)
            {
                descendentHeights.push_back(height);
                assert(height - node->height >= 0);
                heightTracker.insertHeight(height - node->height);
            }
        }
        //cout << "node: " << node->nodeId << " height: " << node->height << " depth underneath: " <<  << endl;
        const int depthUnderneath = (descendentHeights.empty() ? - 1 :  descendentHeights.back() - node->height);
        for (int newParentHeight = 0; newParentHeight < 2 * numNodes; newParentHeight++)
        {
            heightTracker.adjustAllHeights(1);
            const int heightChange = newParentHeight - node->height + 1;
            if (pickNodeAtHeightNotDescendantOf(newParentHeight, node, nodesWithHeight) == nullptr)
                 break;
            //assert(pickNodeAtHeightNotDescendantOf(newParentHeight, node, nodesWithHeight)->height == newParentHeight);
            node->data.maxHeightOfNonDescendant = newParentHeight;
            const int grundyNumberMinusSubtree = originalTreeGrundyNumber ^ node->data.grundyNumber;
            int relocatedSubtreeGrundyNumber = heightTracker.grundyNumber();
            const int newGrundyNumber = grundyNumberMinusSubtree ^ relocatedSubtreeGrundyNumber;
            //cout << "newGrundyNumber: " << newGrundyNumber << endl;
            if (newGrundyNumber == 0)
            {
                numZeroGrundies++;
#if 1
                {
                    lock_guard<mutex> lock(printMutex);
                    cerr << "Forced a grundy number: nodeId: " << node->id() << " node height: " << node->height << " depthUnderneath: " << depthUnderneath << " heightChange: " << heightChange << " new parent height:" << newParentHeight << " total: " << numZeroGrundies << " numNodesProcessed: " << numNodesProcessed << " numNodes: " << numNodes << " originalTreeGrundyNumber: " << originalTreeGrundyNumber << " grundyNumberMinusSubtree: " << grundyNumberMinusSubtree << endl;
                }
#endif
                node->data.newParentHeightsThatGiveZeroGrundy.push_back(newParentHeight);
                //assert((grundyNumberMinusSubtree ^ grundyNumberWithHeightChange(node, heightChange)) == newGrundyNumber);
            }
        }
        {
            lock_guard<mutex> lock(printMutex);
            if (!node->data.newParentHeightsThatGiveZeroGrundy.empty())
                cerr << "node " << node->id() << " had " << node->data.newParentHeightsThatGiveZeroGrundy.size() << " heights that give zero grundy" << endl;
        }
        numNodesProcessed++;
    }
}

void findZeroGrundies(TreeGenerator&  treeGenerator, const vector<vector<TestNode*>> nodesWithHeight)
{
    auto rootNode = treeGenerator.nodes().front();
    auto originalTreeGrundyNumber = findGrundyNumberForNodes(rootNode);

    const int numThreads = thread::hardware_concurrency();
    vector<future<void>> futures;
    cerr << "Using numThreads: " << numThreads << endl;
    std::mutex printMutex;
    for (int nodeBeginIndex = 0; nodeBeginIndex < numThreads; nodeBeginIndex++)
    {
        futures.push_back(std::async(std::launch::async, [nodeBeginIndex, numThreads, &treeGenerator, &nodesWithHeight, &printMutex]() { findZeroGrundies(treeGenerator, nodesWithHeight, nodeBeginIndex, numThreads, printMutex); }));
    }
    for (auto& fut : futures)
    {
        fut.get();
    }
}

void generateQueries(TreeGenerator& treeGenerator, vector<TestQuery>& destQueries, const vector<TestNode*>& allowedNodesToReparent, const int numQueries, double percentageGrundyZero, const vector<vector<TestNode*>> nodesWithHeight)
{
    set<TestQuery> queries(destQueries.begin(), destQueries.end());
    vector<TestNode*> nodesThatCanBeReparentedForGrundyZero;
    for (auto node : allowedNodesToReparent)
    {
        if (!node->data.newParentHeightsThatGiveZeroGrundy.empty())
        {
            nodesThatCanBeReparentedForGrundyZero.push_back(node);
        }
    }
    for (int i = 0; i < numQueries; i++)
    {
        int numAttempts = 0;
        const int maxAttempts = 10'000;
        while (numAttempts < maxAttempts)
        {
            numAttempts++;
            TestNode* nodeToReparent = nullptr;
            TestNode* newParent = nullptr;
            const double randomPercent = static_cast<double>(rand()) / RAND_MAX * 100;
            //cout << "randomPercent: " << randomPercent << " percentageGrundyZero: " << percentageGrundyZero << endl;
            if (randomPercent <= percentageGrundyZero)
            {
                nodeToReparent = nodesThatCanBeReparentedForGrundyZero[rand() % nodesThatCanBeReparentedForGrundyZero.size()];
                const int heightOfNewParent = nodeToReparent->data.newParentHeightsThatGiveZeroGrundy[rand() % nodeToReparent->data.newParentHeightsThatGiveZeroGrundy.size()];

                newParent = pickNodeAtHeightNotDescendantOf(heightOfNewParent, nodeToReparent, nodesWithHeight);
                cerr << "picked grundy zero testcase, apparently - " << nodeToReparent->id() << " -> " << newParent->id() << endl;
            }
            else
            {
                nodeToReparent = allowedNodesToReparent[rand() % allowedNodesToReparent.size()];
                if (nodeToReparent->data.maxHeightOfNonDescendant == -1)
                    continue;
                const int heightOfNewParent = rand() % (nodeToReparent->data.maxHeightOfNonDescendant + 1);
                newParent = pickNodeAtHeightNotDescendantOf(heightOfNewParent, nodeToReparent, nodesWithHeight);
            }

            TestQuery query;
            assert(nodeToReparent);
            assert(newParent);
            query.nodeToReparent = nodeToReparent;
            query.newParent = newParent;
            if (queries.find(query) != queries.end())
                continue;

            queries.insert(query);
            destQueries.push_back(query);
            //cerr << "Generated query: " << query.nodeToReparent->id() << " -> " << query.newParent->id() << endl;
            break;
        }
        if (numAttempts == maxAttempts)
            break;
    }
}

void scrambleAndPrintTestcase(TreeGenerator& treeGenerator, vector<TestQuery> testQueries)
{
    cout << treeGenerator.numNodes() << endl;
    treeGenerator.scrambleNodeIdsAndReorder(treeGenerator.nodes().front());
    for (auto node : treeGenerator.nodes())
    {
        cout << node->data.numCoins << endl;
    }

    treeGenerator.scrambleEdgeOrder();
    treeGenerator.printEdges();

    cout << testQueries.size() << endl;

    random_shuffle(testQueries.begin(), testQueries.end());
    for (const auto query : testQueries)
    {
        cout << query.nodeToReparent->id() << " " << query.newParent->id() << endl;
    }
}

int main(int argc, char** argv)
{
    ios::sync_with_stdio(false);
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        //const int maxNumNodes = 100'000;
        //const int maxNumQueries = 100'000;
        const int maxNumNodes = 100;
        const int maxNumQueries = 100;

        //int numNodes = rand() % maxNumNodes + 1;
        //int numQueries = rand() % maxNumQueries + 1;
        const int numNodes = 100'000;
        const int numQueries = 100'000;


        TreeGenerator treeGenerator;
        auto rootNode = treeGenerator.createNode();
        auto chain1 = treeGenerator.addNodeChain(rootNode, numNodes * 4 / 10);
        auto chain2 = treeGenerator.addNodeChain(rootNode, numNodes * 4/ 10);
        treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes / 20, 1.0);
        treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 98.0);
        for (auto testNode : treeGenerator.nodes())
        {
            testNode->data.numCoins = rand() % 20;
        }
        doGenericInfoDFS(rootNode);
        vector<vector<TestNode*>> nodesWithHeight(treeGenerator.numNodes());
        for (auto& node : treeGenerator.nodes())
        {
            nodesWithHeight[node->height].push_back(node);
        }
        for (auto& nodes : nodesWithHeight)
        {
            sort(nodes.begin(), nodes.end(), [](const auto& lhs, const auto& rhs) { return lhs->visitNum < rhs->visitNum; });
        }

        
        findZeroGrundies(treeGenerator, nodesWithHeight);
        vector<TestNode*> nodesThatCanBeReparentedForGrundyZero;
        for (auto node : treeGenerator.nodes())
        {
            if (node->data.newParentHeightsThatGiveZeroGrundy.size() > 8)
            {
                nodesThatCanBeReparentedForGrundyZero.push_back(node);
            }
        }
        assert(nodesThatCanBeReparentedForGrundyZero.size() >= 3);
        vector<TestNode*> nodesToAddQueriesTo;
        for (int i = 0; i < 3; i++)
        {
            nodesToAddQueriesTo.push_back(nodesThatCanBeReparentedForGrundyZero[rand() % nodesThatCanBeReparentedForGrundyZero.size()]);
        }
        vector<TestQuery> queries;
        generateQueries(treeGenerator, queries, nodesToAddQueriesTo, numQueries - 5000, 20., nodesWithHeight);
        generateQueries(treeGenerator, queries, treeGenerator.nodes(), numQueries - queries.size(), 20.0, nodesWithHeight);

        scrambleAndPrintTestcase(treeGenerator, queries);
                                     
#if 0
        for (auto node : treeGenerator.nodes())
        {
            cout << "testing node: " << node->id() << endl;
            markDescendentsAsUsable(node, false);

            for (auto otherNode : treeGenerator.nodes())
            {
                bool isDescendant = otherNode->visitNum >= node->visitNum && otherNode->visitNum <= node->endVisitNum;
                if (isDescendant != !otherNode->data.usable)
                {
                    cout << "node: " << node->id() << " visitNum: " << node->visitNum << " endVisitNum: " << node->endVisitNum << " otherNode: " << otherNode->id() << " visitNum: " << otherNode->visitNum << endl;
                }
                assert(isDescendant == !otherNode->data.usable);
            }


            markDescendentsAsUsable(node, true);
        }
#endif
#if 0
        for (auto node : treeGenerator.nodes())
        {
            cout << "testing node: " << node->id() << endl;
            for (int height = 0; height < treeGenerator.numNodes(); height++)
            {
                auto pick = pickNodeAtHeightNotDescendantOf(height, node, nodesWithHeight);
                //cout << " picked " << (pick ? pick->id() : -1) << endl;
            }
        }
#endif


        return 0;
    }

    // Draft solution:
    //  - Store all queries that re-parent Node n in node n, so we know which newParent n will be re-parented to on each query.
    //  - For node n, calculate grundy sum Gn of subtree rooted at n, though where heights of each descendant is measured
    //    relative to original root, not to n (easy).
    //  - The xor sum for whole tree after removing n is then Groot (lol) ^ Gn.
    //  - For each q at n, what is grundy sum of new tree where n is re-parented to newParent?
    //  - For each power of 2 up to n, maintain a segment tree, numModuloPowerOf2 of length (you guessed it!) powerOf2.
    //  - Each query will essentially add (or subtract) some number m from the height of each descendent of n.
    //  - If we can find out, for each binary digit/ power of 2, how many descendents of n have a height which, when
    //    m is added to/ subtracted from it, will leave that binary digit 1 (or 0), we're in business.
    //  - Whether a height will have digit B switched on after adding m depends, in a fairly simple way, of height % powerOf2 and m; see
    //    list of binary numbers below:
    //
    //     0000
    //     0001
    //     0010
    //     0011
    //     0100
    //     0101
    //     0110
    //     0111
    //     1000
    //     1001
    //     1010
    //     1011
    //     1100
    //     1101
    //     1110
    //     1111
    //
    //  - The number of descendents with bit B set when we add m to their heights will presumably a range (possibly) split
    //    of moduli modulo the power of 2; for example, the bit corresponding to power of 2 == 4 is on for height h
    //    after adding 3 for h = 1, 2, 3, 4 mod 8 and 0 otherwise i.e. for 5, 6, 7, 0.
    //  - When we visit a node with height h:
    //     - For each power of 2, for each m from each query, count the number of heights we've seen so far in the given 
    //       "h modulo power of 2 + m will give binary digit 1" range. (A)
    //     - Add the height h for this node modulo power of 2 to the segment tree corresponding to each power of 2.
    //     - Explore descendants.
    //     - Recalculate A for each query m, and subtract original from it - we now know how many descendants of h will have a given bit B set to on
    //       for m.
    //     - We can then use this to calculate Grundy number for re-parented tree.  I hope :)

    int numNodes;
    cin >> numNodes;

    vector<Node> nodes(numNodes);
    for (int i = 0; i < numNodes - 1; i++)
    {
        int a, b;
        cin >> a >> b;
        a--;
        b--;

        nodes[a].children.push_back(&nodes[b]);
        nodes[b].children.push_back(&nodes[a]);
    }
    for (int i = 0; i < numNodes; i++)
    {
        nodes[i].nodeId = i;
        cin >> nodes[i].numCoins;
    }

    int numQueries;
    cin >> numQueries;

    //cout << "numQueries: " << numQueries << endl;

    vector<Query> queries(numQueries);

    for (int i = 0; i < numQueries; i++)
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
    for (const auto& node : nodes)
        largestHeight = max(largestHeight, node.height);

    log2LargestHeight = log2(largestHeight) + 1;
    //cout << "largestHeight: " << largestHeight << " log2LargestHeight: " << log2LargestHeight << endl;

    int queryNum = 1;
    const auto result = grundyNumbersForQueries(nodes, queries);
    for (const auto queryResult : result)
    {
        if (queryResult == 0)
            cout << queryNum << endl;
        queryNum++;
    }
    cout << endl;


#ifdef BRUTE_FORCE
    const auto resultBruteForce = grundyNumbersForQueriesBruteForce(nodes, queries);
    cout << "resultBruteForce: " << endl;
    queryNum = 1;
    for (const auto queryResult : resultBruteForce)
    {
        if (queryResult == 0)
            cout << queryNum << endl;
        queryNum++;
    }
    cout << endl;
    cout << "result: " << endl;
    queryNum = 1;
    for (const auto queryResult : result)
    {
        if (queryResult == 0)
            cout << queryNum << endl;
        queryNum++;
    }
    cout << endl;
    assert(result == resultBruteForce);
#endif
}


TestNode* TreeGenerator::createNode()
{
    m_nodes.emplace_back(new TestNode);
    auto newNode = m_nodes.back().get();
    newNode->originalId = m_nodes.size();
    return newNode;
}
TestNode* TreeGenerator::createNode(TestNode* attachedTo)
{
    auto newNode = createNode();
    addEdge(newNode, attachedTo);
    return newNode;
}
TestNode* TreeGenerator::createNodeWithRandomParent()
{
    const int parentIndex = rand() % m_nodes.size();
    auto randomParent = m_nodes[parentIndex].get();
    return createNode(randomParent);
}
vector<TestNode*> TreeGenerator::createNodesWithRandomParent(int numNewNodes)
{
    vector<TestNode*> newNodes;
    for (int i = 0; i < numNewNodes; i++)
    {
        newNodes.push_back(createNodeWithRandomParent());
    }

    return newNodes;
}
vector<TestNode*> TreeGenerator::createNodesWithRandomParentPreferringLeafNodes(int numNewNodes, double leafNodePreferencePercent)
{
    set<TestNode*> leaves;
    for (auto& node : m_nodes)
    {
        if (node->neighbours.size() <= 1)
            leaves.insert(node.get());
    }

    return createNodesWithRandomParentPreferringFromSet(leaves, numNewNodes, leafNodePreferencePercent, [](TestNode* newNode, TestNode* newNodeParent, const bool parentWasPreferred, bool& addNewNodeToSet, bool& removeParentFromSet)
            {
            addNewNodeToSet = true;
            if (newNodeParent->neighbours.size() > 1)
            removeParentFromSet = true;
            });
}

vector<TestNode*> TreeGenerator::createNodesWithRandomParentPreferringFromSet(const set<TestNode*>& preferredSet, int numNewNodes, double preferencePercent, std::function<void(TestNode* newNode, TestNode* parent, const bool parentWasPreferred, bool& addNewNodeToSet, bool& removeParentFromSet)> onCreateNode)
{
    vector<TestNode*> newNodes;
    set<TestNode*> preferredSetCopy(preferredSet);
    set<TestNode*> nonPreferredSet;
    for (auto& node : m_nodes)
    {
        if (preferredSetCopy.find(node.get()) == preferredSetCopy.end())
        {
            nonPreferredSet.insert(node.get());
        }
    }

    auto chooseRandomNodeFromSet = [](set<TestNode*>& nodeSet)
    {
        const int randomIndex = rand() % nodeSet.size();
        //cerr << "nodeSet.size(): " << nodeSet.size() << " randomIndex: " << randomIndex << endl;
        auto nodeIter = nodeSet.begin();
        for (int i = 0; i < randomIndex; i++)
        {
            nodeIter++;
        }
        return *nodeIter;
    };

    for (int i = 0; i < numNewNodes; )
    {
        cerr << "createNodesWithRandomParentPreferringFromSet: " << (i + 1) << " / " << numNewNodes << endl;
        const double random = static_cast<double>(rand()) / RAND_MAX * 100;
        TestNode* newNodeParent = nullptr;
        bool parentWasPreferred = false;
        //cerr << "random: " << random << " preferencePercent: " << preferencePercent << endl;
        if (random <= preferencePercent && !preferredSetCopy.empty())
        {
            // Choose a random element from preferredSetCopy as a parent.
            //cerr << " choosing preferred" << endl;
            newNodeParent = chooseRandomNodeFromSet(preferredSetCopy); 
            parentWasPreferred = true;
            cerr << "Chose leaf: " << newNodeParent->neighbours.size() << endl;
            assert(newNodeParent->neighbours.size() <= 1);
        }
        else if (!nonPreferredSet.empty())
        {
            // Choose a random element from nonPreferredSet as a parent.
            cerr << " choosing non-leaf" << endl;
            newNodeParent = chooseRandomNodeFromSet(nonPreferredSet); 
            assert(newNodeParent->neighbours.size() > 1);
        }
        if (newNodeParent)
        {
            auto newNode = createNode(newNodeParent);
            newNodes.push_back(newNode);
            bool addNewNodeToSet = false;
            bool removeParentFromSet = false;
            onCreateNode(newNode, newNodeParent, parentWasPreferred, addNewNodeToSet, removeParentFromSet);
            if (addNewNodeToSet)
                preferredSetCopy.insert(newNode);
            else
                nonPreferredSet.insert(newNode);
            if (removeParentFromSet)
            {
                preferredSetCopy.erase(newNodeParent);
                nonPreferredSet.insert(newNodeParent);
            }
            i++;
        }
    }

    return newNodes;
}

int TreeGenerator::numNodes() const
{
    return m_nodes.size();
};
TestNode* TreeGenerator::firstNode() const
{
    assert(!m_nodes.empty());
    return m_nodes.front().get();
}
TestEdge* TreeGenerator::addEdge(TestNode* nodeA, TestNode* nodeB)
{
    m_edges.emplace_back(new TestEdge);
    auto newEdge = m_edges.back().get();
    newEdge->nodeA = nodeA;
    newEdge->nodeB = nodeB;
    nodeA->neighbours.push_back(newEdge);
    nodeB->neighbours.push_back(newEdge);

    return newEdge;
}
vector<TestNode*> TreeGenerator::addNodeChain(TestNode* chainStart, int numInChain)
{
    vector<TestNode*> nodeChain;

    auto lastNodeInChain = chainStart;
    for (int i = 0; i < numInChain; i++)
    {
        lastNodeInChain = createNode(lastNodeInChain);
        nodeChain.push_back(lastNodeInChain);
    }

    return nodeChain;
}
void TreeGenerator::scrambleNodeOrder()
{
    random_shuffle(m_nodes.begin(), m_nodes.end());
}
void TreeGenerator::scrambleEdgeOrder()
{
    random_shuffle(m_edges.begin(), m_edges.end());
    for (auto& edge : m_edges)
    {
        if (rand() % 2 == 1)
            swap(edge->nodeA, edge->nodeB);
    }
}
// Scrambles the order of the nodes, then re-ids them, in new order, with the first
// node in the new order having scrambledId 1, then next 2, etc.
void TreeGenerator::scrambleNodeIdsAndReorder(TestNode* forceAsRootNode)
{
    scrambleNodeOrder();
    for (int i = 0; i < m_nodes.size(); i++)
    {
        m_nodes[i]->scrambledId = (i + 1);
    }
    if (forceAsRootNode)
    {
        auto forcedRootNodeIter = find_if(m_nodes.begin(), m_nodes.end(), [forceAsRootNode](const auto& nodePtr) { return nodePtr.get() == forceAsRootNode; });
        assert(forcedRootNodeIter != m_nodes.end());
        if (forcedRootNodeIter != m_nodes.begin())
        {
            swap((*m_nodes.begin())->scrambledId, forceAsRootNode->scrambledId);
            iter_swap(m_nodes.begin(), forcedRootNodeIter);
        } 
        assert(forceAsRootNode->scrambledId == 1);
    }
}
vector<TestNode*> TreeGenerator::nodes() const
{
    vector<TestNode*> nodes;
    for (auto& node : m_nodes)
    {
        nodes.push_back(node.get());
    }
    return nodes;
}
vector<TestEdge*> TreeGenerator::edges() const
{
    vector<TestEdge*> edges;
    for (auto& edge : m_edges)
    {
        edges.push_back(edge.get());
    }
    return edges;
}
void TreeGenerator::printEdges() const
{
    for (const auto& edge : m_edges)
    {
        cout << edge->nodeA->id() << " " << edge->nodeB->id() << endl;
    }
}
