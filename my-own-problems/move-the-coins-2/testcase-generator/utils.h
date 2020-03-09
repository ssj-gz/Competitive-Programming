#ifndef UTILS_H
#define UTILS_H

#include <tree-generator.h>

#include <map>
#include <vector>

struct NodeRelocateInfo
{
    int maxHeightOfNonDescendent = -1;
    std::vector<int> newParentHeightsForBobWin;
};

struct NodeData
{
    int numCounters = -1;

    TestNode<NodeData>* parent = nullptr;
    int height = -1;
    int dfsVisitBegin = -1;
    int dfsVisitEnd = -1;

    NodeRelocateInfo nodeRelocateInfo;
    bool isDescendentOf(TestNode<NodeData>* node)
    {
        return dfsVisitBegin >= node->data.dfsVisitBegin && 
               dfsVisitEnd <= node->data.dfsVisitEnd; 
    }
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

        fillInNodeHeightsAndVisitInfoAux(childNode, currentNode, height + 1, dfsIndex);
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

        maxHeightOfDescendent = std::max(maxHeightOfDescendent, findMaxHeightOfNonDescendent(childNode, currentNode, height + 1, nodeToIgnore));
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
        if (currentNode == nodeToReparent && childNode == oldParent)
            continue;


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
    assert(newParent != oldParent);
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

std::vector<std::vector<TestNode<NodeData>*>> buildNodesAtHeightMap(const TreeGenerator<NodeData>& treeGenerator)
{
    fillInNodeHeightsAndVisitInfo(treeGenerator.nodes().front());
    std::vector<std::vector<TestNode<NodeData>*>> nodesAtHeight(treeGenerator.numNodes());
    for (auto node : treeGenerator.nodes())
        nodesAtHeight[node->data.height].push_back(node);

    return nodesAtHeight;
}


/**
 * For each node v in treeGenerator, find the heights h such that there is a node p
 * of height h, with p not a descendent of v, such that re-parenting v to p
 * gives a game board in which Bob wins.
 */
void findBobWinningRelocatedHeightsForNodes(const TreeGenerator<NodeData>& treeGenerator, const std::vector<std::vector<TestNode<NodeData>*>>& nodesAtHeight)
{
    auto rootNode = treeGenerator.nodes().front();

    int numNodesProcessed = 0;
    for (auto nodeToReparent : treeGenerator.nodes())
    {
        const int maxHeightOfNonDescendent = findMaxHeightOfNonDescendent(rootNode, nullptr, 0, nodeToReparent);
        if (maxHeightOfNonDescendent == -1)
            continue;

        nodeToReparent->data.nodeRelocateInfo.maxHeightOfNonDescendent = maxHeightOfNonDescendent;

        for (int newParentHeight = 0; newParentHeight <= maxHeightOfNonDescendent; newParentHeight++)
        {
            bool foundNewParentAtHeight = false;
            for (auto newParentAtHeight : nodesAtHeight[newParentHeight])
            {
                if (newParentAtHeight == nodeToReparent->data.parent)
                    continue;
                if (newParentAtHeight->data.isDescendentOf(nodeToReparent))
                    continue;

                foundNewParentAtHeight = true;
                const int grundyNumberIfRelocated = findGrundyNumberIfRelocatedNode(rootNode, -1, nodeToReparent, nodeToReparent->data.parent, newParentAtHeight);
                if (grundyNumberIfRelocated == 0)
                {
                    nodeToReparent->data.nodeRelocateInfo.newParentHeightsForBobWin.push_back(newParentHeight);
                }
                // Only need to check one newParentAtHeight: re-parenting nodeToReparent to any of them will give
                // the same grundy number.
                break;
            }
            assert(foundNewParentAtHeight || newParentHeight == nodeToReparent->data.height - 1);
        }
        numNodesProcessed++;
        if (numNodesProcessed % 100 == 0)
        {
            std::cerr << "Processed " << numNodesProcessed << " out of " << treeGenerator.numNodes() << std::endl;
        }
    }
}


#endif

