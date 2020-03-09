#ifndef UTILS_H
#define UTILS_H

#include <tree-generator.h>

#include <map>
#include <vector>

struct NodeData
{
    int numCounters = -1;
};

int findMaxHeightOfNonDescendent(TestNode<NodeData>* currentNode, TestNode<NodeData>* parent, int height, TestNode<NodeData>* nodeToIgnore)
{
    if (currentNode == nodeToIgnore)
        return -1;

    int maxHeightOfDescendent = height; // Current node.

    for (auto edge : currentNode->neighbours)
    {
        auto childNode = edge->otherNode(currentNode);
        if (childNode == parent)
            continue;

        maxHeightOfDescendent = std::max(maxHeightOfDescendent, findMaxHeightOfNonDescendent(childNode, parent, height + 1, nodeToIgnore));
    }

    return maxHeightOfDescendent;
}
/**
 * For each node v in treeGenerator, find the heights h such that there is a node p
 * of height h, with p not a descendent of v, such that re-parenting v to p
 * gives a game board in which Bob wins.
 */
std::map<TestNode<NodeData>*, std::vector<int>> findBobWinningRelocatedHeightsForNodes(const TreeGenerator<NodeData>& treeGenerator)
{
    std::map<TestNode<NodeData>*, std::vector<int>> result;
    auto rootNode = treeGenerator.nodes().front();

    for (auto nodeToReparent : treeGenerator.nodes())
    {
        const int maxHeightOfNonDescendent = findMaxHeightOfNonDescendent(rootNode, nullptr, 0, nodeToReparent);
        if (maxHeightOfNonDescendent == -1)
            continue;
    }
    return result;
}


#endif

