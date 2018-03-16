#define VERIFY_HEIGHT_TRACKER
//#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#undef VERIFY_HEIGHT_TRACKER
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <sys/time.h>


using namespace std;

constexpr auto maxHeight = 100'000;
constexpr int log2(int N, int exponent = 0, int powerOf2 = 1)
{
        return (powerOf2 >= N) ? exponent : log2(N, exponent + 1, powerOf2 * 2);
}
constexpr auto maxBinaryDigits = log2(maxHeight);

struct Node
{
    int id = -1;
    bool hasCoin = false;
    Node* parent = nullptr;
    vector<Node*> children;
    vector<Node*> neighbours;
    int numDescendents = 0;
    bool isParentEdgeHeavy = false;

    int dbgNumVisits = 0;
    int grundyNumber = 0;
};

vector<vector<Node*>> heavyChains;

int fixParentChildAndCountDescendants(Node* node, Node* parentNode)
{
    node->parent = parentNode;
    if (parentNode)
    {
        node->children.erase(find(node->children.begin(), node->children.end(), parentNode));
    }

    for (auto child : node->children)
    {
        node->numDescendents += fixParentChildAndCountDescendants(child, node);
    }

    return node->numDescendents;
}

void doHeavyLightDecomposition(Node* node, bool followedHeavyEdge)
{
    if (followedHeavyEdge)
    {
        node->isParentEdgeHeavy = true;
        heavyChains.back().push_back(node);
    }
    else
    {
        heavyChains.push_back({node});
    }
    if (!node->children.empty())
    {
        auto heaviestChildIter = max_element(node->children.begin(), node->children.end(), [](const Node* lhs, const Node* rhs)
                {
                return lhs->numDescendents < rhs->numDescendents;
                });
        iter_swap(node->children.begin(), heaviestChildIter);
        auto heavyChild = node->children.front();
        doHeavyLightDecomposition(heavyChild, true);

        for (auto lightChildIter = node->children.begin() + 1; lightChildIter != node->children.end(); lightChildIter++)
        {
            doHeavyLightDecomposition(*lightChildIter, false);
        }
    }
}

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
            //cout << "dbgGrundyNumber: " << dbgGrundyNumber << " m_grundyNumber: " << m_grundyNumber << endl;
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
template <typename NodeProcessor>
void doDfs(Node* node, int depth, HeightTracker& heightTracker, bool adjustHeightTracker, NodeProcessor& nodeProcessor)
{
    if (adjustHeightTracker)
        heightTracker.adjustAllHeights(1);
    nodeProcessor(node, depth);
    for (auto child : node->children)
    {
        doDfs(child, depth + 1, heightTracker, adjustHeightTracker, nodeProcessor);
    }
    if (adjustHeightTracker)
        heightTracker.adjustAllHeights(-1);
}

template <typename NodeProcessor>
void doLightFirstDFS(Node* node, HeightTracker& heightTracker, bool adjustHeightTracker, NodeProcessor nodeProcessor)
{
    nodeProcessor(node, 0);
    if (node->children.size() > 1)
    {
        for (auto lightChildIter = node->children.begin() + 1; lightChildIter != node->children.end(); lightChildIter++)
        {
            doDfs(*lightChildIter, 1, heightTracker, adjustHeightTracker, nodeProcessor);
        }
    }
}


int grundyNumberBruteForce(Node* node, Node* parent = nullptr, int depth = 0)
{
    int grundyNumber = (node->hasCoin ? depth : 0);
    for (const auto child : node->neighbours)
    {
        if (child == parent)
            continue;

        grundyNumber ^= grundyNumberBruteForce(child, node, depth + 1);
    }
    return grundyNumber;
}


vector<int> computeGrundyNumberForAllNodes(vector<Node>& nodes)
{
    vector<int> grundyNumbers;
    HeightTracker heightTracker;
    for (auto& chain : heavyChains)
    {
        for (int i = 0; i < 2; i++)
        {
            heightTracker.clear();
            cout << "New chain" <<  endl;
            for (auto node : chain)
            {
                cout << " node in chain: " << node->id << endl;
                heightTracker.adjustAllHeights(1);
                //node->grundyNumber ^= heightTracker.grundyNumber();
                // Broadcast.
                doLightFirstDFS(node, heightTracker, true, [&heightTracker](Node* node, int depth)
                        {
                            node->grundyNumber ^= heightTracker.grundyNumber();
                        });
                // Collect.
                doLightFirstDFS(node, heightTracker, false, [&heightTracker](Node* node, int depth)
                        {
                            if (node->hasCoin)
                                heightTracker.insertHeight(depth);
                        });
            }
            reverse(chain.begin(), chain.end());
        }
    }

#if 1
    for (auto& node : nodes)
    {
        cout << "Handling single nodes; node: " << node.id << endl;
        heightTracker.clear();
        // Collect.
        doLightFirstDFS(&node, heightTracker, false, [&heightTracker](Node* node, int depth)
                {
                    if (node->hasCoin)
                        heightTracker.insertHeight(depth);
                });
        cout << "Updating node " << node.id << " with grundy number: " << heightTracker.grundyNumber() << endl;
        //node.grundyNumber ^= heightTracker.grundyNumber();
        // Broadcast, with adjustments.
        doLightFirstDFS(&node, heightTracker, true, [&heightTracker](Node* node, int depth)
                {
                    node->grundyNumber ^= heightTracker.grundyNumber();
                    if (node->hasCoin)
                        node->grundyNumber ^= (2 * depth);
                });
    }
#endif



    return grundyNumbers;
}

int main(int argc, char* argv[])
{
#if 1
    struct timeval time;
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
#endif
    if (argc == 2)
    {
        //const int numNodes = rand() % 100'000;
        const int numNodes = 100'000;
        const int numEdges = numNodes - 1;
        cout << numNodes << endl;
        for (int i = 0; i < numEdges; i++)
        {
            const int parentNodeIndex = (rand() % (i + 1)) + 1;
            cout << (i + 2) << " " << parentNodeIndex << endl;
        }
        return 0;
    }

#if 0
    HeightTracker heightTracker;

    int numInsertions = 0;
    int numAdjustments = 0;
    int numAdjustmentsUp = 0;
    int numAdjustmentsDown = 0;
    int totalNumInsertions = 0;
    int totalNumAdjustments = 0;
    int64_t blah = 0;
    while (true)
    {
        if (rand() % 100'000 == 0)
        {
            //cout << "numInsertions: " << numInsertions << " numAdjustments: " << numAdjustments << " (up: " << numAdjustmentsUp << " down: " << numAdjustmentsDown << ")" << " totalNumInsertions: " << totalNumInsertions << " totalNumAdjustments: " << totalNumAdjustments << endl;
            heightTracker.clear();
            numInsertions = 0;
            numAdjustments = 0;
            numAdjustmentsUp = 0;
            numAdjustmentsDown = 0;
        }
        if (rand() % 2 == 0)
        {
            const int newHeight = rand() % maxHeight;
            heightTracker.insertHeight(newHeight);
            numInsertions++;
            totalNumInsertions++;
        }
        else
        {
            if (rand() % 3 == 0 && heightTracker.canDecreaseHeights())
            {
                heightTracker.adjustAllHeights(-1);
                numAdjustments++;
                numAdjustmentsDown++;
                totalNumAdjustments++;
            }
            else
            {
                if (heightTracker.canIncreaseHeights())
                {
                    heightTracker.adjustAllHeights(1);
                    numAdjustments++;
                    numAdjustmentsUp++;
                    totalNumAdjustments++;
                }
            }
        }
        //cout << "blah: " << heightTracker.grundyNumber() << endl;
        blah += heightTracker.grundyNumber();

        if (totalNumInsertions + totalNumAdjustments >= 10'000'000)
            break;

    }
    cout << blah << endl;
#endif
    int numNodes;
    cin >> numNodes;

    cout << "numNodes: " << numNodes << endl;

    vector<Node> nodes(numNodes);
    for (int nodeIndex = 0; nodeIndex < numNodes; nodeIndex++)
    {
        nodes[nodeIndex].id = nodeIndex + 1;
    }

    for (int edgeNum = 0; edgeNum < numNodes - 1; edgeNum++)
    {
        int node1;
        cin >> node1;
        int node2;
        cin >> node2;
        cout << "node1 : " << node1 << " node2: " << node2 << endl;
        // Make 0-relative.
        node1--;
        node2--;
        assert(cin);

        cout << "edge: " << nodes[node1].id << " - " << nodes[node2].id << endl;

        nodes[node1].children.push_back(&(nodes[node2]));
        nodes[node2].children.push_back(&(nodes[node1]));

        nodes[node1].neighbours.push_back(&(nodes[node2]));
        nodes[node2].neighbours.push_back(&(nodes[node1]));
    }

    for (int i = 0; i < numNodes; i++)
    {
        int numCoins;
        cin >> numCoins;
        cout << "numCoins: " << numCoins << endl;

        nodes[i].hasCoin = ((numCoins % 2) == 1);
    }
    assert(cin);

    auto rootNode = &(nodes.front());
    cout << "rootNode: " << rootNode->id << endl;
    cout << "about to fixParentChildAndCountDescendants" << endl;
    fixParentChildAndCountDescendants(rootNode, nullptr);
    cout << "done fixParentChildAndCountDescendants" << endl;
    doHeavyLightDecomposition(rootNode, false);

    for (const auto& chain : heavyChains)
    {
        cout << "chain: ";
        for (const auto node : chain)
        {
            cout << node->id << " ";
        }
        cout << endl;
    }

#if 0
    for (auto& node : nodes)
    {
        doLightFirstDFS(&node, [](Node* node, int depth)
                {
                    node->dbgNumVisits++;
                });
    }
    for (auto& node : nodes)
    {
        cout << "Node: " << node.id << " visited " << node.dbgNumVisits << " times in doLightFirstDFS" << endl; 
    }
#endif

    computeGrundyNumberForAllNodes(nodes);
    for (auto& node : nodes)
    {
        cout << "Node: " << node.id << " real grundy number: " << grundyNumberBruteForce(&node) << " optimised grundy number: " << node.grundyNumber << endl; 
    }

}
