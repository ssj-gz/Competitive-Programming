#include <tree-generator.h>
#include <testlib.h>
#include "testcase-generator-lib.h"
#include "random-utilities.h"

#include <iostream>
#include <set>

constexpr int maxNodes = 200'000;
constexpr int maxCounters = 16;
constexpr int maxNumTestcases = 1000;

struct SubtaskInfo
{
    int subtaskId = -1;
    int score = -1;

    int maxNodesPerTestcase = -1;
    int maxNodesOverAllTestcases = -1;
    int maxNumCountersPerNode = -1;
    int maxNumCountersOverAllNodes = -1;
    int maxNumTestcases = -1;

    static SubtaskInfo create()
    {
        return SubtaskInfo();
    }
    SubtaskInfo& withSubtaskId(int subtaskId)
    {
        this->subtaskId = subtaskId;
        return *this;
    }
    SubtaskInfo& withScore(int score)
    {
        this->score = score;
        return *this;
    }
    SubtaskInfo& withMaxNodesPerTestcase(int maxNodesPerTestcase)
    {
        this->maxNodesPerTestcase = maxNodesPerTestcase;
        return *this;
    }
    SubtaskInfo& withMaxNodesOverAllTestcases(int maxNodesOverAllTestcases)
    {
        this->maxNodesOverAllTestcases = maxNodesOverAllTestcases;
        return *this;
    }
    SubtaskInfo& withMaxNumCountersPerNode(int maxNumCountersPerNode)
    {
        this->maxNumCountersPerNode = maxNumCountersPerNode;
        return *this;
    }
    SubtaskInfo& withMaxNumCountersOverAllNodes(int maxNumCountersOverAllNodes)
    {
        this->maxNumCountersOverAllNodes = maxNumCountersOverAllNodes;
        return *this;
    }
    SubtaskInfo& withMaxNumTestcases(int maxNumTestcases)
    {
        this->maxNumTestcases = maxNumTestcases;
        return *this;
    }
};

const int NoExplicitLimit = std::numeric_limits<int>::max();

SubtaskInfo subtask1 = SubtaskInfo::create().withSubtaskId(1)
                                            .withScore(5)
                                            .withMaxNodesPerTestcase(10)
                                            .withMaxNodesOverAllTestcases(NoExplicitLimit)
                                            .withMaxNumCountersPerNode(6)
                                            .withMaxNumCountersOverAllNodes(10)
                                            .withMaxNumTestcases(10);
                                            
SubtaskInfo subtask2 = SubtaskInfo::create().withSubtaskId(2)
                                            .withScore(10)
                                            .withMaxNodesPerTestcase(1000)
                                            .withMaxNodesOverAllTestcases(NoExplicitLimit)
                                            .withMaxNumCountersPerNode(maxCounters)
                                            .withMaxNumCountersOverAllNodes(NoExplicitLimit)
                                            .withMaxNumTestcases(100);

SubtaskInfo subtask3 = SubtaskInfo::create().withSubtaskId(3)
                                            .withScore(85)
                                            .withMaxNodesPerTestcase(maxNodes)
                                            .withMaxNodesOverAllTestcases(2 * maxNodes)
                                            .withMaxNumCountersPerNode(maxCounters)
                                            .withMaxNumCountersOverAllNodes(NoExplicitLimit)
                                            .withMaxNumTestcases(maxNumTestcases);
                                        
std::ostream& operator<<(std::ostream& outStream, const SubtaskInfo& subtaskInfo)
{
    outStream << "Constraints: " << std::endl;
    outStream << " Maximum num nodes per testcase (i.e. N): " << subtaskInfo.maxNodesPerTestcase << std::endl;
    if (subtaskInfo.maxNodesOverAllTestcases != NoExplicitLimit)
        outStream << " Maximum sum of N over all testcases: " << subtaskInfo.maxNodesOverAllTestcases << std::endl;
    outStream << " Max counters initially placed on a node (i.e. c_i): " << subtaskInfo.maxNumCountersPerNode << std::endl;
    if (subtaskInfo.maxNumCountersOverAllNodes != NoExplicitLimit)
        outStream << " Max sum of initial counters over all nodes per testcase (i.e. c_1 + c_2 + ... + c_N) : " << subtaskInfo.maxNumCountersOverAllNodes << std::endl;
    outStream << " Max num testcases (i.e. T): " << subtaskInfo.maxNumTestcases << std::endl;

    return outStream;
};

struct NodeData
{
    int numCounters = -1;
};

using CoTTestCaseInfo = TestcaseInfo<SubtaskInfo>;
using CoTTestFileInfo = TestFileInfo<SubtaskInfo>;

void writeTestCase(TreeGenerator<NodeData>& treeGenerator, Testcase<SubtaskInfo>& destTestcase)
{
    destTestcase.writeLine(treeGenerator.numNodes());

    for (const auto& edge : treeGenerator.edges())
    {
        destTestcase.writeLine(edge->nodeA->id(), edge->nodeB->id());
    }

    std::vector<int> numCountersForNode;
    for (const auto& node : treeGenerator.nodes())
    {
        numCountersForNode.push_back(node->data.numCounters);
    }
    destTestcase.writeObjectsAsLine(numCountersForNode.begin(), numCountersForNode.end());
}

void scrambleAndwriteTestcase(TreeGenerator<NodeData>& treeGenerator, Testcase<SubtaskInfo>& destTestcase)
{
    treeGenerator.scrambleNodeIdsAndReorder(nullptr);
    treeGenerator.scrambleEdgeOrder();

    writeTestCase(treeGenerator, destTestcase);
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

    // SUBTASK 1
    {
        {
            auto& sampleTestFile = testsuite.newTestFile(CoTTestFileInfo().belongingToSubtask(subtask1)
                                                                          .withDescription("Sample testcases")
                                                                          .withSeed(0));
            {
                auto& testcase = sampleTestFile.newTestcase(CoTTestCaseInfo().withDescription("Sample testcase 1"));

                TreeGenerator<NodeData> treeGenerator;

                auto one = treeGenerator.createNode();
                auto two = treeGenerator.createNode();
                auto three = treeGenerator.createNode();

                treeGenerator.addEdge(one, two);
                treeGenerator.addEdge(two, three);

                one->data.numCounters = 1;
                two->data.numCounters = 0;
                three->data.numCounters = 1;

                writeTestCase(treeGenerator, testcase);
            }
            {
                auto& testcase = sampleTestFile.newTestcase(CoTTestCaseInfo().withDescription("Sample testcase 2"));

                TreeGenerator<NodeData> treeGenerator;

                auto one = treeGenerator.createNode();
                auto two = treeGenerator.createNode();
                auto three = treeGenerator.createNode();
                auto four = treeGenerator.createNode();
                auto five = treeGenerator.createNode();
                auto six = treeGenerator.createNode();

                treeGenerator.addEdge(one, two);
                treeGenerator.addEdge(one, three);
                treeGenerator.addEdge(four, three);
                treeGenerator.addEdge(three, five);
                treeGenerator.addEdge(four, six);

                one->data.numCounters = 0;
                two->data.numCounters = 2;
                three->data.numCounters = 2;
                four->data.numCounters = 1;
                five->data.numCounters = 3;
                six->data.numCounters = 2;

                writeTestCase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(CoTTestFileInfo().belongingToSubtask(subtask1)
                    .withSeed(98749873)
                    .withDescription("Misc tiny testcases - purely randomly generated."));

            for (int i = 0; i < subtask1.maxNumTestcases; i++)
            {
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo());
                const int numNodes = 1 + rnd.next(subtask1.maxNodesPerTestcase);
                const int totalCounters = 1 + rnd.next(subtask1.maxNumCountersOverAllNodes);
                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
                while (treeGenerator.numNodes() < numNodes)
                {
                    treeGenerator.createNodeWithRandomParent();
                }
                for (auto& node : treeGenerator.nodes())
                {
                    node->data.numCounters = 0;
                }
                for (int i = 0; i < totalCounters; i++)
                {
                    const int addToNodeId = rnd.next(numNodes);
                    treeGenerator.nodes()[addToNodeId]->data.numCounters++;
                }
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(CoTTestFileInfo().belongingToSubtask(subtask1)
                                                                    .withDescription("Misc tiny testcases - randomly generated to require (naive) state-space searches of ~20'000 - ~40'000 states, which should be solvable with a brute-force implementation"));
            // Random tiny testcases. The seeds have been chosen by trial and error to require
            // a decent number of game states to be explored and to have an "interesting" number of wins for 
            // Bob (i.e. - not all "0 wins for Bob!")
            const std::vector<int64_t> interestingSeeds = {3278532515,3230993539,2064839250,2755995828,1625171967,3635781804,1918932262,222378395,3537075288,2999645071};
            for (const int64_t seed : interestingSeeds)
            {
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo().withSeed(seed));
                const int numNodes = 1 + rnd.next(subtask1.maxNodesPerTestcase);
                const int totalCounters = 1 + rnd.next(subtask1.maxNumCountersOverAllNodes);
                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
                while (treeGenerator.numNodes() < numNodes)
                {
                    treeGenerator.createNodeWithRandomParent();
                }
                for (auto& node : treeGenerator.nodes())
                {
                    node->data.numCounters = 0;
                }
                for (int i = 0; i < totalCounters; i++)
                {
                    const int addToNodeId = rnd.next(numNodes);
                    treeGenerator.nodes()[addToNodeId]->data.numCounters++;
                }
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
    }
    
    // SUBTASK 2
    {
        {
            auto& testFile = testsuite.newTestFile(CoTTestFileInfo().belongingToSubtask(subtask2)
                                                                              .withSeed(9734)
                                                                              .withDescription("100 random testcases with ~1000 nodes"));
            for (int t = 0; t < subtask2.maxNumTestcases; t++)
            {
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo());

                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
                const int numNodes = rnd.next(subtask2.maxNodesPerTestcase - 100, subtask2.maxNodesPerTestcase);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, rnd.next(1.0, 100.0));
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), rnd.next(1.0, 100.0));
                addCounters(treeGenerator, rnd.next(70.0, 95.0));
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(CoTTestFileInfo().belongingToSubtask(subtask2)
                                                                              .withSeed(937439724)
                                                                              .withDescription("Another set of 100 random testcases with ~1000 nodes"));
            for (int t = 0; t < subtask2.maxNumTestcases; t++)
            {
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo());

                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
                const int numNodes = rnd.next(subtask2.maxNodesPerTestcase - 100, subtask2.maxNodesPerTestcase);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, rnd.next(1.0, 100.0));
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), rnd.next(1.0, 100.0));
                addCounters(treeGenerator, rnd.next(70.0, 95.0));
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
    }
    
    // SUBTASK 3
    {
        {
            auto& testFile = testsuite.newTestFile(CoTTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(122));

            {
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo().withDescription("max nodes - randomly generated with 2 percent preference for leaves; 80% with counter; 9489 Bob wins"));

                const auto numNodes = subtask3.maxNodesPerTestcase;

                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - 1, 2.0);
                addCounters(treeGenerator, 80.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
            {
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo().withDescription("almost max nodes - randomly generated with 7 percent preference for leaves; 70% with counter; 2747 Bob wins"));

                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(99'992, 7.0);
                addCounters(treeGenerator, 70.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(CoTTestFileInfo().belongingToSubtask(subtask3)
                    .withDescription("long-ish and jagged"));

            {
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo().withDescription("almost max nodes - 30k long arm then jagged, 85% with counter, 15 Bob wins")
                        .withSeed(1619071763));

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode();
                treeGenerator.addNodeChain(rootNode, 30'000);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(50'000, 1.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(19902, 99.0);
                addCounters(treeGenerator, 85.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
            {
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo().withDescription("almost max nodes - 40k long arm then jagged, 73% with counter, 12 Bob wins")
                        .withSeed(1293843542));

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
            auto& testFile = testsuite.newTestFile(CoTTestFileInfo().belongingToSubtask(subtask3)
                    .withDescription("long"));

            {
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo().withDescription("almost max nodes - 70k long arm; 78% with counter; 7 Bob wins")
                        .withSeed(595100017));

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode();
                treeGenerator.addNodeChain(rootNode, 70'000);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(15'000, 3.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(14876, 80);
                addCounters(treeGenerator, 78.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
            {
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo().withDescription("almost max nodes - 73k long arm; 83% with counter; 6 Bob wins")
                        .withSeed(2643792404));

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode();
                treeGenerator.addNodeChain(rootNode, 73'000);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(11'000, 2.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(15883, 89);
                addCounters(treeGenerator, 83.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(CoTTestFileInfo().belongingToSubtask(subtask3)
                    .withDescription("fat"));

            {
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo().withDescription("almost max nodes four vertices with high degree; 78% with counter; 31166 Bob wins")
                        .withSeed(1018053492));

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode();
                auto node1 = treeGenerator.createNode(rootNode);
                auto node2 = treeGenerator.createNode(rootNode);
                auto node3 = treeGenerator.createNode(node1);
                treeGenerator.createNodesWithRandomParentPreferringFromSet({rootNode, node1, node2, node3}, 70'000, 99.8, 
                        [](auto newNode, auto parent, const bool parentWasPreferred, bool& addNewNodeToSet, bool& removeParentFromSet)
                        {
                        addNewNodeToSet = false;
                        removeParentFromSet = false;
                        });
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(99'998 - treeGenerator.numNodes(), 50);

                addCounters(treeGenerator, 78.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
            {
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo().withDescription("almost max nodes five vertices with high degree; 93% with counter; 33361 Bob wins")
                        .withSeed(2753439527));

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode();
                auto node1 = treeGenerator.createNode(rootNode);
                auto node2 = treeGenerator.createNode(rootNode);
                auto node3 = treeGenerator.createNode(node1);
                auto node4 = treeGenerator.createNode(node2);
                treeGenerator.createNodesWithRandomParentPreferringFromSet({rootNode, node1, node2}, 40'000, 99.8, 
                        [](auto newNode, auto parent, const bool parentWasPreferred, bool& addNewNodeToSet, bool& removeParentFromSet)
                        {
                        addNewNodeToSet = false;
                        removeParentFromSet = false;
                        });
                treeGenerator.createNodesWithRandomParentPreferringFromSet({node1, node3, node4}, 30'000, 99.8, 
                        [](auto newNode, auto parent, const bool parentWasPreferred, bool& addNewNodeToSet, bool& removeParentFromSet)
                        {
                        addNewNodeToSet = false;
                        removeParentFromSet = false;
                        });
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(99'998 - treeGenerator.numNodes(), 80);

                addCounters(treeGenerator, 93.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(CoTTestFileInfo().belongingToSubtask(subtask3)
                    .withDescription("three long arms"));

            {
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo().withDescription("almost max nodes; 3 long arms; 82% with counter; 7 Bob wins")
                        .withSeed(3359440718));

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode();
                treeGenerator.addNodeChain(rootNode, 30'000);
                treeGenerator.addNodeChain(rootNode, 29'176);
                treeGenerator.addNodeChain(rootNode, 31'726);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((99'999 - treeGenerator.numNodes()) / 2, 2.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(99'999 - treeGenerator.numNodes(), 60.0);

                addCounters(treeGenerator, 82.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
            {
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo().withDescription("almost max nodes; 3 long arms; 86% with counter; 6 Bob wins")
                        .withSeed(3247438039));

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode();
                treeGenerator.addNodeChain(rootNode, 25'283);
                treeGenerator.addNodeChain(rootNode, 26'348);
                treeGenerator.addNodeChain(rootNode, 34'736);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((99'999 - treeGenerator.numNodes()) / 2, 5.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(99'999 - treeGenerator.numNodes(), 80.0);

                addCounters(treeGenerator, 86.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(CoTTestFileInfo().belongingToSubtask(subtask3)
                    .withDescription("four long arms"));

            {
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo().withDescription("almost max nodes; 4 long arms; 70% with counter; 9 Bob wins")
                        .withSeed(1435389395));

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode();
                treeGenerator.addNodeChain(rootNode, 20'000);
                treeGenerator.addNodeChain(rootNode, 19'282);
                treeGenerator.addNodeChain(rootNode, 17'183);
                treeGenerator.addNodeChain(rootNode, 22'838);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((99'999 - treeGenerator.numNodes()) / 2, 3.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(99'999 - treeGenerator.numNodes(), 70.0);

                addCounters(treeGenerator, 70.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
            {
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo().withDescription("almost max nodes; 4 long arms; 68% with counter; 7 Bob wins")
                        .withSeed(2473290002));

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode();
                treeGenerator.addNodeChain(rootNode, 25'000);
                treeGenerator.addNodeChain(rootNode, 21'778);
                treeGenerator.addNodeChain(rootNode, 15'589);
                treeGenerator.addNodeChain(rootNode, 18'112);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((99'999 - treeGenerator.numNodes()) / 2, 1.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(99'999 - treeGenerator.numNodes(), 90.0);

                addCounters(treeGenerator, 68.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(CoTTestFileInfo().belongingToSubtask(subtask3)
                    .withDescription("fat with long"));

            {
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo().withDescription("max nodes; two nodes with high degree separated by long distance; 72% with counters; 8735 Bob wins")
                        .withSeed(3724110391));
                const auto numNodes = subtask3.maxNodesPerTestcase;

                TreeGenerator<NodeData> treeGenerator;
                auto fatNode1 = treeGenerator.createNode();
                auto fatNode2 = treeGenerator.addNodeChain(fatNode1, rnd.next(58'000, 62'000)).back();
                treeGenerator.createNodesWithRandomParentPreferringFromSet({fatNode1, fatNode2}, 120'000, 99, 
                        [](auto newNode, auto parent, const bool parentWasPreferred, bool& addNewNodeToSet, bool& removeParentFromSet)
                        {
                        addNewNodeToSet = false;
                        removeParentFromSet = false;
                        });
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, 5.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 80.0);

                addCounters(treeGenerator, 72.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
            {
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo().withDescription("max nodes; three nodes with high degree, two separated by long distance; 75% with counters; 354 Bob wins")
                        .withSeed(1994187731));

                const auto numNodes = subtask3.maxNodesPerTestcase;

                TreeGenerator<NodeData> treeGenerator;
                auto fatNode1 = treeGenerator.createNode();
                auto fatNode2 = treeGenerator.addNodeChain(fatNode1, 40'000).back();
                auto fatNode3 = treeGenerator.addNodeChain(fatNode1, 2646).back();
                treeGenerator.createNodesWithRandomParentPreferringFromSet({fatNode1, fatNode2}, 60'000, 99, 
                        [](auto newNode, auto parent, const bool parentWasPreferred, bool& addNewNodeToSet, bool& removeParentFromSet)
                        {
                        addNewNodeToSet = false;
                        removeParentFromSet = false;
                        });
                treeGenerator.createNodesWithRandomParentPreferringFromSet({fatNode1, fatNode2, fatNode3}, 60'000, 99, 
                        [](auto newNode, auto parent, const bool parentWasPreferred, bool& addNewNodeToSet, bool& removeParentFromSet)
                        {
                        addNewNodeToSet = false;
                        removeParentFromSet = false;
                        });
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, 3.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 90.0);

                addCounters(treeGenerator, 75.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(CoTTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(88893)
                    .withDescription("lots of small testcases up to 1000 nodes each"));

            int numNodesInTestFile = 0;
            while (numNodesInTestFile < subtask3.maxNodesOverAllTestcases)
            {
                const int numNodes = rnd.next(std::min(subtask3.maxNodesOverAllTestcases - numNodesInTestFile, 1000)) + 1;
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo());


                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
                const int numNodesPhase1 = rnd.next(numNodes);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodesPhase1, rnd.next(100.0));
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(treeGenerator.numNodes() - numNodes, rnd.next(100.0));
                addCounters(treeGenerator, rnd.next(100.0));
                scrambleAndwriteTestcase(treeGenerator, testcase);

                numNodesInTestFile += numNodes;
            }
        }
        {
            auto& testFile = testsuite.newTestFile(CoTTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(113388)
                    .withDescription("max testcases, mostly with about 200 nodes each but in total equalling maxNodesOverAllTestcases"));

            int numNodesInTestFile = 0;
            const auto numNodesForTestCase = chooseRandomValuesWithSum(subtask3.maxNumTestcases, subtask3.maxNodesOverAllTestcases, 1);
            for (const auto numNodes : numNodesForTestCase)
            {
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo());

                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
                const int numNodesPhase1 = rnd.next(numNodes);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodesPhase1, rnd.next(100.0));
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(treeGenerator.numNodes() - numNodes, rnd.next(100.0));
                addCounters(treeGenerator, rnd.next(100.0));
                scrambleAndwriteTestcase(treeGenerator, testcase);

                numNodesInTestFile += numNodes;
            }
        }
        {
            auto& testFile = testsuite.newTestFile(CoTTestFileInfo().belongingToSubtask(subtask3)
                    .withDescription("misc fat testcases"));
            {
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo().withDescription("fat node where each 'tendril' has length at least two; 71% with counter; 22325 Bob wins")
                        .withSeed(1808886558));

                const auto numNodes = subtask3.maxNodesPerTestcase;

                TreeGenerator<NodeData> treeGenerator;
                auto fatNode = treeGenerator.createNode();
                while (treeGenerator.numNodes() < 162'000)
                {
                    treeGenerator.addNodeChain(fatNode, 2);
                }
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, 1.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 90.0);

                addCounters(treeGenerator, 71.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
            {
                auto& testcase = testFile.newTestcase(CoTTestCaseInfo().withDescription("fat node with two long arms; 65% with counter; 7 Bob wins")
                                .withSeed(3340030381));

                const auto numNodes = subtask3.maxNodesPerTestcase;

                TreeGenerator<NodeData> treeGenerator;
                auto fatNode = treeGenerator.createNode();
                treeGenerator.createNodesWithRandomParentPreferringFromSet({fatNode}, 66'246, 99, 
                        [](auto newNode, auto parent, const bool parentWasPreferred, bool& addNewNodeToSet, bool& removeParentFromSet)
                        {
                        addNewNodeToSet = false;
                        removeParentFromSet = false;
                        });
                treeGenerator.addNodeChain(fatNode, rnd.next(40'000, 50'000));
                treeGenerator.addNodeChain(fatNode, rnd.next(40'000, 50'000));
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, 15.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 80.0);

                addCounters(treeGenerator, 65.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);

            }
        }
    }
    const bool validatedAndWrittenSuccessfully = testsuite.writeTestFiles();
    if (!validatedAndWrittenSuccessfully)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
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
        
        for (int i = 0; i < numNodes - 1; i++)
        {
            const auto& [edgeNodeAId, edgeNodeBId] = testFileReader.readLine<int, int>();
            testFileReader.addErrorUnless(0 <= edgeNodeAId && edgeNodeAId <= numNodes, "Invalid node id " + std::to_string(edgeNodeAId));
            testFileReader.addErrorUnless(0 <= edgeNodeBId && edgeNodeBId <= numNodes, "Invalid node id " + std::to_string(edgeNodeBId));
            
            nodes[edgeNodeAId - 1].neighbours.push_back(&(nodes[edgeNodeBId - 1]));
            nodes[edgeNodeBId - 1].neighbours.push_back(&(nodes[edgeNodeAId - 1]));
        }

        const auto numCountersForNode = testFileReader.readLineOfValues<int>(numNodes);
        int numCountersAcrossAllNodes = 0;
        for (const auto numCounters : numCountersForNode)
        {
            numCountersAcrossAllNodes += numCounters;
            testFileReader.addErrorUnless(0 <= numCounters && numCounters <= containingSubtask.maxNumCountersPerNode, "Invalid number of counters for a node");
        }

        testFileReader.addErrorUnless(numCountersAcrossAllNodes <= containingSubtask.maxNumCountersOverAllNodes, "Too many counters across all nodes");


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

        testFileReader.addErrorUnless(static_cast<int>(visited.size()) == numNodes, "Tree is not connected!");

        testFileReader.markTestcaseAsValidated();
    }
    testFileReader.addErrorUnless(totalNumNodes <= containingSubtask.maxNodesOverAllTestcases, "Total numNodes must be less than or equal to maxNodesOverAllTestcases, not " + std::to_string(totalNumNodes));
    testFileReader.markTestFileAsValidated();

    return true;
}
