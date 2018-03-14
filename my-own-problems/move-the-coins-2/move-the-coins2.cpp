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
            m_makesDigitOneBegin.resize(maxBinaryDigits + 1);
            m_makesDigitOneEnd.resize(maxBinaryDigits + 1);

            int powerOf2 = 2;
            for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                m_heightsModPowerOf2.push_back(vector<VersionedValue>(powerOf2));
                powerOf2 <<= 1;
            }
            clear();
        }
        void insertHeight(const int newHeight)
        {
            //cout << "insertHeight: " << newHeight << " m_cumulativeHeightAdjustment: " << m_cumulativeHeightAdjustment << endl;
            if (newHeight < m_smallestHeight)
                m_smallestHeight = newHeight;
            int powerOf2 = 2;
            int newHeightAdjusted = newHeight - m_cumulativeHeightAdjustment;
            if (newHeightAdjusted < 0)
            {
                newHeightAdjusted += (1 << (maxBinaryDigits + 1));
            }
            assert(newHeightAdjusted >= 0);
            for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                const int heightModPowerOf2 = newHeightAdjusted % powerOf2;
                numHeightsModPowerOf2(binaryDigitNum, heightModPowerOf2)++;
                if (m_makesDigitOneBegin[binaryDigitNum] <= m_makesDigitOneEnd[binaryDigitNum])
                {
                    if (heightModPowerOf2 >= m_makesDigitOneBegin[binaryDigitNum] && heightModPowerOf2 <= m_makesDigitOneEnd[binaryDigitNum])
                    {
                        m_grundyNumber ^= (powerOf2 >> 1);
                    }
                }
                else
                {
                    const int makeDigitZeroBegin = m_makesDigitOneEnd[binaryDigitNum] + 1;
                    const int makeDigitZeroEnd = m_makesDigitOneBegin[binaryDigitNum] - 1;
                    assert(makeDigitZeroBegin <= makeDigitZeroEnd);
                    assert(0 <= makeDigitZeroEnd < powerOf2);
                    assert(0 <= makeDigitZeroBegin < powerOf2);
                    if (!(heightModPowerOf2 >= makeDigitZeroBegin && heightModPowerOf2 <= makeDigitZeroEnd))
                    {
                        m_grundyNumber ^= (powerOf2 >> 1);
                    }
                }

                powerOf2 <<= 1;
            }
#ifdef VERIFY_HEIGHT_TRACKER
            m_dbgHeights.push_back(newHeight);
#endif
        };
        bool canDecreaseHeights() const
        {
            // This is just  for testing - won't be needed in real-life.
            // TODO - remove this!
            if (m_smallestHeight == 0)
                return false;
            return true;
        }
        bool canIncreaseHeights() const
        {
            // This is just  for testing - won't be needed in real-life.
            // TODO - remove this!
            return m_cumulativeHeightAdjustment <= maxHeight;
        }
        void adjustAllHeights(int heightDiff)
        {
            if (heightDiff == 0)
                return;
            assert(heightDiff == 1 || heightDiff == -1);
            m_cumulativeHeightAdjustment += heightDiff;
            m_smallestHeight += heightDiff;
            if (heightDiff == 1)
            {
                int powerOf2 = 2;
                for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
                {
                    // Scroll the begin/ end of the "makes digit one" zone to the left, updating m_grundyNumber
                    // on-the-fly.
                    int changeToNumberOfHeightsThatMakeDigitsOne = 0;
                    changeToNumberOfHeightsThatMakeDigitsOne -= numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneEnd[binaryDigitNum]);
                    m_makesDigitOneEnd[binaryDigitNum] = (powerOf2 + m_makesDigitOneEnd[binaryDigitNum] - 1) % powerOf2;

                    m_makesDigitOneBegin[binaryDigitNum] = (powerOf2 + m_makesDigitOneBegin[binaryDigitNum] - 1) % powerOf2;
                    changeToNumberOfHeightsThatMakeDigitsOne += numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneBegin[binaryDigitNum]);

                    if (abs(changeToNumberOfHeightsThatMakeDigitsOne) % 2 == 1)
                        m_grundyNumber ^= (powerOf2 >> 1);

                    powerOf2 <<= 1;
                } 
            }
            else
            {
                int powerOf2 = 2;
                for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
                {
                    // As above, but scroll the "makes digit one" zone to the right.
                    int changeToNumberOfHeightsThatMakeDigitsOne = 0;
                    changeToNumberOfHeightsThatMakeDigitsOne -= numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneBegin[binaryDigitNum]);
                    m_makesDigitOneBegin[binaryDigitNum] = (m_makesDigitOneBegin[binaryDigitNum] + 1) % powerOf2;

                    m_makesDigitOneEnd[binaryDigitNum] = (m_makesDigitOneEnd[binaryDigitNum] + 1) % powerOf2;
                    changeToNumberOfHeightsThatMakeDigitsOne += numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneEnd[binaryDigitNum]);

                    if (abs(changeToNumberOfHeightsThatMakeDigitsOne) % 2 == 1)
                        m_grundyNumber ^= (powerOf2 >> 1);

                    powerOf2 <<= 1;
                } 
            }

#ifdef VERIFY_HEIGHT_TRACKER
            for (auto& height : m_dbgHeights)
            {
                height += heightDiff;
                assert(height >= 0);
            }
#endif
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
        int grundyNumber() const
        {
#ifdef VERIFY_HEIGHT_TRACKER
            int dbgGrundyNumber = 0;
            for (const auto height : m_dbgHeights)
            {
                dbgGrundyNumber ^= height;
            }
            cout << "dbgGrundyNumber: " << dbgGrundyNumber << " m_grundyNumber: " << m_grundyNumber << endl;
            assert(dbgGrundyNumber == m_grundyNumber);
#endif
            return m_grundyNumber;
        }
        void clear()
        {
            //cout << "Clear!" << endl;
            m_versionNumber++;
            int powerOf2 = 2;
            for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                //m_heightsModPowerOf2.push_back(vector<int>(powerOf2, 0));
                m_makesDigitOneBegin[binaryDigitNum] = powerOf2 >> 1;
                m_makesDigitOneEnd[binaryDigitNum] = powerOf2 - 1;

                powerOf2 <<= 1;
            }
            m_grundyNumber = 0;
            m_cumulativeHeightAdjustment = 0;
#ifdef VERIFY_HEIGHT_TRACKER
            m_dbgHeights.clear();
#endif
        }
#ifdef VERIFY_HEIGHT_TRACKER
        vector<int> m_dbgHeights;
#endif
        struct VersionedValue
        {
            int value = 0;
            int versionNumber = -1;
        };
        vector<vector<VersionedValue>> m_heightsModPowerOf2;
        vector<int> m_makesDigitOneBegin;
        vector<int> m_makesDigitOneEnd;

        int m_cumulativeHeightAdjustment = 0;
        int m_grundyNumber = 0;

        int m_versionNumber = 0;
        int m_smallestHeight = 0;
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


void grundyNumberForTreeBruteForce(Node* node, const int depth, int& grundyNumber)
{
    for (int i = 0; i < node->numCoins; i++)
    {
        grundyNumber ^= depth;
    }
    for (Node* child : node->children)
    {
        grundyNumberForTreeBruteForce(child, depth + 1, grundyNumber);
    }
}

int grundyNumberForTreeBruteForce(Node* node)
{
    int grundyNumber = 0;
    grundyNumberForTreeBruteForce(node, 0, grundyNumber);
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
    for (const auto& query : queries)
    {
        auto originalParent = query.nodeToMove->parent;
        reparentNode(query.nodeToMove, query.newParent);
        grundyNumbersForQueries.push_back(grundyNumberForTreeBruteForce(rootNode));
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

int grundyNumberWithHeightChange(Node* node, const int heightChange)
{
    int grundyNumber = 0;
    if ((node->numCoins % 2) == 1)
    {
        const int newHeight = node->height + heightChange;
        assert(newHeight >= 0);
        grundyNumber ^= newHeight;
    }
    for (auto child : node->children)
    {
        grundyNumber ^= grundyNumberWithHeightChange(child, heightChange);
    }
    return grundyNumber;
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
    assert(rootNode->grundyNumber == grundyNumberForTreeBruteForce(rootNode));
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
            assert(grundyNumberForTreeBruteForce(rootNode) == (originalTreeGrundyNumber ^ query.nodeToMove->grundyNumber));
        }
        reparentNode(query.nodeToMove, query.newParent);
        grundyNumbersForQueries.push_back(grundyNumberForTreeBruteForce(rootNode));
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

        //const int forceHeight = 30000;
        const int forceHeight = -1;

        int numNodes = rand() % maxNumNodes + 1;
        int numQueries = rand() % maxNumQueries + 1;

        cerr << "numNodes: " << numNodes << " numQueries: " << numQueries << endl;

        int numMetaAttempts = 0;

        int largestHeight = -1;
        while (true)
        {
            //cout << "Random tree with nodes: " << numNodes << endl;
            vector<Node> nodes(numNodes);
            nodes[0].height = 0;
            vector<pair<int, int>> edges;
            for (int i = 0; i < numNodes; i++)
            {
                int parentNodeIndex = -1;
                if (i > 0)
                {
                    if (forceHeight == -1 || i > 2 * forceHeight)
                    {
                        parentNodeIndex = rand() % i;
                    }
                    else
                    {
                        if (i != forceHeight)
                        {
                            parentNodeIndex = i - 1;
                        }
                        else if (i == forceHeight)
                        {
                            parentNodeIndex = 0;
                        }
                    }
                    nodes[parentNodeIndex].children.push_back(&nodes[i]);
                    nodes[i].parent = &(nodes[parentNodeIndex]);
                    nodes[i].height = nodes[i].parent->height + 1;
                    edges.push_back({i, parentNodeIndex});
                }
                nodes[i].numCoins = rand() % 20;
                nodes[i].nodeId = i;
                largestHeight = max(largestHeight, nodes[i].height);
            }
            cerr << "largestHeight:" << largestHeight << endl;

            int numQueriesToGenerate = numQueries;
            int numAttempts = 0;
            vector<pair<int, int>> potentialQueries;
            int64_t numPotentialQueries = 0;
            int64_t numZeroGrundies = 0;
            auto rootNode = &(nodes.front());
            for (int nodeIndexToMove = 0; nodeIndexToMove < numNodes; nodeIndexToMove++)
            {
                if (nodeIndexToMove % 100 == 0)
                    cerr << "nodeIndexToMove: " << nodeIndexToMove << " / " << numNodes << endl;
                auto& nodeToMove = nodes[nodeIndexToMove];
                auto oldParent = nodeToMove.parent;
                markDescendentsAsUsable(&nodeToMove, false);
                for (int newParentNodeIndex = 0; newParentNodeIndex < numNodes; newParentNodeIndex++)
                {
                    auto newParent = &(nodes[newParentNodeIndex]);
                    if (newParent->usable)
                    {
                        //potentialQueries.push_back({nodeIndexToMove, newParentNodeIndex});
                        numPotentialQueries++;
                        //cout << "numPotentialQueries: " << numPotentialQueries << endl;
#if 0
                        auto originalParent = nodeToMove.parent;
                        reparentNode(&nodeToMove, newParent);
                        const auto grundyNumber = grundyNumberForTreeBruteForce(rootNode);
                        //const auto grundyNumber = 1;
                        reparentNode(&nodeToMove, originalParent);
                        if (grundyNumber == 0)
                        {
                            numZeroGrundies++;
                            cout << "numZeroGrundies: " << numZeroGrundies << " newParent height: " << newParent->height << endl;
                        }
#endif
                    }
                }
                markDescendentsAsUsable(&nodeToMove, true);
            }
            cerr << "numZeroGrundies: " << numZeroGrundies << " numPotentialQueries: " << numPotentialQueries << endl;
            //assert(RAND_MAX >= numPotentialQueries);
            const bool successful = (numPotentialQueries >= numQueries);
            if (successful)
            {
                vector<int64_t> queriesChosen;
                while (queriesChosen.size() < numQueries)
                {
                    int64_t queryIndex = ((uint64_t(rand()) << 32) | rand()) % numPotentialQueries;
                    if (find(queriesChosen.begin(), queriesChosen.end(), queryIndex) == queriesChosen.end())
                    {
                        queriesChosen.push_back(queryIndex);
                        if (queriesChosen.size() % 100 == 0)
                        {
                            cerr << "chosen " << queriesChosen.size() << " out of " << numQueries << " queries" << endl;
                        }
                    }
                }
                sort(queriesChosen.begin(), queriesChosen.end());

                int64_t queryIndex = 0;
                vector<pair<int, int>> queries;
                queries.reserve(numQueries);
                auto queryIndexIter = queriesChosen.begin();
                for (int nodeIndexToMove = 0; nodeIndexToMove < numNodes; nodeIndexToMove++)
                {
                    if (nodeIndexToMove % 100 == 0)
                        cerr << "nodeIndexToMove: " << nodeIndexToMove << " / " << numNodes << endl;
                    auto& nodeToMove = nodes[nodeIndexToMove];
                    auto oldParent = nodeToMove.parent;
                    markDescendentsAsUsable(&nodeToMove, false);
                    for (int newParentNodeIndex = 0; newParentNodeIndex < numNodes; newParentNodeIndex++)
                    {
                        if (nodes[newParentNodeIndex].usable)
                        {
                            //potentialQueries.push_back({nodeIndexToMove, newParentNodeIndex});
                            if (queryIndexIter != queriesChosen.end() && *queryIndexIter == queryIndex)
                            {
                                queries.push_back({nodeIndexToMove, newParentNodeIndex});
                                queryIndexIter++;
                                if (queries.size() % 100 == 0)
                                    cerr << "Fulfilled " << queries.size() << " out of " << numQueries << " queries" << endl;
                            }
                            queryIndex++;
                            //cout << "numPotentialQueries: " << numPotentialQueries << endl;
                        }
                    }
                    markDescendentsAsUsable(&nodeToMove, true);
                }
                random_shuffle(queries.begin(), queries.end());
                cout << numNodes << endl;
                for (const auto& node : nodes)
                {
                    cout << node.numCoins << " ";
                }
                cout << endl;
                assert(edges.size() == numNodes - 1);
                for (auto& edge : edges)
                {
                    if (rand() % 2 == 0)
                        swap(edge.first, edge.second);
                }
                random_shuffle(edges.begin(), edges.end());
                for (const auto& edge : edges)
                {
                    cout << (edge.first + 1) << " " << (edge.second + 1) << endl;
                }
                cout << numQueries << endl;
                for (const auto& query : queries)
                {
                    cout << (query.first + 1) << " " << (query.second + 1) << endl;
                }
                break;
            }
            else
            {
                cerr << "Unsuccessful; numPotentialQueries: " << numPotentialQueries << " needed " << numQueries << endl;
            }

            numMetaAttempts++;
            //cout << "numMetaAttempts: " << numMetaAttempts << endl;
            if (numMetaAttempts > 1000)
            {
                //cout << "Whoops" << endl;
                numNodes = rand() % maxNumNodes + 1;
                numQueries = rand() % maxNumQueries + 1;
                numMetaAttempts = 0;
            }
        }
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
    for (int i = 0; i < numNodes; i++)
    {
        nodes[i].nodeId = i;
        cin >> nodes[i].numCoins;
    }
    for (int i = 0; i < numNodes - 1; i++)
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

#ifdef FIND_ZERO_GRUNDYS
    originalTreeGrundyNumber = findGrundyNumberForNodes(rootNode);
    assert(rootNode->grundyNumber == grundyNumberForTreeBruteForce(rootNode));
    int numZeroGrundies = 0;
    vector<int> nodeIds;
    for (int i = 0; i < numNodes; i++)
    {
        nodeIds.push_back(i);
    }
    //random_shuffle(nodeIds.begin(), nodeIds.end());
    int numNodesProcessed = 0;
    HeightTracker heightTracker;
    for (auto nodeId : nodeIds)
    {
        auto node = &(nodes[nodeId]);
        vector<int> numWithHeight(numNodes + 1);
        buildHeightHistogram(node, numWithHeight);
        vector<int> descendentHeights;
        heightTracker.clear();
        for (int height = 0; height < numWithHeight.size(); height++)
        {
            if ((numWithHeight[height] % 2) == 1)
            {
                descendentHeights.push_back(height);
                heightTracker.insertHeight(height - node->height);
            }
        }
#if 0
        for (int binaryDigitNum = 0; binaryDigitNum <= log2MaxN; binaryDigitNum++)
        {
            const int powerOf2 = (1 << (binaryDigitNum + 1));
            numNodesWithHeightModuloPowerOf2[binaryDigitNum].clear();
            numNodesWithHeightModuloPowerOf2[binaryDigitNum].resize(powerOf2 + 1);
            for (int height = 0; height < numWithHeight.size(); height++)
            {
                const int heightModuloPowerOf2 = height % powerOf2;
                //cout << "About to applyOperatorToAllInRange - binaryDigitNum: " << binaryDigitNum << " heightModuloPowerOf2: " << heightModuloPowerOf2 << endl;
                bit_up(numNodesWithHeightModuloPowerOf2[binaryDigitNum].data(), numNodesWithHeightModuloPowerOf2[binaryDigitNum].size(), heightModuloPowerOf2, numWithHeight[height]);
            }
        }
#endif
        //cout << "node: " << node->nodeId << " height: " << node->height << " depth underneath: " <<  << endl;
        const int depthUnderneath = (descendentHeights.empty() ? - 1 :  descendentHeights.back() - node->height);
        for (int heightChange = -node->height; node->height + heightChange <= largestHeight; heightChange++)
        {
            //cout << "heightChange: " << heightChange << endl;
            const int grundyNumberMinusSubtree = originalTreeGrundyNumber ^ node->grundyNumber;
            //const int newGrundyNumber = grundyNumberMinusSubtree ^ grundyNumberWithHeightChange(node, heightChange);
#if 0
            int adjustedXor = 0;
            for (const auto height : descendentHeights)
            {
                const int adjustedHeight = (height + heightChange);
                assert(adjustedHeight >= 0);
                adjustedXor ^= adjustedHeight;
            }
#endif
#if 0
            int relocatedSubtreeGrundyDigits[log2MaxN + 1] = {};
            countCoinsThatMakeDigitOneAfterHeightChange(heightChange, relocatedSubtreeGrundyDigits);
            int relocatedSubtreeGrundyNumber = 0;
            for (int binaryDigitNum = 0; binaryDigitNum <= log2MaxN; binaryDigitNum++)
            {
                //relocatedSubtreeGrundyDigits[binaryDigitNum] -= reorderedQuery.originalNodesThatMakeDigitOne[binaryDigitNum];
                assert(relocatedSubtreeGrundyDigits[binaryDigitNum] >= 0);
                relocatedSubtreeGrundyNumber += (1 << binaryDigitNum) * (relocatedSubtreeGrundyDigits[binaryDigitNum] % 2);
            }
#endif
            int relocatedSubtreeGrundyNumber = heightTracker.grundyNumber();
            //assert(relocatedSubtreeGrundyNumber == heightTracker.grundyNumber());
            //cout << "relocatedSubtreeGrundyNumber: " << relocatedSubtreeGrundyNumber << " heightTracker.grundyNumber(): " << heightTracker.grundyNumber() << endl;
            heightTracker.adjustAllHeights(1);
            //cout << "relocatedSubtreeGrundyNumber: " << relocatedSubtreeGrundyNumber << " adjustedXor: " << adjustedXor << endl;
            //assert(relocatedSubtreeGrundyNumber == adjustedXor);
            //const int newGrundyNumber = grundyNumberMinusSubtree ^ grundyNumberWithHeightChange(node, heightChange);
            //const int newGrundyNumber = grundyNumberMinusSubtree ^ adjustedXor;
            const int newGrundyNumber = grundyNumberMinusSubtree ^ relocatedSubtreeGrundyNumber;
            if (newGrundyNumber == 0)
            {
                numZeroGrundies++;
                cout << "Forced a grundy number: nodeId: " << nodeId << " node height: " << node->height << " depthUnderneath: " << depthUnderneath << " heightChange: " << heightChange << " total: " << numZeroGrundies << " numNodesProcessed: " << numNodesProcessed << " numNodes: " << numNodes << endl;
                //assert((grundyNumberMinusSubtree ^ grundyNumberWithHeightChange(node, heightChange)) == newGrundyNumber);
            }
        }
        numNodesProcessed++;
        //if ((numNodesProcessed % 100) == 0)
            //cerr << "Processed " << numNodesProcessed << " out of " << numNodes << endl;
    }
    return 0;
#endif

    const auto result = grundyNumbersForQueries(nodes, queries);
    for (const auto queryResult : result)
    {
        cout << queryResult << " ";
    }
    cout << endl;


#ifdef BRUTE_FORCE
    const auto resultBruteForce = grundyNumbersForQueriesBruteForce(nodes, queries);
    cout << "resultBruteForce: " << endl;
    for (const auto queryResult : resultBruteForce)
    {
        cout << queryResult << " ";
    }
    cout << endl;
    cout << "result: " << endl;
    for (const auto queryResult : result)
    {
        cout << queryResult << " ";
    }
    cout << endl;
    assert(result == resultBruteForce);
#endif
}
