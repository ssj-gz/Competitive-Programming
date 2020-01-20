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

struct Node
{
    vector<Node*> children;

    bool isSuitable = false;
    int numDescendants = -1;
    Node* parentNode = nullptr;
    int height = 0;

    map<int, int64_t> numPairsWithHeightViaDifferentChildren;
    // Not strictly correct: if numPairsWithHeightViaDifferentChildren > 0, then this
    // will indeed by the number of descendents of this Node which have height
    // numWithHeight[height], but if numPairsWithHeightViaDifferentChildren == 0, this
    // will also be 0.
    // 
    // This is used to avoid the overcount when performing completeTrianglesOfTypeA
    // (Centroid Decomposition has no notion of our parent-child relationship, so
    // will count "children" of a Node alongside "non-children" of a Node.
    // TODO - explain this better.
    map<int, int64_t> numWithHeight;

    vector<Node*> lightChildren;

    vector<Node*> neighbours; // TODO - remove this.
    int id = -1; // TODO - remove this.
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

void completeTrianglesOfTypeA(const vector<Node>& nodes, Node* rootNode, int64_t& numTriangles)
{
    vector<vector<Node*>> heavyChains;
    doHeavyLightDecomposition(rootNode, false, heavyChains);

    DistTracker distTracker(nodes.size());
    auto collectDists = [&distTracker](Node* node, int depth)
    {
        if (node->isSuitable)
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
                    // Once only (first pass chosen arbitrarily) - add this node's dist
                    // (if isSuitable) so that it gets propagated to light descendants ...
                    if (node->isSuitable)
                        distTracker.insertDist(0);
                    // ... and also (partially) complete its Type A Triangles.
                    completeTypeATrianglesForNode(node);
                }

                for (auto lightChild : node->lightChildren)
                {
                    // Propagate all dists of nodes which isSuitable found so far along the chain in this direction
                    // to light descendants ...
                    doDfs(lightChild, 1, distTracker, AdjustWithDepth, propagateDists);
                    // ... and collect from light descendants.
                    doDfs(lightChild, 1, distTracker, DoNotAdjust, collectDists);
                }

                if (pass == 2)
                {
                    // In pass 1, we ensured that this node's dist (if isSuitable) was propagated
                    // to its light descendants.  Don't do it this time - wait until
                    // we've processed this node's light descendants before adding this
                    // node to the distTracker!
                    if (node->isSuitable)
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

void dfsSlow(Node* currentNode, Node* parentNode, int depth, Node* rootNode, int64_t& numTriangles)
{
    if (currentNode == nullptr)
        return;
    if (currentNode->isSuitable)
    {
        const int requiredRootDescendantHeight = rootNode->height + depth;
        numTriangles += rootNode->numPairsWithHeightViaDifferentChildren[requiredRootDescendantHeight] * numTripletPermutations;
    }

    for (auto neighbour : currentNode->neighbours)
    {
        if (neighbour == parentNode)
            continue;
        dfsSlow(neighbour, currentNode, depth + 1, rootNode, numTriangles);
    }
    
}

void countNumWithHeight(Node* currentNode, int height, map<int, int64_t>& numWithHeight)
{
    if (currentNode->isSuitable)
        numWithHeight[height]++;

    for (auto child : currentNode->children)
        countNumWithHeight(child, height + 1, numWithHeight);
}

void completeTrianglesOfTypeASlow(vector<Node>& nodes, Node* rootNode, int64_t& numTriangles)
{
#if 0
    for (auto& node : nodes)
    {
        dfsSlow(node.parentNode, &node, 1, &node, numTriangles);
    }
#else
    for (auto& node : nodes)
    {
        dfsSlow(&node, nullptr, 0, &node, numTriangles);
    }
#endif
    // Fix the overcount caused by Centroid Decomposition (over-)counting descendents of a node as non-descendents
    // of a node!
    for (auto& node : nodes)
    {
        map<int, int64_t> numWithHeight;
        countNumWithHeight(&node, node.height, numWithHeight);

        for (const auto blah : node.numPairsWithHeightViaDifferentChildren)
        {
            const int height = blah.first;
            const int64_t pairs = blah.second;

            if (blah.second == 0)
                continue;

            //cout << "Blah Node: " << node.id << "  height: " << height << " node.numWithHeight: " << node.numWithHeight[height] << " debug: " << numWithHeight[height] << endl;
            assert(node.numWithHeight[height] == numWithHeight[height]);
            numTriangles -= pairs * node.numWithHeight[height] * numTripletPermutations;
        }
    }
}

template <typename NodeProcessor>
void doDfsNew(Node* node, Node* parentNode, int depth, DistTracker& distTracker, DistTrackerAdjustment distTrackerAdjustment, NodeProcessor& processNode)
{
    if (distTrackerAdjustment == AdjustWithDepth)
        distTracker.adjustAllDists(1);

    processNode(node, depth, distTracker);

    for (auto child : node->neighbours)
    {
        if (child == parentNode)
            continue;
        doDfsNew(child, node, depth + 1, distTracker, distTrackerAdjustment, processNode);
    }

    if (distTrackerAdjustment == AdjustWithDepth)
        distTracker.adjustAllDists(-1);
}


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

void doCentroidDecomposition(Node* startNode, int64_t& numTriangles)
{
    Node* centroid = findCentroid(startNode);

    auto propagateDists = [](Node* node, int depth, DistTracker& distTracker)
                        {
                            // TODO - process node.
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
            doDfsNew(child, centroid, 1, distTracker, AdjustWithDepth, propagateDists );
            doDfsNew(child, centroid, 1, distTracker, DoNotAdjust, collectDists );
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
            doDfsNew(child, centroid, 1, distTracker, AdjustWithDepth, propagateDists );
            doDfsNew(child, centroid, 1, distTracker, DoNotAdjust, collectDists );
        }
        // TODO - do something with the Centroid.
        //centroid->grundyNumberIfRoot ^= distTracker.grundyNumber();
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
    // Fix the overcount caused by Centroid Decomposition (over-)counting descendents of a node as non-descendents
    // of a node!
    for (auto& node : nodes)
    {
        map<int, int64_t> numWithHeight;
        countNumWithHeight(&node, node.height, numWithHeight);

        for (const auto blah : node.numPairsWithHeightViaDifferentChildren)
        {
            const int height = blah.first;
            const int64_t pairs = blah.second;

            if (blah.second == 0)
                continue;

            //cout << "Blah Node: " << node.id << "  height: " << height << " node.numWithHeight: " << node.numWithHeight[height] << " debug: " << numWithHeight[height] << endl;
            assert(node.numWithHeight[height] == numWithHeight[height]);
            numTriangles -= pairs * node.numWithHeight[height] * numTripletPermutations;
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
            // child has a descendant of descendantHeight that isSuitable and a previous child of this
            // node also has a descendant of descendantHeight that isSuitable, but may also
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

                // These numPairsWithHeightViaDifferentChildren would, when combined with a non-ancestor of currentNode that isSuitable and is
                // (descendantHeight - currentNode->height) distance away from currentNode, form a "Type A" triangle.
                // We store numPairsWithHeightViaDifferentChildren for this descendantHeight inside currentNode: the required non-ancestors of
                // currentNode will be found by completeTrianglesOfTypeA() later on.
                numPairsWithHeightViaDifferentChildren += numUnprocessedDescendantsWithHeight * numKnownDescendantsWithHeight;
                //cout << "Node: " << currentNode->id << " Height: " << descendantHeight << " numKnownDescendantsWithHeight: " << numKnownDescendantsWithHeight << " numUnprocessedDescendantsWithHeight: " << numUnprocessedDescendantsWithHeight << endl;
                if (currentNode->numWithHeight[descendantHeight] == 0)
                {
                    // This hasn't been updated yet, so has missed the numKnownDescendantsWithHeight.
                    currentNode->numWithHeight[descendantHeight] += numKnownDescendantsWithHeight;
                }
                currentNode->numWithHeight[descendantHeight] += numUnprocessedDescendantsWithHeight;

            }

            heightInfoForNode.numWithHeight += transientHeightInfo.numWithHeight;
            heightInfoForNode.lastUpdatedAtNode = currentNode;
        }
    }

    if (currentNode->isSuitable)
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


    int64_t debugResult = result;

    // Finishes off the computation of the number of "Type A" triangles
    // that we began in buildDescendantHeightInfo.
    completeTrianglesOfTypeA(nodes, rootNode, result);

    completeTrianglesOfTypeASlow(nodes, rootNode, debugResult);
    cout << "result: " << result << " debugResult: " << debugResult << endl;
    assert(debugResult == result);

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
        for (int i = 0; i < numNodes; i++)
        {
            nodes[i].id = (i + 1);
        }

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
