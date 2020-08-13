#include <tree-generator.h>
#include <testlib.h>
#include "testcase-generator-lib.h"
#include "random-utilities.h"
#include "utils.h"

#include <iostream>
#include <set>

using namespace std;

constexpr int maxNodes = 200'000;
constexpr int maxQueries = 200'000;
constexpr int maxCounters = 16;
constexpr int maxNumTestcases = 1000;

struct SubtaskInfo
{
    int subtaskId = -1;
    int score = -1;

    int maxNodesPerTestcase = -1;
    int maxQueriesPerTestcase = -1;
    int maxNodesOverAllTestcases = -1;
    int maxQueriesOverAllTestcases = -1;
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
    SubtaskInfo& withMaxQueriesPerTestcase(int maxQueriesPerTestcase)
    {
        this->maxQueriesPerTestcase = maxQueriesPerTestcase;
        return *this;
    }
    SubtaskInfo& withMaxNodesOverAllTestcases(int maxNodesOverAllTestcases)
    {
        this->maxNodesOverAllTestcases = maxNodesOverAllTestcases;
        return *this;
    }
    SubtaskInfo& withMaxQueriesOverAllTestcases(int maxQueriesOverAllTestcases)
    {
        this->maxQueriesOverAllTestcases = maxQueriesOverAllTestcases;
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
                                            .withMaxQueriesPerTestcase(10)
                                            .withMaxNodesOverAllTestcases(NoExplicitLimit)
                                            .withMaxNumCountersPerNode(6)
                                            .withMaxNumCountersOverAllNodes(10)
                                            .withMaxNumTestcases(10);
                                            
SubtaskInfo subtask2 = SubtaskInfo::create().withSubtaskId(2)
                                            .withScore(10)
                                            .withMaxNodesPerTestcase(1000)
                                            .withMaxQueriesPerTestcase(1000)
                                            .withMaxNodesOverAllTestcases(NoExplicitLimit)
                                            .withMaxQueriesOverAllTestcases(NoExplicitLimit)
                                            .withMaxNumCountersPerNode(maxCounters)
                                            .withMaxNumCountersOverAllNodes(NoExplicitLimit)
                                            .withMaxNumTestcases(100);

SubtaskInfo subtask3 = SubtaskInfo::create().withSubtaskId(3)
                                            .withScore(85)
                                            .withMaxNodesPerTestcase(maxNodes)
                                            .withMaxQueriesPerTestcase(maxQueries)
                                            .withMaxNodesOverAllTestcases(maxNodes)
                                            .withMaxQueriesOverAllTestcases(maxQueries)
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

void scrambleAndwriteTestcase(TreeGenerator<NodeData>& treeGenerator, Testcase<SubtaskInfo>& destTestcase, const std::vector<TestQuery>& queries)
{
    auto rootNode = treeGenerator.nodes().front();
    treeGenerator.scrambleNodeIdsAndReorder(rootNode /* Ensure that the rootNode keeps its id of 1 */);
    treeGenerator.scrambleEdgeOrder();

    cout << "num bob wins: " << count_if(queries.begin(), queries.end(), [](const auto& query) { return query.isBobWin; }) << endl;

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

vector<TestQuery> generateQueriesFromNodes(const vector<TestNode<NodeData>*>& nodes, int numToGenerate, const double percentageBobWin, const std::vector<std::vector<TestNode<NodeData>*>>& nodesAtHeightMap)
{
    const auto originalNumToGenerate = numToGenerate;

    bool canGenerateAtLeastOneQuery = false;
    for (const auto node : nodes)
    {
        if (node->data.nodeRelocateInfo.maxHeightOfNonDescendent != -1)
        {
            canGenerateAtLeastOneQuery = true;
            break;
        }
    }

    if (!canGenerateAtLeastOneQuery)
        throw std::invalid_argument("Can't generate any queries at all!");

    struct NodeAndHeight
    {
        TestNode<NodeData>* nodeToReparent = nullptr;
        int newParentHeight = -1;
        bool operator<(const NodeAndHeight& other) const
        {
            if (nodeToReparent->id() != other.nodeToReparent->id())
                return nodeToReparent->id() < other.nodeToReparent->id();
            return newParentHeight < other.newParentHeight;
        }
    };
    vector<NodeAndHeight> baseGeneratedQueries;

    vector<NodeAndHeight> bobWinPairs;
    for (auto nodeToReparent : nodes)
    {
        for (const auto newParentHeight : nodeToReparent->data.nodeRelocateInfo.newParentHeightsForBobWin)
        {
            bobWinPairs.push_back({nodeToReparent, newParentHeight});
        }
    }
    const int numAvailableBobWins = bobWinPairs.size();
    const int numBobWinsToGenerate = (numToGenerate * percentageBobWin) / 100.0;
    cout << "numAvailableBobWins: " << numAvailableBobWins << " numBobWinsToGenerate: " << numBobWinsToGenerate << endl;
    if (numBobWinsToGenerate > numAvailableBobWins)
    {
        throw std::invalid_argument("Can't generate requested number of Bob Wins!");
    }

    vector<NodeAndHeight> chosenBobWinPairs;
    for (const auto chosenBobWinIndex : chooseKRandomIndicesFrom(numBobWinsToGenerate, numAvailableBobWins))
    {
        chosenBobWinPairs.push_back(bobWinPairs[chosenBobWinIndex]);
    }

    cout << "chosenBobWinPairs.size(): " << chosenBobWinPairs.size() << endl;

    baseGeneratedQueries.insert(baseGeneratedQueries.end(), chosenBobWinPairs.begin(), chosenBobWinPairs.end());
    numToGenerate -= chosenBobWinPairs.size();

    // Add some random (likely) Alice wins.
    //
    // If the maximum value of newParentHeightsForBobWin.size() is large-ish, the strategy above may lead to disproportionately
    // many repetitions of the same nodeToReparent in chosenBobWinPairs, and conceivably people could use this as a strategy to
    // guess results.  For example: if the number of queries with nodeToReparent v is more than one, it's quite likely that
    // most of the queries that reparent v will be Bob wins.
    // 
    // Attempt to obscure this by choosing Alice Wins with a similar "pattern" to the Bob wins (again, for the case where newParentHeightsForBobWin.size()
    // is large-ish).
    //
    // We do this by picking a nodeToReparent v such that v.newParentHeightsForBobWin.size() is large-ish; picking a nodeToReparent u
    // that has no Bob wins; picking a subset of newParentHeightsForBobWin for v; and "translating" these parent heights to u, adjusting
    // the heights according to the height difference between u and v (the offsets between newParentHeightsForBobWin often have a discernible pattern,
    // which we'll echo in the Alice wins that reparent u in order to add some camouflage).
    vector<NodeAndHeight> generatedHeightOffsetFromBobWinQueries;
    int numHeightOffsetFromBobWins = rnd.next(30.0, 60.0) / 100.0 * numToGenerate;
    cout << "numHeightOffsetFromBobWins: " << numHeightOffsetFromBobWins << endl;
    map<TestNode<NodeData>*, vector<int>> randomBobWinsByNodeToReparent;
    for (const auto chosenBobWinIndex : chooseKRandomIndicesFrom(rnd.next(50.0, 80.0) / 100.0 * numAvailableBobWins , numAvailableBobWins))
    {
        const auto& [nodeToReparent, newParentHeight] = bobWinPairs[chosenBobWinIndex];
        randomBobWinsByNodeToReparent[nodeToReparent].push_back(newParentHeight);
    }
    vector<TestNode<NodeData>*> randomBobWinNodes;
    for (const auto& [nodeToReparent, newParentHeights] : randomBobWinsByNodeToReparent)
    {
        randomBobWinNodes.push_back(nodeToReparent);
    }
    // Give randomBobWinNodes a deterministic ordering.
    sort(randomBobWinNodes.begin(), randomBobWinNodes.end(), [](const auto& lhs, const auto& rhs) { return lhs->id() < rhs->id(); });

    if (!randomBobWinNodes.empty())
    {
        while (numHeightOffsetFromBobWins > 0)
        {
            const auto nodeWithBobWin = randomBobWinNodes[rnd.next(static_cast<int>(randomBobWinNodes.size()))];
            const auto bobWinParentHeights = randomBobWinsByNodeToReparent[nodeWithBobWin];
            const auto nodeToEchoTo = nodes[rnd.next(static_cast<int>(nodes.size()))];
            if (!nodeToEchoTo->data.nodeRelocateInfo.newParentHeightsForBobWin.empty())
            {
                // Prefer to echo to nodes which have no Bob wins, but add some wiggle-room.
                if (rnd.next(0.0, 100.0) / 100.0 >= 10.0)
                    continue;
            }
            if (nodeToEchoTo->data.nodeRelocateInfo.maxHeightOfNonDescendent == -1)
            {
                // Can't reparent nodeToEchoTo, so can't use it in queries.
                continue;
            }

            const int heightAdjustment = nodeToEchoTo->data.height - nodeWithBobWin->data.height;

            for (const auto bobWinParentHeight : bobWinParentHeights)
            {
                int adjustedHeight = bobWinParentHeight - heightAdjustment;
                if (adjustedHeight < 0 || adjustedHeight > nodeToEchoTo->data.nodeRelocateInfo.maxHeightOfNonDescendent)
                {
                    // Invalid height; just pick one at random.
                    adjustedHeight = rnd.next(0, nodeToEchoTo->data.nodeRelocateInfo.maxHeightOfNonDescendent);
                }
                generatedHeightOffsetFromBobWinQueries.push_back({nodeToEchoTo, adjustedHeight});
                numHeightOffsetFromBobWins--;
            }
        }
        cout << "generatedHeightOffsetFromBobWinQueries.size(): " << generatedHeightOffsetFromBobWinQueries.size() << endl;

        // We may have generated too many of this type of query - remove some from the end, if so.
        while (numHeightOffsetFromBobWins < 0)
        {
            generatedHeightOffsetFromBobWinQueries.pop_back();
            numHeightOffsetFromBobWins++;
        }
        baseGeneratedQueries.insert(baseGeneratedQueries.end(), generatedHeightOffsetFromBobWinQueries.begin(), generatedHeightOffsetFromBobWinQueries.end());
        numToGenerate -= generatedHeightOffsetFromBobWinQueries.size();
    }

    if (!baseGeneratedQueries.empty())
    {
        // Now just add a small offset to the height of new parent height for any query generated so far (both Bob and Alice wins).
        int numSmallHeightOffsetToGenerate = rnd.next(30.0, 80.0) / 100.0 * numToGenerate;
        vector<NodeAndHeight> smallHeightOffsetQueries;
        cout << "baseGeneratedQueries.size(): " << baseGeneratedQueries.size() << endl;

        while (numSmallHeightOffsetToGenerate > 0)
        {
            const auto& pairToAdjust = baseGeneratedQueries[rnd.next(static_cast<int>(baseGeneratedQueries.size()))];
            int adjustedHeight = pairToAdjust.newParentHeight + chooseWithWeighting<int>({
                    {-1, 30.0},
                    {+1, 30.0},
                    {-2, 12.5},
                    {+2, 12.5},
                    {-3, 6.0},
                    {+3, 6.0},
                    {-4, 1.0},
                    {+4, 1.0},
                    {+0, 1.0}, // Without the possibility of +0, an Alice-Win nodeToReparent is slightly more 
                               // likely to have more than one query with the same newParentHeight, which is
                               // a pattern that we want to squelch.
                    }, 1).front();
            if (adjustedHeight < 0 || adjustedHeight > pairToAdjust.nodeToReparent->data.nodeRelocateInfo.maxHeightOfNonDescendent)
            {
                // Invalid height; just pick one at random.
                adjustedHeight = rnd.next(0, pairToAdjust.nodeToReparent->data.nodeRelocateInfo.maxHeightOfNonDescendent);
            }

            smallHeightOffsetQueries.push_back({pairToAdjust.nodeToReparent, adjustedHeight});
            numSmallHeightOffsetToGenerate--;
        }

        baseGeneratedQueries.insert(baseGeneratedQueries.end(), smallHeightOffsetQueries.begin(), smallHeightOffsetQueries.end());
        numToGenerate -= smallHeightOffsetQueries.size();
    }

    // Just random choices for the remainder.
    while (numToGenerate > 0)
    {
        const auto nodeToReparent = nodes[rnd.next(static_cast<int>(nodes.size()))];
        if (nodeToReparent->data.nodeRelocateInfo.maxHeightOfNonDescendent == -1)
        {
            // Can't reparent this node, so can't use it in queries.
            continue;
        }

        const auto newParentHeight = rnd.next(0, nodeToReparent->data.nodeRelocateInfo.maxHeightOfNonDescendent);
        if (newParentHeight == -1)
            continue;

        baseGeneratedQueries.push_back({nodeToReparent, newParentHeight});
        numToGenerate--;
    }
    assert(static_cast<int>(baseGeneratedQueries.size()) == originalNumToGenerate);

    vector<TestQuery> generatedQueries;
    // Generate a TestQuery from each baseGeneratedQuery by picking a random node (which it not a descendant of nodeToReparent)
    // for at the newParentHeight.
    for (const auto& [nodeToReparent, newParentHeight] : baseGeneratedQueries)
    {
        const auto& nodesAtNewParentHeight = nodesAtHeightMap[newParentHeight];
        auto firstDescendantIter = std::lower_bound(nodesAtNewParentHeight.begin(), nodesAtNewParentHeight.end(), nodeToReparent->data.dfsVisitBegin,
            [](const auto& lhsNode, const auto& rhsDfsBegin)
            {
                return lhsNode->data.dfsVisitBegin < rhsDfsBegin;
            });
        auto firstPostDescendantIter = std::lower_bound(nodesAtNewParentHeight.begin(), nodesAtNewParentHeight.end(), nodeToReparent->data.dfsVisitEnd,
                [](const auto& lhsNode, const auto& rhsDfsBegin)
                {
                return lhsNode->data.dfsVisitBegin < rhsDfsBegin;
                });

        const int numAtHeightBeforeDescendant = firstDescendantIter - nodesAtNewParentHeight.begin();
        const int numAtHeightAfterDescendant = nodesAtNewParentHeight.size() - (firstPostDescendantIter - nodesAtNewParentHeight.begin());
        const int numNonDescendantsAtHeight = numAtHeightBeforeDescendant + numAtHeightAfterDescendant;
        assert(numNonDescendantsAtHeight > 0);
        const int chosenIndexOfNodeAtHeight = rnd.next(numNonDescendantsAtHeight);

        TestNode<NodeData>* chosenNewParent = nullptr;
        if (chosenIndexOfNodeAtHeight < numAtHeightBeforeDescendant)
        {
            chosenNewParent = nodesAtNewParentHeight[chosenIndexOfNodeAtHeight];
        }
        else
        {
            chosenNewParent = nodesAtNewParentHeight[nodesAtNewParentHeight.size() - numAtHeightAfterDescendant + (chosenIndexOfNodeAtHeight - numAtHeightBeforeDescendant)];
        }
        assert(chosenNewParent);
        generatedQueries.push_back({nodeToReparent, chosenNewParent});
    }
    assert(static_cast<int>(generatedQueries.size()) == originalNumToGenerate);

    sort(bobWinPairs.begin(), bobWinPairs.end());
    for (auto& query : generatedQueries)
    {
        query.isBobWin = binary_search(bobWinPairs.begin(), bobWinPairs.end(), NodeAndHeight{ query.nodeToReparent, query.newParentNode->data.height });
    }

    
    return generatedQueries;
};

void addQueriesAlongFirstHalfOfChain(vector<TestQuery>& destQueries, const vector<TestNode<NodeData>*>& nodeChain, int numToGenerate, const double percentageBobWin, const std::vector<std::vector<TestNode<NodeData>*>>& nodesAtHeightMap)
{
    const vector<TestNode<NodeData>*> chainFirstHalf(nodeChain.begin(), nodeChain.begin() + nodeChain.size() / 2);
    const auto queriesAlongFirstHalfOfChain = generateQueriesFromNodes(chainFirstHalf, numToGenerate, percentageBobWin, nodesAtHeightMap);
    destQueries.insert(destQueries.end(), queriesAlongFirstHalfOfChain.begin(), queriesAlongFirstHalfOfChain.end());
}


bool verifyTestFile(TestFileReader& testFileReader, const SubtaskInfo& containingSubtask);

int main(int argc, char* argv[])
{
    TestSuite<SubtaskInfo> testsuite;
    testsuite.setTestFileVerifier(&verifyTestFile);

    // SUBTASK 1
    {
        {
            auto& testFile = testsuite.newTestFile(MC2TestFileInfo().belongingToSubtask(subtask1)
                    .withSeed(0)
                    .withDescription("Sample test input"));
            {
                auto& testcase = testFile.newTestcase(MC2TestCaseInfo().withDescription("First sample testcase - query 2 is winning for Bob."));

                TreeGenerator<NodeData> treeGenerator;
                auto one = treeGenerator.createNode();
                auto two = treeGenerator.createNode();
                auto three = treeGenerator.createNode();
                auto four = treeGenerator.createNode();

                treeGenerator.addEdge(one, two);
                treeGenerator.addEdge(four, three);
                treeGenerator.addEdge(four, one);

                one->data.numCounters = 1;
                two->data.numCounters = 2;
                three->data.numCounters = 1;
                four->data.numCounters = 1;

                std::vector<TestQuery> queries = { 
                                                    {two, three}, // Alice win.
                                                    {three, one}, // Bob win.
                                                    {three, two}  // Alice win.
                                                 };
                writeTestCase(treeGenerator, testcase, queries); // Don't scramble - should match the sample testcase in the Problem Statement exactly.
            }
            {
                auto& testcase = testFile.newTestcase(MC2TestCaseInfo().withDescription("Second sample testcase - queries 1 and 3 are winning for Bob."));

                TreeGenerator<NodeData> treeGenerator;
                auto one = treeGenerator.createNode();
                auto two = treeGenerator.createNode();
                auto three = treeGenerator.createNode();
                auto four = treeGenerator.createNode();
                auto five = treeGenerator.createNode();
                auto six = treeGenerator.createNode();

                treeGenerator.addEdge(one, two);
                treeGenerator.addEdge(three, one);
                treeGenerator.addEdge(one, four);
                treeGenerator.addEdge(five, three);
                treeGenerator.addEdge(six, four);

                one->data.numCounters = 0;
                two->data.numCounters = 2;
                three->data.numCounters = 0;
                four->data.numCounters = 1;
                five->data.numCounters = 1;
                six->data.numCounters = 0;

                std::vector<TestQuery> queries = {
                                                    {four, two},  // Bob Win.
                                                    {three, four}, // Alice Win.
                                                    {five, one},  // Bob Win.
                                                 };
                writeTestCase(treeGenerator, testcase, queries); // Don't scramble - should match the sample testcase in the Problem Statement exactly.
            }
        }
        {
            auto& testFile = testsuite.newTestFile(MC2TestFileInfo().belongingToSubtask(subtask1)
                    .withSeed(1321223)
                    .withDescription("Misc tiny testcases - purely randomly generated."));

            const vector<int64_t> interestingSeeds = { 240249054,2434200703,4226957038,2973646957,1184999958,2508839353,2488325520,1158695956,3472755837,2056034087 };
            assert(static_cast<int>(interestingSeeds.size()) == subtask1.maxNumTestcases);

            for (const auto seed : interestingSeeds)
            {
                auto& testcase = testFile.newTestcase(MC2TestCaseInfo().withSeed(seed));
                bool generatedTestcase = false;
                while (!generatedTestcase)
                {
                    try
                    {
                        const int numNodes = rnd.next(1, subtask1.maxNodesPerTestcase);
                        const int numQueries = rnd.next(1, subtask1.maxQueriesPerTestcase);
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
                        const auto nodesAtHeight = buildNodesAtHeightMap(treeGenerator);
                        findBobWinningRelocatedHeightsForNodes(treeGenerator, nodesAtHeight);
                        vector<TestQuery> queries;
                        queries = generateQueriesFromNodes(treeGenerator.nodes(), numQueries, rnd.next(30.0, 60.0), nodesAtHeight);
                        assert(queries.size() == numQueries);
                        scrambleAndwriteTestcase(treeGenerator, testcase, queries);
                        generatedTestcase = true;
                    }
                    catch (std::invalid_argument& exception)
                    {
                        cout << "Failed; retrying with different numbers" << endl;
                    } 
                }
            }
        }
    }

    // SUBTASK 2
    {
        auto& testFile = testsuite.newTestFile(MC2TestFileInfo().belongingToSubtask(subtask2)
                .withSeed(9734)
                .withDescription("1000 random, medium size testcases, each with 900-1000 nodes and 1000 queries which are mostly random but biased towards Bob wins."));
        for (int i = 0; i < subtask2.maxNumTestcases; i++)
        {
            cout << "i: " << i << endl;
            auto& testcase = testFile.newTestcase(MC2TestCaseInfo());

            TreeGenerator<NodeData> treeGenerator;
            treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
            const int numNodes = rnd.next(subtask2.maxNodesPerTestcase - 100, subtask2.maxNodesPerTestcase);
            treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, rnd.next(1.0, 100.0));
            treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), rnd.next(1.0, 100.0));
            addCounters(treeGenerator, rnd.next(70.0, 95.0));

            const auto nodesAtHeight = buildNodesAtHeightMap(treeGenerator);
            findBobWinningRelocatedHeightsForNodes(treeGenerator, nodesAtHeight);
            vector<TestQuery> queries;
            while (queries.empty())
            {
                try
                {
                    queries = generateQueriesFromNodes(treeGenerator.nodes(), 1'000, rnd.next(0.0, 40.0), nodesAtHeight);
                }
                catch (std::invalid_argument& exception)
                {
                } 
            }

            scrambleAndwriteTestcase(treeGenerator, testcase, queries);
        }

    }

    // SUBTASK 3
    {
        {
            // seed: 627204325 maxBobWinsForNode: 1
            // seed: 470132031 maxBobWinsForNode: 1
            // seed: 581865272 maxBobWinsForNode: 1
            // seed: 854517857 maxBobWinsForNode: 51
            // seed: 942942163 maxBobWinsForNode: 66
            // seed: 899571059 maxBobWinsForNode: 60
            // seed: 873506264 maxBobWinsForNode: 30
            // seed: 899043427 maxBobWinsForNode: 140
            auto& testFile = testsuite.newTestFile(MC2TestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(10992)
                    .withDescription("Two long (~80k) arms originating at (or near) root; then 30% of remaining of bristles; the rest, leaves.  200k nodes total"));
            {
                auto& testcase = testFile.newTestcase(MC2TestCaseInfo());

                const int numNodes = subtask3.maxNodesOverAllTestcases;
                const int numQueries = subtask3.maxQueriesOverAllTestcases;

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
                const auto mainArm = treeGenerator.addNodeChain(rootNode, rnd.next(78'000, 82'000));
                const int posOfSecondArmAlongMain = rnd.next(400, 500);
                const auto secondArm = treeGenerator.addNodeChain(mainArm[posOfSecondArmAlongMain], rnd.next(78'000, 82'000));

                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) * 3 / 10, 70.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 98.0);

                addCounters(treeGenerator, rnd.next(78.0, 85.0));

                const auto nodesAtHeight = buildNodesAtHeightMap(treeGenerator);
                findBobWinningRelocatedHeightsForNodes(treeGenerator, nodesAtHeight);

                std::vector<TestQuery> queries;
                addQueriesAlongFirstHalfOfChain(queries, mainArm, 85'234, 31.3, nodesAtHeight);
                addQueriesAlongFirstHalfOfChain(queries, secondArm , 91'768, rnd.next(30.0, 60.0), nodesAtHeight);

                const auto remainingQueries = generateQueriesFromNodes(treeGenerator.nodes(), numQueries - queries.size(), rnd.next(30.0, 60.0), nodesAtHeight);
                queries.insert(queries.end(), remainingQueries.begin(), remainingQueries.end());

                scrambleAndwriteTestcase(treeGenerator, testcase, queries);
            }
        }
        {
            // Just storing this data for if I need it; maxBobWinsForNode is, as its named suggests,
            // is the max over all nodes v of v.nodeRelocateInfo.newParentHeightsForBobWin
            // seed: 38432 maxBobWinsForNode: 43
            // seed: 627204325 maxBobWinsForNode: 43
            // seed: 267954190 maxBobWinsForNode: 1
            // seed: 976174522 maxBobWinsForNode: 48
            // seed: 448046354 maxBobWinsForNode: 1
            // seed: 167109192 maxBobWinsForNode: 1
            // seed: 16037662 maxBobWinsForNode: 72
            // seed: 992499319 maxBobWinsForNode: 1
            // seed: 39029083 maxBobWinsForNode: 1
            // seed: 903493763 maxBobWinsForNode: 253
            // seed: 661625455 maxBobWinsForNode: 1
            // seed: 43829431 maxBobWinsForNode: 73
            auto& testFile = testsuite.newTestFile(MC2TestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(16037662)
                    .withDescription("Three long (~50k) arms originating at (or near) root; then 35% of remaining of bristles; the rest, leaves.  200k nodes total.  Queries mainly concentrate on the first halves of each of the three arms."));
            {
                auto& testcase = testFile.newTestcase(MC2TestCaseInfo());

                const int numNodes = subtask3.maxNodesOverAllTestcases;
                const int numQueries = subtask3.maxQueriesOverAllTestcases;

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
                const auto arm1 = treeGenerator.addNodeChain(rootNode, rnd.next(48'000, 52'000));
                const auto arm2 = treeGenerator.addNodeChain(rootNode, rnd.next(48'000, 52'000));
                const int posOf3rdArmAlong1st = rnd.next(700, 800);
                const auto arm3 = treeGenerator.addNodeChain(arm1[posOf3rdArmAlong1st], rnd.next(48'000, 52'000));

                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) * 35 / 100, 70.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 98.0);

                addCounters(treeGenerator, rnd.next(78.0, 85.0));

                const auto nodesAtHeight = buildNodesAtHeightMap(treeGenerator);
                findBobWinningRelocatedHeightsForNodes(treeGenerator, nodesAtHeight);

                std::vector<TestQuery> queries;
                addQueriesAlongFirstHalfOfChain(queries, arm1, rnd.next(58'000, 62'000), 24.5, nodesAtHeight);
                addQueriesAlongFirstHalfOfChain(queries, arm2 , rnd.next(58'000, 62'000), 20.6, nodesAtHeight);
                addQueriesAlongFirstHalfOfChain(queries, arm3 , rnd.next(58'000, 62'000), 21.7, nodesAtHeight);

                const auto remainingQueries = generateQueriesFromNodes(treeGenerator.nodes(), numQueries - queries.size(), rnd.next(30.0, 60.0), nodesAtHeight);
                queries.insert(queries.end(), remainingQueries.begin(), remainingQueries.end());

                scrambleAndwriteTestcase(treeGenerator, testcase, queries);
            }
        }
        {
            // seed: 627204325 maxBobWinsForNode: 49
            // seed: 729046313 maxBobWinsForNode: 1
            // seed: 608207936 maxBobWinsForNode: 1
            // seed: 495782303 maxBobWinsForNode: 65
            // seed: 192950544 maxBobWinsForNode: 29
            // seed: 620712104 maxBobWinsForNode: 1
            // seed: 107466891 maxBobWinsForNode: 108
            // seed: 667447149 maxBobWinsForNode: 58
            auto& testFile = testsuite.newTestFile(MC2TestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(107466891)
                    .withDescription("Four long (~40k) arms originating at (or near) root; then 25% of remaining of bristles; the rest, leaves.  200k nodes total.  Most queries concentrated along first half of each of the 4 arms."));
            {
                auto& testcase = testFile.newTestcase(MC2TestCaseInfo());

                const int numNodes = subtask3.maxNodesOverAllTestcases;
                const int numQueries = subtask3.maxQueriesOverAllTestcases;

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
                const auto arm1 = treeGenerator.addNodeChain(rootNode, rnd.next(38'000, 42'000));
                const auto arm2 = treeGenerator.addNodeChain(rootNode, rnd.next(38'000, 42'000));
                const int posOf3rdArmAlong1st = rnd.next(500, 600);
                const int posOf4thArmAlong2nd = rnd.next(900, 1000);
                const auto arm3 = treeGenerator.addNodeChain(arm1[posOf3rdArmAlong1st], rnd.next(38'000, 42'000));
                const auto arm4 = treeGenerator.addNodeChain(arm2[posOf4thArmAlong2nd], rnd.next(38'000, 42'000));

                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) * 25 / 100, 70.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 98.0);

                addCounters(treeGenerator, rnd.next(78.0, 85.0));

                const auto nodesAtHeight = buildNodesAtHeightMap(treeGenerator);
                findBobWinningRelocatedHeightsForNodes(treeGenerator, nodesAtHeight);

                std::vector<TestQuery> queries;
                // Split out the calculation of numToGenerate and percentageBobWin into separate variables, rather than calculating
                // while passing them as arguments to addQueriesAlongFirstHalfOfChain: the order in which the rnd.next calls are
                // made is important (for maintaining determinancy), but the order in which arguments to a function are evaluated
                // is undefined!
                {
                    const auto percentageBobWin = rnd.next(20.0, 30.0);
                    const auto numToGenerate = rnd.next(43'000, 47'000);
                    addQueriesAlongFirstHalfOfChain(queries, arm1, numToGenerate, percentageBobWin, nodesAtHeight);
                }
                {
                    const auto percentageBobWin = rnd.next(20.0, 40.0);
                    const auto numToGenerate = rnd.next(43'000, 47'000);
                    addQueriesAlongFirstHalfOfChain(queries, arm2 , numToGenerate, percentageBobWin, nodesAtHeight);
                }
                {
                    const auto percentageBobWin = rnd.next(20.0, 40.0);
                    const auto numToGenerate = rnd.next(43'000, 47'000);
                    addQueriesAlongFirstHalfOfChain(queries, arm3 , numToGenerate, percentageBobWin, nodesAtHeight);
                }
                {
                    const auto percentageBobWin = rnd.next(20.0, 40.0);
                    const auto numToGenerate = rnd.next(43'000, 47'000);
                    addQueriesAlongFirstHalfOfChain(queries, arm4 , numToGenerate, percentageBobWin, nodesAtHeight);
                }

                const auto remainingQueries = generateQueriesFromNodes(treeGenerator.nodes(), numQueries - queries.size(), rnd.next(30.0, 60.0), nodesAtHeight);
                queries.insert(queries.end(), remainingQueries.begin(), remainingQueries.end());

                scrambleAndwriteTestcase(treeGenerator, testcase, queries);
            }
        }
        {
            // seed: 627204325 maxBobWinsForNode: 1
            // seed: 610081056 maxBobWinsForNode: 1
            // seed: 286426225 maxBobWinsForNode: 1
            // seed: 133312936 maxBobWinsForNode: 57
            // seed: 361078039 maxBobWinsForNode: 141
            // seed: 206174463 maxBobWinsForNode: 52
            // seed: 479832082 maxBobWinsForNode: 26
            // seed: 854565609 maxBobWinsForNode: 52
            // seed: 343688415 maxBobWinsForNode: 1
            // seed: 328184047 maxBobWinsForNode: 59
            // seed: 862381040 maxBobWinsForNode: 1
            // seed: 443418483 maxBobWinsForNode: 1
            // seed: 176024828 maxBobWinsForNode: 32
            auto& testFile = testsuite.newTestFile(MC2TestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(479832082)
                    .withDescription("Generic squat graph of 100 nodes where all node have degree 3.  Then turn each edge into path of length 1500.  Then fill in the remainder with bristles and leaves.  Queries are essentially random, though heavily biased towards Bob Wins"));
            {
                auto& testcase = testFile.newTestcase(MC2TestCaseInfo());

                const int numNodes = subtask3.maxNodesOverAllTestcases;
                const int numQueries = subtask3.maxQueriesOverAllTestcases;

                TreeGenerator<NodeData> treeGenerator;
                makeSquatGraphWhereAllNodesHaveDegreeAtLeast3(treeGenerator, 100);
                treeGenerator.turnEdgesIntoPaths(1500);

                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) * 25 / 100, 70.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 98.0);

                addCounters(treeGenerator, rnd.next(78.0, 85.0));

                const auto nodesAtHeight = buildNodesAtHeightMap(treeGenerator);
                findBobWinningRelocatedHeightsForNodes(treeGenerator, nodesAtHeight);

                const auto queries = generateQueriesFromNodes(treeGenerator.nodes(), numQueries, 13.8, nodesAtHeight);

                scrambleAndwriteTestcase(treeGenerator, testcase, queries);
            }
        }
        {
            // seed: 627204325 maxBobWinsForNode: 1
            // seed: 478433582 maxBobWinsForNode: 101
            // seed: 493861181 maxBobWinsForNode: 195
            // seed: 449630792 maxBobWinsForNode: 1
            // seed: 852721181 maxBobWinsForNode: 1
            // seed: 716885559 maxBobWinsForNode: 279
            // seed: 510026048 maxBobWinsForNode: 1
            // seed: 333830483 maxBobWinsForNode: 1
            // seed: 930883739 maxBobWinsForNode: 1
            // seed: 128190829 maxBobWinsForNode: 1
            // seed: 285621021 maxBobWinsForNode: 1
            // seed: 200792510 maxBobWinsForNode: 201
            // seed: 605985517 maxBobWinsForNode: 63

            auto& testFile = testsuite.newTestFile(MC2TestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(200792510)
                    .withDescription("Three long (~50k) arms originating at (or near) root (again!); then 33% of remaining of bristles; the rest, leaves.  200k nodes total.  Some queries along the first half of one of the arms, then concentrating on two nodes, each with low-ish height."));
            {
                auto& testcase = testFile.newTestcase(MC2TestCaseInfo());

                const int numNodes = subtask3.maxNodesOverAllTestcases;
                const int numQueries = subtask3.maxQueriesOverAllTestcases;

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
                const auto arm1 = treeGenerator.addNodeChain(rootNode, rnd.next(47'000, 53'000));
                const auto arm2 = treeGenerator.addNodeChain(rootNode, rnd.next(47'000, 53'000));
                const int posOf3rdArmAlong1st = rnd.next(700, 800);
                const auto arm3 = treeGenerator.addNodeChain(arm1[posOf3rdArmAlong1st], rnd.next(47'000, 53'000));

                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) * 33 / 100, 70.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 98.0);

                addCounters(treeGenerator, rnd.next(78.0, 85.0));

                const auto nodesAtHeight = buildNodesAtHeightMap(treeGenerator);
                findBobWinningRelocatedHeightsForNodes(treeGenerator, nodesAtHeight);

                std::vector<TestQuery> queries;
                addQueriesAlongFirstHalfOfChain(queries, arm1, rnd.next(58'000, 62'000), 24.5, nodesAtHeight);

                auto allNodes = treeGenerator.nodes();
                sort(allNodes.begin(), allNodes.end(), [](const auto& lhs, const auto& rhs) { return lhs->data.maxDescendantDist > rhs->data.maxDescendantDist; });

                // Prefer to focus on nodes that a) have a large maxDescendantDist (these will be harder to brute force); and b) have at
                // least 40 newParentHeightsForBobWin (for variety).

                vector<TestNode<NodeData>*> acceptableNodesToFocusOn;
                const int minMaxDescendantDist = 47'000;
                for (auto node : allNodes)
                {
                    const auto numNewParentHeightsForBobWin = node->data.nodeRelocateInfo.newParentHeightsForBobWin.size();
                    if (numNewParentHeightsForBobWin >= 40 && node->data.maxDescendantDist >= minMaxDescendantDist)
                    {
                        acceptableNodesToFocusOn.push_back(node);
                    }
                }
                assert(acceptableNodesToFocusOn.size() >= 2);
                shuffle(acceptableNodesToFocusOn.begin(), acceptableNodesToFocusOn.end());
                acceptableNodesToFocusOn.erase(acceptableNodesToFocusOn.begin() + 2, acceptableNodesToFocusOn.end());
                assert(acceptableNodesToFocusOn.size() == 2);

                for (auto node : acceptableNodesToFocusOn)
                {
                    const int numFocusedQueries = rnd.next(61'000, 63'000);
                    const double percentBobWins = static_cast<double>(node->data.nodeRelocateInfo.newParentHeightsForBobWin.size() - rnd.next(1, 4)) / numFocusedQueries * 100.0;
                    cout << "percentBobWins: " << percentBobWins << endl;
                    const auto nodeFocusedQueries = generateQueriesFromNodes({node}, numFocusedQueries, percentBobWins, nodesAtHeight);
                    queries.insert(queries.end(), nodeFocusedQueries.begin(), nodeFocusedQueries.end());
                }

#if 0
                while (true)
                {
                    const int indexAlongArm2 = rnd.next(static_cast<int>(arm2.size()));
                    firstNodeToConcentrateOn = arm2[indexAlongArm2];
                    cout << "Node height: " << firstNodeToConcentrateOn->data.height << " # bob wins: " << firstNodeToConcentrateOn->data.nodeRelocateInfo.newParentHeightsForBobWin.size() << endl;
                    if (firstNodeToConcentrateOn->data.nodeRelocateInfo.newParentHeightsForBobWin.size() >= 200)
                    {
                        break;
                    }
                }
#endif


                const auto remainingQueries = generateQueriesFromNodes(treeGenerator.nodes(), numQueries - queries.size(), rnd.next(30.0, 60.0), nodesAtHeight);
                queries.insert(queries.end(), remainingQueries.begin(), remainingQueries.end());

                scrambleAndwriteTestcase(treeGenerator, testcase, queries);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(MC2TestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(111)
                    .withDescription("A couple of large, random-ish trees and queries"));
            {
                // seed: 627204325 maxBobWinsForNode: 7
                // seed: 280988413 maxBobWinsForNode: 1
                // seed: 797977800 maxBobWinsForNode: 11
                // seed: 557246613 maxBobWinsForNode: 6
                // seed: 40359200 maxBobWinsForNode: 1
                // seed: 78341875 maxBobWinsForNode: 1
                // seed: 693113706 maxBobWinsForNode: 1
                // seed: 681821811 maxBobWinsForNode: 13
                // seed: 98680510 maxBobWinsForNode: 6
                // seed: 514302571 maxBobWinsForNode: 12
                // seed: 316625903 maxBobWinsForNode: 4
                // seed: 721444634 maxBobWinsForNode: 6
                // seed: 319914437 maxBobWinsForNode: 6
                // seed: 614853481 maxBobWinsForNode: 9
                // seed: 983570452 maxBobWinsForNode: 1
                // seed: 475994678 maxBobWinsForNode: 1
                // seed: 912975595 maxBobWinsForNode: 6
                // seed: 531395595 maxBobWinsForNode: 1
                // seed: 543763606 maxBobWinsForNode: 7
                // seed: 972779061 maxBobWinsForNode: 7
                auto& testcase = testFile.newTestcase(MC2TestCaseInfo().withDescription("Mostly leaves with 1% probability, the rest random.  Random queries, though biased towards Bob wins")
                                                                       .withSeed(98680510));

                const int numNodes = subtask3.maxNodesOverAllTestcases / 2;
                const int numQueries = subtask3.maxQueriesOverAllTestcases / 2;

                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.

                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes * 85.0/ 100.0, 1.0);

                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, 70.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 98.0);

                addCounters(treeGenerator, rnd.next(78.0, 85.0));

                const auto nodesAtHeight = buildNodesAtHeightMap(treeGenerator);
                findBobWinningRelocatedHeightsForNodes(treeGenerator, nodesAtHeight);

                const auto queries = generateQueriesFromNodes(treeGenerator.nodes(), numQueries, rnd.next(30.0, 60.0), nodesAtHeight);

                scrambleAndwriteTestcase(treeGenerator, testcase, queries);
            }
            {
                // seed: 627204325 maxBobWinsForNode: 1
                // seed: 749926816 maxBobWinsForNode: 78
                // seed: 731766203 maxBobWinsForNode: 43
                // seed: 804613110 maxBobWinsForNode: 162
                // seed: 765530524 maxBobWinsForNode: 176
                // seed: 118148413 maxBobWinsForNode: 1
                auto& testcase = testFile.newTestcase(MC2TestCaseInfo().withDescription("Mostly leaves with 99% probability, the rest random.  Random queries, though biased towards Bob wins")
                                                                       .withSeed(804613110));

                const int numNodes = subtask3.maxNodesOverAllTestcases / 2;
                const int numQueries = subtask3.maxQueriesOverAllTestcases / 2;

                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.

                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes * 85.0/ 100.0, 99.0);

                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, 70.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 98.0);

                addCounters(treeGenerator, rnd.next(78.0, 85.0));

                const auto nodesAtHeight = buildNodesAtHeightMap(treeGenerator);
                findBobWinningRelocatedHeightsForNodes(treeGenerator, nodesAtHeight);

                const auto queries = generateQueriesFromNodes(treeGenerator.nodes(), numQueries, rnd.next(30.0, 60.0), nodesAtHeight);

                scrambleAndwriteTestcase(treeGenerator, testcase, queries);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(MC2TestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(13984)
                    .withDescription("max testcases, mostly with about 200 nodes/ queries each but in total equalling maxNodesOverAllTestcases/ maxQueriesOverAllTestcases"));

            const auto numNodesForTestCase = chooseRandomValuesWithSum(subtask3.maxNumTestcases, subtask3.maxNodesOverAllTestcases, 2);
            const auto numQueriesForTestCase = chooseRandomValuesWithSum(subtask3.maxNumTestcases, subtask3.maxQueriesOverAllTestcases, 1);
            for (int i = 0; i < subtask3.maxNumTestcases; i++)
            {
                auto& testcase = testFile.newTestcase(MC2TestCaseInfo());

                const int numNodes = numNodesForTestCase[i];
                const int numQueries = numQueriesForTestCase[i];
                bool generatedTestcase = false;
                while (!generatedTestcase)
                {
                    try
                    {

                        cout << "Attempting to generate testcase " << (i + 1) << " of " << subtask3.maxNumTestcases << " numNodes: " << numNodes << " numQueries: " << numQueries << endl;
                        TreeGenerator<NodeData> treeGenerator;
                        treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
                        const int numNodesPhase1 = rnd.next(numNodes);
                        treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodesPhase1, rnd.next(100.0));
                        treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), rnd.next(100.0));
                        addCounters(treeGenerator, rnd.next(100.0));
                        assert(treeGenerator.numNodes() == numNodes);

                        const auto nodesAtHeight = buildNodesAtHeightMap(treeGenerator);
                        findBobWinningRelocatedHeightsForNodes(treeGenerator, nodesAtHeight, false);

                        const auto queries = generateQueriesFromNodes(treeGenerator.nodes(), numQueries, rnd.next(0.0, 60.0), nodesAtHeight);
                        scrambleAndwriteTestcase(treeGenerator, testcase, queries);
                        generatedTestcase = true;
                    }
                    catch (std::invalid_argument& exc)
                    {
                    }
                }
            }
        }
        {
            auto& testFile = testsuite.newTestFile(MC2TestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(283432) // Max Bob Wins: 78.
                    .withDescription("Duplicate of: 'Three long (~50k) arms originating at (or near) root; then 35% of remaining of bristles; the rest, leaves.  200k nodes total.  Queries mainly concentrate on the first halves of each of the three arms', except with a different seed and a ~50% chance of a node having a coin - I was worried that setting the number of nodes having a coin as >75% was exploitable in some way."));
            {
                auto& testcase = testFile.newTestcase(MC2TestCaseInfo());

                const int numNodes = subtask3.maxNodesOverAllTestcases;
                const int numQueries = subtask3.maxQueriesOverAllTestcases;

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
                const auto arm1 = treeGenerator.addNodeChain(rootNode, rnd.next(48'000, 52'000));
                const auto arm2 = treeGenerator.addNodeChain(rootNode, rnd.next(48'000, 52'000));
                const int posOf3rdArmAlong1st = rnd.next(700, 800);
                const auto arm3 = treeGenerator.addNodeChain(arm1[posOf3rdArmAlong1st], rnd.next(48'000, 52'000));

                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) * 35 / 100, 70.0);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 98.0);

                addCounters(treeGenerator, rnd.next(45.0, 55.0));

                const auto nodesAtHeight = buildNodesAtHeightMap(treeGenerator);
                findBobWinningRelocatedHeightsForNodes(treeGenerator, nodesAtHeight);

                std::vector<TestQuery> queries;
                addQueriesAlongFirstHalfOfChain(queries, arm1, rnd.next(58'000, 62'000), 24.5, nodesAtHeight);
                addQueriesAlongFirstHalfOfChain(queries, arm2 , rnd.next(58'000, 62'000), 20.6, nodesAtHeight);
                addQueriesAlongFirstHalfOfChain(queries, arm3 , rnd.next(58'000, 62'000), 21.7, nodesAtHeight);

                const auto remainingQueries = generateQueriesFromNodes(treeGenerator.nodes(), numQueries - queries.size(), rnd.next(30.0, 60.0), nodesAtHeight);
                queries.insert(queries.end(), remainingQueries.begin(), remainingQueries.end());

                scrambleAndwriteTestcase(treeGenerator, testcase, queries);
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
