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

struct LookupInfo
{
    int maxHeight = -1;
    vector<vector<TestNode<NodeData>*>> nodesAtHeightLookup;
    vector<AVLTree> prefixesForHeight;
    vector<AVLTree> suffixesForHeight;
    vector<int> numNodesUpToHeight;
    vector<vector<int>> numProperDescendantsForNodeAtHeightPrefixSum;
    vector<int64_t> numCanReparentToPrefixSum;
};

int findNumNonDescendantsUpToHeight(TestNode<NodeData>* nodeToReparent, const int height, const vector<int>& numNodesUpToHeight, const vector<vector<TestNode<NodeData>*>>& nodesAtHeightLookup,  const vector<vector<int>>& numProperDescendantsForNodeAtHeightPrefixSum);

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

    lookupInfo.prefixesForHeight.resize(lookupInfo.maxHeight + 1, AVLTree(true));
    lookupInfo.suffixesForHeight.resize(lookupInfo.maxHeight + 1, AVLTree(true));
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

    vector<int> allHeights;
    for (int height = 0; height <= lookupInfo.maxHeight; height++)
    {
        allHeights.push_back(height);
    }

    for (auto nodeToReparent : tree.nodes())
    {
        auto largestNonDescendantHeightIter = upper_bound(allHeights.begin(), allHeights.end(), 0, [nodeToReparent, &lookupInfo](const int height, const int)
                {
                    return findNumNonDescendantsUpToHeight(nodeToReparent, height, lookupInfo.numNodesUpToHeight, lookupInfo.nodesAtHeightLookup,  lookupInfo.numProperDescendantsForNodeAtHeightPrefixSum) > 0;
                });
        if (largestNonDescendantHeightIter == allHeights.end())
        {
            nodeToReparent->data.largestNonDescendantHeight = 0;
        }
        else
        {
            nodeToReparent->data.largestNonDescendantHeight = *largestNonDescendantHeightIter;
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

#endif // UTILS_H

