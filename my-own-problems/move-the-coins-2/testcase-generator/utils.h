#ifndef UTILS_H
#define UTILS_H

#include <tree-generator.h>

#include <map>
#include <vector>

struct NodeData
{
    int numCounters = -1;

    TestNode<NodeData>* parent = nullptr;
    int height = -1;
    int dfsVisitBegin = -1;
    int dfsVisitEnd = -1;
};

void fillInNodeHeightsAndVisitInfoAux(TestNode<NodeData>* currentNode, TestNode<NodeData>* parent, int height, int& dfsIndex)
{
    currentNode->data.height = height;
    currentNode->data.dfsVisitBegin = dfsIndex;
    currentNode->data.parent = parent;
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

void calcTreeHash(TestNode<NodeData>* currentNode, TestNode<NodeData>* parent, int& dfsIndex, std::size_t& treeHash)
{
    // Largely arbitrary "hash combine" function, taken from 
    //  
    //   https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
    //
    // Incorporating a node's dfsIndex, id, and numCounters seems like it would give a reasonably good hash.  The magic numbers are
    // arbitrary.
    treeHash ^= std::hash<int>()(dfsIndex * (currentNode->id() + currentNode->data.numCounters + 876474)) + 0x9e3779b9 + (treeHash<<6) + (treeHash>>2);

    for (auto edge : currentNode->neighbours)
    {
        auto childNode = edge->otherNode(currentNode);
        if (childNode == parent)
            continue;

        calcTreeHash(childNode, currentNode, dfsIndex, treeHash);
    }
}

std::size_t calcTreeHash(TestNode<NodeData>* rootNode)
{
    std::size_t treeHash = 0;
    int dfsIndex = 0;
    calcTreeHash(rootNode, nullptr, dfsIndex, treeHash);
    return treeHash;
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

        for (int newParentHeight = 0; newParentHeight <= maxHeightOfNonDescendent; newParentHeight++)
        {
            bool foundNewParentAtHeight = false;
            for (auto newParentAtHeight : nodesAtHeight[newParentHeight])
            {
                const bool isDescendentOfNodeToReparent = (newParentAtHeight->data.dfsVisitBegin > nodeToReparent->data.dfsVisitBegin &&
                                                           newParentAtHeight->data.dfsVisitEnd < nodeToReparent->data.dfsVisitEnd);
                if (isDescendentOfNodeToReparent)
                    continue;

                foundNewParentAtHeight = true;
                const int grundyNumberIfRelocated = findGrundyNumberIfRelocatedNode(rootNode, -1, nodeToReparent, nodeToReparent->data.parent, newParentAtHeight);
                if (grundyNumberIfRelocated == 0)
                {
                    result[nodeToReparent].push_back(newParentHeight);
                }
                // Only need to check one newParentAtHeight: re-parenting nodeToReparent to any of them will give
                // the same grundy number.
                break;
            }
            assert(foundNewParentAtHeight);
        }
    }
    return result;
}


#endif

