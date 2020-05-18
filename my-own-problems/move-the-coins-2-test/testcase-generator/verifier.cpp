#include <iostream>
#include <vector>
#include <algorithm>
#include <deque>
#include <cassert>

#include "verifier.h"
#include "avl-tree.h"

using namespace std;

namespace Verifier
{
    void computeDFSInfo(Node* node, int& dfsVisitNum, vector<vector<Node*>>& nodesAtHeightLookup)
    {
        node->dfsBeginVisit = dfsVisitNum;
        nodesAtHeightLookup[node->height].push_back(node);
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

    int64_t calcFinalDecryptionKey(vector<Node>& nodes, const vector<int64_t>& encryptedQueries, vector<int64_t>& destDecryptedQueries)
    {
        auto rootNode = &(nodes.front());
        fixParentChildAndHeights(rootNode);

        int64_t decryptionKey = 0;
        int64_t powerOf2 = 2;
        int64_t powerOf3 = 3;

        int maxNodeHeight = -1;
        for (const auto& node : nodes)
        {
            maxNodeHeight = max(maxNodeHeight, node.height);
        }
        assert(maxNodeHeight != -1);


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
        vector<AVLTree> prefixesForHeight(maxNodeHeight + 1, AVLTree(true, 10));
        vector<AVLTree> suffixesForHeight(maxNodeHeight + 1, AVLTree(true, 10));
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
            const int64_t decryptedQuery = encryptedQuery ^ decryptionKey;
            destDecryptedQueries.push_back(decryptedQuery);
            const int64_t kthInRemainingToFind = decryptedQuery - 1; // Make 0-relative.

            const int64_t indexInOriginalList = indexRemapper.remapNthRemainingToIndexAndRemove(kthInRemainingToFind);

            const auto firstNodeExceedingIter = std::upper_bound(numCanReparentToPrefixSum.begin(), numCanReparentToPrefixSum.end(), indexInOriginalList);
            const int nodeIndex = firstNodeExceedingIter - numCanReparentToPrefixSum.begin();
            auto nodeToReparent = &(nodes[nodeIndex]);

            // i.e. we now need to find the numOfReparentingThatReparentsNode'th element in the original
            // list that re-parents our nodeToReparent.
            const int64_t numOfReparentingThatReparentsNode = indexInOriginalList - (nodeIndex == 0 ? 0 : numCanReparentToPrefixSum[nodeIndex - 1]);
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
            int64_t numOfReparentingForNodeAndNewHeight = numOfReparentingThatReparentsNode;
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
            const int newParentId = newParentAVLNode->value;
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

}
