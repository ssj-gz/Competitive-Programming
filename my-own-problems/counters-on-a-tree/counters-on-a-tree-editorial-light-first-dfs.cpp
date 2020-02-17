// Simon St James (ssjgz) 16/3/18.
// Editorial solution for "Move the Coins, Yet Again!".
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

constexpr auto maxDist = 100'000;
constexpr int log2(int N, int exponent = 0, int powerOf2 = 1)
{
        return (powerOf2 >= N) ? exponent : log2(N, exponent + 1, powerOf2 * 2);
}
constexpr auto maxBinaryDigits = log2(maxDist);

struct Node
{
    int nodeNumber = -1;
    bool hasCoin = false;
    vector<Node*> children;
    vector<Node*> lightChildren;
    int numDescendants = 0;

    int grundyNumberIfRoot = 0;
};

vector<vector<Node*>> heavyChains;

int fixParentChildAndCountDescendants(Node* node, Node* parentNode)
{
    node->numDescendants = 1;
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
        auto heavyChild = *max_element(node->children.begin(), node->children.end(), [](const Node* lhs, const Node* rhs)
                {
                    return lhs->numDescendants < rhs->numDescendants;
                });
        doHeavyLightDecomposition(heavyChild, true);
        for (auto child : node->children)
        {
            if (child != heavyChild)
                node->lightChildren.push_back(child);
        }

        for (auto lightChild : node->lightChildren)
        {
            doHeavyLightDecomposition(lightChild, false);
        }
    }
}

class DistTracker
{
    public:
        DistTracker()
        {
            auto powerOf2 = 2;
            for (auto binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                m_distsModPowerOf2[binaryDigitNum] = vector<VersionedValue>(powerOf2);
                powerOf2 <<= 1;
            }
            clear();
        }
        void insertDist(const int newDist)
        {
            doPendingDistAdjustments();
            const auto newDistAdjusted = newDist 
                // The m_makesDigitOneBegin/End will have been shifted by a total of m_cumulativeDistAdjustment, so counteract that.
                - m_cumulativeDistAdjustment
                // Add a number guarantees that newDistAdjusted >= 0, but does not affect its value modulo the powers of 2 we care about.
                + (1 << (maxBinaryDigits + 1)); 
            assert(newDistAdjusted >= 0);
            auto powerOf2 = 2;
            for (auto binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                const auto distModPowerOf2 = newDistAdjusted & (powerOf2 - 1); // "& (powerOf2 - 1)" is a faster "% powerOf2".
                numDistsModPowerOf2(binaryDigitNum, distModPowerOf2)++;
                if (m_makesDigitOneBegin[binaryDigitNum] <= m_makesDigitOneEnd[binaryDigitNum])
                {
                    if (distModPowerOf2 >= m_makesDigitOneBegin[binaryDigitNum] && distModPowerOf2 <= m_makesDigitOneEnd[binaryDigitNum])
                        m_grundyNumber ^= (powerOf2 >> 1);
                }
                else
                {
                    const auto makeDigitZeroBegin = m_makesDigitOneEnd[binaryDigitNum] + 1;
                    const auto makeDigitZeroEnd = m_makesDigitOneBegin[binaryDigitNum] - 1;
                    assert(0 <= makeDigitZeroBegin && makeDigitZeroBegin <= makeDigitZeroEnd && makeDigitZeroEnd < powerOf2);
                    if (!(distModPowerOf2 >= makeDigitZeroBegin && distModPowerOf2 <= makeDigitZeroEnd))
                        m_grundyNumber ^= (powerOf2 >> 1);
                }

                powerOf2 <<= 1;
            }
        };

        void adjustAllDists(int distDiff)
        {
            m_pendingDistAdjustment += distDiff;
        }
        void doPendingDistAdjustments()
        {
            auto distDiff = m_pendingDistAdjustment;
            if (distDiff == 0)
                return;
            m_pendingDistAdjustment = 0;
            m_cumulativeDistAdjustment += distDiff;

            auto powerOf2 = 2;
            if (distDiff > 0)
            {
                for (auto binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
                {
                    // Scroll the begin/ end of the "makes digit one" zone to the left, updating m_grundyNumber
                    // on-the-fly.
                    auto parityChangeToNumberOfDistsThatMakeDigitsOne = 0;
                    for (auto i = 0; i < distDiff; i++)
                    {
                        parityChangeToNumberOfDistsThatMakeDigitsOne += numDistsModPowerOf2(binaryDigitNum, m_makesDigitOneEnd[binaryDigitNum]);
                        m_makesDigitOneEnd[binaryDigitNum] = (powerOf2 + m_makesDigitOneEnd[binaryDigitNum] - 1) & (powerOf2 - 1);

                        m_makesDigitOneBegin[binaryDigitNum] = (powerOf2 + m_makesDigitOneBegin[binaryDigitNum] - 1) & (powerOf2 - 1);
                        parityChangeToNumberOfDistsThatMakeDigitsOne += numDistsModPowerOf2(binaryDigitNum, m_makesDigitOneBegin[binaryDigitNum]);
                    }

                    if ((parityChangeToNumberOfDistsThatMakeDigitsOne & 1) == 1)
                        m_grundyNumber ^= (powerOf2 >> 1); // This binary digit in the grundy number has flipped; update grundyNumber.

                    powerOf2 <<= 1;
                } 
            }
            else
            {
                distDiff = -distDiff;
                assert(distDiff > 0);
                for (auto binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
                {
                    // As above, but scroll the "makes digit one" zone to the right.
                    auto parityChangeToNumberOfDistsThatMakeDigitsOne = 0;
                    for (auto i = 0; i < distDiff; i++)
                    {
                        parityChangeToNumberOfDistsThatMakeDigitsOne += numDistsModPowerOf2(binaryDigitNum, m_makesDigitOneBegin[binaryDigitNum]);
                        m_makesDigitOneBegin[binaryDigitNum] = (m_makesDigitOneBegin[binaryDigitNum] + 1) & (powerOf2 - 1);

                        m_makesDigitOneEnd[binaryDigitNum] = (m_makesDigitOneEnd[binaryDigitNum] + 1) & (powerOf2 - 1);
                        parityChangeToNumberOfDistsThatMakeDigitsOne += numDistsModPowerOf2(binaryDigitNum, m_makesDigitOneEnd[binaryDigitNum]);
                    }

                    if ((parityChangeToNumberOfDistsThatMakeDigitsOne & 1) == 1)
                        m_grundyNumber ^= (powerOf2 >> 1);

                    powerOf2 <<= 1;
                } 
            }
        }
        int& numDistsModPowerOf2(int binaryDigitNum, int modPowerOf2)
        {
            auto& numDists = m_distsModPowerOf2[binaryDigitNum][modPowerOf2];
            if (numDists.versionNumber != m_versionNumber)
            {
                numDists.value = 0;
                numDists.versionNumber = m_versionNumber;
            }
            return numDists.value;
        }
        int grundyNumber()
        {
            if (m_pendingDistAdjustment != 0)
            {
                doPendingDistAdjustments();
            }
            return m_grundyNumber;
        }
        void clear()
        {
            // Lazily "clear" in O(log2 N) by updating the version number.
            m_versionNumber++;
            auto powerOf2 = 2;
            for (auto binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                m_makesDigitOneBegin[binaryDigitNum] = powerOf2 >> 1;
                m_makesDigitOneEnd[binaryDigitNum] = powerOf2 - 1;

                powerOf2 <<= 1;
            }
            m_grundyNumber = 0;
            m_cumulativeDistAdjustment = 0;
        }
    private:
        struct VersionedValue
        {
            int value = 0;
            int versionNumber = -1;
        };
        vector<VersionedValue> m_distsModPowerOf2[maxBinaryDigits + 1];
        int m_makesDigitOneBegin[maxBinaryDigits + 1];
        int m_makesDigitOneEnd[maxBinaryDigits + 1];

        int m_cumulativeDistAdjustment = 0;
        int m_grundyNumber = 0;

        int m_versionNumber = 0;

        int m_pendingDistAdjustment = 0;
};

enum DistTrackerAdjustment {DoNotAdjust, AdjustWithDepth};
template <typename NodeProcessor>
void doDfs(Node* node, int depth, DistTracker& distTracker, DistTrackerAdjustment distTrackerAdjustment, NodeProcessor& processNode)
{
    if (distTrackerAdjustment == AdjustWithDepth)
        distTracker.adjustAllDists(1);

    processNode(node, depth);

    for (auto child : node->children)
        doDfs(child, depth + 1, distTracker, distTrackerAdjustment, processNode);

    if (distTrackerAdjustment == AdjustWithDepth)
        distTracker.adjustAllDists(-1);
}

void computeGrundyNumberIfRootForAllNodes(vector<Node>& nodes)
{
    DistTracker distTracker;
    auto collectDists = [&distTracker](Node* node, int depth)
                        {
                            if (node->hasCoin)
                                distTracker.insertDist(depth);
                        };
    auto propagateDists = [&distTracker](Node* node, int depth)
                        {
                            node->grundyNumberIfRoot ^= distTracker.grundyNumber();
                        };
    for (auto& chain : heavyChains)
    {
        for (auto pass = 1; pass <= 2; pass++)
        {
            distTracker.clear();
            // Crawl along chain, collecting from one node and propagating to the next.
            for (auto node : chain)
            {
                if (pass == 1 )
                {
                    // Once only (first pass chosen arbitrarily) - add this node's coin
                    // (if any) so that it gets propagated to light descendants ...
                    if (node->hasCoin)
                        distTracker.insertDist(0);
                    // ... and update its grundy number now, so that it *doesn't* include
                    // the contributions from its light descendants.
                    node->grundyNumberIfRoot ^= distTracker.grundyNumber();
                }

                for (auto lightChild : node->lightChildren)
                {
                    // Propagate all coins found so far along the chain in this direction
                    // to light descendants ...
                    doDfs(lightChild, 1, distTracker, AdjustWithDepth, propagateDists);
                    // ... and collect from light descendants.
                    doDfs(lightChild, 1, distTracker, DoNotAdjust, collectDists);
                }

                if (pass == 2)
                {
                    // In pass 1, we ensured that this node's coin (if any) was propagated
                    // to its light descendants.  Don't do it this time - wait until
                    // we've processed this coin's light descendants before adding this
                    // coin's node to the distTracker!
                    if (node->hasCoin)
                        distTracker.insertDist(0);
                    // In pass 1, we ensured that this node's grundy number *wasn't* updated from
                    // its light descendants - this time, ensure that it is updated, by
                    // waiting until we've processed this coin's light descendants before updating
                    // its grundyNumberIfRoot.
                    node->grundyNumberIfRoot ^= distTracker.grundyNumber();
                }

                // Prepare for the reverse pass.
                reverse(node->lightChildren.begin(), node->lightChildren.end());
                // Move one node along the chain - increase all dists accordingly!
                distTracker.adjustAllDists(1);
            }
            // Now do it backwards.
            reverse(chain.begin(), chain.end());
        }
    }
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    auto readInt = [](){ int x; cin >> x; return x; };

    const auto numTestcases = readInt();
    for (int t = 0; t < numTestcases; t++)
    {
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
            nodes[i].nodeNumber = (i + 1);
        }

        auto rootNode = &(nodes.front());
        fixParentChildAndCountDescendants(rootNode, nullptr);
        heavyChains.clear(); // TODO - stop using globals!
        doHeavyLightDecomposition(rootNode, false);

        computeGrundyNumberIfRootForAllNodes(nodes);

        int64_t MOD = 1'000'000'007;
        int64_t result = 0;
        int64_t powerOf2 = 2; // 2 to the power of 1.
        for (auto& node : nodes)
        {
            if (node.grundyNumberIfRoot == 0)
            {
                result = (result + powerOf2) % MOD;
            }
            powerOf2 = (powerOf2 * 2) % MOD;
        }
        cout << result << '\n';

    }
}
