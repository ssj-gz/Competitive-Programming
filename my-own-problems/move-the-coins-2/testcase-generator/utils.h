#ifndef UTILS_H
#define UTILS_H

#include <tree-generator.h>
#include "disttracker.h"

#include <map>
#include <vector>
#include <fstream>

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
    int grundySubtreeContrib = 0;

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

int findGrundySubtreeContrib(TestNode<NodeData>* subtreeRoot, TestNode<NodeData>* parent)
{
    int grundyContrib = 0;
    if (subtreeRoot->data.numCounters % 2 == 1)
        grundyContrib ^= subtreeRoot->data.height;

    for (auto edge : subtreeRoot->neighbours)
    {
        auto childNode = edge->otherNode(subtreeRoot);
        if (childNode == parent)
            continue;

        grundyContrib ^= findGrundySubtreeContrib(childNode, subtreeRoot);
    }

    subtreeRoot->data.grundySubtreeContrib = grundyContrib;

    return grundyContrib;
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
            // Simulate severing of nodeToReparent.
            assert(currentNode == oldParent);
            continue;
        }
        if (currentNode == nodeToReparent && childNode == nodeToReparent->data.parent)
        {
            // If we're nodeToReparent, don't go exploring up through our new parent!
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
    assert(newParent != oldParent);
    int grundyNumber = 0;
    findGrundyNumberIfRelocatedNodeAux(rootNode, nullptr, 0, nodeToReparent, oldParent, newParent, grundyNumber);

    return grundyNumber;
}

void populateDistTracker(TestNode<NodeData>* currentNode, DistTracker& distTracker, const int nodeHeightAdjustment)
{
    if (currentNode->data.numCounters % 2 == 1)
    {
        assert(currentNode->data.height >= nodeHeightAdjustment);
        distTracker.insertDist(currentNode->data.height - nodeHeightAdjustment);
    }

    for (auto edge : currentNode->neighbours)
    {
        auto childNode = edge->otherNode(currentNode);
        if (childNode == currentNode->data.parent)
            continue;

        populateDistTracker(childNode, distTracker, nodeHeightAdjustment);
    }
}

size_t combineHashes(size_t hash1, const size_t hash2)
{
    // Largely arbitrary "hash combine" function, taken from
    //
    //   https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
    //
    hash1 ^= hash2 + 0x9e3779b9 + (hash1<<6) + (hash1>>2);
    return hash1;
}

void calcTreeHash(TestNode<NodeData>* currentNode, TestNode<NodeData>* parent, int& dfsIndex, std::size_t& treeHash)
{
    // Incorporating a node's dfsIndex, id, and numCounters seems like it would give a reasonably good hash.
    treeHash = combineHashes(treeHash, std::hash<int>()(dfsIndex));
    treeHash = combineHashes(treeHash, std::hash<int>()(currentNode->id()));
    treeHash = combineHashes(treeHash, std::hash<int>()(currentNode->data.numCounters));

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

    const auto treeHash = calcTreeHash(rootNode);
    const std::string treeCacheFilename = "relocated-height-info-cache-" + std::to_string(treeHash);

    // Check cache.
    std::ifstream cacheFileIn(treeCacheFilename);
    if (cacheFileIn)
    {
        const auto allNodes = treeGenerator.nodes();
        for (int i = 0; i < allNodes.size(); i++)
        {
            int nodeToReparentId;
            cacheFileIn >> nodeToReparentId;
            auto nodeToReparent = allNodes[nodeToReparentId - 1];
            cacheFileIn >> nodeToReparent->data.nodeRelocateInfo.maxHeightOfNonDescendent;

            int numNewParentHeightsForBobWin;
            cacheFileIn >> numNewParentHeightsForBobWin;

            for (int i = 0; i < numNewParentHeightsForBobWin; i++)
            {
                int newParentHeight;
                cacheFileIn >> newParentHeight;
                nodeToReparent->data.nodeRelocateInfo.newParentHeightsForBobWin.push_back(newParentHeight);
            }
        }
        
        if (cacheFileIn)
        {
            std::cerr << "Used cached version of newParentHeightsForBobWin" << std::endl;
            return;
        }
    }

    // No cache; compute manually.

    int numNodesProcessed = 0;
    findGrundySubtreeContrib(rootNode, nullptr);
    const int fullTreeGrundy = rootNode->data.grundySubtreeContrib;
    int numPairs = 0;
    for (auto nodeToReparent : treeGenerator.nodes())
    {
        const int maxHeightOfNonDescendent = findMaxHeightOfNonDescendent(rootNode, nullptr, 0, nodeToReparent);
        if (maxHeightOfNonDescendent == -1)
            continue;

        nodeToReparent->data.nodeRelocateInfo.maxHeightOfNonDescendent = maxHeightOfNonDescendent;

        DistTracker distTracker;
        populateDistTracker(nodeToReparent, distTracker, nodeToReparent->data.height - 1); // TODO - not sure why the "- 1" is there(?)

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
                const int grundyNumberIfRelocated = fullTreeGrundy ^ nodeToReparent->data.grundySubtreeContrib ^ distTracker.grundyNumber();

                if (grundyNumberIfRelocated == 0)
                {
                    nodeToReparent->data.nodeRelocateInfo.newParentHeightsForBobWin.push_back(newParentHeight);
                    numPairs++;
                }
                // Only need to check one newParentAtHeight: re-parenting nodeToReparent to any of them will give
                // the same grundy number.
                break;
            }
            assert(foundNewParentAtHeight || newParentHeight == nodeToReparent->data.height - 1);
            distTracker.adjustAllDists(1);
        }
        numNodesProcessed++;
        if (numNodesProcessed % 100 == 0)
        {
            std::cerr << "Processed " << numNodesProcessed << " out of " << treeGenerator.numNodes() << " numPairs: " << numPairs << std::endl;
        }
    }

    // Cache results for next time.
    std::ofstream cacheFileOut(treeCacheFilename);

    for (auto nodeToReparent : treeGenerator.nodes())
    {
        cacheFileOut << nodeToReparent->id() << std::endl;
        cacheFileOut << nodeToReparent->data.nodeRelocateInfo.maxHeightOfNonDescendent << std::endl;
        cacheFileOut << nodeToReparent->data.nodeRelocateInfo.newParentHeightsForBobWin.size() << std::endl;
        for (const auto newParentHeight : nodeToReparent->data.nodeRelocateInfo.newParentHeightsForBobWin)
        {
            cacheFileOut << newParentHeight << " ";
        }
        cacheFileOut << std::endl;
        cacheFileOut << std::endl;
    }
    assert(cacheFileOut);
    cacheFileOut.close();
}


#endif

