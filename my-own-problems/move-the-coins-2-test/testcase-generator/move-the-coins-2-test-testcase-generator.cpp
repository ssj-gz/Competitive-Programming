#include "utils.h"
#include "verifier.h"
#include <tree-generator.h>
#include <testlib.h>
#include "testcase-generator-lib.h"
#include "random-utilities.h"

#include <iostream>
#include <set>
#include <exception>

using namespace std;

constexpr int maxNodes = 200'000;
constexpr int maxQueries = 200'000;
constexpr int maxNumTestcases = 1000;

struct SubtaskInfo
{
    int subtaskId = -1;
    int score = -1;

    int maxNodesPerTestcase = -1;
    int maxQueriesPerTestcase = -1;
    int maxNodesOverAllTestcases = -1;
    int maxQueriesOverAllTestcases = -1;
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
                                            .withMaxNumTestcases(10);
                                            
SubtaskInfo subtask2 = SubtaskInfo::create().withSubtaskId(2)
                                            .withScore(10)
                                            .withMaxNodesPerTestcase(1000)
                                            .withMaxQueriesPerTestcase(1000)
                                            .withMaxNodesOverAllTestcases(NoExplicitLimit)
                                            .withMaxQueriesOverAllTestcases(NoExplicitLimit)
                                            .withMaxNumTestcases(100);

SubtaskInfo subtask3 = SubtaskInfo::create().withSubtaskId(3)
                                            .withScore(85)
                                            .withMaxNodesPerTestcase(maxNodes)
                                            .withMaxQueriesPerTestcase(maxQueries)
                                            .withMaxNodesOverAllTestcases(maxNodes)
                                            .withMaxQueriesOverAllTestcases(maxQueries)
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

using MC2TTestCaseInfo = TestcaseInfo<SubtaskInfo>;
using MC2TTestFileInfo = TestFileInfo<SubtaskInfo>;

struct TestQuery
{
    TestNode<NodeData>* nodeToReparent = nullptr;
    TestNode<NodeData>* newParentNode = nullptr;
    int64_t asIndexInRemaining = -1;
    bool operator<(const TestQuery& other) const
    {
        if (nodeToReparent != other.nodeToReparent)
            return nodeToReparent->id() < other.nodeToReparent->id();
        if (newParentNode != other.newParentNode)
            return newParentNode->id() < other.newParentNode->id();
        return asIndexInRemaining < other.asIndexInRemaining;
    }
};

struct Range
{
    int leftIndex = -1;
    int rightIndex = -1;
    bool isValid() const
    {
        return (leftIndex >= 0 && rightIndex >= 0) && (leftIndex <= rightIndex);
    }
    int numInRange() const
    {
        if (!isValid())
            return 0;
        return rightIndex - leftIndex + 1;
    }
};

Range descendantRangeFor(TestNode<NodeData>* nodeToReparent, int newParentHeight, const LookupInfo& lookupInfo)
{
    const auto descendantsAtHeightBegin = std::lower_bound(lookupInfo.nodesAtHeightLookup[newParentHeight].begin(), lookupInfo.nodesAtHeightLookup[newParentHeight].end(), nodeToReparent->data.dfsBeginVisit,
            [](const TestNode<NodeData>* node, const int dfsBeginVisit)
            {
            return node->data.dfsBeginVisit < dfsBeginVisit;
            });
    const auto descendantsAtHeightEnd = std::upper_bound(lookupInfo.nodesAtHeightLookup[newParentHeight].begin(), lookupInfo.nodesAtHeightLookup[newParentHeight].end(), nodeToReparent->data.dfsEndVisit,
            [](const int dfsEndVisit, const TestNode<NodeData>* node)
            {
            return dfsEndVisit < node->data.dfsEndVisit;
            });
    const bool hasDescendantsAtThisHeight = descendantsAtHeightBegin != lookupInfo.nodesAtHeightLookup[newParentHeight].end() && (*descendantsAtHeightBegin)->data.dfsEndVisit <= nodeToReparent->data.dfsEndVisit;
    if (!hasDescendantsAtThisHeight)
        return {-1, -1};

    return {descendantsAtHeightBegin - lookupInfo.nodesAtHeightLookup[newParentHeight].begin(), descendantsAtHeightEnd - lookupInfo.nodesAtHeightLookup[newParentHeight].begin() - 1};
}

struct NodeAndHeightPair
{
    TestNode<NodeData>* nodeToReparent = nullptr;
    int newParentHeight = -1;
    bool operator<(const NodeAndHeightPair& other) const
    {
        if (nodeToReparent != other.nodeToReparent)
            return nodeToReparent->id() < other.nodeToReparent->id();
        return newParentHeight < other.newParentHeight;
    }
};

void setQueryIndexForQueries(vector<TestQuery>& queries, TreeGenerator<NodeData>& treeGenerator)
{
    if (set<TestQuery>(queries.begin(), queries.end()).size() != queries.size())
    {
        throw std::invalid_argument("Queries contain duplicates!");
    }
    auto lookupInfo = computeLookupInfo(treeGenerator);
    auto allNodes = treeGenerator.nodes();
    AVLTree removedIndices;
    int queryNum = 0;
    int64_t numNonDescendantHeightSum = 0;
    int64_t numDescendantHeightSum = 0;
    TestNode<NodeData>* previousNodeToReparent = nullptr;
    set<pair<TestNode<NodeData>*, int>> blah;
    for (auto& query : queries)
    {
        queryNum++;
        int64_t queryIndex = 0;
        if (query.nodeToReparent->id() - 1 - 1 >= 0)
        {
            const auto numFromPriorNodesToReparent = lookupInfo.numCanReparentToPrefixSum[query.nodeToReparent->id() - 1 - 1];
            queryIndex += numFromPriorNodesToReparent;
        }

        const int newParentHeight = query.newParentNode->data.height;
        auto nodeToReparent = query.nodeToReparent;
        if (newParentHeight > 0)
        {
            const auto numFromSmallerNewParentHeights = findNumNonDescendantsUpToHeight(query.nodeToReparent, newParentHeight - 1, lookupInfo.numNodesUpToHeight, lookupInfo.nodesAtHeightLookup, lookupInfo.numProperDescendantsForNodeAtHeightPrefixSum);
            assert(numFromSmallerNewParentHeights >= 0);
            queryIndex += numFromSmallerNewParentHeights;
        }

        const auto descendantRange = descendantRangeFor(nodeToReparent, newParentHeight, lookupInfo);
        const bool hasDescendantsAtThisHeight = descendantRange.isValid();
        int numNonDescendantsToLeft = lookupInfo.nodesAtHeightLookup[newParentHeight].size();
        int numNonDescendantsToRight = 0;
        if (hasDescendantsAtThisHeight)
        {
            numNonDescendantsToLeft = descendantRange.leftIndex;
            numNonDescendantsToRight = lookupInfo.nodesAtHeightLookup[newParentHeight].size() - descendantRange.rightIndex - 1;
        }
        //if (nodeToReparent != previousNodeToReparent)
        {
            //cout << "queryNum: " << queryNum << " nodeToReparent: " << nodeToReparent->id() << " newParentHeight: " << newParentHeight << " numDescendants at height: " << lookupInfo.nodesAtHeightLookup[newParentHeight].size() - (numNonDescendantsToLeft + numNonDescendantsToRight) << " nonDescendants at height: " << (numNonDescendantsToLeft + numNonDescendantsToRight) << endl;
        }
        previousNodeToReparent = nodeToReparent;
        const auto numDescendantsAtHeight = lookupInfo.nodesAtHeightLookup[newParentHeight].size() - (numNonDescendantsToLeft + numNonDescendantsToRight) ;
        //cout << "numDescendantsAtHeight: " << numDescendantsAtHeight << " bloo.numInRange(): " << bloo.numInRange() << endl;
        //assert(numDescendantsAtHeight == bloo.numInRange());
        //cout << "fleep nodeToReparent: " << nodeToReparent->id() << " height: " << nodeToReparent->data.height << " newParentHeight: " << newParentHeight << " numDescendants at height: " << numDescendantsAtHeight << " nonDescendants at height: " << (numNonDescendantsToLeft + numNonDescendantsToRight) << endl;
        // The AVLTree's prefixesForHeight and suffixesForHeight now represent the node ids to the
        // left and the right of the descendant-range, respectively, in sorted order.
        // Performing the switch is O(1).
        lookupInfo.prefixesForHeight[newParentHeight].switchToRevision(numNonDescendantsToLeft);
        lookupInfo.suffixesForHeight[newParentHeight].switchToRevision(numNonDescendantsToRight);

        numNonDescendantHeightSum += (numNonDescendantsToLeft + numNonDescendantsToRight);
        numDescendantHeightSum += numDescendantsAtHeight;


        if (numDescendantsAtHeight > 1000)
        {
            if (blah.find({nodeToReparent, newParentHeight}) == blah.end())
            {
                cout << "blee: nodeToReparent: " << nodeToReparent->id() << " newParentHeight: " << newParentHeight << " numDescendants at newParentHeight: " << numDescendantsAtHeight << endl;
            }
            blah.insert({nodeToReparent, newParentHeight});
        }

        const auto numFromPriorNewParents = findIndexOfInPair(query.newParentNode->id() - 1, lookupInfo.prefixesForHeight[newParentHeight], lookupInfo.suffixesForHeight[newParentHeight]);
        queryIndex += numFromPriorNewParents;

        int numRemovedIndicesToLeft = 0;
        const auto removedIndicesToLeftInfo = findLastLessThanOrEqualTo(queryIndex, removedIndices);
        numRemovedIndicesToLeft += removedIndicesToLeftInfo.second;
        if (removedIndicesToLeftInfo.first)
        {
            numRemovedIndicesToLeft++;
        }

        query.asIndexInRemaining = queryIndex - numRemovedIndicesToLeft;
        assert(query.asIndexInRemaining >= 0);

        removedIndices.insertValue(queryIndex);
    }
    cout << "numNonDescendantHeightSum: " << numNonDescendantHeightSum << endl;
    cout << "numDescendantHeightSum: " << numDescendantHeightSum << endl;
    cout << "blah: " << blah.size() << endl;
}

void writeTestCase(TreeGenerator<NodeData>& treeGenerator, Testcase<SubtaskInfo>& destTestcase, const std::vector<TestQuery>& originalQueries)
{
    std::vector<TestQuery> queriesToWrite(originalQueries);
    setQueryIndexForQueries(queriesToWrite, treeGenerator);

    destTestcase.writeLine(treeGenerator.numNodes());

    for (const auto& edge : treeGenerator.edges())
    {
        destTestcase.writeLine(edge->nodeA->id(), edge->nodeB->id());
    }

    destTestcase.writeLine(queriesToWrite.size());

    int64_t encryptionKey = 0;
    int64_t powerOf2 = 2;
    int64_t powerOf3 = 3;

    for (const auto& query : queriesToWrite)
    {
        const int64_t encryptedQuery = (query.asIndexInRemaining + 1) ^ encryptionKey;
        destTestcase.writeLine(encryptedQuery);

        encryptionKey = (encryptionKey + powerOf2 * query.nodeToReparent->id()) % Mod;
        encryptionKey = (encryptionKey + powerOf3 * query.newParentNode->id()) % Mod;
        powerOf2 = (powerOf2 * 2) % Mod;
        powerOf3 = (powerOf3 * 3) % Mod;
    }
    cout << "final encryptionKey: " << encryptionKey << endl;
}

void scrambleAndwriteTestcase(TreeGenerator<NodeData>& treeGenerator, Testcase<SubtaskInfo>& destTestcase, const std::vector<TestQuery>& queries)
{
    auto rootNode = treeGenerator.nodes().front();
    treeGenerator.scrambleNodeIdsAndReorder(rootNode /* Ensure that the rootNode keeps its id of 1 */);
    treeGenerator.scrambleEdgeOrder();

    auto scrambledQueries = queries;
    shuffle(scrambledQueries.begin(), scrambledQueries.end());

    writeTestCase(treeGenerator, destTestcase, scrambledQueries);
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
            int numNewChildren = 0;
            if (treeGenerator.numNodes() < 5000)
            {
                numNewChildren = 1 + (rnd.next(50) == 0 ? 1 : 0);
            }
            else
            {
#if 0
                numNewChildren = 2 + (rnd.next(100) == 0 ? 1 : 0);
#else
                numNewChildren = chooseWithWeighting<int>({
                        {2, 70.0},
                        {3, 5},
                        {1, 25.0}
                        }, 1).front();
#endif
            }

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



bool verifyTestFile(TestFileReader& testFileReader, const SubtaskInfo& containingSubtask);

/**
 * Also ensures that the queries contain no duplicates.
 * Order of queries might be changed quite significiantly!
 */
void addRandomQueries(TreeGenerator<NodeData>& treeGenerator, vector<TestQuery>& queries, const int targetNumQueries, LookupInfo& lookupInfo)
{
    const auto allNodes = treeGenerator.nodes();
    set<TestQuery> querySet(queries.begin(), queries.end());
    sort(queries.begin(), queries.end());
#if 0
    while (static_cast<int>(querySet.size()) < targetNumQueries)
    {
        auto nodeToReparent = rnd.nextFrom(allNodes);
        if (nodeToReparent->data.largestNonDescendantHeight == 0)
            continue;

        auto newParent = findRandomValidNewParent(nodeToReparent, allNodes, 0, nodeToReparent->data.largestNonDescendantHeight, lookupInfo);

        querySet.insert({nodeToReparent, newParent});
    }

#endif
    queries.assign(querySet.begin(), querySet.end());

#if 0
    assert(set<TestQuery>(queries.begin(), queries.end()).size() == queries.size());
    assert(static_cast<int>(set<TestQuery>(queries.begin(), queries.end()).size()) == targetNumQueries);
#endif

}

int main(int argc, char* argv[])
{
    TestSuite<SubtaskInfo> testsuite;
    testsuite.setTestFileVerifier(&verifyTestFile);

    // SUBTASK 1
    {
        // TODO - remove this - debugging only!
        auto& testFile = testsuite.newTestFile(MC2TTestFileInfo().belongingToSubtask(subtask1)
                .withSeed(9734)
                .withDescription("TODO - remove - debugging only!"));
        {
            auto& testcase = testFile.newTestcase(MC2TTestCaseInfo());


            TreeGenerator<NodeData> treeGenerator;
            treeGenerator.createNode();
            const int numNodes = 2;
            const int numQueries = 1;
            treeGenerator.createNodesWithRandomParent(numNodes - treeGenerator.numNodes());
            const auto& lookupInfo = computeLookupInfo(treeGenerator);

            vector<TestQuery> queries;
            const auto allNodes = treeGenerator.nodes();
            while (queries.size() < numQueries)
            {
                const auto nodeToReparent = rnd.nextFrom(allNodes);
                const auto newParent = rnd.nextFrom(allNodes);

                if (!newParent->data.isDescendantOf(*nodeToReparent))
                {
                    queries.push_back({nodeToReparent, newParent});
                }
            }

            scrambleAndwriteTestcase(treeGenerator, testcase, queries);
        }
    }

    // SUBTASK 2
    {
        // TODO - remove this - debugging only!
        auto& testFile = testsuite.newTestFile(MC2TTestFileInfo().belongingToSubtask(subtask2)
                .withSeed(9734)
                .withDescription("TODO - remove - debugging only!"));
        {
            auto& testcase = testFile.newTestcase(MC2TTestCaseInfo());


            TreeGenerator<NodeData> treeGenerator;
            treeGenerator.createNode();
            const int numNodes = 2;
            const int numQueries = 1;

            treeGenerator.createNodesWithRandomParent(numNodes - treeGenerator.numNodes());
            const auto& lookupInfo = computeLookupInfo(treeGenerator);

            vector<TestQuery> queries;
            const auto allNodes = treeGenerator.nodes();
            while (queries.size() < numQueries)
            {
                const auto nodeToReparent = allNodes[rnd.next(static_cast<int>(allNodes.size()))];
                const auto newParent = allNodes[rnd.next(static_cast<int>(allNodes.size()))];

                if (!newParent->data.isDescendantOf(*nodeToReparent))
                {
                    queries.push_back({nodeToReparent, newParent});
                }
            }

            scrambleAndwriteTestcase(treeGenerator, testcase, queries);
        }
    }

    // SUBTASK 3
    {
        {
            // TODO - remove this - debugging only!
            auto& testFile = testsuite.newTestFile(MC2TTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(9734)
                    .withDescription("Squat graph where all nodes have degree approximately 3 taking up ~120'000 nodes, then random nodes."));
            {
                auto& testcase = testFile.newTestcase(MC2TTestCaseInfo());


                TreeGenerator<NodeData> treeGenerator;
                const int numNodes = 200'000;
                const int numQueries = 200'000;
                makeSquatGraphWhereAllNodesHaveDegreeAtLeast3(treeGenerator, 180'000);
                treeGenerator.createNodesWithRandomParent(numNodes - treeGenerator.numNodes());

                const auto allNodes = treeGenerator.nodes();

                vector<TestQuery> queries;
                map<NodeAndHeightPair, double> numDescendantsForNodeAndHeight;
                {
                    auto lookupInfo = computeLookupInfo(treeGenerator);
                    for (auto nodeToReparent : allNodes)
                    {
                        if (nodeToReparent->data.largestNonDescendantHeight == 0)
                            continue;
                        for (int newParentHeight = 0; newParentHeight <= nodeToReparent->data.largestNonDescendantHeight; newParentHeight++)
                        {
                            const auto descendantRange = descendantRangeFor(nodeToReparent, newParentHeight, lookupInfo);
                            numDescendantsForNodeAndHeight[{nodeToReparent, newParentHeight}] = descendantRange.numInRange();
                        }
                    }
                    WeightedChooser<NodeAndHeightPair> nodeAndHeightPairChooser(numDescendantsForNodeAndHeight);
                    for (int i = 0; i < numQueries; i++)
                    {
                        const auto nodeToReparentAndHeight = nodeAndHeightPairChooser.nextValue();
                        auto newParent = findRandomValidNewParent(nodeToReparentAndHeight.nodeToReparent, allNodes, nodeToReparentAndHeight.newParentHeight, nodeToReparentAndHeight.newParentHeight, lookupInfo);
                        queries.push_back({nodeToReparentAndHeight.nodeToReparent, newParent});
                    }
                    addRandomQueries(treeGenerator, queries, numQueries, lookupInfo);
                }

                scrambleAndwriteTestcase(treeGenerator, testcase, queries);
            }
        }
        {
            // TODO - remove this - debugging only!
            auto& testFile = testsuite.newTestFile(MC2TTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(9734)
                    .withDescription("TODO - remove - debugging only!"));
            {
                auto& testcase = testFile.newTestcase(MC2TTestCaseInfo());

                const int numNodes = 200'000;
                const int numQueries = 200'000;

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode();
                treeGenerator.addNodeChain(rootNode, 80'000);
                treeGenerator.addNodeChain(rootNode, 80'000);
                treeGenerator.createNodesWithRandomParent(numNodes - treeGenerator.numNodes());

                fixParentChildAndHeights(rootNode);
                computeDFSInfo(rootNode);

                vector<TestQuery> queries;
                const auto allNodes = treeGenerator.nodes();

                while (static_cast<int>(queries.size()) < numQueries)
                {
                    const auto nodeToReparent = allNodes[rnd.next(static_cast<int>(allNodes.size()))];
                    const auto newParent = allNodes[rnd.next(static_cast<int>(allNodes.size()))];

                    if (!newParent->data.isDescendantOf(*nodeToReparent))
                    {
                        queries.push_back({nodeToReparent, newParent});
                    }
                }
                // Remove duplicates.
                set<TestQuery> querySet(queries.begin(), queries.end());
                queries.assign(querySet.begin(), querySet.end());

                //scrambleAndwriteTestcase(treeGenerator, testcase, queries);
                writeTestCase(treeGenerator, testcase, queries);
            }
        }
    }

    const bool validatedAndWrittenSuccessfully = testsuite.writeTestFiles();
    if (!validatedAndWrittenSuccessfully)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

bool verifyTestFile(TestFileReader& testFileReader, const SubtaskInfo& containingSubtask)
{
    using std::cout;
    using std::endl;
    using Verifier::Node;

    int64_t totalNumNodes = 0;
    int64_t totalNumQueries = 0;

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
            nodes[i].id = (i + 1);
        }
        
        for (int i = 0; i < numNodes - 1; i++)
        {
            const auto& [edgeNodeAId, edgeNodeBId] = testFileReader.readLine<int, int>();
            testFileReader.addErrorUnless(0 <= edgeNodeAId && edgeNodeAId <= numNodes, "Invalid node id " + std::to_string(edgeNodeAId));
            testFileReader.addErrorUnless(0 <= edgeNodeBId && edgeNodeBId <= numNodes, "Invalid node id " + std::to_string(edgeNodeBId));
            
            nodes[edgeNodeAId - 1].children.push_back(&(nodes[edgeNodeBId - 1]));
            nodes[edgeNodeBId - 1].children.push_back(&(nodes[edgeNodeAId - 1]));
        }

        const auto [numQueries] = testFileReader.readLine<int>();
        testFileReader.addErrorUnless(numQueries >= 1, "Number of queries should be >= 1, not " + std::to_string(numQueries) );
        testFileReader.addErrorUnless(numQueries <= containingSubtask.maxQueriesPerTestcase, "Number of queries for a testcase should be <= " + std::to_string(containingSubtask.maxQueriesPerTestcase) + " not " + std::to_string(numQueries) );

        totalNumQueries += numQueries;
        testFileReader.addErrorUnless(totalNumQueries <= containingSubtask.maxQueriesOverAllTestcases, "Total number of queries over all testcases should be <= " + std::to_string(containingSubtask.maxQueriesOverAllTestcases) + " not " + std::to_string(numQueries) );

        std::vector<int64_t> encryptedQueries;
        for (int i = 0; i < numQueries; i++)
        {
            const auto& [encryptedQuery] = testFileReader.readLine<int64_t>();
            encryptedQueries.push_back(encryptedQuery);
        }

        for (const auto& node : nodes)
        {
            std::set<Node*> distinctNeighbours;
            for (const auto neighbour : node.children)
            {
                testFileReader.addErrorUnless(neighbour != &node, "Node " + std::to_string(node.id) + " is connected to itself!");
                testFileReader.addErrorUnless(distinctNeighbours.find(neighbour) == distinctNeighbours.end(), "Node " + std::to_string(node.id) + " is connected to node " + std::to_string(neighbour->id) + " more than once!");

                distinctNeighbours.insert(neighbour);
            }
        }

        auto rootNode = &(nodes.front());
        std::vector<Node*> toExplore = { rootNode };
        std::set<Node*> visited = { rootNode };
        while (!toExplore.empty())
        {
            std::vector<Node*> nextToExplore;
            for (const auto& node : toExplore)
            {
                for (const auto neighbour : node->children)
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

        vector<int64_t> destDecryptedQueries;
        const auto finalDecryptionKey = Verifier::calcFinalDecryptionKey(nodes, encryptedQueries, destDecryptedQueries);
        cout << "verifier: finalDecryptionKey: " << finalDecryptionKey << endl;

        int64_t totalNumValidReparentings = 0;
        for (const auto& node : nodes)
        {
            totalNumValidReparentings += numNodes - node.numDescendants;
        }
        for (int queryIndex = 0; queryIndex < numQueries; queryIndex++)
        {
            testFileReader.addErrorUnless(destDecryptedQueries[queryIndex] >= 1, "Decrypted queries should be >= 1!");
            const auto numValidReparentingsRemaining = totalNumValidReparentings - queryIndex;
            testFileReader.addErrorUnless(destDecryptedQueries[queryIndex] <= numValidReparentingsRemaining, "The " + to_string(queryIndex + 1) + "th decrypted query should be <= " + to_string(numValidReparentingsRemaining) + "!");
        }

        testFileReader.markTestcaseAsValidated();
    }
    testFileReader.addErrorUnless(totalNumNodes <= containingSubtask.maxNodesOverAllTestcases, "Total numNodes must be less than or equal to maxNodesOverAllTestcases, not " + std::to_string(totalNumNodes));
    testFileReader.markTestFileAsValidated();

    return true;
}
