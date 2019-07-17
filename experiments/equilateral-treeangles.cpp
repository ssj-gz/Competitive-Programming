//#define BRUTE_FORCE
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>

#include <cassert>

#include <sys/time.h>

#include <chrono>

#define SLOW_ANCESTOR_COUNT


using namespace std;

const int numTripletPermutations = 6;

constexpr auto maxHeight = 100'000;
constexpr int log2(int N, int exponent = 0, int powerOf2 = 1)
{
    return (powerOf2 >= N) ? exponent : log2(N, exponent + 1, powerOf2 * 2);
}
constexpr auto log2MaxHeight = log2(maxHeight);




class ModNum
{
    public:
        ModNum(int64_t n = 0)
            : m_n{n}
        {
        }
        ModNum& operator+=(const ModNum& other)
        {
            m_n = (m_n + other.m_n) % modulus;
            return *this;
        }
        ModNum& operator-=(const ModNum& other)
        {
            m_n += modulus;
            assert(m_n >= other.m_n);
            m_n = (m_n - other.m_n) % modulus;
            return *this;
        }
        ModNum& operator*=(const ModNum& other)
        {
            m_n = (m_n * other.m_n) % modulus;
            return *this;
        }
        ModNum operator++(int)
        {
            m_n += 1;
            return *this;
        }
        int64_t value() const { return m_n; };

        static const int64_t modulus = 1'000'000'007ULL;
    private:
        int64_t m_n;
};

ModNum operator+(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result += rhs;
    return result;
}
ModNum operator-(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result -= rhs;
    return result;
}

ModNum operator*(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result *= rhs;
    return result;
}

ostream& operator<<(ostream& os, const ModNum& toPrint)
{
    os << toPrint.value();
    return os;
}

bool operator==(const ModNum& lhs, const ModNum& rhs)
{
    return lhs.value() == rhs.value();
}


ModNum quickPower(long n, int m)
{
    // Raise n to the m mod modulus using as few multiplications as 
    // we can e.g. n ^ 8 ==  (((n^2)^2)^2).
    ModNum result = 1;
    int power = 0;
    while (m > 0)
    {
        if (m & 1)
        {
            ModNum subResult = n;
            for (int i = 0; i < power; i++)
            {
                subResult *= subResult;
            }
            result *= subResult;
        }
        m >>= 1;
        power++;
    }
    return result;
}

const ModNum inverseOf6 = quickPower(6, ModNum::modulus - 2);


struct Node
{
    vector<Node*> neighbours;
    int id = -1; // 1-relative - handy for debugging.
    bool hasPerson = false;
    int numDescendants = -1;
    Node* parentNode = nullptr;
    int height = 0;

    map<int, int64_t> numPairsWithHeightViaDifferentChildren;

    vector<Node*> lightChildren;
    vector<Node*> children;

    int index = -1; // 0-relative.

    bool visitedInBruteForceDFS = false;
    int dbgHeightInOptimisedDFS = -1; // TODO - remove
    std::array<Node*, log2MaxHeight + 1> ancestorPowerOf2Above;
};

void fillInDFSInfo(Node* node, Node* parent, vector<Node*>& ancestors)
{
    int powerOf2 = 1;
    for (int exponent = 0; exponent < log2MaxHeight; exponent++)
    {
        if (ancestors.size() >= powerOf2)
        {
            node->ancestorPowerOf2Above[exponent] = ancestors[ancestors.size() - powerOf2];
        }
        else
        {
            node->ancestorPowerOf2Above[exponent] = nullptr;
        }
        powerOf2 *= 2;
    }

    ancestors.push_back(node);
    for (auto neighbour : node->neighbours)
    {
        if (neighbour == parent)
            continue;
        fillInDFSInfo(neighbour, node, ancestors);
    }
    ancestors.pop_back();
}


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
    node->children = node->neighbours;

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

void dbgCountHeights(Node* currentNode, Node* parentNode, int height, vector<int>& dbgNumAncestorsWithHeight)
{
    assert(height >= 0 && height < dbgNumAncestorsWithHeight.size());
    if (currentNode->hasPerson)
        dbgNumAncestorsWithHeight[height]++;
    //cout << "dbgCountHeights currentNode: " << (currentNode->id) << endl;
    for (auto child : currentNode->neighbours)
    {
        //cout << "wee" << endl;
        if (child == parentNode)
            continue;
        //cout << " child: " << child->id << endl;

        dbgCountHeights(child, currentNode, height + 1, dbgNumAncestorsWithHeight);
    }
}

void completeTrianglesOfTypeA(vector<Node>& nodes, int64_t& numTriangles)
{
#if 0
    cout << "completeTrianglesOfTypeA" << endl;
    for (auto& node : nodes)
    {
        for (const auto& heightPair : node.numPairsWithHeightViaDifferentChildren)
        {
            if(node.parentNode)
            {
                const int descendentHeight = heightPair.first;
                const int64_t numPairsWithHeightViaDifferentChildren = heightPair.second;
                cout << "node: " << node.id << " descendentHeight: " << descendentHeight << " node height: " << node.height << " numPairsWithHeightViaDifferentChildren: " << numPairsWithHeightViaDifferentChildren << endl;

                vector<int> dbgNumDescendantsWithHeight(nodes.size(), 0);
                const int requiredNonDescendantDist = (descendentHeight - node.height);
                dbgCountHeights(node.parentNode, &node, 1, dbgNumDescendantsWithHeight);
                const int numNewTriangles = numPairsWithHeightViaDifferentChildren * dbgNumDescendantsWithHeight[requiredNonDescendantDist];
                //cout << " currentNode: " << currentNode->id << " num non-ancestors with dist: " << requiredNonDescendantDist << ": " << dbgNumDescendantsWithHeight[requiredNonDescendantDist] << endl;
                if (numNewTriangles > 0)
                {
                    //cout << " found double: adding: " << numNewTriangles << endl;
                }
                numTriangles += numNewTriangles * numTripletPermutations;
            }
            else
            {
                cout << " node: " << node.id << " has no parent" << endl;
            }
        }
    }
#else
    HeightTracker heightTracker(nodes.size());
    auto collectHeights = [&heightTracker](Node* node, int depth)
    {
        if (node->hasPerson)
            heightTracker.insertHeight(depth);
    };
    auto processNode = [&heightTracker, &numTriangles](Node* node)
    {
        //cout << " processNode node: " << node->id << endl;
        for (const auto& heightPair : node->numPairsWithHeightViaDifferentChildren)
        {
            const int descendentHeight = heightPair.first;
            const int64_t numPairsWithHeightViaDifferentChildren = heightPair.second;
            if (descendentHeight > node->height)
            {
                const int requiredNonDescendantDist = (descendentHeight - node->height);
                //cout << " node: " << node->id << " descendentHeight: " << descendentHeight << " node height: " << node->height << " requiredNonDescendantDist: " << requiredNonDescendantDist  << " num required dists: " << heightTracker.numWithHeight(requiredNonDescendantDist) << " numPairsWithHeightViaDifferentChildren: " << numPairsWithHeightViaDifferentChildren << endl;
                const int64_t numNewTriangles = numPairsWithHeightViaDifferentChildren * heightTracker.numWithHeight(requiredNonDescendantDist);
                numTriangles += numNewTriangles * numTripletPermutations;
            }
        }
    };
    auto propagateHeights = [&heightTracker, &processNode](Node* node, int depth)
    {
        processNode(node);
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
                    processNode(node);
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
#endif
}

void distDFS(const int rootNodeIndex, const Node* currentNode, const Node* parentNode, int height, vector<vector<int>>& distanceBetweenNodes)
{
    distanceBetweenNodes[rootNodeIndex][currentNode->index] = height;
    for (auto childNode : currentNode->neighbours)
    {
        if (childNode == parentNode)
            continue;

        distDFS(rootNodeIndex, childNode, currentNode, height + 1, distanceBetweenNodes);
    }
}

int64_t solveBruteForce(const vector<Node>& nodes)
{
    cout << "solutionBruteForce: Computing distance lookup table" << endl;
    int64_t result = 0;

    const int numNodes = nodes.size();
    vector<vector<int>> distanceBetweenNodes(numNodes, vector<int>(numNodes, -1));
    for (const auto& rootNode : nodes)
    {
        distDFS(rootNode.index, &rootNode, nullptr, 0, distanceBetweenNodes);
    }
    for (int i = 0; i < numNodes; i++)
    {
        for (int j = 0; j < numNodes; j++)
        {
            assert(distanceBetweenNodes[i][j] != -1);
            assert(distanceBetweenNodes[i][j] == distanceBetweenNodes[j][i]);
            assert(distanceBetweenNodes[i][j] < numNodes);
        }
    }
    cout << "Done computing distance lookup table" << endl;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point lastReportedTime = std::chrono::steady_clock::now();

    int numNodesProcessed = 0;
    const int totalNodesToProcess = count_if(nodes.begin(), nodes.end(), [](const Node& node) { return node.hasPerson ;});
    for (const auto& node : nodes)
    {
        if (!node.hasPerson)
            continue;

        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastReportedTime).count() == 2)
        {
            lastReportedTime = now;
            const int totalSecondsElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - begin).count();
            cout << "Processed " << numNodesProcessed << " out of " << totalNodesToProcess << " in " << totalSecondsElapsed << " seconds; estimated seconds remaining: " << static_cast<float>(totalSecondsElapsed) / numNodesProcessed * (totalNodesToProcess - numNodesProcessed)  << endl; 

        }

        vector<vector<const Node*>> nodesAtDistance(numNodes);
        for (int i = 0; i < numNodes; i++)
        {
            nodesAtDistance[distanceBetweenNodes[node.index][i]].push_back(&(nodes[i]));
        }
        for (int distance = 1; distance < numNodes; distance++)
        {
            const auto& nd = nodesAtDistance[distance];

            for (const auto node1 : nd)
            {
                if (!node1->hasPerson)
                    continue;
                for (const auto node2 : nd)
                {
                    if (!node2->hasPerson)
                        continue;
                    assert(distanceBetweenNodes[node.index][node1->index] == distance);
                    assert(distanceBetweenNodes[node.index][node2->index] == distance);

                    if (distanceBetweenNodes[node1->index][node2->index] == distance)
                    {
                        //cout << " Found  triple: " << node.id << ", " << node1->id << ", " << node2->id << " (distance: " << distance << ")" << endl;
                        result++;
                    }
                }
            }
        }
        numNodesProcessed++;
    }

    return result;
}


Node* findKthAncestor(Node* node, int k)
{
    const auto originalHeight = node->height;
    const auto originalK = k;
    Node* ancestor = node;
    for (int exponent = log2MaxHeight - 1; exponent >= 0; exponent--)
    {
        const auto powerOf2 = (1 << exponent);
        if (k >= powerOf2) 
        {
            assert(ancestor->ancestorPowerOf2Above[exponent] && ancestor->ancestorPowerOf2Above[exponent]->height == ancestor->height - powerOf2);
            ancestor = ancestor->ancestorPowerOf2Above[exponent];
            k -= powerOf2;
        }
    }
    assert(ancestor);
    assert(ancestor->height == originalHeight - originalK);
    return ancestor;
}

Node* findLCA(Node* node1, Node* node2)
{
    // Equalise node heights.
    if (node1->height != node2->height)
    {
        if (node1->height > node2->height)
        {
            node1 = findKthAncestor(node1, node1->height - node2->height);
        }
        else
        {
            node2 = findKthAncestor(node2, node2->height - node1->height);
        }
    }
    assert(node1 && node2);
    assert(node1->height == node2->height);

    // Use a binary search to find the common ancestor: since the 
    // nodes always have the same height, if we jump both nodes up by x places
    // and they are still not equal, then the lca is still above them: if they
    // do become equal, the the lca is at or below them.
    int currentNodesHeight = node1->height;
    int minLCAHeight = 0;
    int maxLCAHeight = currentNodesHeight;
    bool found = false;
    while (true)
    {
        const int heightDecrease = (currentNodesHeight - minLCAHeight) / 2;
        assert(node1->height == node2->height);
        assert(node1->height == currentNodesHeight); 
        if (node1 != node2 && node1->parentNode == node2->parentNode && node1->parentNode)
            return node1->parentNode;

        if (heightDecrease == 0)
        {
            assert(node1 == node2);
            return node1;;
        }
        const int nodesAncestorHeight = currentNodesHeight - heightDecrease;
        auto node1Ancestor = findKthAncestor(node1, heightDecrease);
        auto node2Ancestor = findKthAncestor(node2, heightDecrease);
        assert(node1Ancestor && node2Ancestor);

        if (node1Ancestor == node2Ancestor)
        {
            minLCAHeight = nodesAncestorHeight;
        }
        else
        {
            currentNodesHeight = nodesAncestorHeight - 1;
            maxLCAHeight = currentNodesHeight;
            node1 = node1Ancestor->parentNode;
            node2 = node2Ancestor->parentNode;
        }
    }
    assert(false && "Shouldn't be able to reach here!");
    return nullptr;
}


int64_t solveBruteForce2(vector<Node>& nodes)
{
    int64_t result = 0;

    int numNodesProcessed = 0;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point lastReportedTime = std::chrono::steady_clock::now();
    const int totalNodesToProcess = count_if(nodes.begin(), nodes.end(), [](const Node& node) { return node.hasPerson ;});
    for (auto& node : nodes)
    {
        if (!node.hasPerson)
            continue;

        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastReportedTime).count() == 2)
        {
            lastReportedTime = now;
            const int totalSecondsElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - begin).count();
            cout << "Processed " << numNodesProcessed << " out of " << totalNodesToProcess << " in " << totalSecondsElapsed << " seconds; estimated seconds remaining: " << static_cast<float>(totalSecondsElapsed) / numNodesProcessed * (totalNodesToProcess - numNodesProcessed)  << endl; 

        }

        for (auto& otherNode : nodes)
        {
            otherNode.visitedInBruteForceDFS = false;
        }
        int distance = 0;
        vector<Node*> nodesToExplore = { &node };
        node.visitedInBruteForceDFS = true;

        while (!nodesToExplore.empty())
        {
            if (distance != 0)
            {
                for (const auto node1 : nodesToExplore)
                {
                    assert(node1 != &node);
                    if (!node1->hasPerson)
                        continue;
                    for (const auto node2 : nodesToExplore)
                    {
                        if (!node2->hasPerson)
                            continue;

                        Node* lca = findLCA(node1, node2);
                        assert(lca);
                        const int distanceBetweenNodes = (node2->height - lca->height) + (node1->height - lca->height);

                        if (distanceBetweenNodes == distance)
                        {
                            //cout << " Found  triple: " << node.id << ", " << node1->id << ", " << node2->id << " (distance: " << distance << ")" << endl;
                            result++;
                        }
                    }
                }
            }

            vector<Node*> nextNodesToExplore;
            for (const auto node1 : nodesToExplore)
            {
                for (auto neighbour : node1->neighbours)
                {
                    if (!neighbour->visitedInBruteForceDFS)
                    {
                        neighbour->visitedInBruteForceDFS = true;
                        nextNodesToExplore.push_back(neighbour);
                    }
                }
            }

            nodesToExplore = nextNodesToExplore;
            distance++;
        }
        numNodesProcessed++;
    }

    return result;
}


int numNodes = 0;

int coreIterations = 0;
int numNodesFinished = 0;

int dbgFindNumNonDescendentsWithHeight(Node* currentNode, Node* parent, int height, const int desiredHeight)
{
    if (currentNode == nullptr)
        return 0;
    if (height > desiredHeight)
        return 0;
    if (currentNode->hasPerson && height == desiredHeight)
        return 1;

    int result = 0;
    for (auto neighbour : currentNode->neighbours)
    {
        if (neighbour == parent)
            continue;

        result += dbgFindNumNonDescendentsWithHeight(neighbour, currentNode, height + 1, desiredHeight);
    }

    return result;
}
 
map<int, HeightInfo> solveOptimisedAux(Node* currentNode, Node* parentNode, int height, int64_t& numTriangles)
{
    //cout << " # neighbours: " << currentNode->neighbours.size() << endl;
    assert(currentNode->dbgHeightInOptimisedDFS == -1);
    currentNode->dbgHeightInOptimisedDFS = height;
    map<int, HeightInfo> infoForDescendentHeight;

    for (auto child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;

        auto infoForChildDescendentHeight = solveOptimisedAux(child, currentNode, height + 1, numTriangles);
        if (infoForChildDescendentHeight.size() > infoForDescendentHeight.size())
        {
            swap(infoForDescendentHeight, infoForChildDescendentHeight);
        }

        for (auto descendentHeightPair : infoForChildDescendentHeight)
        {
            coreIterations++;
            const int descendentHeight = descendentHeightPair.first;

            const auto& heightInfo = descendentHeightPair.second;
            auto& otherHeightInfo = infoForDescendentHeight[descendentHeight];

            assert (descendentHeight > height);

            //cout << " solveOptimisedAux currentNode: " << currentNode->id << " descendentHeight: " << descendentHeight << " heightInfo.numWithHeight: " << heightInfo.numWithHeight << " otherHeightInfo.numWithHeight: " << otherHeightInfo.numWithHeight << " after child: " << child->id << " numPairsWithHeightViaDifferentChildren:" << endl;
            // Triplets with currentNode as LCA of all pairs out of the three nodes.
            int newExtraDescendentHeight = -1;
            int knownDescendtHeight = -1;
            if (heightInfo.lastUpdatedAtNode == currentNode || heightInfo.lastUpdatedAtNode == nullptr)
            {
                assert(otherHeightInfo.lastUpdatedAtNode != currentNode);
                newExtraDescendentHeight = otherHeightInfo.numWithHeight;
                knownDescendtHeight = heightInfo.numWithHeight;

                otherHeightInfo.numWithHeight = heightInfo.numWithHeight;
            }
            else
            {
                assert(heightInfo.lastUpdatedAtNode != currentNode);
                //cout << " currentNode: " << currentNode->id << " otherHeightInfo.lastUpdatedAtNode: " << (otherHeightInfo.lastUpdatedAtNode == nullptr ? -1 : otherHeightInfo.lastUpdatedAtNode->id) << endl;
                newExtraDescendentHeight = heightInfo.numWithHeight;
                knownDescendtHeight = otherHeightInfo.numWithHeight;
            }

            const bool previousChildHasThisHeight = (knownDescendtHeight > 0);
            if (previousChildHasThisHeight)
            {
                int64_t& numPairsWithHeightViaDifferentChildren = currentNode->numPairsWithHeightViaDifferentChildren[descendentHeight];

                if (newExtraDescendentHeight * numPairsWithHeightViaDifferentChildren > 0)
                {
                    // Found a triple where all three nodes have currentNode as their LCA.
                    numTriangles += newExtraDescendentHeight * numPairsWithHeightViaDifferentChildren * numTripletPermutations;
                }

#ifdef SLOW_ANCESTOR_COUNT
                numTriangles += static_cast<int64_t>(knownDescendtHeight) * newExtraDescendentHeight * dbgFindNumNonDescendentsWithHeight(parentNode, currentNode, 1, (descendentHeight - currentNode->height)) * numTripletPermutations;
#endif
                numPairsWithHeightViaDifferentChildren += newExtraDescendentHeight * knownDescendtHeight;

            }

            otherHeightInfo.numWithHeight += newExtraDescendentHeight;
            otherHeightInfo.lastUpdatedAtNode = currentNode;
        }
    }

    if (currentNode->hasPerson)
    {
        infoForDescendentHeight[height].numWithHeight++;
        infoForDescendentHeight[height].lastUpdatedAtNode = currentNode;
    }

    numNodesFinished++;
    return infoForDescendentHeight;
}

unique_ptr<map<int, HeightInfo>> blah(Node* currentNode, Node* parentNode, int height, int64_t& numTriangles)
{
    unique_ptr<map<int, HeightInfo>> infoForDescendentHeight = make_unique<map<int, HeightInfo>>();

    for (auto child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;

        unique_ptr<map<int, HeightInfo>> infoForChildDescendentHeight = blah(child, currentNode, height + 1, numTriangles);
        if (infoForChildDescendentHeight->size() > infoForDescendentHeight->size())
        {
            swap(infoForDescendentHeight, infoForChildDescendentHeight);
        }

        for (auto descendentHeightPair : *infoForChildDescendentHeight)
        {
            coreIterations++;
            const int descendentHeight = descendentHeightPair.first;

            const auto& heightInfo = descendentHeightPair.second;
            auto& otherHeightInfo = (*infoForDescendentHeight)[descendentHeight];

            //assert (descendentHeight > height);

            // Triplets with currentNode as LCA of all pairs out of the three nodes.
            int newExtraDescendentHeight = -1;
            int knownDescendtHeight = -1;
            if (heightInfo.lastUpdatedAtNode == currentNode || heightInfo.lastUpdatedAtNode == nullptr)
            {
                assert(otherHeightInfo.lastUpdatedAtNode != currentNode);
                newExtraDescendentHeight = otherHeightInfo.numWithHeight;
                knownDescendtHeight = heightInfo.numWithHeight;

                otherHeightInfo.numWithHeight = heightInfo.numWithHeight;
            }
            else
            {
                assert(heightInfo.lastUpdatedAtNode != currentNode);
                newExtraDescendentHeight = heightInfo.numWithHeight;
                knownDescendtHeight = otherHeightInfo.numWithHeight;
            }


            otherHeightInfo.numWithHeight += newExtraDescendentHeight;
            otherHeightInfo.lastUpdatedAtNode = currentNode;
        }
    }

    if (currentNode->hasPerson)
    {
        (*infoForDescendentHeight)[currentNode->id].numWithHeight++;
        (*infoForDescendentHeight)[currentNode->id].lastUpdatedAtNode = currentNode;
    }

    numNodesFinished++;
    cout << " blah finished node: " << currentNode->id  << " " << numNodesFinished << " / " << numNodes << " coreIterations: " << coreIterations << " infoForDescendentHeight.size: " << infoForDescendentHeight->size() << endl;
    return infoForDescendentHeight;
}

int64_t solveOptimised(vector<Node>& nodes)
{
    int64_t result = 0;

    const int numNodes = nodes.size();
    ::numNodes = numNodes;

    Node* rootNode = &(nodes.front());
    doHeavyLightDecomposition(rootNode, false);
    solveOptimisedAux(rootNode, nullptr, 0, result);
    //blah(rootNode, nullptr, 0, result);

#ifndef SLOW_ANCESTOR_COUNT
    completeTrianglesOfTypeA(nodes, result);
#endif

    return result;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    if (argc == 2)
    {
        if (string(argv[1]) == "--test")
        {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int numNodes = 1 + rand() % 1000;
        cout << numNodes << endl;

        for (int i = 0; i < numNodes - 1; i++)
        {
            cout << (i + 2) << " " << ((rand() % (i + 1) + 1)) << endl;
        }
        for (int i = 0; i < numNodes; i++)
        {
            cout << (rand() % 2) << endl;
        }
        return 0;
        }
        else if (string(argv[1]) == "--convert")
        {
            int n;
            cin >> n;
            for (int i = 0; i < n; i++)
            {
                int numCoins;
                cin >> numCoins;
            }
            cout << n << endl;
            for (int i = 0; i < n - 1; i++)
            {
                int u, v;
                cin >> u >> v;
                cout << u << " " << v << endl;
            }
            for (int i = 0; i < n; i++)
            {
                cout << (rand() % 2) << endl;
            }
            return 0;
        }
        else
        {
            assert(false);
        }
    }
    int numNodes;
    cin >> numNodes;

    vector<Node> nodes(numNodes);

#if 0
    HeightTracker heightTracker(100);
    heightTracker.insertHeight(3);
    heightTracker.insertHeight(5);
    heightTracker.adjustAllHeights(10);
    heightTracker.insertHeight(7);
    cout << "blee: " << endl;
    cout << " " << heightTracker.numWithHeight(3) << endl;
    cout << " " << heightTracker.numWithHeight(5) << endl;
    cout << " " << heightTracker.numWithHeight(13) << endl;
    cout << " " << heightTracker.numWithHeight(15) << endl;
    cout << " " << heightTracker.numWithHeight(7) << endl;
#endif

    for (int i = 0; i < numNodes - 1; i++)
    {
        int u;
        cin >> u;
        int v;
        cin >> v;

        // Make 0-relative.
        u--;
        v--;

        nodes[u].neighbours.push_back(&(nodes[v]));
        nodes[v].neighbours.push_back(&(nodes[u]));
    }
    for (int i = 0; i < numNodes; i++)
    {
        int hasPerson;
        cin >> hasPerson;

        nodes[i].hasPerson = (hasPerson == 1);

        nodes[i].index = i;
        nodes[i].id = i + 1;
    }
    assert(cin);

    auto rootNode = &(nodes.front());
    fixParentChildAndCountDescendants(rootNode, nullptr, 0);
    vector<Node*> ancestors;
    fillInDFSInfo(rootNode, nullptr, ancestors);


#ifdef BRUTE_FORCE
    const auto solutionBruteForce = solveBruteForce(nodes);
    const auto solutionBruteForce2 = solveBruteForce2(nodes);
    const auto solutionOptimised = solveOptimised(nodes);
    //cout << "solutionBruteForce: " << solutionBruteForce << endl;
    //cout << "solutionBruteForce2: " << solutionBruteForce2 << endl;
    cout << "solutionOptimised: " << solutionOptimised << endl;
    //assert(solutionOptimised == solutionBruteForce);
    assert(solutionOptimised == solutionBruteForce2);
#else
    const auto solutionOptimised = solveOptimised(nodes);
    cout << solutionOptimised << endl;
#endif
}
