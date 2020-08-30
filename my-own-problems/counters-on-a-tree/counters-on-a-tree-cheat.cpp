#pragma GCC optimize("Ofast")
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
    bool hasCounter = false;
    vector<Node*> neighbours;
    int id = -1;

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
        distTracker.adjustAllDists(1);

    processNode(node, depth, distTracker);

    for (auto child : node->neighbours)
    {
        if (child == parentNode)
            continue;
        doDfs(child, node, depth + 1, distTracker, distTrackerAdjustment, processNode);
    }

    if (distTrackerAdjustment == AdjustWithDepth)
        distTracker.adjustAllDists(-1);
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

void findNumCoinsAtDist(Node* currentNode, Node* parentNode, int distance, int* numCoinsAtDist)
{
    if (currentNode->hasCounter)
    {
        numCoinsAtDist[distance] = 1 - numCoinsAtDist[distance];
        cout << " setting numCoinsAtDist[" << distance << "] to " << numCoinsAtDist[distance] << endl; 
    }

    for (const auto& child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;

        findNumCoinsAtDist(child, currentNode, distance + 1, numCoinsAtDist);
    }

}

void findMaxDistance(Node* currentNode, Node* parentNode, int distance, int& maxDist)
{
    if (currentNode->hasCounter)
        maxDist = max(maxDist, distance);

    for (const auto& child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;

        findMaxDistance(child, currentNode, distance + 1, maxDist);
    }

}

int grundyNumberForNode[maxDist];
int distOfNode[maxDist];

void findGrundyNumberForNode(Node* currentNode, Node* parentNode, int distance, int& index)
{
    //grundyNumberForNode[index].first = currentNode->grundyNumberIfRoot;
    //grundyNumberForNode[index].second = distance;
    grundyNumberForNode[index] = currentNode->grundyNumberIfRoot;
    distOfNode[index] = distance;
    index++;

    for (const auto& child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;

        findGrundyNumberForNode(child, currentNode, distance + 1, index);
    }

}

void setGrundyNumberForNode(Node* currentNode, Node* parentNode, int& index)
{
    //currentNode->grundyNumberIfRoot = grundyNumberForNode[index].first;
    currentNode->grundyNumberIfRoot = grundyNumberForNode[index];
    index++;

    for (const auto& child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;

        setGrundyNumberForNode(child, currentNode, index);
    }

}

int64_t blee = 0;
void doCentroidDecomposition(Node* startNode)
{
    Node* centroid = findCentroid(startNode);

    auto propagateDists = [](Node* node, int depth [[maybe_unused]], DistTracker& distTracker)
                        {
                            node->grundyNumberIfRoot ^= distTracker.grundyNumber();
                        };
    auto collectDists = [](Node* node, int depth, DistTracker& distTracker)
                        {
                            if (node->hasCounter)
                                distTracker.insertDist(depth);
                        };

    const auto numNodesInComponent = countDescendants(startNode, nullptr);


    cout << "centroid: " << centroid->id << " hasCounter: " << centroid->hasCounter << " numNodesInComponent: " << numNodesInComponent << " # neighbours: " << centroid->neighbours.size() << endl;
    for (int pass = 0; pass < 2; pass++)
    {
        cout << "pass " << pass << endl;
        int maxMaxDistance = 0;
        int numCoinsAtDist[numNodesInComponent] = {};
        for (auto& neighbour : centroid->neighbours)
        {
            cout << "Neighbour: " << neighbour->id << endl;
            int numDescendents = 0;
            findGrundyNumberForNode(neighbour, centroid, 1, numDescendents);

            int distancesWithCoins[maxMaxDistance] = {};
            int numDistanceWithCoin = 0;
            cout << "maxMaxDistance:" << maxMaxDistance << endl;
            for (int distance = 0; distance <= maxMaxDistance; distance++)
            {
                assert(numCoinsAtDist[distance] == 0 || numCoinsAtDist[distance] == 1);
                if (numCoinsAtDist[distance] == 1)
                {
                    distancesWithCoins[numDistanceWithCoin] = distance;
                    numDistanceWithCoin++;
                    cout << " distance " << distance << " with coin" << endl;
                }
            }
            //cout << "maxMaxDistance: " << maxMaxDistance << " numDistanceWithCoin: " << numDistanceWithCoin << " maxDistance: " << maxDistance << " numNodesInComponent: " << numNodesInComponent << endl;
            cout << "Mass setting grundies" << endl;
            for (int i = 0; i < numDistanceWithCoin; i++)
            {
                const int distance = distancesWithCoins[i];
                for (int j = 0; j < numDescendents; j++)
                {
#if 1
                    grundyNumberForNode[j] ^= (distance + distOfNode[j]);
#else
                    grundyNumberForNode[j] ^= (distance + j);
#endif
                }
            }

            if (pass == 0)
            {
                for (int i = 0; i < numDistanceWithCoin; i++)
                {
                    const int distance = distancesWithCoins[i];
                    centroid->grundyNumberIfRoot ^= distance;
                }
                if (centroid->hasCounter)
                {
                    for (int j = 0; j < numDescendents; j++)
                    {
                        grundyNumberForNode[j] ^= distOfNode[j];
                    }
                }

            }
            blee += static_cast<int64_t>(numDistanceWithCoin) * numDescendents;


            int blah = 0;
            setGrundyNumberForNode(neighbour, centroid, blah);

            int maxDistance = 0;
            findMaxDistance(neighbour, centroid, 1, maxDistance);
            maxMaxDistance = max(maxMaxDistance, maxDistance);
            findNumCoinsAtDist(neighbour, centroid, 1, numCoinsAtDist);

            //cout << "Blee: " << blee << endl;
        }
        std::reverse(centroid->neighbours.begin(), centroid->neighbours.end());
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

        int numNodesWithCounter = 0;
        int nodeId = 1;
        for (auto& node : nodes)
        {
            const auto numCounters = readInt();
            node.id = nodeId;
            nodeId++;

            node.hasCounter = ((numCounters % 2) == 1);
            if (node.hasCounter)
                numNodesWithCounter++;
        }
        //cout << "numNodesWithCounter: " << numNodesWithCounter << endl;
        


        auto rootNode = &(nodes.front());
        doCentroidDecomposition(rootNode);

        int64_t MOD = 1'000'000'007;
        int64_t result = 0;
        int64_t powerOf2 = 2; // 2 to the power of 1.
        for (auto& node : nodes)
        {
            cout << "node: " << node.id << " grundyNumberIfRoot: " << node.grundyNumberIfRoot << endl;
            if (node.grundyNumberIfRoot == 0)
            {
                result = (result + powerOf2) % MOD;
            }
            powerOf2 = (powerOf2 * 2) % MOD;
        }
        cout << result << endl;
    }
    //cout << "blee: " << blee << endl;
}
