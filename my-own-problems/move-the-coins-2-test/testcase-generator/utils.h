#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <deque>
#include <algorithm>
#include <tree-generator.h>
#include <cassert>

using namespace std;

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

void computeDFSInfo(TestNode<NodeData>* node, TestNode<NodeData>* parent, int& dfsVisitNum)
{
    node->data.dfsBeginVisit = dfsVisitNum;
    dfsVisitNum++;

    for (auto childEdge : node->neighbours)
    {
        auto child = childEdge->otherNode(node);
        if (child == parent)
            continue;
        computeDFSInfo(child, node, dfsVisitNum);
    }

    node->data.dfsEndVisit = dfsVisitNum;
    dfsVisitNum++;
}

void computeDFSInfo(TestNode<NodeData>* rootNode)
{
    int dfsVisitNum = 1;
    computeDFSInfo(rootNode, nullptr, dfsVisitNum);
}

void fillInNodesAtHeightLookup(const vector<TestNode<NodeData>*>& nodes, vector<vector<TestNode<NodeData>*>>& nodesAtHeightLookup)
{
    for (auto node : nodes)
    {
        nodesAtHeightLookup[node->data.height].push_back(node);
    }
    for (auto& nodesAtHeight : nodesAtHeightLookup)
    {
        sort(nodesAtHeight.begin(), nodesAtHeight.end(), [](const auto& lhs, const auto& rhs)
                {
                    return lhs->data.dfsBeginVisit < rhs->data.dfsBeginVisit;
                });
    }
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

void printSubTree(MVCN2TST::AVLNode* subtreeRoot)
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

void printTree(MVCN2TST::AVLTree& tree)
{
    printSubTree(tree.root());
}

struct LookupInfo
{
    int maxHeight = -1;
    vector<vector<TestNode<NodeData>*>> nodesAtHeightLookup;
    vector<MVCN2TST::AVLTree> prefixesForHeight;
    vector<MVCN2TST::AVLTree> suffixesForHeight;
    vector<int> numNodesUpToHeight;
    vector<vector<int>> numProperDescendantsForNodeAtHeightPrefixSum;
    vector<int64_t> numCanReparentToPrefixSum;
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
    computeDFSInfo(rootNode);

    lookupInfo.nodesAtHeightLookup.resize(lookupInfo.maxHeight + 1);
    fillInNodesAtHeightLookup(tree.nodes(), lookupInfo.nodesAtHeightLookup);

    lookupInfo.prefixesForHeight.resize(lookupInfo.maxHeight + 1, MVCN2TST::AVLTree(true));
    lookupInfo.suffixesForHeight.resize(lookupInfo.maxHeight + 1, MVCN2TST::AVLTree(true));
    for (int height = 0; height <= lookupInfo.maxHeight; height++)
    {
        for (const auto nodeAtHeight : lookupInfo.nodesAtHeightLookup[height])
        {
            lookupInfo.prefixesForHeight[height].insertValue(nodeAtHeight->id() - 1);
        }
        for (auto nodeAtHeightRevIter = lookupInfo.nodesAtHeightLookup[height].rbegin(); nodeAtHeightRevIter != lookupInfo.nodesAtHeightLookup[height].rend(); nodeAtHeightRevIter++)
        {
            lookupInfo.suffixesForHeight[height].insertValue((*nodeAtHeightRevIter)->id() - 1);
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
    {
        int64_t sumOfNumCanReparentTo = 0;
        auto nodesInIdOrder = tree.nodes();
        const int numNodes = tree.numNodes();
        sort(nodesInIdOrder.begin(), nodesInIdOrder.end(), [](const auto& lhsNode, const auto& rhsNode) { return lhsNode->id() < rhsNode->id(); });
        for (const auto& node : nodesInIdOrder)
        {
            const auto numCanReparentNodeTo = numNodes - node->data.numDescendants; // Can reparent a node to any of its non-descendants.
            sumOfNumCanReparentTo += numCanReparentNodeTo;
            lookupInfo.numCanReparentToPrefixSum.push_back(sumOfNumCanReparentTo);
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

std::pair<MVCN2TST::AVLNode*, int> findLastLessThanOrEqualTo(int64_t k, MVCN2TST::AVLTree& tree)
{
    MVCN2TST::AVLTreeIterator treeIter = tree.root();
    std::pair<MVCN2TST::AVLNode*, int> result = {nullptr, 0};
    while (treeIter.currentNode())
    {
        
        if (treeIter.currentNode()->value == k)
            return {treeIter.currentNode(), treeIter.numToLeft()};
        else if (treeIter.currentNode()->value > k)
            treeIter.followLeftChild();
        else if (treeIter.currentNode()->value < k)
        {
            result = {treeIter.currentNode(), treeIter.numToLeft()}; // Best candidate so far.
            treeIter.followRightChild();
        }
        else
        {
            assert(false);
        }


    }
    return result;
}

int findIndexOfInPair(int k, MVCN2TST::AVLTree& tree1, MVCN2TST::AVLTree& tree2)
{
    auto node1Info = findLastLessThanOrEqualTo(k, tree1);
    auto node2Info = findLastLessThanOrEqualTo(k, tree2);
    assert((node1Info.first && node1Info.first->value == k) || (node2Info.first && node2Info.first->value == k));
    int index = 0;
    if (node1Info.first)
    {
        index += node1Info.second;
        if (node1Info.first->value < k)
        {
            index ++;
        }
    }
    if (node2Info.first)
    {
        index += node2Info.second;
        if (node2Info.first->value < k)
        {
            index ++;
        }
    }
    return index;
}

#endif // UTILS_H

