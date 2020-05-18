#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <algorithm>
#include <tree-generator.h>
#include <cassert>

#include "avl-tree.h"

using namespace std;

struct NodeData
{
    int height = -1;
    int id = -1;
    int numDescendants = -1; // Includes the node itself.
    int largestNonDescendantHeight = 0;
    TestNode<NodeData>* parent = nullptr;

    bool isDescendantOf(TestNode<NodeData>& otherNode)
    {
        assert(dfsBeginVisit != -1 && dfsEndVisit != -1 && otherNode.data.dfsBeginVisit != -1 && otherNode.data.dfsEndVisit != -1);
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
    node->data.parent = parent;

    for (auto childEdge : node->neighbours)
    {
        auto child = childEdge->otherNode(node);
        if (child == parent)
            continue;

        fixParentChildAndHeights(child, node, height + 1);
        node->data.numDescendants += child->data.numDescendants;
    }
}

struct LookupInfo
{
    int maxHeight = -1;
    vector<vector<TestNode<NodeData>*>> nodesAtHeightLookup;
    vector<AVLTree> prefixesForHeight;
    vector<AVLTree> suffixesForHeight;
    vector<int> numNodesUpToHeight;
    vector<vector<int>> numProperDescendantsForNodeAtHeightPrefixSum;
    vector<int64_t> numCanReparentToPrefixSum;
    vector<int> allHeights;
};

void findLargestNonDescendants(TestNode<NodeData>* node, TestNode<NodeData>* parent, int& largestHeightSoFar, bool reverseChildOrder)
{
    node->data.largestNonDescendantHeight = max(node->data.largestNonDescendantHeight, largestHeightSoFar);
    largestHeightSoFar = max(largestHeightSoFar, node->data.height);

    vector<TestNode<NodeData>*> children;
    for (auto childEdge : node->neighbours)
    {
        auto child = childEdge->otherNode(node);
        if (child == parent)
            continue;

        children.push_back(child);
    }
    if (reverseChildOrder)
    {
        reverse(children.begin(), children.end());
    }

    for (auto child : children)
    {
        findLargestNonDescendants(child, node, largestHeightSoFar, reverseChildOrder);
    }

}

void findLargestNonDescendants(TestNode<NodeData>* rootNode)
{
    int largestHeightSoFar = 0;
    findLargestNonDescendants(rootNode, nullptr, largestHeightSoFar, false);
    largestHeightSoFar = 0;
    findLargestNonDescendants(rootNode, nullptr, largestHeightSoFar, true);
}

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

    lookupInfo.prefixesForHeight.resize(lookupInfo.maxHeight + 1, AVLTree(true, 10));
    lookupInfo.suffixesForHeight.resize(lookupInfo.maxHeight + 1, AVLTree(true, 10));
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

    lookupInfo.allHeights.clear();
    for (int height = 0; height <= lookupInfo.maxHeight; height++)
    {
        lookupInfo.allHeights.push_back(height);
    }

    findLargestNonDescendants(rootNode);

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
        AVLTree removedIndices;
};

/**
 * Find the total number of nodes v such that both:
 *
 *  1. v.height <= height; and
 *  2. v is not a descendant of nodeToReparent
 *
 *  Runs in O(N).
 */
int findNumNonDescendantsUpToHeight(const TestNode<NodeData>* nodeToReparent, const int height, const vector<int>& numNodesUpToHeight, const vector<vector<TestNode<NodeData>*>>& nodesAtHeightLookup,  const vector<vector<int>>& numProperDescendantsForNodeAtHeightPrefixSum)
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

std::pair<AVLNode*, int> findLastLessThanOrEqualTo(int64_t k, AVLTree& tree)
{
    AVLTreeIterator treeIter = tree.root();
    std::pair<AVLNode*, int> result = {nullptr, 0};
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

int findIndexOfInPair(int k, AVLTree& tree1, AVLTree& tree2)
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



TestNode<NodeData>* findRandomValidNewParent(TestNode<NodeData>* nodeToReparent, const vector<TestNode<NodeData>*>& nodes, const int minNewParentHeight, const int maxNewParentHeight, LookupInfo& lookupInfo)
{
    assert(minNewParentHeight >= 0);
    assert(maxNewParentHeight >= minNewParentHeight);
    assert(maxNewParentHeight <= nodeToReparent->data.largestNonDescendantHeight);
    int64_t minIndex = 0;
    if (nodeToReparent->id() - 1 - 1 >= 0)
    {
        minIndex += lookupInfo.numCanReparentToPrefixSum[nodeToReparent->id() - 1 - 1];
    }
    int64_t numReparentingWithNewHeightLessThanMin = 0;
    if (minNewParentHeight - 1 >= 0)
        numReparentingWithNewHeightLessThanMin = findNumNonDescendantsUpToHeight(nodeToReparent, minNewParentHeight - 1, lookupInfo.numNodesUpToHeight, lookupInfo.nodesAtHeightLookup, lookupInfo.numProperDescendantsForNodeAtHeightPrefixSum);
    minIndex += numReparentingWithNewHeightLessThanMin;
    const int64_t numReparentingWithNewHeightLEMax = findNumNonDescendantsUpToHeight(nodeToReparent, maxNewParentHeight, lookupInfo.numNodesUpToHeight, lookupInfo.nodesAtHeightLookup, lookupInfo.numProperDescendantsForNodeAtHeightPrefixSum);
    const int64_t maxIndex = minIndex + numReparentingWithNewHeightLEMax - numReparentingWithNewHeightLessThanMin - 1;
    const int64_t indexInOriginalList = rnd.next(minIndex, maxIndex);

    TestNode<NodeData>* newParent = nullptr;
    {

        const auto firstNodeExceedingIter = std::upper_bound(lookupInfo.numCanReparentToPrefixSum.begin(), lookupInfo.numCanReparentToPrefixSum.end(), indexInOriginalList);
        const int nodeIndex = firstNodeExceedingIter - lookupInfo.numCanReparentToPrefixSum.begin();
        auto nodeToReparent = nodes[nodeIndex];

        // i.e. we now need to find the numOfReparentingThatReparentsNode'th element in the original
        // list that re-parents our nodeToReparent.
        const int64_t numOfReparentingThatReparentsNode = indexInOriginalList - (nodeIndex == 0 ? 0 : lookupInfo.numCanReparentToPrefixSum[nodeIndex - 1]);
        const auto heightIter = upper_bound(lookupInfo.allHeights.begin(), lookupInfo.allHeights.end(), numOfReparentingThatReparentsNode,
                [nodeToReparent, &lookupInfo](const int numOfReparentingThatReparentsNode, const int height)
                {
                const int numReparentingsUpToHeight = findNumNonDescendantsUpToHeight(nodeToReparent, height, lookupInfo.numNodesUpToHeight, lookupInfo.nodesAtHeightLookup, lookupInfo.numProperDescendantsForNodeAtHeightPrefixSum);
                return numOfReparentingThatReparentsNode < numReparentingsUpToHeight;
                });
        assert(heightIter != lookupInfo.allHeights.end());
        const int newParentHeight = *heightIter;
        assert(newParentHeight != -1);

        // i.e. we now need to find the numOfReparentingThatReparentsNode's item in the original list
        // that reparents nodeToReparent to a newParentHeight whose height is newParentHeight.
        int64_t numOfReparentingForNodeAndNewHeight = numOfReparentingThatReparentsNode;
        if (heightIter != lookupInfo.allHeights.begin())
        {
            numOfReparentingForNodeAndNewHeight -= findNumNonDescendantsUpToHeight(nodeToReparent, *std::prev(heightIter), lookupInfo.numNodesUpToHeight, lookupInfo.nodesAtHeightLookup, lookupInfo.numProperDescendantsForNodeAtHeightPrefixSum);
        }

        const auto descendantsAtHeightBegin = std::lower_bound(lookupInfo.nodesAtHeightLookup[newParentHeight].begin(), lookupInfo.nodesAtHeightLookup[newParentHeight].end(), nodeToReparent->data.dfsBeginVisit,
                [](const auto node, const int dfsBeginVisit)
                {
                return node->data.dfsBeginVisit < dfsBeginVisit;
                });
        const auto descendantsAtHeightEnd = std::upper_bound(lookupInfo.nodesAtHeightLookup[newParentHeight].begin(), lookupInfo.nodesAtHeightLookup[newParentHeight].end(), nodeToReparent->data.dfsEndVisit,
                [](const int dfsEndVisit, const auto node)
                {
                return dfsEndVisit < node->data.dfsEndVisit;
                });
        const bool hasDescendantsAtThisHeight = descendantsAtHeightBegin != lookupInfo.nodesAtHeightLookup[newParentHeight].end() && (*descendantsAtHeightBegin)->data.dfsEndVisit <= nodeToReparent->data.dfsEndVisit;
        int numNonDescendantsToLeft = lookupInfo.nodesAtHeightLookup[newParentHeight].size();
        int numNonDescendantsToRight = 0;
        if (hasDescendantsAtThisHeight)
        {
            numNonDescendantsToLeft = descendantsAtHeightBegin - lookupInfo.nodesAtHeightLookup[newParentHeight].begin();
            numNonDescendantsToRight = lookupInfo.nodesAtHeightLookup[newParentHeight].end() - descendantsAtHeightEnd;
        }
        // The AVLTree's prefixesForHeight and suffixesForHeight now represent the node ids to the
        // left and the right of the descendant-range, respectively, in sorted order.
        // Performing the switch is O(1).
        lookupInfo.prefixesForHeight[newParentHeight].switchToRevision(numNonDescendantsToLeft);
        lookupInfo.suffixesForHeight[newParentHeight].switchToRevision(numNonDescendantsToRight);
        const auto newParentAVLNode = findKthFromPair(numOfReparentingForNodeAndNewHeight, lookupInfo.prefixesForHeight[newParentHeight], lookupInfo.suffixesForHeight[newParentHeight]);
        assert(newParentAVLNode);
        const int newParentId = newParentAVLNode->value;
        newParent = nodes[newParentId];
    }

    assert(!newParent->data.isDescendantOf(*nodeToReparent));

    return newParent;
}

#endif // UTILS_H

