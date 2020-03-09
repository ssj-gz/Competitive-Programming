#ifndef UTILS_H
#define UTILS_H

#include <tree-generator.h>

#include <map>
#include <vector>

struct NodeData
{
    int numCounters = -1;
};

/**
 * For each node v in treeGenerator, find the heights h such that there is a node p
 * of height h, with p not a descendent of v, such that re-parenting v to p
 * gives a game board in which Bob wins.
 */
std::map<TestNode<NodeData>*, std::vector<int>> findBobWinningRelocatedHeightsForNodes(const TreeGenerator<NodeData>& treeGenerator)
{
    std::map<TestNode<NodeData>*, std::vector<int>> result;
    return result;
}


#endif

