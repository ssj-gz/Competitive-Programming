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
            return nodeToReparent < other.nodeToReparent;
        if (newParentNode != other.newParentNode)
            return newParentNode < other.newParentNode;
        return asIndexInRemaining < other.asIndexInRemaining;
    }
};

// TODO - remove this - debugging only!
vector<pair<TestNode<NodeData>*, TestNode<NodeData>*>> computeOrderedValidReparentings(vector<TestNode<NodeData>*>& nodes)
{
    vector<pair<TestNode<NodeData>*, TestNode<NodeData>*>> validReparentings;

    for (auto nodeToReparent : nodes)
    {
        if (nodeToReparent->data.height == 0)
            continue;

        for (auto newParent : nodes)
        {
            assert(newParent->data.dfsBeginVisit != -1 && newParent->data.dfsEndVisit != -1);
            const bool newParentIsDescendant = (newParent->data.dfsBeginVisit >= nodeToReparent->data.dfsBeginVisit && newParent->data.dfsEndVisit <= nodeToReparent->data.dfsEndVisit);
            if (!newParentIsDescendant)
                validReparentings.push_back({nodeToReparent, newParent});
        }
    }

    sort(validReparentings.begin(), validReparentings.end(),
            [](const auto& lhs, const auto& rhs)
            {
            if (lhs.first->id() != rhs.first->id())
                return lhs.first->id() < rhs.first->id();
            if (lhs.second->data.height != rhs.second->data.height)
                return lhs.second->data.height < rhs.second->data.height;
            return lhs.second->id() < rhs.second->id();
            });


    return validReparentings;
}

void setQueryIndexForQueries(vector<TestQuery>& queries, TreeGenerator<NodeData>& treeGenerator)
{
    if (set<TestQuery>(queries.begin(), queries.end()).size() != queries.size())
    {
        throw std::invalid_argument("Queries contain duplicates!");
    }
    cout << "setQueryIndexForQueries" << endl;
    auto lookupInfo = computeLookupInfo(treeGenerator);
    auto allNodes = treeGenerator.nodes();
    const auto validReparentingsOriginal = computeOrderedValidReparentings(allNodes);
    auto validReparentings = computeOrderedValidReparentings(allNodes);
    vector<int> dbgRemovedIndices;
#if 0
    cout << "validReparentingsOriginal: " << endl;
    for (const auto& reparenting : validReparentingsOriginal)
    {
        cout << " nodeToReparent: " << reparenting.first->id() << " newParentNode: " << reparenting.second->id() << " newParentNode height: " << reparenting.second->data.height << endl;
    }
#endif
    MVCN2TST::AVLTree removedIndices;
    for (auto& query : queries)
    {
#if 0
        cout << "query - nodeToReparent: " << query.nodeToReparent->id() << " newParentNode: " << query.newParentNode->id() << endl;
        cout << "validReparentings: " << endl;
        for (const auto& reparenting : validReparentings)
        {
            cout << " nodeToReparent: " << reparenting.first->id() << " newParentNode: " << reparenting.second->id() << " newParentNode height: " << reparenting.second->data.height << endl;
        }
#endif
        int64_t queryIndex = 0;
        if (query.nodeToReparent->id() - 1 - 1 >= 0)
        {
            const auto numFromPriorNodesToReparent = lookupInfo.numCanReparentToPrefixSum[query.nodeToReparent->id() - 1 - 1];
            //cout << "  numFromPriorNodesToReparent: " << numFromPriorNodesToReparent << endl;
            queryIndex += numFromPriorNodesToReparent;
        }

        const int newParentHeight = query.newParentNode->data.height;
        auto nodeToReparent = query.nodeToReparent;
        if (newParentHeight > 0)
        {
            const auto numFromSmallerNewParentHeights = findNumNonDescendantsUpToHeight(query.nodeToReparent, newParentHeight - 1, lookupInfo.numNodesUpToHeight, lookupInfo.nodesAtHeightLookup, lookupInfo.numProperDescendantsForNodeAtHeightPrefixSum);
            //cout << "  numFromSmallerNewParentHeights: " << numFromSmallerNewParentHeights << endl;
            queryIndex += numFromSmallerNewParentHeights;
        }

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
        int numNonDescendantsToLeft = lookupInfo.nodesAtHeightLookup[newParentHeight].size();
        int numNonDescendantsToRight = 0;
        if (hasDescendantsAtThisHeight)
        {
            numNonDescendantsToLeft = descendantsAtHeightBegin - lookupInfo.nodesAtHeightLookup[newParentHeight].begin();
            numNonDescendantsToRight = lookupInfo.nodesAtHeightLookup[newParentHeight].end() - descendantsAtHeightEnd;
        }
        // The AVLTree's prefixesForHeight and suffixesForHeight now represent the node ids to the
        // left and the right of the descendant-range, respectively, in sorted order.
        // Performing the switch is O(1).
        lookupInfo.prefixesForHeight[newParentHeight].switchToRevision(numNonDescendantsToLeft);
        lookupInfo.suffixesForHeight[newParentHeight].switchToRevision(numNonDescendantsToRight);

        const auto numFromPriorNewParents = findIndexOfInPair(query.newParentNode->id() - 1, lookupInfo.prefixesForHeight[newParentHeight], lookupInfo.suffixesForHeight[newParentHeight]);
        //cout << "  numFromPriorNewParents: " << numFromPriorNewParents << endl;
        queryIndex += numFromPriorNewParents;

        int64_t debugQueryIndex = 0;
        for (const auto& reparenting : validReparentingsOriginal)
        {
            if (reparenting.first == query.nodeToReparent && reparenting.second == query.newParentNode)
            {
                break;
            }
            debugQueryIndex++;
        }
        
        assert(debugQueryIndex != static_cast<int64_t>(validReparentingsOriginal.size()));
        cout << "queryIndex: " << queryIndex << " debugQueryIndex: " << debugQueryIndex << endl;
        assert(queryIndex == debugQueryIndex);

#if 0
        cout << "dbgRemovedIndices: " << endl;
        for (const auto x : dbgRemovedIndices)
        {
            cout << " " << x;
        }
        cout << endl;
#endif

        int numRemovedIndicesToLeft = 0;
        const auto removedIndicesToLeftInfo = findLastLessThanOrEqualTo(queryIndex, removedIndices);
        numRemovedIndicesToLeft += removedIndicesToLeftInfo.second;
        if (removedIndicesToLeftInfo.first)
        {
            numRemovedIndicesToLeft++;
        }
        //cout << "numRemovedIndicesToLeft: " << numRemovedIndicesToLeft << endl;

        query.asIndexInRemaining = queryIndex - numRemovedIndicesToLeft; // TODO - perform adjustment, taking into account indices that have been removed.
        //cout << "asIndexInRemaining: " << query.asIndexInRemaining << endl;

        assert(validReparentings[query.asIndexInRemaining].first == query.nodeToReparent);
        assert(validReparentings[query.asIndexInRemaining].second == query.newParentNode);

        removedIndices.insertValue(queryIndex);
        dbgRemovedIndices.push_back(queryIndex);
        validReparentings.erase(validReparentings.begin() + query.asIndexInRemaining);
    }
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
        const int64_t encryptedQuery = query.asIndexInRemaining ^ encryptionKey;
        destTestcase.writeLine(encryptedQuery);

        encryptionKey = (encryptionKey + powerOf2 * query.nodeToReparent->id()) % Mod;
        encryptionKey = (encryptionKey + powerOf3 * query.newParentNode->id()) % Mod;
        powerOf2 = (powerOf2 * 2) % Mod;
        powerOf3 = (powerOf3 * 3) % Mod;
    }
}

void scrambleAndwriteTestcase(TreeGenerator<NodeData>& treeGenerator, Testcase<SubtaskInfo>& destTestcase, const std::vector<TestQuery>& queries)
{
    auto rootNode = treeGenerator.nodes().front();
    treeGenerator.scrambleNodeIdsAndReorder(rootNode /* Ensure that the rootNode keeps its id of 1 */);
    treeGenerator.scrambleEdgeOrder();

    writeTestCase(treeGenerator, destTestcase, queries);
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



bool verifyTestFile(TestFileReader& testFileReader, const SubtaskInfo& containingSubtask);

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
                    .withDescription("TODO - remove - debugging only!"));
            {
                auto& testcase = testFile.newTestcase(MC2TTestCaseInfo());


                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode();
                const int numNodes = 2 + rand() % 200;
                const int numQueries = 1 + rand() % 200;
                treeGenerator.createNodesWithRandomParent(numNodes - treeGenerator.numNodes());
                const auto& lookupInfo = computeLookupInfo(treeGenerator);

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

                scrambleAndwriteTestcase(treeGenerator, testcase, queries);
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
        std::vector<int64_t> encryptedQueries;
        for (int i = 0; i < numQueries; i++)
        {
            const auto& [encryptedQuery] = testFileReader.readLine<int64_t>();
            encryptedQueries.push_back(encryptedQuery);
        }
        // TODO - validation of (encrypted) queries - making sure they lie within expected range.


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
        Verifier::calcFinalDecryptionKey(nodes, encryptedQueries, destDecryptedQueries);
        // TODO - validation of (decrypted) queries - making sure they lie within expected range (1 <= q_i <= L - i).


        testFileReader.markTestcaseAsValidated();
    }
    testFileReader.addErrorUnless(totalNumNodes <= containingSubtask.maxNodesOverAllTestcases, "Total numNodes must be less than or equal to maxNodesOverAllTestcases, not " + std::to_string(totalNumNodes));
    testFileReader.markTestFileAsValidated();

    return true;
}
