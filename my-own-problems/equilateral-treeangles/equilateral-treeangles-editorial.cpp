// Simon St James (ssjgz) - Editorial solution for "Equilateral Treeangles" - 2019-07-17.
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include <cassert>

using namespace std;

const int numTripletPermutations = 6; // i.e. == factorial(3).

struct Node
{
    vector<Node*> children;

    bool hasPerson = false;
    int numDescendants = -1;
    Node* parentNode = nullptr;
    int height = 0;

    map<int, int64_t> numPairsWithHeightViaDifferentChildren;

    vector<Node*> lightChildren;
};

struct HeightInfo
{
    int numWithHeight = 0;
    Node* lastUpdatedAtNode = nullptr;
};

int fixParentChildAndCountDescendants(Node* node, Node* parentNode, int height)
{
    node->numDescendants = 1;
    node->parentNode = parentNode;
    node->height = height;

    if (parentNode)
        node->children.erase(find(node->children.begin(), node->children.end(), parentNode));

    for (auto child : node->children)
        node->numDescendants += fixParentChildAndCountDescendants(child, node, height + 1);

    return node->numDescendants;
}

// Build up heavyChains.
void doHeavyLightDecomposition(Node* node, bool followedHeavyEdge, vector<vector<Node*>>& heavyChains)
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
        doHeavyLightDecomposition(heavyChild, true, heavyChains);
        for (auto child : node->children)
        {
            if (child != heavyChild)
                node->lightChildren.push_back(child);
        }

        for (auto lightChild : node->lightChildren)
        {
            doHeavyLightDecomposition(lightChild, false, heavyChains);
        }
    }
}

class DistTracker
{
    public:
        DistTracker(int maxDist)
            : m_numWithDist(2 * maxDist + 1), 
            m_maxDist(maxDist)
    {
    }
        void insertDist(const int newDist)
        {
            numWithDistValue(newDist)++;
        };
        int numWithDist(int dist)
        {
            return numWithDistValue(dist);
        }
        void adjustAllDists(int distDiff)
        {
            m_cumulativeDistAdjustment += distDiff;
            assert(m_cumulativeDistAdjustment >= 0);
        }
        void clear()
        {
            // Reset all dist counts to 0 in O(1) by upping the 
            // m_versionNum.
            m_cumulativeDistAdjustment = 0;
            m_versionNum++;
        }
    private:
        int m_cumulativeDistAdjustment = 0;
        struct VersionedValue
        {
            int versionNum = -1;
            int value = -1;
        };
        vector<VersionedValue> m_numWithDist;
        int m_maxDist = -1;
        int m_versionNum = 0;

        int& numWithDistValue(int dist)
        {
            VersionedValue& versionedValue = m_numWithDist[dist - m_cumulativeDistAdjustment + m_maxDist];
            if (versionedValue.versionNum != m_versionNum)
            {
                versionedValue.value = 0;
                versionedValue.versionNum = m_versionNum;
            }

            return versionedValue.value;
        }
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

void completeTrianglesOfTypeA(vector<Node>& nodes, Node* rootNode, int64_t& numTriangles)
{
    vector<vector<Node*>> heavyChains;
    doHeavyLightDecomposition(rootNode, false, heavyChains);

    DistTracker distTracker(nodes.size());
    auto collectDists = [&distTracker](Node* node, int depth)
    {
        if (node->hasPerson)
            distTracker.insertDist(depth);
    };
    auto completeTypeATrianglesForNode = [&distTracker, &numTriangles](Node* node)
    {
        // This will actually be called O(log2 n) for each node before the node's
        // Type A Triangles are fully completed.
        for (const auto& heightPair : node->numPairsWithHeightViaDifferentChildren)
        {
            const int descendantHeight = heightPair.first;
            const int64_t numPairsWithHeightViaDifferentChildren = heightPair.second;
            assert(descendantHeight > node->height);

            const int requiredNonDescendantDist = (descendantHeight - node->height);
            const int64_t numNewTriangles = numPairsWithHeightViaDifferentChildren * distTracker.numWithDist(requiredNonDescendantDist) * numTripletPermutations;
            assert(numNewTriangles >= 0);
            numTriangles += numNewTriangles;
        }
    };
    auto propagateDists = [&completeTypeATrianglesForNode](Node* node, int depth)
    {
        completeTypeATrianglesForNode(node);
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
                    if (node->hasPerson)
                        distTracker.insertDist(0);
                    // ... and also (partially) complete its Type A Triangles.
                    completeTypeATrianglesForNode(node);
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
                    if (node->hasPerson)
                        distTracker.insertDist(0);
                }

                // Prepare for the reverse pass.
                reverse(node->lightChildren.begin(), node->lightChildren.end());
                // Move one node along the chain - increase all heights accordingly!
                distTracker.adjustAllDists(1);
            }
            // Now do it backwards.
            reverse(chain.begin(), chain.end());
        }
    }
}

map<int, HeightInfo> buildDescendantHeightInfo(Node* currentNode, int64_t& numTriangles)
{
    map<int, HeightInfo> infoForDescendantHeight;

    for (auto child : currentNode->children)
    {
        // Quick C++ performance note: in C++11 onwards, capturing a returned std::map
        // in a local variable is O(1), due to Move Semantics.  Prior to this, though,
        // it could have been O(size of std::map), which would (silently!) lead to 
        // asymptotically worse performance!
        // Luckily, this code uses C++11 features so we can't accidentally fall into this trap.
        auto infoForChildDescendantHeight = buildDescendantHeightInfo(child, numTriangles);
        if (infoForChildDescendantHeight.size() > infoForDescendantHeight.size())
        {
            swap(infoForDescendantHeight, infoForChildDescendantHeight);
        }

        for (auto descendantHeightPair : infoForChildDescendantHeight)
        {
            // This block of code (i.e. the body of the containing for... loop) 
            // is executed O(n log2 n) times over the whole run.
            // It is guaranteed to be executed with descendantHeight if the current
            // child has a descendant of descendantHeight that hasPerson and a previous child of this
            // node also has a descendant of descendantHeight that hasPerson, but may also
            // be executed under different circumstances.
            const auto descendantHeight = descendantHeightPair.first;

            const auto& transientHeightInfo = descendantHeightPair.second;
            auto& heightInfoForNode = infoForDescendantHeight[descendantHeight];

            assert (descendantHeight > currentNode->height);

            int numUnprocessedDescendantsWithHeight = -1;
            int numKnownDescendantsWithHeight = -1;
            if (transientHeightInfo.lastUpdatedAtNode == currentNode || transientHeightInfo.lastUpdatedAtNode == nullptr)
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
                int64_t& numPairsWithHeightViaDifferentChildren = currentNode->numPairsWithHeightViaDifferentChildren[descendantHeight];

                if (numUnprocessedDescendantsWithHeight * numPairsWithHeightViaDifferentChildren > 0)
                {
                    // Found a triple where all three nodes have currentNode as their LCA: a "Type B" triangle.
                    const int64_t numNewTriangles = numPairsWithHeightViaDifferentChildren * numUnprocessedDescendantsWithHeight * numTripletPermutations;
                    assert(numNewTriangles >= 0);
                    numTriangles += numNewTriangles;
                }

                // These numPairsWithHeightViaDifferentChildren would, when combined with a non-ancestor of currentNode that hasPerson and is
                // (descendantHeight - currentNode->height) distance away from currentNode, form a "Type A" triangle.
                // We store numPairsWithHeightViaDifferentChildren for this descendantHeight inside currentNode: the required non-ancestors of
                // currentNode will be found by completeTrianglesOfTypeA() later on.
                numPairsWithHeightViaDifferentChildren += numUnprocessedDescendantsWithHeight * numKnownDescendantsWithHeight;

            }

            heightInfoForNode.numWithHeight += transientHeightInfo.numWithHeight;
            heightInfoForNode.lastUpdatedAtNode = currentNode;
        }
    }

    if (currentNode->hasPerson)
    {
        infoForDescendantHeight[currentNode->height].numWithHeight++;
        infoForDescendantHeight[currentNode->height].lastUpdatedAtNode = currentNode;
    }

    return infoForDescendantHeight;
}

int64_t findNumTriplets(vector<Node>& nodes)
{
    int64_t result = 0;

    auto rootNode = &(nodes.front());
    fixParentChildAndCountDescendants(rootNode, nullptr, 0);

    // Fills in numPairsWithHeightViaDifferentChildren for each node, and 
    // additionally counts all "Type B" triangles and adds them to results.
    buildDescendantHeightInfo(rootNode, result);

    // Finishes off the computation of the number of "Type A" triangles
    // that we began in buildDescendantHeightInfo.
    completeTrianglesOfTypeA(nodes, rootNode, result);

    return result;
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

        for (int i = 0; i < numNodes - 1; i++)
        {
            const auto u = read<int>();
            const auto v = read<int>();

            assert(1 <= u && u <= numNodes);
            assert(1 <= v && v <= numNodes);

            // More "neighbours" than "children" at the moment, but we'll sort that out
            // in fixParentChildAndCountDescendants!
            nodes[u - 1].children.push_back(&(nodes[v - 1]));
            nodes[v - 1].children.push_back(&(nodes[u - 1]));
        }
        for (auto i = 0; i < numNodes; i++)
        {
            const auto hasPerson = read<int>();

            assert(hasPerson == 0 || hasPerson == 1);

            nodes[i].hasPerson = (hasPerson == 1);
        }

        const auto numTriplets = findNumTriplets(nodes);
        cout << numTriplets << endl;
    }

    assert(cin);
}
