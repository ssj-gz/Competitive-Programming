#include <tree-generator.h>
#include <testlib.h>
#include "testcase-generator-lib.h"

#include <iostream>
#include <set>

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

SubtaskInfo subtask2 = { 2, 1000, 2 * maxNodes, maxCounters, 10 };
SubtaskInfo subtask3 = { 3, maxNodes, 2 * maxNodes, maxCounters, maxNumTestcases };

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
            auto& testFile = testsuite.newTestFile(TestFileInfo<SubtaskInfo>().belongingToSubtask(subtask3)
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
            auto& testFile = testsuite.newTestFile(TestFileInfo<SubtaskInfo>().belongingToSubtask(subtask3)
                                                                              .withDescription("long-ish and jagged")
                                                                              .withSeed(123));

            {
                auto& testcase = testFile.newTestcase(TestcaseInfo<SubtaskInfo>().withDescription("almost max nodes - 30k long arm then jagged, 85% with counter, 12 Bob wins")
                                                                                 .withSeed(31050));

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode();
                treeGenerator.addNodeChain(rootNode, 30'000);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(50'000, 1.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(19902, 99.0);
                addCounters(treeGenerator, 85.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
            {
                auto& testcase = testFile.newTestcase(TestcaseInfo<SubtaskInfo>().withDescription("almost max nodes - 40k long arm then jagged, 73% with counter, 13 Bob wins")
                                                                                 .withSeed(93786));

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode();
                treeGenerator.addNodeChain(rootNode, 30'000);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(40'000, 2.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(28'000, 90);
                addCounters(treeGenerator, 74.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(TestFileInfo<SubtaskInfo>().belongingToSubtask(subtask3)
                                                                              .withDescription("long")
                                                                              .withSeed(123));

            {
                auto& testcase = testFile.newTestcase(TestcaseInfo<SubtaskInfo>().withDescription("almost max nodes - 70k long arm; 78% with counter; 10 Bob wins")
                                                                                 .withSeed(31552));

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode();
                treeGenerator.addNodeChain(rootNode, 70'000);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(15'000, 3.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(14876, 80);
                addCounters(treeGenerator, 78.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
            {
                auto& testcase = testFile.newTestcase(TestcaseInfo<SubtaskInfo>().withDescription("almost max nodes - 73k long arm; 83% with counter; 6 Bob wins")
                                                                                 .withSeed(10570));

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode();
                treeGenerator.addNodeChain(rootNode, 73'000);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(14'000, 2.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(15883, 89);
                addCounters(treeGenerator, 83.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(TestFileInfo<SubtaskInfo>().belongingToSubtask(subtask3)
                                                                              .withSeed(88893)
                                                                              .withDescription("lots of small testcases up to 1000 nodes each"));

            int numNodesInTestFile = 0;
            while (numNodesInTestFile < subtask3.maxNodesOverAllTestcases)
            {
                const int numNodes = rnd.next(std::min(subtask3.maxNodesOverAllTestcases - numNodesInTestFile, 1000)) + 1;
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

struct Node
{
    std::vector<Node*> neighbours;
    int nodeId = -1;
};

bool verifyTestFile(TestFileReader& testFileReader, const SubtaskInfo& containingSubtask)
{
    using std::cout;
    using std::endl;

    int64_t totalNumNodes = 0;

    const auto& [numTestCases] = testFileReader.readLine<int>();
    for (int t = 0; t < numTestCases; t++)
    {
        const auto& [numNodes] = testFileReader.readLine<int>();
        testFileReader.addErrorUnless(numNodes >= 1, "numNodes must be greater than or equal to 1, not " + std::to_string(numNodes));
        testFileReader.addErrorUnless(numNodes <= containingSubtask.maxNodesPerTestcase, "numNodes must be greater than or equal to maxNodesPerTestcase, not " + std::to_string(numNodes));

        totalNumNodes += numNodes;

        std::vector<Node> nodes(numNodes);
        for (int i = 0; i < numNodes; i++)
        {
            nodes[i].nodeId = (i + 1);
        }
        
        const auto numCountersForNode = testFileReader.readLineOfValues<int>(numNodes);
        for (const auto numCounters : numCountersForNode)
        {
            testFileReader.addErrorUnless(0 <= numCounters && numCounters <= containingSubtask.maxNumCountersPerNode, "Invalid number of counters for a node");
        }

        for (int i = 0; i < numNodes - 1; i++)
        {
            const auto& [edgeNodeAId, edgeNodeBId] = testFileReader.readLine<int, int>();
            testFileReader.addErrorUnless(0 <= edgeNodeAId && edgeNodeAId <= numNodes, "Invalid node id " + std::to_string(edgeNodeAId));
            testFileReader.addErrorUnless(0 <= edgeNodeBId && edgeNodeBId <= numNodes, "Invalid node id " + std::to_string(edgeNodeBId));
            
            nodes[edgeNodeAId - 1].neighbours.push_back(&(nodes[edgeNodeBId - 1]));
            nodes[edgeNodeBId - 1].neighbours.push_back(&(nodes[edgeNodeAId - 1]));
        }

        int nodeId = 1;
        for (const auto& node : nodes)
        {
            std::set<Node*> distinctNeighbours;
            for (const auto neighbour : node.neighbours)
            {
                testFileReader.addErrorUnless(neighbour != &node, "Node " + std::to_string(node.nodeId) + " is connected to itself!");
                testFileReader.addErrorUnless(distinctNeighbours.find(neighbour) == distinctNeighbours.end(), "Node " + std::to_string(nodeId) + " is connected to node " + std::to_string(neighbour->nodeId) + " more than once!");

                distinctNeighbours.insert(neighbour);
            }
            nodeId++;
        }

        auto rootNode = &(nodes.front());
        std::vector<Node*> toExplore = { rootNode };
        std::set<Node*> visited = { rootNode };
        while (!toExplore.empty())
        {
            std::vector<Node*> nextToExplore;
            for (const auto& node : toExplore)
            {
                for (const auto neighbour : node->neighbours)
                {
                    if (visited.find(neighbour) == visited.end())
                    {
                        nextToExplore.push_back(neighbour);
                        visited.insert(neighbour);
                    }
                }
            }
            toExplore = nextToExplore;
        }

        testFileReader.addErrorUnless(visited.size() == numNodes, "Tree is not connected!");

        testFileReader.markTestcaseAsValidated();
    }
    testFileReader.addErrorUnless(totalNumNodes <= containingSubtask.maxNodesOverAllTestcases, "Total numNodes must be less than or equal to maxNodesOverAllTestcases, not " + std::to_string(totalNumNodes));

    return true;
}
