// Simon St James (ssjgz) 16/3/18.
// Setter's solution for "'Move the Coins' 2".
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

constexpr auto maxDist = 200'000;
constexpr int log2(int N, int exponent = 0, int powerOf2 = 1)
{
        return (powerOf2 >= N) ? exponent : log2(N, exponent + 1, powerOf2 * 2);
}
constexpr auto maxBinaryDigits = log2(maxDist);

struct Node
{
    bool hasCoin = false;
    vector<Node*> neighbours;

    int grundyNumberIfRoot = 0;
};

class DistTracker
{
    public:
        DistTracker(int maxDist)
        {
            auto powerOf2 = 2;
            m_numBits = -1;
            for (auto binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                m_distsModPowerOf2[binaryDigitNum] = vector<int>(powerOf2);
                m_makesDigitOneBegin[binaryDigitNum] = powerOf2 >> 1;
                m_makesDigitOneEnd[binaryDigitNum] = powerOf2 - 1;
                if (powerOf2 / 2 > maxDist) 
                    break; // i.e. even if we scrolled this powerOf2 maxDist units, we'd never enter the "make digit 1" zone.
                powerOf2 <<= 1;
                m_numBits++;
            }
        }
        void insertDist(const int newDist)
        {
            const int numBits = m_numBits;
            doPendingDistAdjustments();
            const auto newDistAdjusted = newDist 
                // The m_makesDigitOneBegin/End will have been shifted by a total of m_cumulativeDistAdjustment, so counteract that.
                - m_cumulativeDistAdjustment
                // Add a number guarantees that newDistAdjusted >= 0, but does not affect its value modulo the powers of 2 we care about.
                + (1 << (maxBinaryDigits + 1)); 
            assert(newDistAdjusted >= 0);
            auto powerOf2 = 2;
            for (auto binaryDigitNum = 0; binaryDigitNum <= numBits; binaryDigitNum++)
            {
                const auto distModPowerOf2 = newDistAdjusted & (powerOf2 - 1); // "& (powerOf2 - 1)" is a faster "% powerOf2".
                m_distsModPowerOf2[binaryDigitNum][distModPowerOf2]++;
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

        void addToAllDists(int distDiff)
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

            const int numBits = m_numBits;
            auto powerOf2 = 2;
            if (distDiff > 0)
            {
                for (auto binaryDigitNum = 0; binaryDigitNum <= numBits; binaryDigitNum++)
                {
                    // Scroll the begin/ end of the "makes digit one" zone to the left, updating m_grundyNumber
                    // on-the-fly.
                    const auto reducedDistDiff = distDiff & (powerOf2 - 1); // Scrolling powerOf2 units is the same as not scrolling at all!
                    auto parityChangeToNumberOfDistsThatMakeDigitsOne = 0;
                    for (auto i = 0; i < reducedDistDiff; i++)
                    {
                        parityChangeToNumberOfDistsThatMakeDigitsOne += m_distsModPowerOf2[binaryDigitNum][m_makesDigitOneEnd[binaryDigitNum]];
                        m_makesDigitOneEnd[binaryDigitNum] = (powerOf2 + m_makesDigitOneEnd[binaryDigitNum] - 1) & (powerOf2 - 1);

                        m_makesDigitOneBegin[binaryDigitNum] = (powerOf2 + m_makesDigitOneBegin[binaryDigitNum] - 1) & (powerOf2 - 1);
                        parityChangeToNumberOfDistsThatMakeDigitsOne += m_distsModPowerOf2[binaryDigitNum][m_makesDigitOneBegin[binaryDigitNum]];
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
                for (auto binaryDigitNum = 0; binaryDigitNum <= numBits; binaryDigitNum++)
                {
                    // As above, but scroll the "makes digit one" zone to the right.
                    auto parityChangeToNumberOfDistsThatMakeDigitsOne = 0;
                    const auto reducedDistDiff = distDiff & (powerOf2 - 1); // Scrolling powerOf2 units is the same as not scrolling at all!
                    for (auto i = 0; i < reducedDistDiff; i++)
                    {
                        parityChangeToNumberOfDistsThatMakeDigitsOne += m_distsModPowerOf2[binaryDigitNum][m_makesDigitOneBegin[binaryDigitNum]];
                        m_makesDigitOneBegin[binaryDigitNum] = (m_makesDigitOneBegin[binaryDigitNum] + 1) & (powerOf2 - 1);

                        m_makesDigitOneEnd[binaryDigitNum] = (m_makesDigitOneEnd[binaryDigitNum] + 1) & (powerOf2 - 1);
                        parityChangeToNumberOfDistsThatMakeDigitsOne += m_distsModPowerOf2[binaryDigitNum][m_makesDigitOneEnd[binaryDigitNum]];
                    }

                    if ((parityChangeToNumberOfDistsThatMakeDigitsOne & 1) == 1)
                        m_grundyNumber ^= (powerOf2 >> 1);

                    powerOf2 <<= 1;
                } 
            }
        }
        int grundyNumber()
        {
            if (m_pendingDistAdjustment != 0)
            {
                doPendingDistAdjustments();
            }
            return m_grundyNumber;
        }
    private:
        int m_numBits = 0;
        vector<int> m_distsModPowerOf2[maxBinaryDigits + 1];
        int m_makesDigitOneBegin[maxBinaryDigits + 1];
        int m_makesDigitOneEnd[maxBinaryDigits + 1];

        int m_cumulativeDistAdjustment = 0;
        int m_grundyNumber = 0;

        int m_pendingDistAdjustment = 0;
};

enum DistTrackerAdjustment {DoNotAdjust, AdjustWithDepth};

int countDescendants(Node* node, Node* parentNode)
{
    int numDescendants = 1; // Current node.

    for (const auto& child : node->neighbours)
    {
        if (child == parentNode)
            continue;

        numDescendants += countDescendants(child, node);
    }

    return numDescendants;
}

template <typename NodeProcessor>
void doDfs(Node* node, Node* parentNode, int depth, DistTracker& distTracker, DistTrackerAdjustment distTrackerAdjustment, NodeProcessor& processNode)
{
    if (distTrackerAdjustment == AdjustWithDepth)
        distTracker.addToAllDists(1);

    processNode(node, depth, distTracker);

    for (auto child : node->neighbours)
    {
        if (child == parentNode)
            continue;
        doDfs(child, node, depth + 1, distTracker, distTrackerAdjustment, processNode);
    }

    if (distTrackerAdjustment == AdjustWithDepth)
        distTracker.addToAllDists(-1);
}

int findCentroidAux(Node* currentNode, Node* parentNode, const int totalNodes, Node** centroid)
{
    int numDescendents = 1;

    bool childHasTooManyDescendants = false;

    for (const auto& child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;

        const auto numChildDescendants = findCentroidAux(child, currentNode, totalNodes, centroid);
        if (numChildDescendants > totalNodes / 2)
        {
            // Not the centroid, but can't break here - must continue processing children.
            childHasTooManyDescendants = true;
        }

        numDescendents += numChildDescendants;
    }

    if (!childHasTooManyDescendants)
    {
        // No child has more than totalNodes/2 descendants, but what about the remainder of the graph?
        const auto nonChildDescendants = totalNodes - numDescendents;
        if (nonChildDescendants <= totalNodes / 2)
        {
            assert(centroid);
            *centroid = currentNode;
        }
    }

    return numDescendents;
}

Node* findCentroid(Node* startNode)
{
    const auto totalNumNodes = countDescendants(startNode, nullptr);
    Node* centroid = nullptr;
    findCentroidAux(startNode, nullptr, totalNumNodes, &centroid);
    assert(centroid);
    return centroid;
}

void doCentroidDecomposition(Node* startNode)
{
    Node* centroid = findCentroid(startNode);

    auto propagateDists = [](Node* node, int depth [[maybe_unused]], DistTracker& distTracker)
                        {
                            node->grundyNumberIfRoot ^= distTracker.grundyNumber();
                        };
    auto collectDists = [](Node* node, int depth, DistTracker& distTracker)
                        {
                            if (node->hasCoin)
                                distTracker.insertDist(depth);
                        };

    const auto numNodesInComponent = countDescendants(startNode, nullptr);

    {
        // Collect and Propagate contributions from each branch to the next branches.
        DistTracker distTracker(numNodesInComponent);
        for (auto& child : centroid->neighbours)
        {
            doDfs(child, centroid, 1, distTracker, AdjustWithDepth, propagateDists );
            doDfs(child, centroid, 1, distTracker, DoNotAdjust, collectDists );
        }
    }
    {
        // Do it again ... 
        DistTracker distTracker(numNodesInComponent);
        // ... this time with the branches processed in the opposite order ...
        reverse(centroid->neighbours.begin(), centroid->neighbours.end());
        // ... and also include the centre, this time.
        if (centroid->hasCoin)
            distTracker.insertDist(0);
        for (auto& child : centroid->neighbours)
        {
            doDfs(child, centroid, 1, distTracker, AdjustWithDepth, propagateDists );
            doDfs(child, centroid, 1, distTracker, DoNotAdjust, collectDists );
        }
        centroid->grundyNumberIfRoot ^= distTracker.grundyNumber();
    }

    for (auto& neighbour : centroid->neighbours)
    {
        assert(std::find(neighbour->neighbours.begin(), neighbour->neighbours.end(), centroid) != neighbour->neighbours.end());
        // Erase the edge from the centroid's neighbour to the centroid, essentially "chopping off" each child into its own
        // component.
        neighbour->neighbours.erase(std::find(neighbour->neighbours.begin(), neighbour->neighbours.end(), centroid));
        doCentroidDecomposition(neighbour);
    }
}

int main()
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

            nodes[node1Index].neighbours.push_back(&(nodes[node2Index]));
            nodes[node2Index].neighbours.push_back(&(nodes[node1Index]));
        }

        for (auto& node : nodes)
        {
            const auto numCoins = readInt();

            node.hasCoin = ((numCoins % 2) == 1);
        }


        auto rootNode = &(nodes.front());
        doCentroidDecomposition(rootNode);

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
        cout << result << endl;
    }
}
