// Simon St James (ssjgz) - 2020-04-01
// 
// Editorial Solution for: https://www.codechef.com/problems/MVCN2TST
//
#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

#include <cassert>

using namespace std;

const int64_t Mod = 1'000'000'007;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}


struct Node
{
    vector<Node*> children;
    Node* parent = nullptr;
    int height = -1;
    int id = -1;
    int numDescendants = -1; // Includes the node itself.

    int dfsBeginVisit = -1;
    int dfsEndVisit = -1;
};

void computeDFSInfo(Node* node, int& dfsVisitNum, vector<vector<Node*>>& nodesAtHeightInDFSOrder)
{
    node->dfsBeginVisit = dfsVisitNum;
    nodesAtHeightInDFSOrder[node->height].push_back(node);
    dfsVisitNum++;

    for (auto child : node->children)
    {
        computeDFSInfo(child, dfsVisitNum, nodesAtHeightInDFSOrder);
    }

    node->dfsEndVisit = dfsVisitNum;
    dfsVisitNum++;
}

void computeDFSInfo(Node* rootNode, vector<vector<Node*>>& nodesAtHeightInDFSOrder)
{
    int dfsVisitNum = 1;
    computeDFSInfo(rootNode, dfsVisitNum, nodesAtHeightInDFSOrder);
}

// Calculate the height of each node, and remove its parent from its list of "children".
void fixParentChildAndHeights(Node* node, Node* parent = nullptr, int height = 0)
{
    node->height = height;
    node->parent = parent;
    node->numDescendants = 1; // A node is classed as a descendant of itself.

    node->children.erase(remove(node->children.begin(), node->children.end(), parent), node->children.end());

    for (auto child : node->children)
    {
        fixParentChildAndHeights(child, node, height + 1);
        node->numDescendants += child->numDescendants;
    }
}

struct AVLNode
{
    int64_t value = -1;
    AVLNode *leftChild = nullptr;
    AVLNode *rightChild = nullptr;
    int balanceFactor = 0;
    int maxDescendantDepth = 0;
    int numDescendants = 1;
};

class AVLTree
{
    public:
        AVLTree(bool isPersistent = false)
            : m_isPersistent{isPersistent}
        {
            if (m_isPersistent)
            {
                m_rootForRevision.push_back(nullptr);
            }
        }
        AVLNode* root()
        {
            if (!m_isPersistent)
                return m_root;
            else
                return m_rootForRevision[m_revisionNumber];
        }
        void insertValue(int64_t newValue)
        {
            if (!m_root)
                m_root = createNode(newValue);
            else
                m_root = insertValue(newValue, m_root);

            if (m_isPersistent)
            {
                m_rootForRevision.erase(m_rootForRevision.begin() + m_revisionNumber + 1, m_rootForRevision.end());
                m_rootForRevision.push_back(m_root);
                m_revisionNumber++;
                assert(m_revisionNumber == static_cast<int>(m_rootForRevision.size()) - 1);
            }
        }
        void switchToRevision(int revisionNum)
        {
            assert(m_isPersistent);
            m_revisionNumber = revisionNum;
        }

    private:
        AVLNode* m_root = nullptr;

        AVLNode* insertValue(int64_t newValue, AVLNode* currentNode)
        {
            if (m_isPersistent)
            {
                auto newCurrentNode = createNode(*currentNode);
                currentNode = newCurrentNode;
            }
            if (newValue < currentNode->value)
            {
                // Values in the left subtree of node must be *strictly less* than
                // that of currentNode.
                assert(newValue < currentNode->value);
                if (currentNode->leftChild)
                    currentNode->leftChild = insertValue(newValue, currentNode->leftChild);
                else
                    currentNode->leftChild = createNode(newValue);
            }
            else
            {
                // Values in the right subtree of node must be *greater than or equal to* that
                // that of currentNode.
                assert(newValue >= currentNode->value);
                if (currentNode->rightChild)
                    currentNode->rightChild = insertValue(newValue, currentNode->rightChild);
                else
                    currentNode->rightChild = createNode(newValue);
            }
            updateInfoFromChildren(currentNode);

            if (currentNode->balanceFactor < -1)
            {
                if (currentNode->leftChild->balanceFactor <= 0)
                {
                    // Simple rotation.
                    return rotateRight(currentNode);
                }
                else
                {
                    // Double-rotation.
                    currentNode->leftChild = rotateLeft(currentNode->leftChild);
                    return rotateRight(currentNode);
                }
            }
            if (currentNode->balanceFactor > +1)
            {
                if (currentNode->rightChild->balanceFactor >= 0)
                {
                    // Simple rotation.
                    return rotateLeft(currentNode);
                }
                else
                {
                    // Double-rotation.
                    currentNode->rightChild = rotateRight(currentNode->rightChild);
                    return rotateLeft(currentNode);
                }
            }

            return currentNode;
        }

        AVLNode* rotateRight(AVLNode* subtreeRoot)
        {
            auto newSubtreeRoot = subtreeRoot->leftChild;
            if (m_isPersistent)
            {
                // We're modifying newSubtreeRoot and subtreeRoot, but with Persistent
                // AVLTree's, we must copy-on-write.
                newSubtreeRoot = createNode(*newSubtreeRoot);
                subtreeRoot = createNode(*subtreeRoot);

            }
            auto previousNewSubtreeRootRightChild = newSubtreeRoot->rightChild;
            newSubtreeRoot->rightChild = subtreeRoot;
            subtreeRoot->leftChild = previousNewSubtreeRootRightChild;

            updateInfoFromChildren(subtreeRoot);

            updateInfoFromChildren(newSubtreeRoot);
            return newSubtreeRoot;
        }
        AVLNode* rotateLeft(AVLNode* subtreeRoot)
        {
            auto newSubtreeRoot = subtreeRoot->rightChild;
            if (m_isPersistent)
            {
                // We're modifying newSubtreeRoot and subtreeRoot, but with Persistent
                // AVLTree's, we must copy-on-write.
                newSubtreeRoot = createNode(*newSubtreeRoot);
                subtreeRoot = createNode(*subtreeRoot);
            }
            auto previousNewSubtreeRootLeftChild = newSubtreeRoot->leftChild;
            newSubtreeRoot->leftChild = subtreeRoot;
            subtreeRoot->rightChild = previousNewSubtreeRootLeftChild;
            updateInfoFromChildren(subtreeRoot);

            updateInfoFromChildren(newSubtreeRoot);
            return newSubtreeRoot;
        }

        void updateInfoFromChildren(AVLNode* nodeToUpdate)
        {
            // If m_isPersistent, assume that nodeToUpdate is already a newly-created
            // copy of the original nodeToUpdate.
            nodeToUpdate->balanceFactor = 0;
            nodeToUpdate->maxDescendantDepth = 0;
            nodeToUpdate->numDescendants = 1;

            auto leftChild = nodeToUpdate->leftChild;

            if (leftChild)
            {
                nodeToUpdate->balanceFactor -= 1 + leftChild->maxDescendantDepth;
                nodeToUpdate->maxDescendantDepth = max(nodeToUpdate->maxDescendantDepth, 1 + leftChild->maxDescendantDepth);
                nodeToUpdate->numDescendants += leftChild->numDescendants;
            }

            auto rightChild = nodeToUpdate->rightChild;
            if (rightChild)
            {
                nodeToUpdate->balanceFactor += 1 + rightChild->maxDescendantDepth;
                nodeToUpdate->maxDescendantDepth = max(nodeToUpdate->maxDescendantDepth, 1 + rightChild->maxDescendantDepth);
                nodeToUpdate->numDescendants += rightChild->numDescendants;
            }
        }

        AVLNode* createNode(int64_t value)
        {
            auto newNode = createNode();
            newNode->value = value;
            return newNode;
        }

        AVLNode* createNode(const AVLNode& nodeToCopy)
        {
            auto newNode = createNode();
            *newNode = nodeToCopy;
            return newNode;
        }

        AVLNode* createNode()
        {
            m_nodes.push_back(AVLNode());
            auto newNode = &(m_nodes.back());
            return newNode;
        }

        bool m_isPersistent = false;

        deque<AVLNode> m_nodes;

        int m_revisionNumber = 0;
        vector<AVLNode*> m_rootForRevision;

};

class AVLTreeIterator
{
    public:
        AVLTreeIterator(AVLNode* root)
            : m_currentNode(root)
        {
        };
        AVLNode* currentNode() const
        {
            return m_currentNode;
        }
        int numToLeft() const
        {
            const auto numInLeftSubTree = (m_currentNode->leftChild ? m_currentNode->leftChild->numDescendants : 0);
            return m_numToLeftOffset + numInLeftSubTree;
        }
        void followLeftChild()
        {
            m_currentNode = m_currentNode->leftChild;
        }
        void followRightChild()
        {
            const auto numInLeftSubTree = (m_currentNode->leftChild ? m_currentNode->leftChild->numDescendants : 0);
            m_numToLeftOffset += numInLeftSubTree + 1;
            m_currentNode = m_currentNode->rightChild;
        }
    private:
        AVLNode* m_currentNode = nullptr;

        int m_numToLeftOffset = 0;
};

class IndexRemapper
{
    public:
        int64_t remapNthRemainingToIndexAndRemove(int64_t nthOfRemainingToChoose)
        {
            // Be optimistic and give remappedIndex the smallest possible value:
            // we'll correct our optimism as we go along :)
            int64_t remappedIndex = nthOfRemainingToChoose;
            AVLTreeIterator treeIter(removedIndices.root());
            while (treeIter.currentNode())
            {
                const int64_t indexOfCurrentNode = treeIter.currentNode()->value;
                const int64_t numRemovedUpToCurrentNodeIndex = treeIter.numToLeft();
                const int64_t numFreeUpToCurrentNodeIndex = indexOfCurrentNode - numRemovedUpToCurrentNodeIndex;
                if (numFreeUpToCurrentNodeIndex >= nthOfRemainingToChoose + 1)
                {
                    // We've overshot; the required remappedIndex is to the left of indexOfCurrentNode; "recurse"
                    // into left child.
                    treeIter.followLeftChild();
                }
                else
                {
                    // Again, be optimistic about remappedIndex - we'll correct it as we go along.
                    remappedIndex = max(remappedIndex, indexOfCurrentNode + (nthOfRemainingToChoose - numFreeUpToCurrentNodeIndex) + 1);
                    // Required index is to the right of here; "recurse" into the right child.
                    treeIter.followRightChild();
                }

            }
            // We've successfully found the index in the original array; now mark it as Removed.
            removedIndices.insertValue(remappedIndex);
            return remappedIndex;
        }
    private:
        AVLTree removedIndices{false};
};

struct Range
{
    int leftIndex = -1;
    int rightIndex = -1;
    bool empty() const
    {
        return !(leftIndex >= 0 && rightIndex >= 0) && (leftIndex <= rightIndex);
    }
    int numInRange() const
    {
        if (empty())
            return 0;
        return rightIndex - leftIndex + 1;
    }
};

/**
 * Returns the Range [begin, end] of indices in nodesAtHeightInDFSOrder[height] such that all nodes in that
 * range are descendants of nodeToReparent, or (-1, -1) if nodeToReparent has no descendants at that height.
 *
 * Runs in O(log N).
 */
Range descendantRangeFor(Node* nodeToReparent, int height, const vector<vector<Node*>>& nodesAtHeightInDFSOrder)
{
    const auto descendantsAtHeightBegin = std::lower_bound(nodesAtHeightInDFSOrder[height].begin(), nodesAtHeightInDFSOrder[height].end(), nodeToReparent->dfsBeginVisit,
            [](const Node* node, const int dfsBeginVisit)
            {
            return node->dfsBeginVisit < dfsBeginVisit;
            });
    const auto descendantsAtHeightEnd = std::upper_bound(nodesAtHeightInDFSOrder[height].begin(), nodesAtHeightInDFSOrder[height].end(), nodeToReparent->dfsEndVisit,
            [](const int dfsEndVisit, const Node* node)
            {
            return dfsEndVisit < node->dfsEndVisit;
            });
    const bool hasDescendantsAtThisHeight = descendantsAtHeightBegin != nodesAtHeightInDFSOrder[height].end() && (*descendantsAtHeightBegin)->dfsEndVisit <= nodeToReparent->dfsEndVisit;
    if (!hasDescendantsAtThisHeight)
        return {-1, -1};

    return {static_cast<int>(descendantsAtHeightBegin - nodesAtHeightInDFSOrder[height].begin()), static_cast<int>(descendantsAtHeightEnd - nodesAtHeightInDFSOrder[height].begin() - 1)};
}


/**
 * Find the total number of nodes v such that both:
 *  
 *  1. v.height <= height; and
 *  2. v is not a descendant of nodeToReparent
 *
 *  Runs in O(log N).
 */
int findNumNonDescendantsUpToHeight(Node* nodeToReparent, const int height, const vector<int>& numNodesUpToHeight, const vector<vector<Node*>>& nodesAtHeightInDFSOrder,  const vector<vector<int>>& numProperDescendantsForNodeAtHeightPrefixSum)
{
    int numNonDescendantsUpToThisHeight = numNodesUpToHeight[height];
    if (height >= nodeToReparent->height)
    {
        int sumOfProperDescendantsOfDescendantsAtHeight = 0;
        const auto descendantRange = descendantRangeFor(nodeToReparent, height, nodesAtHeightInDFSOrder);
        if (!descendantRange.empty())
        {
            sumOfProperDescendantsOfDescendantsAtHeight = numProperDescendantsForNodeAtHeightPrefixSum[height][descendantRange.rightIndex];
            if (descendantRange.leftIndex > 0)
                sumOfProperDescendantsOfDescendantsAtHeight -= numProperDescendantsForNodeAtHeightPrefixSum[height][descendantRange.leftIndex - 1];
        }
        numNonDescendantsUpToThisHeight -= nodeToReparent->numDescendants - sumOfProperDescendantsOfDescendantsAtHeight;
    }
    return numNonDescendantsUpToThisHeight;
}

/**
  * Imagine we placed all the elements of tree1 and all the elements of tree2
  * into a list, and that this list contained no duplicates.  Imagine further
  * that we then sorted this list.
  * If the kth element in this list is present in tree1, then this function
  * finds that kth element in:
  *
  *   O(log (|tree1|) * log (|tree2|))
  *
  * and returns the AVLNode in tree1 that holds that element.  If the kth
  * element is not present in tree1, this function returns nullptr.
  */
AVLNode* findKthFromPairAux(int k, AVLTree& tree1, AVLTree& tree2)
{
    AVLTreeIterator tree1Iter = tree1.root();
    while (tree1Iter.currentNode())
    {
        const auto currentValue1 = tree1Iter.currentNode()->value;
        auto numToLeft2 = 0;
        AVLTreeIterator tree2Iter = tree2.root();
        while (tree2Iter.currentNode())
        {
            if (tree2Iter.currentNode()->value < currentValue1)
                numToLeft2 = tree2Iter.numToLeft() + 1;
            if (tree2Iter.currentNode()->value > currentValue1)
                tree2Iter.followLeftChild();
            else
                tree2Iter.followRightChild();
        }

        const auto numToLeft1 = tree1Iter.numToLeft();
        const auto numToLeftInBoth = numToLeft1 + numToLeft2;
        if (numToLeftInBoth == k)
        {
            return tree1Iter.currentNode();
        }

        if (numToLeftInBoth > k)
            tree1Iter.followLeftChild();
        else
            tree1Iter.followRightChild();
    }

    return nullptr;
}

/**
 * See findKthFromPairAux.
 */
AVLNode* findKthFromPair(int k, AVLTree& tree1, AVLTree& tree2)
{
    auto kthAVLNode = findKthFromPairAux(k, tree1, tree2);
    if (!kthAVLNode)
    {
        // Element was not in tree1; assume it is in tree2.
        kthAVLNode = findKthFromPairAux(k, tree2, tree1);
    }
    return kthAVLNode;
}

int64_t calcFinalDecryptionKey(vector<Node>& nodes, const vector<int64_t>& encryptedQueries)
{
    // Precompute various lookups.
    const auto maxNodeHeight = (*max_element(nodes.begin(), nodes.end(), [](const auto& node1, const auto& node2)
            {
                return node1.height < node2.height;
            })).height;

    auto rootNode = &(nodes.front());
    vector<vector<Node*>> nodesAtHeightInDFSOrder(maxNodeHeight + 1);
    computeDFSInfo(rootNode, nodesAtHeightInDFSOrder);
    
    // The numCanReparentToPrefixSum lookup is for use with Phase One i.e.
    // finding the nodeToReparent of the required reparenting.
    // THe value numCanReparentToPrefixSum[nodeId - 1] is the number of valid reparentings
    // (x, y), where x's id is <= nodeId.
    vector<int64_t> numCanReparentToPrefixSum;
    int64_t sumOfNumCanReparentTo = 0;
    for (const auto& node : nodes)
    {
        const auto numCanReparentNodeTo = nodes.size() - node.numDescendants; // Can reparent a node to any of its non-descendants.
        sumOfNumCanReparentTo += numCanReparentNodeTo;
        numCanReparentToPrefixSum.push_back(sumOfNumCanReparentTo);
    }

    // The numNodesUpToHeight and numProperDescendantsForNodeAtHeightPrefixSum are used for Phase Two i.e. finding
    // the height of the newParent in the required reparenting.
    vector<int> numNodesUpToHeight(maxNodeHeight + 1);
    {
        auto numNodes = 0;
        for (auto height = 0; height <= maxNodeHeight; height++)
        {
            numNodes += nodesAtHeightInDFSOrder[height].size();
            numNodesUpToHeight[height] = numNodes;
        }
    }
    // Consider the list of nodes at height h, in order of their visitation in the DFS we performed in computeDFSInfo.
    // Then numProperDescendantsForNodeAtHeightPrefixSum[height][x] represents the sum of the 
    // numbers of proper ancestors of the first x + 1 nodes in this list.
    vector<vector<int>> numProperDescendantsForNodeAtHeightPrefixSum(maxNodeHeight + 1);
    {
        for (auto height = 0; height <= maxNodeHeight; height++)
        {
            const auto& nodesAtHeight = nodesAtHeightInDFSOrder[height];
            auto numProperDescendantsSum = 0;
            for (const auto node : nodesAtHeight)
            {
                numProperDescendantsSum += node->numDescendants - 1; // "-1" as we want *proper* descendants.
                numProperDescendantsForNodeAtHeightPrefixSum[height].push_back(numProperDescendantsSum);
            }
        }
    }
    vector<int> allHeights; // Used so we can leverage upper_bound for a binary search.
    for (auto height = 0; height <= maxNodeHeight; height++)
    {
        allHeights.push_back(height);
    }
    // Compute the lookups for use with Phase Three i.e. finally computing the newParent for the required reparenting,
    // having found the nodeToReparent and the height of the newParent in the previous two Phases, respectively.
    // Again, consider the list of nodes at height h, in order of their visitation in the DFS.
    // Then the xth revision of the prefixesForHeight[h] Persistent AVLTree represents a balance tree containing
    // the ids of the first x nodes in this list, ordered numerically.
    // Similarly, the xth revision of the suffixesForHeight[h] Persistent AVLTree represents a balance tree containing
    // the ids of the *last* x nodes in this list, again ordered numerically.
    vector<AVLTree> prefixesForHeight(maxNodeHeight + 1);
    vector<AVLTree> suffixesForHeight(maxNodeHeight + 1);
    for (auto height = 0; height <= maxNodeHeight; height++)
    {
        prefixesForHeight[height] = AVLTree(true);
        for (const auto nodeAtHeight : nodesAtHeightInDFSOrder[height])
        {
            prefixesForHeight[height].insertValue(nodeAtHeight->id);
        }
        suffixesForHeight[height] = AVLTree(true);
        for (auto nodeAtHeightRevIter = nodesAtHeightInDFSOrder[height].rbegin(); nodeAtHeightRevIter != nodesAtHeightInDFSOrder[height].rend(); nodeAtHeightRevIter++)
        {
            suffixesForHeight[height].insertValue((*nodeAtHeightRevIter)->id);
        }
    }

    int64_t decryptionKey = 0;
    int64_t powerOf2 = 2;
    int64_t powerOf3 = 3;
    IndexRemapper indexRemapper;

    for (const auto encryptedQuery : encryptedQueries)
    {
        const auto kthInRemainingToFind = (encryptedQuery ^ decryptionKey) - 1; // Make 0-relative.

        // "Pretend" to remove this kthInRemainingToFind index, and figure out the index in the original list
        // corresponding to kthInRemainingToFind.
        const auto indexInOriginalList = indexRemapper.remapNthRemainingToIndexAndRemove(kthInRemainingToFind); // 0-relative.

        // Phase One: compute nodeToReparent.  Easy :)
        const auto firstNodeExceedingIter = std::upper_bound(numCanReparentToPrefixSum.begin(), numCanReparentToPrefixSum.end(), indexInOriginalList);
        const auto nodeToReparentIndex = firstNodeExceedingIter - numCanReparentToPrefixSum.begin();
        const auto nodeToReparent = &(nodes[nodeToReparentIndex]);
        // Phase One complete.

        // We now need to find the numOfReparentingThatReparentsNode'th element in the original
        // list that re-parents our nodeToReparent.
        // This takes up the two remaining Phases.  This block is Phase Two, where we just compute newParentHeight.
        const auto numOfReparentingThatReparentsNode = indexInOriginalList - (nodeToReparentIndex == 0 ? 0 : numCanReparentToPrefixSum[nodeToReparentIndex - 1]);
        const auto newParentHeightIter = upper_bound(allHeights.begin(), allHeights.end(), numOfReparentingThatReparentsNode,
                [nodeToReparent, &numNodesUpToHeight, &nodesAtHeightInDFSOrder, &numProperDescendantsForNodeAtHeightPrefixSum](const int numOfReparentingThatReparentsNode, const int height)
                {
                    const auto numReparentingsUpToHeight = findNumNonDescendantsUpToHeight(nodeToReparent, height, numNodesUpToHeight, nodesAtHeightInDFSOrder, numProperDescendantsForNodeAtHeightPrefixSum);
                    return numOfReparentingThatReparentsNode < numReparentingsUpToHeight;
                });
        assert(newParentHeightIter != allHeights.end());
        const auto newParentHeight = *newParentHeightIter;
        assert(newParentHeight != -1);
        // Phase Two complete.

        // We now need to find the numOfReparentingForNodeAndNewHeight'th item in the original list
        // that reparents nodeToReparent to a node whose height is newParentHeight.
        // This is the final phase, Phase Three.
        auto numOfReparentingForNodeAndNewHeight = numOfReparentingThatReparentsNode;
        if (newParentHeightIter != allHeights.begin())
        {
            // Adjust numOfReparentingForNodeAndNewHeight by skipping all reparentings that reparent nodeToReparent
            // to a node with height < newParentHeight.
            numOfReparentingForNodeAndNewHeight -= findNumNonDescendantsUpToHeight(nodeToReparent, *std::prev(newParentHeightIter), numNodesUpToHeight, nodesAtHeightInDFSOrder, numProperDescendantsForNodeAtHeightPrefixSum);
        }

        const auto descendantRange = descendantRangeFor(nodeToReparent, newParentHeight, nodesAtHeightInDFSOrder);

        auto numNonDescendantsToLeft = nodesAtHeightInDFSOrder[newParentHeight].size();
        auto numNonDescendantsToRight = 0;
        if (!descendantRange.empty())
        {
            numNonDescendantsToLeft = descendantRange.leftIndex;
            numNonDescendantsToRight = nodesAtHeightInDFSOrder[newParentHeight].size() - descendantRange.rightIndex - 1;
        }
        // The AVLTree's "prefixesForHeight" and "suffixesForHeight" after the switch, will represent the node ids (in sorted order) to the
        // left and the right (respectively) of the descendant-range of nodeToReparent in the list of nodes
        // with height newParentHeight.
        // Performing each switch is O(1).
        prefixesForHeight[newParentHeight].switchToRevision(numNonDescendantsToLeft);
        suffixesForHeight[newParentHeight].switchToRevision(numNonDescendantsToRight);
        const auto newParentAVLNode = findKthFromPair(numOfReparentingForNodeAndNewHeight, prefixesForHeight[newParentHeight], suffixesForHeight[newParentHeight]);
        assert(newParentAVLNode);
        const auto newParentId = newParentAVLNode->value;
        auto newParent = &(nodes[newParentId - 1]);
        // Phase Three complete.

        // We've found the required reparenting (nodeToReparent, newParentId).
        // Use it to update the decryptionKey.
        decryptionKey = (decryptionKey + powerOf2 * nodeToReparent->id) % Mod;
        decryptionKey = (decryptionKey + powerOf3 * newParent->id) % Mod;
        powerOf2 = (powerOf2 * 2) % Mod;
        powerOf3 = (powerOf3 * 3) % Mod;
    }

    return decryptionKey;
}

int main()
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (auto t = 0; t < T; t++)
    {
        const auto numNodes = read<int>();

        vector<Node> nodes(numNodes);
        for (auto i = 0; i < numNodes; i++)
        {
            nodes[i].id = i + 1;
        }
        for (auto i = 0; i < numNodes - 1; i++)
        {
            // Make a and b 0-relative.
            const auto a = read<int>() - 1;
            const auto b = read<int>() - 1;

            nodes[a].children.push_back(&nodes[b]);
            nodes[b].children.push_back(&nodes[a]);
        }

        auto rootNode = &(nodes.front());
        fixParentChildAndHeights(rootNode);

        const auto numQueries = read<int>();
        vector<int64_t> encryptedQueries(numQueries);
        for (auto& encryptedQuery : encryptedQueries)
        {
            encryptedQuery = read<int64_t>();
        }

        cout << calcFinalDecryptionKey(nodes, encryptedQueries) << endl;
    }

    assert(cin);
}

