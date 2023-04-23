#include <tree-generator.h>
#include <testlib.h>
#include "testcase-generator-lib.h"
#include "random-utilities.h"

#include <iostream>
#include <set>
#include <limits>

constexpr int maxNodes = 200'000;
constexpr int maxNumTestcases = 1000;

struct SubtaskInfo
{
    int subtaskId = -1;
    int score = -1;
    int maxNodesPerTestcase = -1;
    int maxNodesOverAllTestcases = -1;
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
    SubtaskInfo& withMaxNumTestcases(int maxNumTestcases)
    {
        this->maxNumTestcases = maxNumTestcases;
        return *this;
    }
};

const int NoExplicitLimit = std::numeric_limits<int>::max();

// For subtask1, a very naive O(N^3) solution should be able to pass.
SubtaskInfo subtask1 = SubtaskInfo::create().withSubtaskId(1)
                                            .withScore(5)
                                            .withMaxNodesPerTestcase(100)
                                            .withMaxNodesOverAllTestcases(NoExplicitLimit)
                                            .withMaxNumTestcases(100);
                                            
// For subtask2, a naive O(N^2) solution should be able to pass.
SubtaskInfo subtask2 = SubtaskInfo::create().withSubtaskId(2)
                                            .withScore(10)
                                            .withMaxNodesPerTestcase(1000)
                                            .withMaxNodesOverAllTestcases(NoExplicitLimit)
                                            .withMaxNumTestcases(100);

// Sub-quadratic for subtask3, please :)
SubtaskInfo subtask3 = SubtaskInfo::create().withSubtaskId(3)
                                            .withScore(85)
                                            .withMaxNodesPerTestcase(maxNodes)
                                            .withMaxNodesOverAllTestcases(maxNodes)
                                            .withMaxNumTestcases(maxNumTestcases);
                                        
std::ostream& operator<<(std::ostream& outStream, const SubtaskInfo& subtaskInfo)
{
    outStream << "Constraints: " << std::endl;
    outStream << " Maximum num nodes per testcase (i.e. N): " << subtaskInfo.maxNodesPerTestcase << std::endl;
    if (subtaskInfo.maxNodesOverAllTestcases != NoExplicitLimit)
        outStream << " Maximum sum of N over all testcases: " << subtaskInfo.maxNodesOverAllTestcases << std::endl;
    outStream << " Max num testcases (i.e. T): " << subtaskInfo.maxNumTestcases << std::endl;

    return outStream;
};

struct NodeData
{
    bool isSuitable = false;
};

using EQTTestCaseInfo = TestcaseInfo<SubtaskInfo>;
using EQTTestFileInfo = TestFileInfo<SubtaskInfo>;

void writeTestcase(TreeGenerator<NodeData>& treeGenerator, Testcase<SubtaskInfo>& destTestcase)
{
    destTestcase.writeLine(treeGenerator.numNodes());

    for (const auto& edge : treeGenerator.edges())
    {
        destTestcase.writeLine(edge->nodeA->id(), edge->nodeB->id());
    }

    std::vector<int> nodeIsSuitable;
    for (const auto& node : treeGenerator.nodes())
    {
        nodeIsSuitable.push_back(node->data.isSuitable ? 1 : 0);
    }
    destTestcase.writeObjectsAsLine(nodeIsSuitable.begin(), nodeIsSuitable.end());
}

void scrambleAndwriteTestcase(TreeGenerator<NodeData>& treeGenerator, Testcase<SubtaskInfo>& destTestcase)
{
    treeGenerator.scrambleNodeIdsAndReorder(nullptr);
    treeGenerator.scrambleEdgeOrder();

    writeTestcase(treeGenerator, destTestcase);
}


/**
 * Set isSuitable for random nodes, ensuring that the given percentage of nodes have isSuitable == true.
 */
void setRandomSuitable(TreeGenerator<NodeData>& treeGenerator, double percentageSuitable)
{
    const int numSuitable = (percentageSuitable / 100.0) * treeGenerator.numNodes();
    std::vector<bool> nodeIsSuitable(treeGenerator.numNodes(), false);
    for (int i = 0; i < numSuitable; i++)
    {
        nodeIsSuitable[i] = true;
    }
    shuffle(nodeIsSuitable.begin(), nodeIsSuitable.end());
    int nodeIndex = 0;
    for (auto node : treeGenerator.nodes())
    {
        node->data.isSuitable = nodeIsSuitable[nodeIndex];
        nodeIndex++;
    }
}

TestNode<NodeData>* makeSquatGraphWhereAllNodesHaveDegreeAtLeast3(TreeGenerator<NodeData>& treeGenerator, const int approxNumNodes)
{
    auto rootNode = treeGenerator.createNode();

    std::vector<TestNode<NodeData>*> leafNodes;

    for (int i = 0; i < 3; i++)
    {
        leafNodes.push_back(treeGenerator.createNode(rootNode));
    }

    while (treeGenerator.numNodes() < approxNumNodes)
    {
        std::vector<TestNode<NodeData>*> nextLeafNodes;
        for (auto leafNode : leafNodes)
        {
            const int numNewChildren = 2 + (rnd.next(100) == 0 ? 1 : 0);
            for (int i = 0; i < numNewChildren; i++)
            {
                nextLeafNodes.push_back(treeGenerator.createNode(leafNode));
            }

            if (treeGenerator.numNodes() >= approxNumNodes)
                break;
        }

        leafNodes = nextLeafNodes;
    }

    return rootNode;
}

TestNode<NodeData>* makeRandomGrowthPhaseTree(TreeGenerator<NodeData>& treeGenerator, const int numNodes)
{
    auto rootNode = treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.

    const int numGrowthPhases = rnd.next(1, 10);
    const auto numNodesForGrowthPhase = chooseRandomValuesWithSum(numGrowthPhases, numNodes - 1, 1);
    std::vector<double> leafPreferenceForGrowthPhase;
    for (int i = 0; i < numGrowthPhases; i++)
    {
        leafPreferenceForGrowthPhase.push_back(rnd.next(0.0, 100.0));
    }

    for (int growthPhase = 0; growthPhase < numGrowthPhases; growthPhase++)
    {
        treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodesForGrowthPhase[growthPhase], leafPreferenceForGrowthPhase[growthPhase]);
    }

    const double percentage = rnd.next(70.0, 90.0);
    setRandomSuitable(treeGenerator, percentage);

    return rootNode;
}

bool verifyTestFile(TestFileReader& testFileReader, const SubtaskInfo& containingSubtask);

int main(int argc, char* argv[])
{
    TestSuite<SubtaskInfo> testsuite;
    testsuite.setTestFileVerifier(&verifyTestFile);

    // SUBTASK 1
    {
        {
            auto& testFile = testsuite.newTestFile(EQTTestFileInfo().belongingToSubtask(subtask1)
                                                                    .withDescription("Sample testcase")
                                                                    .withSeed(0));

            {
                auto& sampleTestcase1 = testFile.newTestcase(EQTTestCaseInfo().withDescription("Sample testcase 1 - answer 0, as only two suitable sites"));
                TreeGenerator<NodeData> treeGenerator;
                auto one = treeGenerator.createNode();
                auto two = treeGenerator.createNode();
                auto three = treeGenerator.createNode();
                auto four = treeGenerator.createNode();

                for (auto node : {one, two})
                {
                    node->data.isSuitable = true;
                }

                treeGenerator.addEdge(three, two);
                treeGenerator.addEdge(four, three);
                treeGenerator.addEdge(one, three);

                writeTestcase(treeGenerator, sampleTestcase1);
            }
            {
                auto& sampleTestcase2 = testFile.newTestcase(EQTTestCaseInfo().withDescription("Sample testcase 2 - four suitable nodes each connected directly to central unsuitable node; any triple of suitable nodes will do, giving 24 triples"));
                TreeGenerator<NodeData> treeGenerator;
                auto one = treeGenerator.createNode();
                auto two = treeGenerator.createNode();
                auto three = treeGenerator.createNode();
                auto four = treeGenerator.createNode();
                auto five = treeGenerator.createNode();

                for (auto node : {one, two, four, five})
                {
                    node->data.isSuitable = true;
                }

                treeGenerator.addEdge(three, one);
                treeGenerator.addEdge(two, three);
                treeGenerator.addEdge(four, three);
                treeGenerator.addEdge(five, three);

                writeTestcase(treeGenerator, sampleTestcase2);
            }
            {
                auto& sampleTestcase3 = testFile.newTestcase(EQTTestCaseInfo().withDescription("Sample testcase 3 - three suitable nodes, but not equidistant, therefore 0 triples"));
                TreeGenerator<NodeData> treeGenerator;
                auto one = treeGenerator.createNode();
                auto two = treeGenerator.createNode();
                auto three = treeGenerator.createNode();
                auto four = treeGenerator.createNode();
                auto five = treeGenerator.createNode();

                for (auto node : {two, four, five})
                {
                    node->data.isSuitable = true;
                }

                treeGenerator.addEdge(one, three);
                treeGenerator.addEdge(four, three);
                treeGenerator.addEdge(two, one);
                treeGenerator.addEdge(one, five);

                writeTestcase(treeGenerator, sampleTestcase3);
            }
            {
                auto& sampleTestcase4 = testFile.newTestcase(EQTTestCaseInfo().withDescription("Sample testcase 4 - a total of 18 triples, with some triples having a mutual distance of 2; the others, 4"));
                TreeGenerator<NodeData> treeGenerator;
                auto one = treeGenerator.createNode();
                auto two = treeGenerator.createNode();
                auto three = treeGenerator.createNode();
                auto four = treeGenerator.createNode();
                auto five = treeGenerator.createNode();
                auto six = treeGenerator.createNode();
                auto seven = treeGenerator.createNode();
                auto eight = treeGenerator.createNode();
                auto nine = treeGenerator.createNode();

                for (auto node : {one, three, four, five, seven})
                {
                    node->data.isSuitable = true;
                }

                treeGenerator.addEdge(two, six);
                treeGenerator.addEdge(one, two);
                treeGenerator.addEdge(seven, two);
                treeGenerator.addEdge(five, eight);
                treeGenerator.addEdge(three, nine);
                treeGenerator.addEdge(eight, four);
                treeGenerator.addEdge(five, nine);
                treeGenerator.addEdge(two, five);

                writeTestcase(treeGenerator, sampleTestcase4);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(EQTTestFileInfo().belongingToSubtask(subtask1)
                                                                    .withDescription(std::to_string(subtask1.maxNumTestcases) + " random testcases with at most 100 nodes each.")
                                                                    .withSeed(0));
            for (int i = 1; i <= subtask1.maxNumTestcases; i++)
            {
                auto& testcase = testFile.newTestcase(EQTTestCaseInfo());
                const int numNodes = rnd.next(1, subtask1.maxNodesPerTestcase);

                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode();

                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, rnd.next(1.0, 100.0));
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), rnd.next(1.0, 100.0));
                setRandomSuitable(treeGenerator, rnd.next(70.0, 95.0));
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
    }
    
    // SUBTASK 2
    {
        {
            auto& testFile = testsuite.newTestFile(EQTTestFileInfo().belongingToSubtask(subtask2)
                                                                    .withDescription(std::to_string(subtask2.maxNumTestcases) + " random testcases with each having <= 1000 nodes.")
                                    .withSeed(3432432));

            for (int t = 0; t < subtask2.maxNumTestcases; t++)
            {
                auto& testcase = testFile.newTestcase(EQTTestCaseInfo());
                const int numNodes = rnd.next(900, 1000);
                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode();

                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, rnd.next(1.0, 100.0));
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), rnd.next(1.0, 100.0));
                setRandomSuitable(treeGenerator, rnd.next(70.0, 95.0));
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
    }
    
    // SUBTASK 3
    {
        {
            auto& testFile = testsuite.newTestFile(EQTTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(983242));

            {
                auto& testcase = testFile.newTestcase(EQTTestCaseInfo().withDescription("Root node has degree 50'000, then 150'000 random nodes added."));
                const int numNodes = maxNodes;
                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode();

                for (int i = 0; i < 50'000; i++)
                {
                    treeGenerator.createNode(rootNode);
                }

                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, 90);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, 1.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 98);

                setRandomSuitable(treeGenerator, 80.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(EQTTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(122));

            {
                auto& testcase = testFile.newTestcase(EQTTestCaseInfo().withDescription("The core is a squat graph of 50'000 nodes where all nodes have degree at least 3.  50'000 more nodes are then added to the root node, then 100'000 more random nodes."));
                const int numNodes = maxNodes;
                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = makeSquatGraphWhereAllNodesHaveDegreeAtLeast3(treeGenerator, 50'000);

                for (int i = 0; i < 50'000; i++)
                {
                    treeGenerator.createNode(rootNode);
                }

                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, 90);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, 1.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 98);

                setRandomSuitable(treeGenerator, 80.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(EQTTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(38938));

            {
                auto& testcase = testFile.newTestcase(EQTTestCaseInfo().withDescription("The core is a squat graph of 50'000 nodes where all nodes have degree at least 3.  50'000 more nodes are then added to three nodes close to the centre, then 100'000 more random nodes."));
                const int numNodes = maxNodes;
                TreeGenerator<NodeData> treeGenerator;
                makeSquatGraphWhereAllNodesHaveDegreeAtLeast3(treeGenerator, 50'000);

                const auto nodes = treeGenerator.nodes();
                std:: vector<TestNode<NodeData>*> allCentralIshNodes(nodes.begin(), nodes.begin() + 50);
                const auto threeCentralNodes = chooseKRandomFrom(3, allCentralIshNodes);

                treeGenerator.createNodesWithRandomParentPreferringFromSet(threeCentralNodes, 50'000, 99.8,
                        [](auto newNode, auto parent, const bool parentWasPreferred, bool& addNewNodeToSet, bool& removeParentFromSet)
                        {
                            addNewNodeToSet = false;
                            removeParentFromSet = false;
                        });

                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, 90);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, 1.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 98);

                setRandomSuitable(treeGenerator, 83.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(EQTTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(364234));

            {
                auto& testcase = testFile.newTestcase(EQTTestCaseInfo().withDescription("Start with a squat graph of 10'000 nodes where all nodes have degree at least 3.  Then turn all original edges into paths of length 17.  Then finish off with random nodes."));
                const int numNodes = maxNodes;
                TreeGenerator<NodeData> treeGenerator;
                makeSquatGraphWhereAllNodesHaveDegreeAtLeast3(treeGenerator, 10'000);

                treeGenerator.turnEdgesIntoPaths(17);

                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, 90);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, 1.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 98);

                setRandomSuitable(treeGenerator, 83.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(EQTTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(364234));

            {
                auto& testcase = testFile.newTestcase(EQTTestCaseInfo().withDescription("Start with a squat graph of 1'000 nodes where all nodes have degree at least 3.  Then turn all original edges into paths of length 180.  Then finish off with random nodes."));
                const int numNodes = maxNodes;
                TreeGenerator<NodeData> treeGenerator;
                makeSquatGraphWhereAllNodesHaveDegreeAtLeast3(treeGenerator, 1'000);

                treeGenerator.turnEdgesIntoPaths(180);

                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, 90);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, 1.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 98);

                setRandomSuitable(treeGenerator, 83.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(EQTTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(94543));

            {
                auto& testcase = testFile.newTestcase(EQTTestCaseInfo().withDescription("Start with a squat graph of 100 nodes where all nodes have degree at least 3.  Then turn all original edges into paths of length 1800.  Then finish off with random nodes."));
                const int numNodes = maxNodes;
                TreeGenerator<NodeData> treeGenerator;
                makeSquatGraphWhereAllNodesHaveDegreeAtLeast3(treeGenerator, 100);

                treeGenerator.turnEdgesIntoPaths(1800);

                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, 90);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, 1.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 98);

                setRandomSuitable(treeGenerator, 74.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(EQTTestFileInfo().belongingToSubtask(subtask3)
                                                                    .withSeed(94543));

            {
                auto& testcase = testFile.newTestcase(EQTTestCaseInfo().withDescription("Generic squat-ish graph: first 50'000 nodes are grown preferring non-leafs; then remaining 150'000 preferring leaves."));
                const int numNodes = maxNodes;
                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode();


                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(1 * (numNodes - treeGenerator.numNodes()) / 4, 3.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 98);

                setRandomSuitable(treeGenerator, 89.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(EQTTestFileInfo().belongingToSubtask(subtask3)
                                                                    .withSeed(94543));

            {
                auto& testcase = testFile.newTestcase(EQTTestCaseInfo().withDescription("Generic spindly-ish graph, using maxNodes, with three long arms grafted onto it: first ~5000 nodes are grown more-or-less randomly; then ~75'000 preferring leaves; then three arms of length ~24000 are attached to random nodes.  Then the remaining nodes are essentially random.  The exact parameters used for each 'growth' phase were found by a random search that tried to make the 'cheat' solution time-out as hard as it could :)"));
                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode();

                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(4849, 43.3214);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(77511, 96.5569);

                const auto nodesWithoutArms = treeGenerator.nodes();
                for (int i = 0; i < 3; i++)
                {
                    auto armRootNode = nodesWithoutArms[rnd.next(0, static_cast<int>(nodesWithoutArms.size()))];
                    treeGenerator.addNodeChain(armRootNode, 24039);
                }

                // These weird-looking numbers made the cheat solution time-out pretty hard.  No idea why, but not complaining :)
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(11674, 98.2326);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(7678, 15.073);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(26170, 97.8658);

                setRandomSuitable(treeGenerator, 78.0);
                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(EQTTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(113388)
                    .withDescription("max testcases, mostly with about 200 nodes each but in total equalling maxNodesOverAllTestcases."));

            int numNodesInTestFile = 0;
            const auto numNodesForTestCase = chooseRandomValuesWithSum(subtask3.maxNumTestcases, subtask3.maxNodesOverAllTestcases, 1);
            for (const auto numNodes : numNodesForTestCase)
            {
                auto& testcase = testFile.newTestcase(EQTTestCaseInfo());

                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
                const int numNodesPhase1 = rnd.next(numNodes);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodesPhase1, rnd.next(100.0));
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), rnd.next(100.0));
                setRandomSuitable(treeGenerator, rnd.next(100.0));
                scrambleAndwriteTestcase(treeGenerator, testcase);

                numNodesInTestFile += treeGenerator.numNodes();
            }
            assert(numNodesInTestFile == subtask3.maxNodesOverAllTestcases);
        }
        {
            auto& testFile = testsuite.newTestFile(EQTTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(93824)
                    .withDescription("100 testcases, mostly with about 2000 nodes each but in total equalling maxNodesOverAllTestcases."));

            int numNodesInTestFile = 0;
            const auto numNodesForTestCase = chooseRandomValuesWithSum(100, subtask3.maxNodesOverAllTestcases, 1);
            for (const auto numNodes : numNodesForTestCase)
            {
                auto& testcase = testFile.newTestcase(EQTTestCaseInfo());

                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
                const int numNodesPhase1 = rnd.next(numNodes);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodesPhase1, rnd.next(100.0));
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), rnd.next(100.0));
                setRandomSuitable(treeGenerator, rnd.next(100.0));
                scrambleAndwriteTestcase(treeGenerator, testcase);

                numNodesInTestFile += treeGenerator.numNodes();
            }
            assert(numNodesInTestFile == subtask3.maxNodesOverAllTestcases);
        }
        {
            auto& testFile = testsuite.newTestFile(EQTTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(1066015433) // Seed is important - was chosen by randomised search!
                    .withDescription("A testfile designed (by random search!) to make radoslav192-29145890.cpp TLE extra hard.  I have no idea why it works as well as it does :)"));

            {
                auto& testcase = testFile.newTestcase(EQTTestCaseInfo());

                TreeGenerator<NodeData> treeGenerator;
                makeRandomGrowthPhaseTree(treeGenerator, subtask3.maxNodesOverAllTestcases);

                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(EQTTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(2872316772) // Seed is important - was chosen by randomised search!
                    .withDescription("A testfile designed (by random search!) to make radoslav192-29145890.cpp TLE extra hard, redux - this one seems to work better than the previous one, though again, it's a mystery as to why :)"));

            {
                auto& testcase = testFile.newTestcase(EQTTestCaseInfo());

                TreeGenerator<NodeData> treeGenerator;
                makeRandomGrowthPhaseTree(treeGenerator, subtask3.maxNodesOverAllTestcases);

                scrambleAndwriteTestcase(treeGenerator, testcase);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(EQTTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(111)
                    .withDescription("Root node has degree 39'000.  Each neighbour has two chains of length two added to it.  Then random nodes."));

            {
                auto& testcase = testFile.newTestcase(EQTTestCaseInfo());
                const int numNodes = subtask3.maxNodesPerTestcase;

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode();
                for (int i = 0; i < 39'000; i++)
                {
                    treeGenerator.createNode(rootNode);
                }
                for (auto& neighbourEdge : rootNode->neighbours)
                {
                    treeGenerator.addNodeChain(neighbourEdge->otherNode(rootNode), 2);
                }

                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, rnd.next(100.0));
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), rnd.next(100.0));
                setRandomSuitable(treeGenerator, 84.0);
                assert(treeGenerator.numNodes() == numNodes);

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
        testFileReader.addErrorUnless(numNodes <= containingSubtask.maxNodesPerTestcase, "numNodes must be less than or equal to maxNodesPerTestcase, not " + std::to_string(numNodes));

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

        const auto nodeIsSuitable = testFileReader.readLineOfValues<int>(numNodes);
        testFileReader.addErrorUnless(static_cast<int>(nodeIsSuitable.size()) == numNodes, "Line of nodeIsSuitable values should contain " + std::to_string(numNodes) + " values");
        for (const auto isSuitableValue : nodeIsSuitable)
        {
            testFileReader.addErrorUnless(isSuitableValue == 0 || isSuitableValue == 1, "nodeIsSuitable value should be 0 or 1, not " + std::to_string(isSuitableValue));
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

        testFileReader.addErrorUnless(static_cast<int>(visited.size()) == numNodes, "Tree is not connected!");

        testFileReader.markTestcaseAsValidated();
    }
    testFileReader.addErrorUnless(totalNumNodes <= containingSubtask.maxNodesOverAllTestcases, "Total numNodes must be less than or equal to maxNodesOverAllTestcases, not " + std::to_string(totalNumNodes));
    testFileReader.markTestFileAsValidated();

    return true;
}
