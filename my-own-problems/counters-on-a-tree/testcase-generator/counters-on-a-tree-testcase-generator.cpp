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
}

/**
 * Add a random number of counters to each node (0 <= c <= maxCounters), ensuring that the
 * given percentage of nodes have an *odd* number of counters.
 */
void addCounters(TreeGenerator<NodeData>& treeGenerator, double percentageWithCounters)
{
    const int numWithCounters = (percentageWithCounters / 100.0) * treeGenerator.numNodes();
    std::vector<bool> nodeHasCounter(treeGenerator.numNodes(), false);
    for (int i = 0; i < numWithCounters; i++)
    {
        nodeHasCounter[i] = true;
    }
    shuffle(nodeHasCounter.begin(), nodeHasCounter.end());
    int nodeIndex = 0;
    for (auto node : treeGenerator.nodes())
    {
        const int randomNumCounters = rnd.next(maxCounters + 1);
        int numCounters = randomNumCounters;
        if (nodeHasCounter[nodeIndex])
        {
            if (numCounters % 2 == 0)
                numCounters = (numCounters == 0 ? 1 : numCounters - 1);
            assert(numCounters % 2 == 1);
        }
        else
        {
            if (numCounters % 2 == 1)
                numCounters = numCounters - 1;
            assert(numCounters % 2 == 0);
        }
        node->data.numCounters = numCounters;

        nodeIndex++;
    }

}

bool verifyTestFile(std::istream& testFileStream, const SubtaskInfo& containingSubtask)
{
    return true;
}

int main(int argc, char* argv[])
{
    TestSuite<SubtaskInfo> testsuite;
    testsuite.setTestFileVerifier(&verifyTestFile);
    {
        auto& testFile = testsuite.newTestFile(TestFileInfo<SubtaskInfo>().belongingToSubtask(subtask1)
                                                                          .withSeed(122));

        {
            auto& testcase = testFile.newTestcase(TestcaseInfo<SubtaskInfo>().withDescription("max nodes - randomly generated with 2 percent preference for leaves; 80% with counter; 10944 Bob wins"));

            TreeGenerator<NodeData> treeGenerator;
            auto rootNode = treeGenerator.createNode();
            treeGenerator.createNodesWithRandomParentPreferringLeafNodes(99'999, 2.0);
            addCounters(treeGenerator, 80.0);
            scrambleAndwriteTestcase(treeGenerator, testcase);
        }
        {
            auto& testcase = testFile.newTestcase(TestcaseInfo<SubtaskInfo>().withDescription("almost max nodes - randomly generated with 7 percent preference for leaves; 70% with counter; 4295 Bob wins"));

            TreeGenerator<NodeData> treeGenerator;
            auto rootNode = treeGenerator.createNode();
            treeGenerator.createNodesWithRandomParentPreferringLeafNodes(99'992, 7.0);
            addCounters(treeGenerator, 70.0);
            scrambleAndwriteTestcase(treeGenerator, testcase);
        }
    }
    testsuite.writeTestFiles();
}
