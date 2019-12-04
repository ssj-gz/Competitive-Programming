#include <tree-generator.h>
#include <testlib.h>
#include "testcase-generator-lib.h"

#include <iostream>

constexpr int maxNodes = 100'000;
constexpr int maxCounters = 16;
constexpr int maxNumTestcases = 1000;

struct SubtaskInfo
{
    int subtaskId = -1;
    int maxNodesPerTestcase = -1;
    int maxNodesOverAllTestcases = -1;
    int maxNumCountersPerNode = -1;
    int maxNumTestcases = -1;
};

SubtaskInfo subtask1 = { 1, maxNodes, 2 * maxNodes, maxCounters, maxNumTestcases };
SubtaskInfo subtask2 = { 2, 1000, 2 * maxNodes, maxCounters, 10 };

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

bool verifyTestFile(TestFileReader& testFileReader, const SubtaskInfo& containingSubtask);

int main(int argc, char* argv[])
{
    TestSuite<SubtaskInfo> testsuite;
    testsuite.setTestFileVerifier(&verifyTestFile);
    {
        {
            auto& testFile = testsuite.newTestFile(TestFileInfo<SubtaskInfo>().belongingToSubtask(subtask1)
                    .withSeed(122));

            {
                auto& testcase = testFile.newTestcase(TestcaseInfo<SubtaskInfo>().withDescription("max nodes - randomly generated with 2 percent preference for leaves; 80% with counter; 10942 Bob wins"));

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode();
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(99'999, 2.0);
                addCounters(treeGenerator, 80.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
            {
                auto& testcase = testFile.newTestcase(TestcaseInfo<SubtaskInfo>().withDescription("almost max nodes - randomly generated with 7 percent preference for leaves; 70% with counter; 2923 Bob wins"));

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode();
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(99'992, 7.0);
                addCounters(treeGenerator, 70.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(TestFileInfo<SubtaskInfo>().belongingToSubtask(subtask1)
                                                                              .withSeed(88893)
                                                                              .withDescription("lots of small testcases up to 1000 nodes each"));

            int numNodesInTestFile = 0;
            while (numNodesInTestFile < subtask1.maxNodesOverAllTestcases)
            {
                const int numNodes = rnd.next(std::min(subtask1.maxNodesOverAllTestcases - numNodesInTestFile, 1000)) + 1;
                auto& testcase = testFile.newTestcase(TestcaseInfo<SubtaskInfo>());


                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode();
                const int numNodesPhase1 = rnd.next(numNodes);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodesPhase1, rnd.next(100.0));
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(treeGenerator.numNodes() - numNodes, rnd.next(100.0));
                addCounters(treeGenerator, rnd.next(100.0));
                scrambleAndwriteTestcase(treeGenerator, testcase);

                numNodesInTestFile += numNodes;
            }
        }
    }
    testsuite.writeTestFiles();
}

bool verifyTestFile(TestFileReader& testFileReader, const SubtaskInfo& containingSubtask)
{
    using std::cout;
    using std::endl;

    const auto& [numTestCases] = testFileReader.readLine<int>();
    for (int t = 0; t < numTestCases; t++)
    {
        const auto& [numNodes] = testFileReader.readLine<int>();
        
        const auto numCountersForNode = testFileReader.readLineOfValues<int>(numNodes);
        for (const auto numCounters : numCountersForNode)
        {
            testFileReader.addErrorUnless(0 <= numCounters && numCounters <= containingSubtask.maxNumCountersPerNode, "Invalid number of counters for a node");
        }

        for (int i = 0; i < numNodes - 1; i++)
        {
            const auto& [edgeNodeAId, edgeNodeBId] = testFileReader.readLine<int, int>();
        }
        testFileReader.markTestcaseAsValidated();
    }

    return true;
}
