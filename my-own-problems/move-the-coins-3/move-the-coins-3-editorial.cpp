// Simon St James (ssjgz) 16/3/18.
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

constexpr auto maxHeight = 100'000;
constexpr int log2(int N, int exponent = 0, int powerOf2 = 1)
{
        return (powerOf2 >= N) ? exponent : log2(N, exponent + 1, powerOf2 * 2);
}
constexpr auto maxBinaryDigits = log2(maxHeight);

struct Node
{
    bool hasCoin = false;
    vector<Node*> children;
    int numDescendants = 0;

    int grundyNumber = 0;
};

vector<vector<Node*>> heavyChains;

int fixParentChildAndCountDescendants(Node* node, Node* parentNode)
{
    if (parentNode)
        node->children.erase(find(node->children.begin(), node->children.end(), parentNode));

    for (auto child : node->children)
        node->numDescendants += fixParentChildAndCountDescendants(child, node);

    return node->numDescendants;
}

// Build up heavyChains; move the heavy child of each node to the front of that node's children.
void doHeavyLightDecomposition(Node* node, bool followedHeavyEdge)
{
    if (followedHeavyEdge)
    {
        // Continue this chain.
        heavyChains.back().push_back(node);
    }
    else
    {
        // Start a new chain.
        heavyChains.push_back({node});
    }
    if (!node->children.empty())
    {
        auto heaviestChildIter = max_element(node->children.begin(), node->children.end(), [](const Node* lhs, const Node* rhs)
                {
                    return lhs->numDescendants < rhs->numDescendants;
                });
        iter_swap(node->children.begin(), heaviestChildIter);
        auto heavyChild = node->children.front();
        doHeavyLightDecomposition(heavyChild, true);

        for (auto lightChildIter = node->children.begin() + 1; lightChildIter != node->children.end(); lightChildIter++)
            doHeavyLightDecomposition(*lightChildIter, false);
    }
}

class HeightTracker
{
    public:
        HeightTracker()
        {
            m_makesDigitOneBegin.resize(maxBinaryDigits + 1);
            m_makesDigitOneEnd.resize(maxBinaryDigits + 1);

            auto powerOf2 = 2;
            for (auto binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                m_heightsModPowerOf2.push_back(vector<VersionedValue>(powerOf2));
                powerOf2 <<= 1;
            }
            clear();
        }
        void insertHeight(const int newHeight)
        {
            auto powerOf2 = 2;
            auto newHeightAdjusted = newHeight - m_cumulativeHeightAdjustment;
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
        };

        void adjustAllHeights(int heightDiff)
        {
            if (heightDiff == 0)
                return;
            assert(heightDiff == 1 || heightDiff == -1);
            m_cumulativeHeightAdjustment += heightDiff;

            auto powerOf2 = 2;
            if (heightDiff == 1)
            {
                for (auto binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
                {
                    // Scroll the begin/ end of the "makes digit one" zone to the left, updating m_grundyNumber
                    // on-the-fly.
                    auto changeToNumberOfHeightsThatMakeDigitsOne = 0;
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
                for (auto binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
                {
                    // As above, but scroll the "makes digit one" zone to the right.
                    auto changeToNumberOfHeightsThatMakeDigitsOne = 0;
                    changeToNumberOfHeightsThatMakeDigitsOne -= numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneBegin[binaryDigitNum]);
                    m_makesDigitOneBegin[binaryDigitNum] = (m_makesDigitOneBegin[binaryDigitNum] + 1) % powerOf2;

                    m_makesDigitOneEnd[binaryDigitNum] = (m_makesDigitOneEnd[binaryDigitNum] + 1) % powerOf2;
                    changeToNumberOfHeightsThatMakeDigitsOne += numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneEnd[binaryDigitNum]);

                    if (abs(changeToNumberOfHeightsThatMakeDigitsOne) % 2 == 1)
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
        int grundyNumber() const
        {
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
        vector<vector<VersionedValue>> m_heightsModPowerOf2;
        vector<int> m_makesDigitOneBegin;
        vector<int> m_makesDigitOneEnd;

        int m_cumulativeHeightAdjustment = 0;
        int m_grundyNumber = 0;

        int m_versionNumber = 0;
};

enum HeightTrackerAdjustment {DoNotAdjust, AdjustWithDepth};
template <typename NodeProcessor>
void doDfs(Node* node, int depth, HeightTracker& heightTracker, HeightTrackerAdjustment heightTrackerAdjustment, NodeProcessor& nodeProcessor)
{
    if (heightTrackerAdjustment == AdjustWithDepth)
        heightTracker.adjustAllHeights(1);

    nodeProcessor(node, depth);

    for (auto child : node->children)
        doDfs(child, depth + 1, heightTracker, heightTrackerAdjustment, nodeProcessor);

    if (heightTrackerAdjustment == AdjustWithDepth)
        heightTracker.adjustAllHeights(-1);
}

template <typename NodeProcessor>
void doLightFirstDFS(Node* node, HeightTracker& heightTracker, HeightTrackerAdjustment heightTrackerAdjustment, NodeProcessor nodeProcessor)
{
    nodeProcessor(node, 0);
    if (node->children.size() > 1)
    {
        for (auto lightChildIter = node->children.begin() + 1; lightChildIter != node->children.end(); lightChildIter++)
        {
            doDfs(*lightChildIter, 1, heightTracker, heightTrackerAdjustment, nodeProcessor);
        }
    }
}

void computeGrundyNumberForAllNodes(vector<Node>& nodes)
{
    HeightTracker heightTracker;
    auto collectHeights = [&heightTracker](Node* node, int depth)
                        {
                            if (node->hasCoin)
                                heightTracker.insertHeight(depth);
                        };
    auto broadcastHeights = [&heightTracker](Node* node, int depth)
                        {
                            node->grundyNumber ^= heightTracker.grundyNumber();
                        };
    for (auto& chain : heavyChains)
    {
        for (int i = 0; i < 2; i++)
        {
            heightTracker.clear();
            // Crawl along chain, collecting from one node and broadcasting to the next.
            for (auto node : chain)
            {
                // Adjust the heights we collected from previous node in the chain, and broadcast it to
                // light-first descendants of this node.
                heightTracker.adjustAllHeights(1);
                doLightFirstDFS(node, heightTracker, AdjustWithDepth, broadcastHeights);
                
                // Collect heights from light-first descendants.
                doLightFirstDFS(node, heightTracker, DoNotAdjust, collectHeights);
            }
            // Now do it backwards.
            reverse(chain.begin(), chain.end());
        }
    }
    for (auto& node : nodes)
    {
        const bool hasLightChildren = (node.children.size() > 1);
        if (!hasLightChildren)
            continue;
        // Update node with height info from all its light-first descendants.
        doLightFirstDFS(&node, heightTracker, DoNotAdjust, [&node](Node* descendantNode, int depth) 
                { 
                    if (descendantNode->hasCoin) node.grundyNumber ^= depth; 
                });
        if (node.hasCoin)
        {
            // Broadcast this nodes' coin info to descendants.
            doLightFirstDFS(&node, heightTracker, DoNotAdjust, [](Node* node, int depth) 
                    { 
                        node->grundyNumber ^= depth; 
                    });
        }
        // Broadcast light-first descendant info to other light-first descendants.
        vector<Node*> lightChildren = vector<Node*>(node.children.begin() + 1, node.children.end());
        heightTracker.clear();
        for (auto lightChild : lightChildren)
        {
            doDfs(lightChild, 1, heightTracker, AdjustWithDepth, broadcastHeights);
            doDfs(lightChild, 1, heightTracker, DoNotAdjust, collectHeights);
        }
        reverse(lightChildren.begin(), lightChildren.end());
        // ... and again, using reversed order of children.
        heightTracker.clear();
        for (auto lightChild : lightChildren)
        {
            doDfs(lightChild, 1, heightTracker, AdjustWithDepth, broadcastHeights);
            doDfs(lightChild, 1, heightTracker, DoNotAdjust, collectHeights);
        }
    }

    int numBlah = 0;
    for (auto& node : nodes)
    {
        cout << node.grundyNumber << '\n';
        if (node.grundyNumber == 0)
            numBlah++;
    }
    cout << "numBlah (editorial):" << numBlah << endl;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    auto readInt = [](){ int x; cin >> x; return x; };

    const auto numNodes = readInt();
    vector<Node> nodes(numNodes);

    for (auto edgeNum = 0; edgeNum < numNodes - 1; edgeNum++)
    {
        const auto node1Index = readInt() - 1;
        const auto node2Index = readInt() - 1;

        nodes[node1Index].children.push_back(&(nodes[node2Index]));
        nodes[node2Index].children.push_back(&(nodes[node1Index]));
    }

    for (auto i = 0; i < numNodes; i++)
    {
        const auto numCoins = readInt();

        nodes[i].hasCoin = ((numCoins % 2) == 1);
    }

    auto rootNode = &(nodes.front());
    fixParentChildAndCountDescendants(rootNode, nullptr);
    doHeavyLightDecomposition(rootNode, false);

    computeGrundyNumberForAllNodes(nodes);
}
