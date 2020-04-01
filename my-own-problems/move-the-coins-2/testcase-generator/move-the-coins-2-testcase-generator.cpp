#include <tree-generator.h>
#include <testlib.h>
#include "testcase-generator-lib.h"
#include "random-utilities.h"
#include "utils.h"

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

using MC2TestCaseInfo = TestcaseInfo<SubtaskInfo>;
using MC2TestFileInfo = TestFileInfo<SubtaskInfo>;

struct TestQuery
{
    TestNode<NodeData>* nodeToReparent = nullptr;
    TestNode<NodeData>* newParentNode = nullptr;
    bool isBobWin = false;
};

void writeTestCase(TreeGenerator<NodeData>& treeGenerator, Testcase<SubtaskInfo>& destTestcase, const std::vector<TestQuery>& queries)
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

    destTestcase.writeLine(queries.size());
    for (const auto& query : queries)
    {
        destTestcase.writeLine(query.nodeToReparent->id(), query.newParentNode->id());
    }
}

void scrambleAndwriteTestcase(TreeGenerator<NodeData>& treeGenerator, Testcase<SubtaskInfo>& destTestcase, std::vector<TestQuery>& queries)
{
    auto rootNode = treeGenerator.nodes().front();
    treeGenerator.scrambleNodeIdsAndReorder(rootNode /* Ensure that the rootNode keeps its id of 1 */);
    treeGenerator.scrambleEdgeOrder();

    writeTestCase(treeGenerator, destTestcase, queries);
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
        // TODO - remove this and replace with proper testcase - this is just here for debugging/ testing.
        auto& testFile = testsuite.newTestFile(MC2TestFileInfo().belongingToSubtask(subtask1)
                .withSeed(9734)
                .withDescription("TODO - dummy subtask 1 testfile"));
        {
            auto& testcase = testFile.newTestcase(MC2TestCaseInfo());

            TreeGenerator<NodeData> treeGenerator;
            treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
            addCounters(treeGenerator, rnd.next(70.0, 95.0));

            std::vector<TestQuery> queries;

            scrambleAndwriteTestcase(treeGenerator, testcase, queries);
        }
    }

    // SUBTASK 2
    {
        // TODO - remove this and replace with proper testcase - this is just here for debugging/ testing.
        auto& testFile = testsuite.newTestFile(MC2TestFileInfo().belongingToSubtask(subtask2)
                .withSeed(9734)
                .withDescription("TODO - first testcase test"));
        {
            auto& testcase = testFile.newTestcase(MC2TestCaseInfo());

            TreeGenerator<NodeData> treeGenerator;
            treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
            const int numNodes = rnd.next(900, 1000);
            treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, rnd.next(1.0, 100.0));
            treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), rnd.next(1.0, 100.0));
            addCounters(treeGenerator, rnd.next(70.0, 95.0));

            const auto nodesAtHeight = buildNodesAtHeightMap(treeGenerator);
            findBobWinningRelocatedHeightsForNodes(treeGenerator, nodesAtHeight);

            // For debugging, generate a list of queries that are all wins for Bob.
            std::vector<TestQuery> bobWinQueries;
            std::vector<TestQuery> aliceWinQueries;

            std::vector<TestQuery> queries;
            for (auto nodeToReparent : treeGenerator.nodes())
            {
                for (int newParentHeight = 0; newParentHeight <= nodeToReparent->data.nodeRelocateInfo.maxHeightOfNonDescendent; newParentHeight++)
                {
                    const bool isBobWin = (std::find(nodeToReparent->data.nodeRelocateInfo.newParentHeightsForBobWin.begin(), nodeToReparent->data.nodeRelocateInfo.newParentHeightsForBobWin.end(), newParentHeight) != nodeToReparent->data.nodeRelocateInfo.newParentHeightsForBobWin.end());
                    for (auto newParent : nodesAtHeight[newParentHeight])
                    {
                        if (newParent->data.isDescendentOf(nodeToReparent))
                            continue;

                        if (isBobWin)
                            bobWinQueries.push_back({nodeToReparent, newParent, true});
                        else
                            aliceWinQueries.push_back({nodeToReparent, newParent, false});
                    }

                }
            }

            for (const auto bobWinQuery : bobWinQueries)
                queries.push_back(bobWinQuery);
            for (const auto aliceWinQuery : aliceWinQueries)
                queries.push_back(aliceWinQuery);

            std::cerr << "Num bob wins:" << bobWinQueries.size() << std::endl;
            std::cerr << "Num alice wins:" << aliceWinQueries.size() << std::endl;

            scrambleAndwriteTestcase(treeGenerator, testcase, queries);
        }

    }

    // SUBTASK 3
    {
        auto& testFile = testsuite.newTestFile(MC2TestFileInfo().belongingToSubtask(subtask3)
                .withSeed(10992)
                .withDescription("Two long (~80k) arms originating at (or near) root; then 30% of remaining of bristles; the rest, leaves.  200k nodes total"));
        {
            auto& testcase = testFile.newTestcase(MC2TestCaseInfo());

            const int numNodes = 200'000;

            TreeGenerator<NodeData> treeGenerator;
            auto rootNode = treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
            const auto mainArm = treeGenerator.addNodeChain(rootNode, rnd.next(78'000, 82'000));
            const int posOfSecondArmAlongMain = rnd.next(400, 500);
            treeGenerator.addNodeChain(mainArm[posOfSecondArmAlongMain], rnd.next(78'000, 82'000));

            treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) * 3 / 10, 70.0);
            treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 98.0);

            addCounters(treeGenerator, rnd.next(78.0, 85.0));

            const auto nodesAtHeight = buildNodesAtHeightMap(treeGenerator);
            findBobWinningRelocatedHeightsForNodes(treeGenerator, nodesAtHeight);

            std::vector<TestQuery> queries;
            scrambleAndwriteTestcase(treeGenerator, testcase, queries);
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

struct Query
{
    Node* nodeToReparent = nullptr;
    Node* newParentNode = nullptr;
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

        const auto [numQueries] = testFileReader.readLine<int>();
        std::vector<Query> queries;
        for (int i = 0; i < numQueries; i++)
        {
            const auto& [nodeToReparentId, newParentNodeId] = testFileReader.readLine<int, int>();
            // TODO - validation of queries.
            queries.push_back({&(nodes[nodeToReparentId - 1]), &(nodes[newParentNodeId - 1])});
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
