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
    int numDescendents = 0;

    int grundyNumber = 0;
};

vector<vector<Node*>> heavyChains;

int fixParentChildAndCountDescendants(Node* node, Node* parentNode)
{
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
        };

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
            int powerOf2 = 2;
            for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                m_makesDigitOneBegin[binaryDigitNum] = powerOf2 >> 1;
                m_makesDigitOneEnd[binaryDigitNum] = powerOf2 - 1;

                powerOf2 <<= 1;
            }
            m_grundyNumber = 0;
            m_cumulativeHeightAdjustment = 0;
        }
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

enum HeightTrackerAdjustment {DoNotAdjust, AdjustUpWithDepth};
template <typename NodeProcessor>
void doDfs(Node* node, int depth, HeightTracker& heightTracker, HeightTrackerAdjustment heightTrackerAdjustment, NodeProcessor& nodeProcessor)
{
    if (heightTrackerAdjustment == AdjustUpWithDepth)
        heightTracker.adjustAllHeights(1);

    nodeProcessor(node, depth);

    for (auto child : node->children)
    {
        doDfs(child, depth + 1, heightTracker, heightTrackerAdjustment, nodeProcessor);
    }

    if (heightTrackerAdjustment == AdjustUpWithDepth)
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
    auto collect = [&heightTracker](Node* node, int depth)
                        {
                            if (node->hasCoin)
                                heightTracker.insertHeight(depth);
                        };
    auto broadcast = [&heightTracker](Node* node, int depth)
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
                heightTracker.adjustAllHeights(1);
                doLightFirstDFS(node, heightTracker, AdjustUpWithDepth, broadcast);
                doLightFirstDFS(node, heightTracker, DoNotAdjust, collect);
            }
            // Now do it backwards.
            reverse(chain.begin(), chain.end());
        }
    }
    for (auto& node : nodes)
    {
        if (node.children.empty())
            continue;
        // Collect and update node from all its light-first descendents.
        heightTracker.clear();
        doLightFirstDFS(&node, heightTracker, DoNotAdjust, collect);
        node.grundyNumber ^= heightTracker.grundyNumber();
        // Broadcast this nodes' coin info to descendents.
        if (node.hasCoin)
        {
            heightTracker.clear();
            heightTracker.insertHeight(0);
            doLightFirstDFS(&node, heightTracker, AdjustUpWithDepth, broadcast);
        }
        // Broadcast light-first descendent info to other light-first descendents.
        vector<Node*> lightChildren = vector<Node*>(node.children.begin() + 1, node.children.end());
        heightTracker.clear();
        for (auto lightChild : lightChildren)
        {
            doDfs(lightChild, 1, heightTracker, AdjustUpWithDepth, broadcast);
            doDfs(lightChild, 1, heightTracker, DoNotAdjust, collect);
        }
        reverse(lightChildren.begin(), lightChildren.end());
        // ... and again, using reversed order of children.
        heightTracker.clear();
        for (auto lightChild : lightChildren)
        {
            doDfs(lightChild, 1, heightTracker, AdjustUpWithDepth, broadcast);
            doDfs(lightChild, 1, heightTracker, DoNotAdjust, collect);
        }
    }

    int numBlah = 0;
    for (auto& node : nodes)
    {
        cout << node.grundyNumber << endl;
        if (node.grundyNumber == 0)
            numBlah++;
    }
    cout << "numBlah (editorial):" << numBlah << endl;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    int numNodes;
    cin >> numNodes;

    vector<Node> nodes(numNodes);

    for (int edgeNum = 0; edgeNum < numNodes - 1; edgeNum++)
    {
        int node1;
        cin >> node1;
        int node2;
        cin >> node2;
        // Make 0-relative.
        node1--;
        node2--;


        nodes[node1].children.push_back(&(nodes[node2]));
        nodes[node2].children.push_back(&(nodes[node1]));

    }

    for (int i = 0; i < numNodes; i++)
    {
        int numCoins;
        cin >> numCoins;

        nodes[i].hasCoin = ((numCoins % 2) == 1);
    }

    auto rootNode = &(nodes.front());
    fixParentChildAndCountDescendants(rootNode, nullptr);
    doHeavyLightDecomposition(rootNode, false);

    computeGrundyNumberForAllNodes(nodes);

}
