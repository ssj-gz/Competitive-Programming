// Simon St James (ssjgz) - Editorial solution for "Equilateral Treeangles" - 2019-07-17.
// Problem was later renamed to "Chef and Gordon Ramsay 2" when I saw CHGORAM in AUG19
// and noticed how my Problem could be turned into a sequel to it :)
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include <cassert>

using namespace std;

const int numTripletPermutations = 1 * 2 * 3; // i.e. == factorial(3).

struct DescendantWithHeightInfo
{
    // The number of suitable pairs of descendants of Node with the given height
    // that have Node as their Lowest Common Ancestor.
    int64_t numPairsWithHeightWithNodeAsLCA = 0;
    // The name "number" is not strictly correct: if numPairsWithHeightWithNodeAsLCA > 0, then this
    // will indeed be the number of suitable descendants of this Node which have this height
    // but if numPairsWithHeightWithNodeAsLCA == 0, "number" will also be 0.
    // 
    // The "number" count is used to avoid the overcount when performing completeTrianglesOfTypeA
    // (Centroid Decomposition has no notion of our parent-child relationship, so
    // will count "descendants" of a Node alongside "non-descendants" of a Node.)
    int number = 0;
};

struct Node
{
    vector<Node*> neighbours;
    bool isSuitable = false;
    int height = 0;

    // The total number of entries into descendantWithHeightInfo, summed across *all* nodes,
    // will be O(n log n).
    map<int, DescendantWithHeightInfo> descendantWithHeightInfo;
};

struct HeightInfo
{
    int numWithHeight = 0;
    Node* lastUpdatedAtNode = nullptr;
};

class DistTracker
{
    public:
        // O(maxDist).
        DistTracker(int maxDist)
            : m_numWithDist(2 * maxDist + 1), 
            m_maxDist(maxDist)
        {
        }
        // O(1).
        void insertDist(const int newDist)
        {
            numWithDistValue(newDist)++;
            m_largestDist = max(m_largestDist, newDist);
        };
        // O(1).
        int numWithDist(int dist)
        {
            return numWithDistValue(dist);
        }
        // O(1).
        void adjustAllDists(int distDiff)
        {
            m_cumulativeDistAdjustment += distDiff;
            assert(m_cumulativeDistAdjustment >= 0);
            if (m_largestDist != -1)
                m_largestDist += distDiff;
        }
        // O(1).
        int largestDist() const
        {
            return m_largestDist;
        }
    private:
        int m_cumulativeDistAdjustment = 0;
        vector<int> m_numWithDist;
        int m_maxDist = -1;

        int m_largestDist = -1;

        int& numWithDistValue(int dist)
        {
            return m_numWithDist[dist - m_cumulativeDistAdjustment + m_maxDist];
        }
};

enum DistTrackerAdjustment {DoNotAdjust, AdjustWithDepth};

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

int countDescendants(Node* node, Node* parentNode)
{
    auto numDescendants = 1; // Current node.

    for (const auto& child : node->neighbours)
    {
        if (child == parentNode)
            continue;

        numDescendants += countDescendants(child, node);
    }

    return numDescendants;
}

int findCentroidAux(Node* currentNode, Node* parentNode, const int totalNodes, Node** centroid)
{
    auto numDescendants = 1;

    auto childHasTooManyDescendants = false;

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

        numDescendants += numChildDescendants;
    }

    if (!childHasTooManyDescendants)
    {
        // No child has more than totalNodes/2 descendants, but what about the remainder of the graph?
        const auto nonChildDescendants = totalNodes - numDescendants;
        if (nonChildDescendants <= totalNodes / 2)
        {
            assert(centroid);
            *centroid = currentNode;
        }
    }

    return numDescendants;
}

Node* findCentroid(Node* startNode)
{
    const auto totalNumNodes = countDescendants(startNode, nullptr);
    Node* centroid = nullptr;
    findCentroidAux(startNode, nullptr, totalNumNodes, &centroid);
    assert(centroid);
    return centroid;
}

void doCentroidDecomposition(Node* startNode, int64_t& numTriangles)
{
    Node* centroid = findCentroid(startNode);

    auto addSomeTypeATrianglesForNode = [&numTriangles](Node* node, DistTracker& distTracker)
    {
        // This will be called O(log2 n) times for each node before the node's
        // Type A Triangles are fully completed.
        for (const auto& heightPair : node->descendantWithHeightInfo)
        {
            const auto descendantHeight = heightPair.first;
            const auto requiredNonDescendantDist = (descendantHeight - node->height);
            if (requiredNonDescendantDist > distTracker.largestDist())
                break; // Optimisation - no point continuing with larger descendantHeights.

            const auto numPairsWithHeightWithNodeAsLCA = heightPair.second.numPairsWithHeightWithNodeAsLCA;
            const auto numNewTriangles = numPairsWithHeightWithNodeAsLCA * distTracker.numWithDist(requiredNonDescendantDist) * numTripletPermutations;
            assert(numNewTriangles >= 0);
            numTriangles += numNewTriangles;
        }
    };
    auto propagateDists = [&addSomeTypeATrianglesForNode](Node* node, int depth, DistTracker& distTracker)
                        {
                            addSomeTypeATrianglesForNode(node, distTracker);
                        };
    auto collectDists = [](Node* node, int depth, DistTracker& distTracker)
                        {
                            if (node->isSuitable)
                                distTracker.insertDist(depth);
                        };

    const auto numNodesInComponent = countDescendants(startNode, nullptr);

    {
        DistTracker distTracker(numNodesInComponent);
        for (auto& child : centroid->neighbours)
        {
            doDfs(child, centroid, 1, distTracker, AdjustWithDepth, propagateDists );
            doDfs(child, centroid, 1, distTracker, DoNotAdjust, collectDists );
        }
    }
    {
        DistTracker distTracker(numNodesInComponent);
        // Do it again, this time backwards ...
        reverse(centroid->neighbours.begin(), centroid->neighbours.end());
        // ... and also include the centre, this time.
        if (centroid->isSuitable)
            distTracker.insertDist(0);
        for (auto& child : centroid->neighbours)
        {
            doDfs(child, centroid, 1, distTracker, AdjustWithDepth, propagateDists );
            doDfs(child, centroid, 1, distTracker, DoNotAdjust, collectDists );
        }
        addSomeTypeATrianglesForNode(centroid, distTracker);
    }

    for (auto& neighbour : centroid->neighbours)
    {
        assert(std::find(neighbour->neighbours.begin(), neighbour->neighbours.end(), centroid) != neighbour->neighbours.end());
        // Erase the edge from the centroid's neighbour to the centroid, essentially "chopping off" each child into its own
        // component.
        neighbour->neighbours.erase(std::find(neighbour->neighbours.begin(), neighbour->neighbours.end(), centroid));
        doCentroidDecomposition(neighbour, numTriangles);
    }
}

void completeTrianglesOfTypeACentroidDecomposition(vector<Node>& nodes, Node* rootNode, int64_t& numTriangles)
{
    doCentroidDecomposition(rootNode, numTriangles);
    // Fix the overcount caused by Centroid Decomposition (over-)counting descendants of a node as non-descendants
    // of a node!
    for (auto& node : nodes)
    {
        for (const auto descendantHeightPair : node.descendantWithHeightInfo)
        {
            const auto height = descendantHeightPair.first;
            const auto numPairsWithHeightWithNodeAsLCA = descendantHeightPair.second.numPairsWithHeightWithNodeAsLCA;

            // Centroid decomposition would have (wrongly) added numPairsWithHeightWithNodeAsLCA[height] * numTripletPermutations 
            // for each suitable descendant of node which had height - correct for this.
            numTriangles -= numPairsWithHeightWithNodeAsLCA * node.descendantWithHeightInfo[height].number * numTripletPermutations;
        }
    }
}

map<int, HeightInfo> buildDescendantHeightInfo(Node* currentNode, Node* parentNode, int height, int64_t& numTriangles)
{
    currentNode->height = height;

    map<int, HeightInfo> persistentInfoForDescendantHeight;

    for (auto child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;
        // Quick C++ performance note: in C++11 onwards, capturing a returned std::map
        // in a local variable is O(1), due to Move Semantics.  Prior to this, though,
        // it could have been O(size of std::map) (if the Return Value Optimisation ended up
        // not being used), which would (silently!) lead to asymptotically worse performance!
        //
        // Luckily, this code uses C++11 features so we can't accidentally fall into this trap.
        auto transientInfoForDescendantHeight = buildDescendantHeightInfo(child, currentNode, height + 1, numTriangles);
        if (transientInfoForDescendantHeight.size() > persistentInfoForDescendantHeight.size())
        {
            // We'll be "copying" the tran
            // Swapping is O(1).
            swap(persistentInfoForDescendantHeight, transientInfoForDescendantHeight);
        }

        for (auto transientDescendantHeightPair : transientInfoForDescendantHeight)
        {
            // This block of code (i.e. the body of the containing for... loop) 
            // is executed O(n log2 n) times over the whole run.
            // It is guaranteed to be executed with descendantHeight if the current
            // child has a descendant of descendantHeight that isSuitable and a previous child of this
            // node also has a descendant of descendantHeight that isSuitable, but may also
            // be executed under different circumstances.
            const auto descendantHeight = transientDescendantHeightPair.first;

            const auto& transientHeightInfo = transientDescendantHeightPair.second;
            auto& heightInfoForNode = persistentInfoForDescendantHeight[descendantHeight];

            assert (descendantHeight > currentNode->height);

            auto numUnprocessedDescendantsWithHeight = -1;
            auto numKnownDescendantsWithHeight = -1;
            assert(transientHeightInfo.lastUpdatedAtNode != nullptr);
            if (transientHeightInfo.lastUpdatedAtNode == currentNode)
            {
                assert(heightInfoForNode.lastUpdatedAtNode != currentNode);
                numUnprocessedDescendantsWithHeight = heightInfoForNode.numWithHeight;
                numKnownDescendantsWithHeight = transientHeightInfo.numWithHeight;
            }
            else
            {
                assert(transientHeightInfo.lastUpdatedAtNode != currentNode);
                numUnprocessedDescendantsWithHeight = transientHeightInfo.numWithHeight;
                numKnownDescendantsWithHeight = heightInfoForNode.numWithHeight;
            }

            const auto earlierChildHasThisHeight = (numKnownDescendantsWithHeight > 0);
            if (earlierChildHasThisHeight)
            {
                auto& descendantHeightInfo = currentNode->descendantWithHeightInfo[descendantHeight];
                auto& numPairsWithHeightWithNodeAsLCA = descendantHeightInfo.numPairsWithHeightWithNodeAsLCA;
                auto& numberDescendantsWithThisHeight = descendantHeightInfo.number;

                if (numUnprocessedDescendantsWithHeight * numPairsWithHeightWithNodeAsLCA > 0)
                {
                    // Found a triple where all three nodes have currentNode as their LCA: a "Type B" triangle.
                    const auto numNewTriangles = numPairsWithHeightWithNodeAsLCA * numUnprocessedDescendantsWithHeight * numTripletPermutations;
                    assert(numNewTriangles >= 0);
                    numTriangles += numNewTriangles;
                }

                // These numPairsWithHeightWithNodeAsLCA would, when combined with a non-ancestor of currentNode that isSuitable and is
                // (descendantHeight - currentNode->height) distance away from currentNode, form a "Type A" triangle.
                // We store numPairsWithHeightWithNodeAsLCA for this descendantHeight inside currentNode: the required non-ancestors of
                // currentNode will be found by completeTrianglesOfTypeA() later on.
                numPairsWithHeightWithNodeAsLCA += numUnprocessedDescendantsWithHeight * numKnownDescendantsWithHeight;

                if (numberDescendantsWithThisHeight == 0)
                {
                    // This hasn't been updated yet, so has missed the numKnownDescendantsWithHeight; incorporate it now.
                    numberDescendantsWithThisHeight += numKnownDescendantsWithHeight;
                }
                numberDescendantsWithThisHeight += numUnprocessedDescendantsWithHeight;

            }

            heightInfoForNode.numWithHeight += transientHeightInfo.numWithHeight;
            heightInfoForNode.lastUpdatedAtNode = currentNode;
        }
    }

    if (currentNode->isSuitable)
    {
        persistentInfoForDescendantHeight[currentNode->height].numWithHeight++;
        persistentInfoForDescendantHeight[currentNode->height].lastUpdatedAtNode = currentNode;
    }

    return persistentInfoForDescendantHeight;
}

int64_t findNumTriplets(vector<Node>& nodes)
{
    int64_t numTriplets = 0;

    auto rootNode = &(nodes.front());

    // Fills in numPairsWithHeightWithNodeAsLCA for each node, and 
    // additionally counts all "Type B" triangles and adds them to results.
    buildDescendantHeightInfo(rootNode, nullptr, 0, numTriplets);

    // Finishes off the computation of the number of "Type A" triangles
    // that we began in buildDescendantHeightInfo.
    completeTrianglesOfTypeACentroidDecomposition(nodes, rootNode, numTriplets);

    return numTriplets;
}

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    const auto numTestcases = read<int>();

    for (auto t = 0; t < numTestcases; t++)
    {
        const auto numNodes = read<int>();
        assert(1 <= numNodes && numNodes <= 200'000);

        vector<Node> nodes(numNodes);

        for (auto i = 0; i < numNodes - 1; i++)
        {
            const auto u = read<int>();
            const auto v = read<int>();

            assert(1 <= u && u <= numNodes);
            assert(1 <= v && v <= numNodes);

            nodes[u - 1].neighbours.push_back(&(nodes[v - 1]));
            nodes[v - 1].neighbours.push_back(&(nodes[u - 1]));
        }
        for (auto i = 0; i < numNodes; i++)
        {
            const auto isSuitable = read<int>();

            assert(isSuitable == 0 || isSuitable == 1);

            nodes[i].isSuitable = (isSuitable == 1);
        }

        const auto numTriplets = findNumTriplets(nodes);
        cout << numTriplets << endl;
    }

    assert(cin);
}
