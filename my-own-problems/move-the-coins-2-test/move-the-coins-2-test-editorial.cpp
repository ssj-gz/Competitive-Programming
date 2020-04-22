// Simon St James (ssjgz) - 2020-04-01
// 
// Solution to: https://www.codechef.com/problems/MVCN2TST
//
//#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#else
#define _GLIBCXX_DEBUG       // Iterator safety; out-of-bounds access for Containers, etc.
#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.
#define BRUTE_FORCE
#endif
#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

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
    int numCanReparentTo = -1;

    bool isDescendantOf(Node& otherNode)
    {
        return (dfsBeginVisit >= otherNode.dfsBeginVisit && dfsEndVisit <= otherNode.dfsEndVisit);
    }

    int dfsBeginVisit = -1;
    int dfsEndVisit = -1;
};

void computeDFSInfo(Node* node, int& dfsVisitNum, vector<vector<Node*>>& nodesAtHeightLookup)
{
    node->dfsBeginVisit = dfsVisitNum;
    if (!nodesAtHeightLookup.empty()) // TODO - remove this condition - it's only there as the  testgen and brute force don't use nodesAtHeightLookup.
    {
        nodesAtHeightLookup[node->height].push_back(node);
    }
    dfsVisitNum++;

    for (auto child : node->children)
    {
        computeDFSInfo(child, dfsVisitNum, nodesAtHeightLookup);
    }

    node->dfsEndVisit = dfsVisitNum;
    dfsVisitNum++;
}

void computeDFSInfo(Node* rootNode, vector<vector<Node*>>& nodesAtHeightLookup)
{
    int dfsVisitNum = 1;
    computeDFSInfo(rootNode, dfsVisitNum, nodesAtHeightLookup);
}

// Calculate the height of each node, and remove its parent from its list of "children".
void fixParentChildAndHeights(Node* node, Node* parent = nullptr, int height = 0)
{
    node->height = height;
    node->parent = parent;
    node->numDescendants = 1; // This node.

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

    int id = -1;
};

class AVLTree
{
    public:
        AVLTree(bool isPersistent = false, int nodeBlockSize = 1)
            : m_isPersistent{isPersistent}, m_nodeBlockSize{nodeBlockSize}
        {
            if (m_isPersistent)
                m_rootForRevision.push_back(nullptr);
        }
        AVLNode* root()
        {
            if (!m_isPersistent)
                return m_root;
            else
                return m_rootForRevision[m_revisionNumber];
        }
        void insertValue(int newValue)
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
                assert(m_revisionNumber == m_rootForRevision.size() - 1);
            }
        }
        void switchToRevision(int revisionNum)
        {
            assert(m_isPersistent);
            m_revisionNumber = revisionNum;
        }

    private:
        AVLNode* m_root = nullptr;

        AVLNode* insertValue(int newValue, AVLNode* currentNode)
        {
            if (m_isPersistent)
            {
                auto newCurrentNode = createNode(currentNode->value);
                *newCurrentNode = *currentNode;
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

        AVLNode* createNode(int value)
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
            if (m_nodes.empty() || static_cast<int>(m_nodes.back().size()) == m_nodeBlockSize)
            {
                m_nodes.push_back(vector<AVLNode>());
                m_nodes.back().reserve(m_nodeBlockSize);
            }
            m_nodes.back().push_back(AVLNode());
            auto newNode = &(m_nodes.back().back());
            newNode->id = m_nextNodeId;
            m_nextNodeId++;
            return newNode;
        }

        bool m_isPersistent = false;

        int m_nodeBlockSize = 1;
        deque<vector<AVLNode>> m_nodes;

        int m_nextNodeId = 1;

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

void printSubTree(AVLNode* subtreeRoot)
{
    if (subtreeRoot == nullptr)
        return;
    cout << "Node " << subtreeRoot->id << " has value: " << subtreeRoot->value << " balanceFactor: " << subtreeRoot->balanceFactor << " maxDescendantDepth: " << subtreeRoot->maxDescendantDepth << " numDescendants: " << subtreeRoot->numDescendants;
    cout << " leftChild: " << (subtreeRoot->leftChild ? subtreeRoot->leftChild->id : -1) << " rightChild: " << (subtreeRoot->rightChild ? subtreeRoot->rightChild->id : -1) << endl;

    if (subtreeRoot->leftChild)
        printSubTree(subtreeRoot->leftChild);
    if (subtreeRoot->rightChild)
        printSubTree(subtreeRoot->rightChild);
}

void printTree(AVLTree& tree)
{
    printSubTree(tree.root());
}


class IndexRemapper
{
    public:
        int64_t remapNthRemainingToIndexAndRemove(int64_t nthOfRemainingToChoose)
        {
            // Be optimistic and give remappedIndex the smallest possible value:
            // we'll correct our optimism as we go along :)
            int64_t remappedIndex = nthOfRemainingToChoose;
            auto currentNode = removedIndices.root();
            int numRemovedUpToCurrentNodeIndexOffset = 0;
            AVLTreeIterator treeIter(removedIndices.root());
            while (treeIter.currentNode())
            {
                const int64_t indexOfCurrentNode = treeIter.currentNode()->value;
                const int numRemovedUpToCurrentNodeIndex = treeIter.numToLeft();
                const int numFreeUpToCurrentNodeIndex = indexOfCurrentNode - numRemovedUpToCurrentNodeIndex;
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
        AVLTree removedIndices;
};



vector<pair<Node*, Node*>> computeOrderedValidReparentings(vector<Node>& nodes)
{
    vector<pair<Node*, Node*>> validReparentings;

    for (auto& nodeToReparent : nodes)
    {
        if (nodeToReparent.parent == nullptr)
            continue;

        for (auto& newParent : nodes)
        {
            assert(newParent.dfsBeginVisit != -1 && newParent.dfsEndVisit != -1);
            const bool newParentIsDescendant = (newParent.dfsBeginVisit >= nodeToReparent.dfsBeginVisit && newParent.dfsEndVisit <= nodeToReparent.dfsEndVisit);
            if (!newParentIsDescendant)
                validReparentings.push_back({&nodeToReparent, &newParent});
        }
    }

    sort(validReparentings.begin(), validReparentings.end(),
            [](const auto& lhs, const auto& rhs)
            {
            if (lhs.first->id != rhs.first->id)
                return lhs.first->id < rhs.first->id;
            if (lhs.second->height != rhs.second->height)
                return lhs.second->height < rhs.second->height;
            return lhs.second->id < rhs.second->id;
            });


    return validReparentings;
}

int64_t solveBruteForce(vector<Node>& nodes, const vector<int64_t>& encryptedQueries)
{
    auto rootNode = &(nodes.front());
    vector<vector<Node*>> nodesAtHeightLookupDummy;
    computeDFSInfo(rootNode, nodesAtHeightLookupDummy);

    auto validReparentings = computeOrderedValidReparentings(nodes);
    int64_t decryptionKey = 0;
    int64_t powerOf2 = 2;
    int64_t powerOf3 = 3;

    for (const auto encryptedQuery : encryptedQueries)
    {
        const auto index = (encryptedQuery ^ decryptionKey) - 1; // Make 0-relative.
        assert(0 <= index && index < validReparentings.size());
        auto queryResultIter = validReparentings.begin() + index;

        const auto nodeToReparent = queryResultIter->first;
        const auto newParent = queryResultIter->second;

        validReparentings.erase(queryResultIter);

        decryptionKey = (decryptionKey + powerOf2 * nodeToReparent->id) % Mod;
        decryptionKey = (decryptionKey + powerOf3 * newParent->id) % Mod;
        powerOf2 = (powerOf2 * 2) % Mod;
        powerOf3 = (powerOf3 * 3) % Mod;
    }

    return decryptionKey;
}

/**
 * Find the total number of nodes v such that both:
 *  
 *  1. v.height <= height; and
 *  2. v is not a descendant of nodeToReparent
 *
 *  Runs in O(N).
 */
int findNumNonDescendantsUpToHeight(Node* nodeToReparent, const int height, const vector<int>& numNodesUpToHeight, const vector<vector<Node*>>& nodesAtHeightLookup,  const vector<vector<int>>& numProperDescendantsForNodeAtHeightPrefixSum)
{
    int numNonDescendantsUpToThisHeight = numNodesUpToHeight[height];
    if (height >= nodeToReparent->height)
    {
        const auto descendantsAtHeightBegin = std::lower_bound(nodesAtHeightLookup[height].begin(), nodesAtHeightLookup[height].end(), nodeToReparent->dfsBeginVisit,
                [](const Node* node, const int dfsBeginVisit)
                {
                return node->dfsBeginVisit < dfsBeginVisit;
                });
        const auto descendantsAtHeightEnd = std::upper_bound(nodesAtHeightLookup[height].begin(), nodesAtHeightLookup[height].end(), nodeToReparent->dfsEndVisit,
                [](const int dfsEndVisit, const Node* node)
                {
                return dfsEndVisit < node->dfsEndVisit;
                });
        const bool hasDescendantsAtThisHeight = descendantsAtHeightBegin != nodesAtHeightLookup[height].end() && (*descendantsAtHeightBegin)->dfsEndVisit <= nodeToReparent->dfsEndVisit;
        int sumOfProperDescendantsOfDescendantsAtHeight = 0;
        if (hasDescendantsAtThisHeight)
        {
            assert(descendantsAtHeightBegin < descendantsAtHeightEnd);
            int firstDescendantIndex = descendantsAtHeightBegin - nodesAtHeightLookup[height].begin();
            int lastDescendantIndex = descendantsAtHeightEnd - nodesAtHeightLookup[height].begin() - 1;
            sumOfProperDescendantsOfDescendantsAtHeight = numProperDescendantsForNodeAtHeightPrefixSum[height][lastDescendantIndex];
            if (firstDescendantIndex > 0)
                sumOfProperDescendantsOfDescendantsAtHeight -= numProperDescendantsForNodeAtHeightPrefixSum[height][firstDescendantIndex - 1];
        }
        numNonDescendantsUpToThisHeight -= nodeToReparent->numDescendants - sumOfProperDescendantsOfDescendantsAtHeight;
    }
    return numNonDescendantsUpToThisHeight;
}

AVLNode* findKthFromPairAux(int k, AVLTree& tree1, AVLTree& tree2)
{
    AVLTreeIterator tree1Iter = tree1.root();
    while (tree1Iter.currentNode())
    {
        const auto currentValue1 = tree1Iter.currentNode()->value;
        int numToLeft2 = 0;
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

AVLNode* findKthFromPair(int k, AVLTree& tree1, AVLTree& tree2)
{
    auto kthAVLNode = findKthFromPairAux(k, tree1, tree2);
    if (!kthAVLNode)
    {
        kthAVLNode = findKthFromPairAux(k, tree2, tree1);
    }
    return kthAVLNode;
}


int64_t solveOptimised(vector<Node>& nodes, const vector<int64_t>& encryptedQueries)
{
    int64_t decryptionKey = 0;
    int64_t powerOf2 = 2;
    int64_t powerOf3 = 3;

    int maxNodeHeight = -1;
    for (const auto& node : nodes)
    {
        maxNodeHeight = max(maxNodeHeight, node.height);
    }
    assert(maxNodeHeight != -1);

    auto rootNode = &(nodes.front());
    vector<vector<Node*>> nodesAtHeightLookup(maxNodeHeight + 1);
    computeDFSInfo(rootNode, nodesAtHeightLookup);
    const int numNodes = nodes.size();
    for (auto& node : nodes)
    {
        // We can reparent to any node that is not a descendant.
        node.numCanReparentTo = numNodes - node.numDescendants;
    }
    
    IndexRemapper indexRemapper;
    vector<int64_t> numCanReparentToPrefixSum;
    int64_t sumOfNumCanReparentTo = 0;
    for (const auto& node : nodes)
    {
        sumOfNumCanReparentTo += node.numCanReparentTo;
        numCanReparentToPrefixSum.push_back(sumOfNumCanReparentTo);
    }

    vector<int> numNodesUpToHeight(maxNodeHeight + 1);
    {
        int numNodes = 0;
        for (int height = 0; height <= maxNodeHeight; height++)
        {
            numNodes += nodesAtHeightLookup[height].size();
            numNodesUpToHeight[height] = numNodes;
        }
    }
    vector<vector<int>> numProperDescendantsForNodeAtHeightPrefixSum(maxNodeHeight + 1);
    {
        for (int height = 0; height <= maxNodeHeight; height++)
        {
            const auto& nodesAtHeight = nodesAtHeightLookup[height];
            int numProperDescendantsSum = 0;
            for (const auto node : nodesAtHeight)
            {
                numProperDescendantsSum += node->numDescendants - 1; // "-1" as we want *proper* descendants.
                numProperDescendantsForNodeAtHeightPrefixSum[height].push_back(numProperDescendantsSum);
            }
        }
    }
    vector<AVLTree> prefixesForHeight(maxNodeHeight + 1, AVLTree(true));
    vector<AVLTree> suffixesForHeight(maxNodeHeight + 1, AVLTree(true));
    for (int height = 0; height <= maxNodeHeight; height++)
    {
        for (const auto nodeAtHeight : nodesAtHeightLookup[height])
        {
            prefixesForHeight[height].insertValue(nodeAtHeight->id);
        }
        for (auto nodeAtHeightRevIter = nodesAtHeightLookup[height].rbegin(); nodeAtHeightRevIter != nodesAtHeightLookup[height].rend(); nodeAtHeightRevIter++)
        {
            suffixesForHeight[height].insertValue((*nodeAtHeightRevIter)->id);
        }
    }

    vector<int> allHeights; // Used so we can leverage upper_bound for a binary search.
    for (int height = 0; height <= maxNodeHeight; height++)
    {
        allHeights.push_back(height);
    }

    for (const auto encryptedQuery : encryptedQueries)
    {
        const auto kthInRemainingToFind = (encryptedQuery ^ decryptionKey) - 1; // Make 0-relative.

        const int indexInOriginalList = indexRemapper.remapNthRemainingToIndexAndRemove(kthInRemainingToFind);

        const auto firstNodeExceedingIter = std::upper_bound(numCanReparentToPrefixSum.begin(), numCanReparentToPrefixSum.end(), indexInOriginalList);
        const auto nodeIndex = firstNodeExceedingIter - numCanReparentToPrefixSum.begin();
        auto nodeToReparent = &(nodes[nodeIndex]);

        // i.e. we now need to find the numOfReparentingThatReparentsNode'th element in the original
        // list that re-parents our nodeToReparent.
        const auto numOfReparentingThatReparentsNode = indexInOriginalList - (nodeIndex == 0 ? 0 : numCanReparentToPrefixSum[nodeIndex - 1]);
        const auto heightIter = upper_bound(allHeights.begin(), allHeights.end(), numOfReparentingThatReparentsNode,
                [nodeToReparent, &numNodesUpToHeight, &nodesAtHeightLookup, &numProperDescendantsForNodeAtHeightPrefixSum](const int numOfReparentingThatReparentsNode, const int height)
                {
                    const int numReparentingsUpToHeight = findNumNonDescendantsUpToHeight(nodeToReparent, height, numNodesUpToHeight, nodesAtHeightLookup, numProperDescendantsForNodeAtHeightPrefixSum);
                    return numOfReparentingThatReparentsNode < numReparentingsUpToHeight;
                });
        assert(heightIter != allHeights.end());
        const int newParentHeight = *heightIter;
        assert(newParentHeight != -1);

        // i.e. we now need to find the numOfReparentingThatReparentsNode's item in the original list
        // that reparents nodeToReparent to a newParentHeight whose height is newParentHeight.
        auto numOfReparentingForNodeAndNewHeight = numOfReparentingThatReparentsNode;
        if (heightIter != allHeights.begin())
        {
            numOfReparentingForNodeAndNewHeight -= findNumNonDescendantsUpToHeight(nodeToReparent, *std::prev(heightIter), numNodesUpToHeight, nodesAtHeightLookup, numProperDescendantsForNodeAtHeightPrefixSum);
        }

        const auto descendantsAtHeightBegin = std::lower_bound(nodesAtHeightLookup[newParentHeight].begin(), nodesAtHeightLookup[newParentHeight].end(), nodeToReparent->dfsBeginVisit,
                [](const Node* node, const int dfsBeginVisit)
                {
                return node->dfsBeginVisit < dfsBeginVisit;
                });
        const auto descendantsAtHeightEnd = std::upper_bound(nodesAtHeightLookup[newParentHeight].begin(), nodesAtHeightLookup[newParentHeight].end(), nodeToReparent->dfsEndVisit,
                [](const int dfsEndVisit, const Node* node)
                {
                return dfsEndVisit < node->dfsEndVisit;
                });
        const bool hasDescendantsAtThisHeight = descendantsAtHeightBegin != nodesAtHeightLookup[newParentHeight].end() && (*descendantsAtHeightBegin)->dfsEndVisit <= nodeToReparent->dfsEndVisit;
        int numNonDescendantsToLeft = nodesAtHeightLookup[newParentHeight].size();
        int numNonDescendantsToRight = 0;
        if (hasDescendantsAtThisHeight)
        {
            numNonDescendantsToLeft = descendantsAtHeightBegin - nodesAtHeightLookup[newParentHeight].begin();
            numNonDescendantsToRight = nodesAtHeightLookup[newParentHeight].end() - descendantsAtHeightEnd;
        }
        prefixesForHeight[newParentHeight].switchToRevision(numNonDescendantsToLeft);
        suffixesForHeight[newParentHeight].switchToRevision(numNonDescendantsToRight);
        const auto newParentAVLNode = findKthFromPair(numOfReparentingForNodeAndNewHeight, prefixesForHeight[newParentHeight], suffixesForHeight[newParentHeight]);
        assert(newParentAVLNode);
        const auto newParentId = newParentAVLNode->value;
        auto newParent = &(nodes[newParentId - 1]);

        decryptionKey = (decryptionKey + powerOf2 * nodeToReparent->id) % Mod;
        decryptionKey = (decryptionKey + powerOf3 * newParent->id) % Mod;
        powerOf2 = (powerOf2 * 2) % Mod;
        powerOf3 = (powerOf3 * 3) % Mod;
    }

    return decryptionKey;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T;)
        {
            int64_t encryptionKey = 0;
            int64_t powerOf2 = 2;
            int64_t powerOf3 = 3;

            const auto numNodes = 1 + rand() % 200;
            vector<Node> nodes(numNodes);
            for (int i = 0; i < numNodes; i++)
                nodes[i].id = (i + 1);

            vector<pair<Node*, Node*>> edges;

            for (int childNodeIndex = 1; childNodeIndex < numNodes; childNodeIndex++)
            {
                const int parentIndex = rand() % childNodeIndex;
                edges.push_back({&(nodes[parentIndex]), &(nodes[childNodeIndex])});
            }
            for (auto& edge : edges)
            {
                edge.first->children.push_back(edge.second);
                edge.second->children.push_back(edge.first);
            }

            auto rootNode = &(nodes.front());
            fixParentChildAndHeights(rootNode);
            vector<vector<Node*>> nodesAtHeightLookupDummy;
            computeDFSInfo(rootNode, nodesAtHeightLookupDummy);
            auto validReparentings = computeOrderedValidReparentings(nodes);

            if (validReparentings.empty())
                continue;
            const auto numQueries = validReparentings.empty() ? 0 : 1 + rand() % validReparentings.size();

            vector<int64_t> encryptedQueries;
            for (int queryNum = 0; queryNum < numQueries; queryNum++)
            {
                const int kthRemainingReparenting = rand() % validReparentings.size();
                const auto nodeToReparent = validReparentings[kthRemainingReparenting].first;
                const auto newParent = validReparentings[kthRemainingReparenting].second;
                encryptedQueries.push_back((1 + kthRemainingReparenting) ^ encryptionKey);
                validReparentings.erase(validReparentings.begin() + kthRemainingReparenting);

                encryptionKey = (encryptionKey + powerOf2 * nodeToReparent->id) % Mod;
                encryptionKey = (encryptionKey + powerOf3 * newParent->id) % Mod;
                powerOf2 = (powerOf2 * 2) % Mod;
                powerOf3 = (powerOf3 * 3) % Mod;
            }

            cout << numNodes << endl;
            for (const auto edge : edges)
            {
                cout << edge.first->id << " " << edge.second->id << endl;
            }
            cout << numQueries << endl;
            for (const auto encryptedQuery : encryptedQueries)
            {
                cout << encryptedQuery << endl;
            }
            t++;
        }

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto numNodes = read<int>();

        vector<Node> nodes(numNodes);
        for (int i = 0; i < numNodes; i++)
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

        // TODO - eventually, we will need to make this online, using a decryption-key
        // approach (similar to "Simple Markdown Editor").
        const auto numQueries = read<int>();
        vector<int64_t> encryptedQueries(numQueries);
        for (auto& encryptedQuery : encryptedQueries)
        {
            encryptedQuery = read<int64_t>();
        }


#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(nodes, encryptedQueries);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 1
        const auto solutionOptimised = solveOptimised(nodes, encryptedQueries);
        cout << "solutionOptimised: " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised(nodes, encryptedQueries);
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}


