// Simon St James (ssjgz) 16/3/18.
// Editorial solution for "Counters on a Tree".
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
    vector<Node*> neighbours;

    int grundyNumberIfRoot = 0;
};

int numInsertHeights = 0;
int numAdjustHeights = 0;
int numGrundyNumberCalls = 0;
int numCallsToPropagateHeights = 0;
int numCallsToCollectHeights = 0;

class HeightTracker
{
    public:
        HeightTracker(int maxHeight)
        {
            auto powerOf2 = 2;
            m_numBits = -1;
            for (auto binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                m_heightsModPowerOf2[binaryDigitNum] = vector<int>(powerOf2);
                m_makesDigitOneBegin[binaryDigitNum] = powerOf2 >> 1;
                m_makesDigitOneEnd[binaryDigitNum] = powerOf2 - 1;
                powerOf2 <<= 1;
                if (powerOf2 / 4 > maxHeight)
                    break;
                m_numBits++;
            }
        }
        void insertHeight(const int newHeight)
        {
            const int numBits = m_numBits;
            numInsertHeights++;
            doPendingHeightAdjustments();
            const auto newHeightAdjusted = newHeight 
                // The m_makesDigitOneBegin/End will have been shifted by a total of m_cumulativeHeightAdjustment, so counteract that.
                - m_cumulativeHeightAdjustment
                // Add a number guarantees that newHeightAdjusted >= 0, but does not affect its value modulo the powers of 2 we care about.
                + (1 << (maxBinaryDigits + 1)); 
            assert(newHeightAdjusted >= 0);
            auto powerOf2 = 2;
            for (auto binaryDigitNum = 0; binaryDigitNum <= numBits; binaryDigitNum++)
            {
                const auto heightModPowerOf2 = newHeightAdjusted & (powerOf2 - 1); // "& (powerOf2 - 1)" is a faster "% powerOf2".
                m_heightsModPowerOf2[binaryDigitNum][heightModPowerOf2]++;
                if (m_makesDigitOneBegin[binaryDigitNum] <= m_makesDigitOneEnd[binaryDigitNum])
                {
                    if (heightModPowerOf2 >= m_makesDigitOneBegin[binaryDigitNum] && heightModPowerOf2 <= m_makesDigitOneEnd[binaryDigitNum])
                        m_grundyNumber ^= (powerOf2 >> 1);
                }
                else
                {
                    const auto makeDigitZeroBegin = m_makesDigitOneEnd[binaryDigitNum] + 1;
                    const auto makeDigitZeroEnd = m_makesDigitOneBegin[binaryDigitNum] - 1;
                    assert(0 <= makeDigitZeroBegin && makeDigitZeroBegin <= makeDigitZeroEnd && makeDigitZeroEnd < powerOf2);
                    if (!(heightModPowerOf2 >= makeDigitZeroBegin && heightModPowerOf2 <= makeDigitZeroEnd))
                        m_grundyNumber ^= (powerOf2 >> 1);
                }

                powerOf2 <<= 1;
            }
        };

        void adjustAllHeights(int heightDiff)
        {
            numAdjustHeights++;
            m_pendingHeightAdjustment += heightDiff;
        }
        void doPendingHeightAdjustments()
        {
            auto heightDiff = m_pendingHeightAdjustment;
            if (heightDiff == 0)
                return;
            m_pendingHeightAdjustment = 0;
            m_cumulativeHeightAdjustment += heightDiff;

            const int numBits = m_numBits;
            auto powerOf2 = 2;
            if (heightDiff > 0)
            {
                for (auto binaryDigitNum = 0; binaryDigitNum <= numBits; binaryDigitNum++)
                {
                    // Scroll the begin/ end of the "makes digit one" zone to the left, updating m_grundyNumber
                    // on-the-fly.
                    const auto reducedHeightDiff = heightDiff & (powerOf2 - 1); // Scrolling powerOf2 units is the same as not scrolling at all!
                    auto parityChangeToNumberOfHeightsThatMakeDigitsOne = 0;
                    for (auto i = 0; i < reducedHeightDiff; i++)
                    {
                        parityChangeToNumberOfHeightsThatMakeDigitsOne += m_heightsModPowerOf2[binaryDigitNum][m_makesDigitOneEnd[binaryDigitNum]];
                        m_makesDigitOneEnd[binaryDigitNum] = (powerOf2 + m_makesDigitOneEnd[binaryDigitNum] - 1) & (powerOf2 - 1);

                        m_makesDigitOneBegin[binaryDigitNum] = (powerOf2 + m_makesDigitOneBegin[binaryDigitNum] - 1) & (powerOf2 - 1);
                        parityChangeToNumberOfHeightsThatMakeDigitsOne += m_heightsModPowerOf2[binaryDigitNum][m_makesDigitOneBegin[binaryDigitNum]];
                    }

                    if ((parityChangeToNumberOfHeightsThatMakeDigitsOne & 1) == 1)
                        m_grundyNumber ^= (powerOf2 >> 1); // This binary digit in the grundy number has flipped; update grundyNumber.

                    powerOf2 <<= 1;
                } 
            }
            else
            {
                heightDiff = -heightDiff;
                assert(heightDiff > 0);
                for (auto binaryDigitNum = 0; binaryDigitNum <= numBits; binaryDigitNum++)
                {
                    // As above, but scroll the "makes digit one" zone to the right.
                    auto parityChangeToNumberOfHeightsThatMakeDigitsOne = 0;
                    const auto reducedHeightDiff = heightDiff & (powerOf2 - 1); // Scrolling powerOf2 units is the same as not scrolling at all!
                    for (auto i = 0; i < reducedHeightDiff; i++)
                    {
                        parityChangeToNumberOfHeightsThatMakeDigitsOne += m_heightsModPowerOf2[binaryDigitNum][m_makesDigitOneBegin[binaryDigitNum]];
                        m_makesDigitOneBegin[binaryDigitNum] = (m_makesDigitOneBegin[binaryDigitNum] + 1) & (powerOf2 - 1);

                        m_makesDigitOneEnd[binaryDigitNum] = (m_makesDigitOneEnd[binaryDigitNum] + 1) & (powerOf2 - 1);
                        parityChangeToNumberOfHeightsThatMakeDigitsOne += m_heightsModPowerOf2[binaryDigitNum][m_makesDigitOneEnd[binaryDigitNum]];
                    }

                    if ((parityChangeToNumberOfHeightsThatMakeDigitsOne & 1) == 1)
                        m_grundyNumber ^= (powerOf2 >> 1);

                    powerOf2 <<= 1;
                } 
            }
        }
        int grundyNumber()
        {
            numGrundyNumberCalls++;
            if (m_pendingHeightAdjustment != 0)
            {
                doPendingHeightAdjustments();
            }
            return m_grundyNumber;
        }
    private:
        int m_numBits = 0;
        vector<int> m_heightsModPowerOf2[maxBinaryDigits + 1];
        int m_makesDigitOneBegin[maxBinaryDigits + 1];
        int m_makesDigitOneEnd[maxBinaryDigits + 1];

        int m_cumulativeHeightAdjustment = 0;
        int m_grundyNumber = 0;

        int m_pendingHeightAdjustment = 0;
};

enum HeightTrackerAdjustment {DoNotAdjust, AdjustWithDepth};

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
void doDfs(Node* node, Node* parentNode, int depth, HeightTracker& heightTracker, HeightTrackerAdjustment heightTrackerAdjustment, NodeProcessor& processNode)
{
    if (heightTrackerAdjustment == AdjustWithDepth)
        heightTracker.adjustAllHeights(1);

    processNode(node, depth, heightTracker);

    for (auto child : node->neighbours)
    {
        if (child == parentNode)
            continue;
        doDfs(child, node, depth + 1, heightTracker, heightTrackerAdjustment, processNode);
    }

    if (heightTrackerAdjustment == AdjustWithDepth)
        heightTracker.adjustAllHeights(-1);
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

    auto propagateHeights = [](Node* node, int depth, HeightTracker& heightTracker)
                        {
                            numCallsToPropagateHeights++;
                            node->grundyNumberIfRoot ^= heightTracker.grundyNumber();
                        };
    auto collectHeights = [](Node* node, int depth, HeightTracker& heightTracker)
                        {
                            numCallsToCollectHeights++;
                            if (node->hasCoin)
                                heightTracker.insertHeight(depth);
                        };

    const auto numNodesInComponent = countDescendants(startNode, nullptr);

    {
        HeightTracker heightTracker(numNodesInComponent);
        for (auto& child : centroid->neighbours)
        {
            doDfs(child, centroid, 1, heightTracker, AdjustWithDepth, propagateHeights );
            doDfs(child, centroid, 1, heightTracker, DoNotAdjust, collectHeights );
        }
    }
    {
        HeightTracker heightTracker(numNodesInComponent);
        // Do it again, this time backwards ...  
        reverse(centroid->neighbours.begin(), centroid->neighbours.end());
        // ... and also include the centre, this time.
        if (centroid->hasCoin)
            heightTracker.insertHeight(0);
        for (auto& child : centroid->neighbours)
        {
            doDfs(child, centroid, 1, heightTracker, AdjustWithDepth, propagateHeights );
            doDfs(child, centroid, 1, heightTracker, DoNotAdjust, collectHeights );
        }
        centroid->grundyNumberIfRoot ^= heightTracker.grundyNumber();
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

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    auto readInt = [](){ int x; cin >> x; return x; };

    const auto numTestcases = readInt();
    for (int t = 0; t < numTestcases; t++)
    {
        const auto numNodes = readInt();
        vector<Node> nodes(numNodes);

        for (auto& node : nodes)
        {
            const auto numCoins = readInt();

            node.hasCoin = ((numCoins % 2) == 1);
        }

        for (auto edgeNum = 0; edgeNum < numNodes - 1; edgeNum++)
        {
            const auto node1Index = readInt() - 1;
            const auto node2Index = readInt() - 1;

            nodes[node1Index].neighbours.push_back(&(nodes[node2Index]));
            nodes[node2Index].neighbours.push_back(&(nodes[node1Index]));
        }

        auto rootNode = &(nodes.front());
        doCentroidDecomposition(rootNode);

        vector<int> nodesThatGiveBobWinWhenRoot;
        int nodeNumber = 1;
        for (auto& node : nodes)
        {
            if (node.grundyNumberIfRoot == 0)
                nodesThatGiveBobWinWhenRoot.push_back(nodeNumber);

            nodeNumber++;
        }
        cout << nodesThatGiveBobWinWhenRoot.size() << endl;
        for (const auto bobWinNodeNum : nodesThatGiveBobWinWhenRoot)
        {
            cout << bobWinNodeNum << endl;
        }
    }
#if 0
    cout << "numInsertHeights: " << numInsertHeights << endl;
    cout << "numAdjustHeights: " << numAdjustHeights << endl;
    cout << "numGrundyNumberCalls: " << numGrundyNumberCalls << endl;
    cout << "numCallsToPropagateHeights: " << numCallsToPropagateHeights << endl;
    cout << "numCallsToCollectHeights: " << numCallsToCollectHeights << endl;
#endif
}
