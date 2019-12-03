#include <tree-generator.h>
#include <testlib.h>

#include <iostream>

struct NodeData
{
    int numCounters = -1;
};

constexpr int maxNodes = 100'000;
constexpr int maxCounters = 16;

void scrambleAndwriteTestcase(TreeGenerator<NodeData>& treeGenerator, std::ostream& testcaseOutStream)
{
    treeGenerator.scrambleNodeIdsAndReorder(nullptr);
    treeGenerator.scrambleEdgeOrder();
    testcaseOutStream << treeGenerator.numNodes() << std::endl;
    for (const auto& node : treeGenerator.nodes())
    {
        testcaseOutStream << node->data.numCounters << std::endl;
    }
    treeGenerator.printEdges(testcaseOutStream);
}

int main()
{
    rnd.setSeed(17666);
    TreeGenerator<NodeData> treeGenerator;
    auto rootNode = treeGenerator.createNode();
    treeGenerator.createNodesWithRandomParentPreferringLeafNodes(99'999, 2.0);
    for (auto& node : treeGenerator.nodes())
    {
        node->data.numCounters = rnd.next(maxCounters + 1);
    }
    scrambleAndwriteTestcase(treeGenerator, std::cout);
}
