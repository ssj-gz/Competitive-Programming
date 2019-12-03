#include <tree-generator.h>
#include <testlib.h>

#include <iostream>

constexpr int maxNodes = 100'000;
constexpr int maxCounters = 16;
constexpr int maxNumTestcases = 16;

struct SubtaskInfo
{
    int subtaskId = -1;
    int maxNodesPerTestcase = -1;
    int maxNodesOverAllTestcases = -1;
    int maxNumCountersPerNode = -1;
    int maxNumTestcases = -1;
};

SubtaskInfo subtask1 = { 1, maxNodes, 2 * maxNodes, maxCounters, maxNumTestcases };

struct NodeData
{
    int numCounters = -1;
};

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
