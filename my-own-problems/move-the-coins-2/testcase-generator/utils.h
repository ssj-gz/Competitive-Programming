#ifndef UTILS_H
#define UTILS_H

#include <tree-generator.h>

#include <map>
#include <vector>

struct NodeData
{
    int numCounters = -1;

    int height = -1;
    int dfsVisitBegin = -1;
    int dfsVisitEnd = -1;
};

void fillInNodeHeightsAndVisitInfoAux(TestNode<NodeData>* currentNode, TestNode<NodeData>* parent, int height, int& dfsIndex)
{
    currentNode->data.height = height;
    currentNode->data.dfsVisitBegin = dfsIndex;
    dfsIndex++;

    for (auto edge : currentNode->neighbours)
    {
        auto childNode = edge->otherNode(currentNode);
        if (childNode == parent)
            continue;

        fillInNodeHeightsAndVisitInfoAux(childNode, parent, height + 1, dfsIndex);
    }

    currentNode->data.dfsVisitEnd = dfsIndex;
}

void fillInNodeHeightsAndVisitInfo(TestNode<NodeData>* rootNode)
{
    int dfsIndex = 0;
    fillInNodeHeightsAndVisitInfoAux(rootNode, nullptr, 0, dfsIndex);
}

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

void findGrundyNumberIfRelocatedNodeAux(TestNode<NodeData>* currentNode, TestNode<NodeData>* parent, int height, TestNode<NodeData>* nodeToReparent, TestNode<NodeData>* oldParent, TestNode<NodeData>* newParent, int& grundyNumber)
{
    if (currentNode->data.numCounters % 2 == 1)
        grundyNumber ^= height;

    std::vector<TestNode<NodeData>*> childNodes;

    for (auto edge : currentNode->neighbours)
    {
        auto childNode = edge->otherNode(currentNode);
        if (childNode == parent)
            continue;
        if (childNode == nodeToReparent)
        {
            assert(currentNode == oldParent);
            continue;
        }

        childNodes.push_back(childNode);
    }
    if (currentNode == newParent)
        childNodes.push_back(nodeToReparent);

    for (auto childNode : childNodes)
    {
        findGrundyNumberIfRelocatedNodeAux(childNode, currentNode, height + 1, nodeToReparent, oldParent, newParent, grundyNumber);
    }
}

int findGrundyNumberIfRelocatedNode(TestNode<NodeData>* rootNode, int height, TestNode<NodeData>* nodeToReparent, TestNode<NodeData>* oldParent, TestNode<NodeData>* newParent)
{
    int grundyNumber = 0;
    findGrundyNumberIfRelocatedNodeAux(rootNode, nullptr, 0, nodeToReparent, oldParent, newParent, grundyNumber);

    return grundyNumber;
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

    fillInNodeHeightsAndVisitInfo(rootNode);
    std::vector<std::vector<TestNode<NodeData>*>> nodesAtHeight(treeGenerator.numNodes());
    for (auto node : treeGenerator.nodes())
        nodesAtHeight[node->data.height].push_back(node);

    for (auto nodeToReparent : treeGenerator.nodes())
    {
        const int maxHeightOfNonDescendent = findMaxHeightOfNonDescendent(rootNode, nullptr, 0, nodeToReparent);
        if (maxHeightOfNonDescendent == -1)
            continue;
    }
    return result;
}


#endif

