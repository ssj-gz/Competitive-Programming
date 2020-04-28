#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <deque>
#include <algorithm>
#include <tree-generator.h>
#include <cassert>

using namespace std;

const int64_t Mod = 1'000'000'007;


struct NodeData
{
    int height = -1;
    int id = -1;
    int numDescendants = -1; // Includes the node itself.

    bool isDescendantOf(TestNode<NodeData>& otherNode)
    {
        return (dfsBeginVisit >= otherNode.data.dfsBeginVisit && dfsEndVisit <= otherNode.data.dfsEndVisit);
    }

    int dfsBeginVisit = -1;
    int dfsEndVisit = -1;
};

void computeDFSInfo(TestNode<NodeData>* node, TestNode<NodeData>* parent, int& dfsVisitNum, vector<vector<TestNode<NodeData>*>>& nodesAtHeightLookup)
{
    node->data.dfsBeginVisit = dfsVisitNum;
    nodesAtHeightLookup[node->data.height].push_back(node);
    dfsVisitNum++;

    for (auto childEdge : node->neighbours)
    {
        auto child = childEdge->otherNode(node);
        if (child == parent)
            continue;
        computeDFSInfo(child, node, dfsVisitNum, nodesAtHeightLookup);
    }

    node->data.dfsEndVisit = dfsVisitNum;
    dfsVisitNum++;
}

void computeDFSInfo(TestNode<NodeData>* rootNode, vector<vector<TestNode<NodeData>*>>& nodesAtHeightLookup)
{
    int dfsVisitNum = 1;
    computeDFSInfo(rootNode, nullptr, dfsVisitNum, nodesAtHeightLookup);
}

// Calculate the height of each node, and remove its parent from its list of "children".
void fixParentChildAndHeights(TestNode<NodeData>* node, TestNode<NodeData>* parent = nullptr, int height = 0)
{
    node->data.height = height;
    node->data.numDescendants = 1; // This node.

    for (auto childEdge : node->neighbours)
    {
        auto child = childEdge->otherNode(node);
        if (child == parent)
            continue;

        fixParentChildAndHeights(child, node, height + 1);
        node->data.numDescendants += child->data.numDescendants;
    }
}

namespace MVCN2TST
{
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
}

struct LookupInfo
{
    int maxHeight = -1;
    vector<vector<TestNode<NodeData>*>> nodesAtHeightLookup;
    vector<MVCN2TST::AVLTree> prefixesForHeight;
    vector<MVCN2TST::AVLTree> suffixesForHeight;
    vector<int> numNodesUpToHeight;
    vector<vector<int>> numProperDescendantsForNodeAtHeightPrefixSum;
};

LookupInfo computeLookupInfo(TreeGenerator<NodeData>& tree)
{
    LookupInfo lookupInfo;
    auto rootNode = tree.nodes().front();
    fixParentChildAndHeights(rootNode);
    lookupInfo.maxHeight = -1;
    for (const auto& node : tree.nodes())
    {
        lookupInfo.maxHeight = max(lookupInfo.maxHeight, node->data.height);
    }
    lookupInfo.nodesAtHeightLookup.resize(lookupInfo.maxHeight + 1);
    computeDFSInfo(rootNode, lookupInfo.nodesAtHeightLookup);
    lookupInfo.prefixesForHeight.resize(lookupInfo.maxHeight + 1, MVCN2TST::AVLTree(true));
    lookupInfo.suffixesForHeight.resize(lookupInfo.maxHeight + 1, MVCN2TST::AVLTree(true));
    for (int height = 0; height <= lookupInfo.maxHeight; height++)
    {
        for (const auto nodeAtHeight : lookupInfo.nodesAtHeightLookup[height])
        {
            lookupInfo.prefixesForHeight[height].insertValue(nodeAtHeight->id());
        }
        for (auto nodeAtHeightRevIter = lookupInfo.nodesAtHeightLookup[height].rbegin(); nodeAtHeightRevIter != lookupInfo.nodesAtHeightLookup[height].rend(); nodeAtHeightRevIter++)
        {
            lookupInfo.suffixesForHeight[height].insertValue((*nodeAtHeightRevIter)->id());
        }
    }
    lookupInfo.numNodesUpToHeight.resize(lookupInfo.maxHeight + 1);
    {
        int numNodes = 0;
        for (int height = 0; height <= lookupInfo.maxHeight; height++)
        {
            numNodes += lookupInfo.nodesAtHeightLookup[height].size();
            lookupInfo.numNodesUpToHeight[height] = numNodes;
        }
    }
    lookupInfo.numProperDescendantsForNodeAtHeightPrefixSum.resize(lookupInfo.maxHeight + 1);
    {
        for (int height = 0; height <= lookupInfo.maxHeight; height++)
        {
            const auto& nodesAtHeight = lookupInfo.nodesAtHeightLookup[height];
            int numProperDescendantsSum = 0;
            for (const auto node : nodesAtHeight)
            {
                numProperDescendantsSum += node->data.numDescendants - 1; // "-1" as we want *proper* descendants.
                lookupInfo.numProperDescendantsForNodeAtHeightPrefixSum[height].push_back(numProperDescendantsSum);
            }
        }
    }

    return lookupInfo;
}

class IndexRemapper
{
    public:
        int64_t remapNthRemainingToIndexAndRemove(int64_t nthOfRemainingToChoose)
        {
            // Be optimistic and give remappedIndex the smallest possible value:
            // we'll correct our optimism as we go along :)
            int64_t remappedIndex = nthOfRemainingToChoose;
            MVCN2TST::AVLTreeIterator treeIter(removedIndices.root());
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
        MVCN2TST::AVLTree removedIndices;
};

/**
 * Find the total number of nodes v such that both:
 *
 *  1. v.height <= height; and
 *  2. v is not a descendant of nodeToReparent
 *
 *  Runs in O(N).
 */
int findNumNonDescendantsUpToHeight(TestNode<NodeData>* nodeToReparent, const int height, const vector<int>& numNodesUpToHeight, const vector<vector<TestNode<NodeData>*>>& nodesAtHeightLookup,  const vector<vector<int>>& numProperDescendantsForNodeAtHeightPrefixSum)
{
    int numNonDescendantsUpToThisHeight = numNodesUpToHeight[height];
    if (height >= nodeToReparent->data.height)
    {
        const auto descendantsAtHeightBegin = std::lower_bound(nodesAtHeightLookup[height].begin(), nodesAtHeightLookup[height].end(), nodeToReparent->data.dfsBeginVisit,
                [](const TestNode<NodeData>* node, const int dfsBeginVisit)
                {
                return node->data.dfsBeginVisit < dfsBeginVisit;
                });
        const auto descendantsAtHeightEnd = std::upper_bound(nodesAtHeightLookup[height].begin(), nodesAtHeightLookup[height].end(), nodeToReparent->data.dfsEndVisit,
                [](const int dfsEndVisit, const TestNode<NodeData>* node)
                {
                return dfsEndVisit < node->data.dfsEndVisit;
                });
        const bool hasDescendantsAtThisHeight = descendantsAtHeightBegin != nodesAtHeightLookup[height].end() && (*descendantsAtHeightBegin)->data.dfsEndVisit <= nodeToReparent->data.dfsEndVisit;
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
        numNonDescendantsUpToThisHeight -= nodeToReparent->data.numDescendants - sumOfProperDescendantsOfDescendantsAtHeight;
    }
    return numNonDescendantsUpToThisHeight;
}

MVCN2TST::AVLNode* findKthFromPairAux(int k, MVCN2TST::AVLTree& tree1, MVCN2TST::AVLTree& tree2)
{
    MVCN2TST::AVLTreeIterator tree1Iter = tree1.root();
    while (tree1Iter.currentNode())
    {
        const auto currentValue1 = tree1Iter.currentNode()->value;
        int numToLeft2 = 0;
        MVCN2TST::AVLTreeIterator tree2Iter = tree2.root();
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

MVCN2TST::AVLNode* findKthFromPair(int k, MVCN2TST::AVLTree& tree1, MVCN2TST::AVLTree& tree2)
{
    auto kthAVLNode = findKthFromPairAux(k, tree1, tree2);
    if (!kthAVLNode)
    {
        kthAVLNode = findKthFromPairAux(k, tree2, tree1);
    }
    return kthAVLNode;
}

#if 0
int64_t calcFinalDecryptionKey(vector<TestNode<NodeData>>& nodes, const vector<int64_t>& encryptedQueries)
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

    IndexRemapper indexRemapper;
    vector<int64_t> numCanReparentToPrefixSum;
    int64_t sumOfNumCanReparentTo = 0;
    for (const auto& node : nodes)
    {
        const auto numCanReparentNodeTo = numNodes - node.numDescendants; // Can reparent a node to any of its non-descendants.
        sumOfNumCanReparentTo += numCanReparentNodeTo;
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
    vector<MVCN2TST::AVLTree> prefixesForHeight(maxNodeHeight + 1, MVCN2TST::AVLTree(true));
    vector<MVCN2TST::AVLTree> suffixesForHeight(maxNodeHeight + 1, MVCN2TST::AVLTree(true));
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
        // The AVLTree's prefixesForHeight and suffixesForHeight now represent the node ids to the
        // left and the right of the descendant-range, respectively, in sorted order.
        // Performing the switch is O(1).
        prefixesForHeight[newParentHeight].switchToRevision(numNonDescendantsToLeft);
        suffixesForHeight[newParentHeight].switchToRevision(numNonDescendantsToRight);
        const auto newParentAVLNode = findKthFromPair(numOfReparentingForNodeAndNewHeight, prefixesForHeight[newParentHeight], suffixesForHeight[newParentHeight]);
        assert(newParentAVLNode);
        const auto newParentId = newParentAVLNode->value;
        auto newParent = &(nodes[newParentId - 1]);

        // We've found the required reparenting (nodeToReparent, newParentId).
        // Use it to update the decryptionKey.
        decryptionKey = (decryptionKey + powerOf2 * nodeToReparent->id) % Mod;
        decryptionKey = (decryptionKey + powerOf3 * newParent->id) % Mod;
        powerOf2 = (powerOf2 * 2) % Mod;
        powerOf3 = (powerOf3 * 3) % Mod;
    }

    return decryptionKey;
}
#endif



#endif // UTILS_H
