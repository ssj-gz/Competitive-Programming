#include <tree-generator.h>
#include <testlib.h>
#include "testcase-generator-lib.h"

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

void scrambleAndwriteTestcase(TreeGenerator<NodeData>& treeGenerator, Testcase<SubtaskInfo>& destTestcase)
{
    treeGenerator.scrambleNodeIdsAndReorder(nullptr);
    treeGenerator.scrambleEdgeOrder();
    destTestcase.writeLine(treeGenerator.numNodes());

    std::vector<int> numCountersForNode;
    for (const auto& node : treeGenerator.nodes())
    {
        numCountersForNode.push_back(node->data.numCounters);
    }
    destTestcase.writeObjectsAsLine(numCountersForNode.begin(), numCountersForNode.end());
    for (const auto& edge : treeGenerator.edges())
    {
        destTestcase.writeLine(edge->nodeA->id(), edge->nodeB->id());
    }
    std::cout << destTestcase.contents() << std::endl;
}

bool verifyTestFile(std::istream& testFileStream, const SubtaskInfo& containingSubtask)
{
    return true;
}

int main()
{
    TestSuite<SubtaskInfo> testsuite;
    testsuite.setTestFileVerifier(&verifyTestFile);
    {
        auto& testFile = testsuite.newTestFile(TestFileInfo<SubtaskInfo>().belongingToSubtask(subtask1)
                                                                          .withSeed(17666));

        auto& testcase = testFile.newTestcase(TestcaseInfo<SubtaskInfo>().withDescription("max nodes - randomly generated with 2 percent preference for leaves"));

        TreeGenerator<NodeData> treeGenerator;
        auto rootNode = treeGenerator.createNode();
        treeGenerator.createNodesWithRandomParentPreferringLeafNodes(99'999, 2.0);
        for (auto& node : treeGenerator.nodes())
        {
            node->data.numCounters = rnd.next(maxCounters + 1);
        }
        scrambleAndwriteTestcase(treeGenerator, testcase);
    }
}
