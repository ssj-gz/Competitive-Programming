#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#define NDEBUG
#include <cassert>

using namespace std;

const int numTripletPermutations = 6;

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

vector<vector<Node*>> heavyChains;

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

class HeightTracker
{
    public:
        HeightTracker(int maxHeight)
            : m_numWithHeight(2 * maxHeight + 1), 
            m_maxHeight(maxHeight)
    {
    }
        void insertHeight(const int newHeight)
        {
            numWithHeightValue(newHeight)++;
        };
        int numWithHeight(int height)
        {
            return numWithHeightValue(height);
        }
        void adjustAllHeights(int heightDiff)
        {
            m_cumulativeHeightAdjustment += heightDiff;
            assert(m_cumulativeHeightAdjustment >= 0);
        }
        void clear()
        {
            // Reset all height counts to 0 in O(1) by upping the 
            // m_versionNum.
            m_cumulativeHeightAdjustment = 0;
            m_versionNum++;
        }
    private:
        int m_cumulativeHeightAdjustment = 0;
        struct VersionedValue
        {
            int versionNum = -1;
            int value = -1;
        };
        vector<VersionedValue> m_numWithHeight;
        int m_maxHeight = -1;
        int m_versionNum = 0;

        int& numWithHeightValue(int height)
        {
            VersionedValue& versionedValue = m_numWithHeight[height - m_cumulativeHeightAdjustment + m_maxHeight];
            if (versionedValue.versionNum != m_versionNum)
            {
                versionedValue.value = 0;
                versionedValue.versionNum = m_versionNum;
            }

            return versionedValue.value;
        }
};

enum HeightTrackerAdjustment {DoNotAdjust, AdjustWithDepth};

template <typename NodeProcessor>
void doDfs(Node* node, int depth, HeightTracker& heightTracker, HeightTrackerAdjustment heightTrackerAdjustment, NodeProcessor& processNode)
{
    if (heightTrackerAdjustment == AdjustWithDepth)
        heightTracker.adjustAllHeights(1);

    processNode(node, depth);

    for (auto child : node->children)
        doDfs(child, depth + 1, heightTracker, heightTrackerAdjustment, processNode);

    if (heightTrackerAdjustment == AdjustWithDepth)
        heightTracker.adjustAllHeights(-1);
}


void completeTrianglesOfTypeA(vector<Node>& nodes, int64_t& numTriangles)
{
    HeightTracker heightTracker(nodes.size());
    auto collectHeights = [&heightTracker](Node* node, int depth)
    {
        if (node->hasPerson)
            heightTracker.insertHeight(depth);
    };
    auto completeTypeATrianglesForNode = [&heightTracker, &numTriangles](Node* node)
    {
        // This will actually be called O(log2 n) for each node before the node's
        // Type A Triangles are completed.
        for (const auto& heightPair : node->numPairsWithHeightViaDifferentChildren)
        {
            const int descendentHeight = heightPair.first;
            const int64_t numPairsWithHeightViaDifferentChildren = heightPair.second;
            if (descendentHeight > node->height)
            {
                const int requiredNonDescendantDist = (descendentHeight - node->height);
                const int64_t numNewTriangles = numPairsWithHeightViaDifferentChildren * heightTracker.numWithHeight(requiredNonDescendantDist);
                assert(numNewTriangles >= 0);
                numTriangles += numNewTriangles * numTripletPermutations;
            }
        }
    };
    auto propagateHeights = [&completeTypeATrianglesForNode](Node* node, int depth)
    {
        completeTypeATrianglesForNode(node);
    };
    for (auto& chain : heavyChains)
    {
        for (auto pass = 1; pass <= 2; pass++)
        {
            heightTracker.clear();
            // Crawl along chain, collecting from one node and propagating to the next.
            for (auto node : chain)
            {
                if (pass == 1 )
                {
                    // Once only (first pass chosen arbitrarily) - add this node's coin
                    // (if any) so that it gets propagated to light descendants ...
                    if (node->hasPerson)
                        heightTracker.insertHeight(0);
                    // ... and also complete its Type A Triangles.
                    completeTypeATrianglesForNode(node);
                }

                for (auto lightChild : node->lightChildren)
                {
                    // Propagate all coins found so far along the chain in this direction
                    // to light descendants ...
                    doDfs(lightChild, 1, heightTracker, AdjustWithDepth, propagateHeights);
                    // ... and collect from light descendants.
                    doDfs(lightChild, 1, heightTracker, DoNotAdjust, collectHeights);
                }

                if (pass == 2)
                {
                    // In pass 1, we ensured that this node's coin (if any) was propagated
                    // to its light descendants.  Don't do it this time - wait until
                    // we've processed this coin's light descendants before adding this
                    // coin's node to the heightTracker!
                    if (node->hasPerson)
                        heightTracker.insertHeight(0);
                }

                // Prepare for the reverse pass.
                reverse(node->lightChildren.begin(), node->lightChildren.end());
                // Move one node along the chain - increase all heights accordingly!
                heightTracker.adjustAllHeights(1);
            }
            // Now do it backwards.
            reverse(chain.begin(), chain.end());
        }
    }
}

int numNodes = 0;

map<int, HeightInfo> solveOptimisedAux(Node* currentNode, int64_t& numTriangles)
{
    map<int, HeightInfo> infoForDescendentHeight;

    for (auto child : currentNode->children)
    {
        auto infoForChildDescendentHeight = solveOptimisedAux(child, numTriangles);
        if (infoForChildDescendentHeight.size() > infoForDescendentHeight.size())
        {
            swap(infoForDescendentHeight, infoForChildDescendentHeight);
        }

        for (auto descendentHeightPair : infoForChildDescendentHeight)
        {
            const int descendentHeight = descendentHeightPair.first;

            const auto& heightInfo = descendentHeightPair.second;
            auto& otherHeightInfo = infoForDescendentHeight[descendentHeight];

            assert (descendentHeight > currentNode->height);

            int newExtraDescendentHeight = -1;
            int knownDescendantHeight = -1;
            if (heightInfo.lastUpdatedAtNode == currentNode || heightInfo.lastUpdatedAtNode == nullptr)
            {
                assert(otherHeightInfo.lastUpdatedAtNode != currentNode);
                newExtraDescendentHeight = otherHeightInfo.numWithHeight;
                knownDescendantHeight = heightInfo.numWithHeight;

                otherHeightInfo.numWithHeight = heightInfo.numWithHeight;
            }
            else
            {
                assert(heightInfo.lastUpdatedAtNode != currentNode);
                newExtraDescendentHeight = heightInfo.numWithHeight;
                knownDescendantHeight = otherHeightInfo.numWithHeight;
            }

            const bool previousChildHasThisHeight = (knownDescendantHeight > 0);
            if (previousChildHasThisHeight)
            {
                int64_t& numPairsWithHeightViaDifferentChildren = currentNode->numPairsWithHeightViaDifferentChildren[descendentHeight];

                if (newExtraDescendentHeight * numPairsWithHeightViaDifferentChildren > 0)
                {
                    // Found a triple where all three nodes have currentNode as their LCA.
                    const int64_t numNewTriangles = numPairsWithHeightViaDifferentChildren * newExtraDescendentHeight * numTripletPermutations;
                    assert(numNewTriangles >= 0);
                    numTriangles += numNewTriangles;
                }

                numPairsWithHeightViaDifferentChildren += newExtraDescendentHeight * knownDescendantHeight;

            }

            otherHeightInfo.numWithHeight += newExtraDescendentHeight;
            otherHeightInfo.lastUpdatedAtNode = currentNode;
        }
    }

    if (currentNode->hasPerson)
    {
        infoForDescendentHeight[currentNode->height].numWithHeight++;
        infoForDescendentHeight[currentNode->height].lastUpdatedAtNode = currentNode;
    }

    return infoForDescendentHeight;
}

int64_t solveOptimised(vector<Node>& nodes)
{
    int64_t result = 0;

    const int numNodes = nodes.size();
    ::numNodes = numNodes;

    Node* rootNode = &(nodes.front());
    doHeavyLightDecomposition(rootNode, false);
    solveOptimisedAux(rootNode, result);

    return result;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    int numNodes;
    cin >> numNodes;

    vector<Node> nodes(numNodes);

    for (int i = 0; i < numNodes - 1; i++)
    {
        int u;
        cin >> u;
        int v;
        cin >> v;

        // Make 0-relative.
        u--;
        v--;

        // More "neighbours" than "children", but we'll sort that out
        // in fixParentChildAndCountDescendants!
        nodes[u].children.push_back(&(nodes[v]));
        nodes[v].children.push_back(&(nodes[u]));
    }
    for (int i = 0; i < numNodes; i++)
    {
        int hasPerson;
        cin >> hasPerson;

        nodes[i].hasPerson = (hasPerson == 1);
    }
    assert(cin);

    auto rootNode = &(nodes.front());
    fixParentChildAndCountDescendants(rootNode, nullptr, 0);


    const auto solutionOptimised = solveOptimised(nodes);
    cout << solutionOptimised << endl;
}
