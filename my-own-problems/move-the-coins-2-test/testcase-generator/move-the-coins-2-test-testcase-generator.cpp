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
                                            .withMaxNodesPerTestcase(100)
                                            .withMaxQueriesPerTestcase(100)
                                            .withMaxNodesOverAllTestcases(NoExplicitLimit)
                                            .withMaxQueriesOverAllTestcases(NoExplicitLimit)
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
    const auto& nodesAtNewParentHeight = lookupInfo.nodesAtHeightLookup[newParentHeight];
    const auto descendantsAtHeightBegin = std::lower_bound(nodesAtNewParentHeight.begin(), nodesAtNewParentHeight.end(), nodeToReparent->data.dfsBeginVisit,
            [](const TestNode<NodeData>* node, const int dfsBeginVisit)
            {
            return node->data.dfsBeginVisit < dfsBeginVisit;
            });
    const auto descendantsAtHeightEnd = std::upper_bound(nodesAtNewParentHeight.begin(), nodesAtNewParentHeight.end(), nodeToReparent->data.dfsEndVisit,
            [](const int dfsEndVisit, const TestNode<NodeData>* node)
            {
            return dfsEndVisit < node->data.dfsEndVisit;
            });
    const bool hasDescendantsAtThisHeight = descendantsAtHeightBegin != lookupInfo.nodesAtHeightLookup[newParentHeight].end() && (*descendantsAtHeightBegin)->data.dfsEndVisit <= nodeToReparent->data.dfsEndVisit;
    if (!hasDescendantsAtThisHeight)
        return {-1, -1};

    return {static_cast<int>(descendantsAtHeightBegin - nodesAtNewParentHeight.begin()), static_cast<int>(descendantsAtHeightEnd - nodesAtNewParentHeight.begin() - 1)};
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

/**
 * For each query in \a queries, in the order given, populate its TestQuery::asIndexInRemaining
 * field.
 */
void setQueryIndexForQueries(vector<TestQuery>& queries, TreeGenerator<NodeData>& treeGenerator)
{
    if (set<TestQuery>(queries.begin(), queries.end()).size() != queries.size())
    {
        throw std::invalid_argument("Queries contain duplicates!");
    }
    auto lookupInfo = computeLookupInfo(treeGenerator);
    auto allNodes = treeGenerator.nodes();
    AVLTree removedIndices;
    int64_t numNonDescendantHeightSum = 0;
    int64_t numDescendantHeightSum = 0;
    for (auto& query : queries)
    {
        int64_t indexInOriginalList = 0;
        // Account for nodeToReparent's id's contribution to indexInOriginalList.
        if (query.nodeToReparent->id() - 1 - 1 >= 0)
        {
            const auto numFromPriorNodesToReparent = lookupInfo.numCanReparentToPrefixSum[query.nodeToReparent->id() - 1 - 1];
            indexInOriginalList += numFromPriorNodesToReparent;
        }

        // Account for newParentHeight's contribution to indexInOriginalList.
        const int newParentHeight = query.newParentNode->data.height;
        auto nodeToReparent = query.nodeToReparent;
        if (newParentHeight > 0)
        {
            const auto numFromSmallerNewParentHeights = findNumNonDescendantsUpToHeight(query.nodeToReparent, newParentHeight - 1, lookupInfo.numNodesUpToHeight, lookupInfo.nodesAtHeightLookup, lookupInfo.numProperDescendantsForNodeAtHeightPrefixSum);
            assert(numFromSmallerNewParentHeights >= 0);
            indexInOriginalList += numFromSmallerNewParentHeights;
        }

        // Finally, account for newParent's id's contribution to indexInOriginalList.
        const auto descendantRange = descendantRangeFor(nodeToReparent, newParentHeight, lookupInfo);
        const bool hasDescendantsAtThisHeight = descendantRange.isValid();
        int numNonDescendantsToLeft = lookupInfo.nodesAtHeightLookup[newParentHeight].size();
        int numNonDescendantsToRight = 0;
        if (hasDescendantsAtThisHeight)
        {
            numNonDescendantsToLeft = descendantRange.leftIndex;
            numNonDescendantsToRight = lookupInfo.nodesAtHeightLookup[newParentHeight].size() - descendantRange.rightIndex - 1;
        }

        const auto numDescendantsAtHeight = lookupInfo.nodesAtHeightLookup[newParentHeight].size() - (numNonDescendantsToLeft + numNonDescendantsToRight) ;
        // The AVLTree's prefixesForHeight and suffixesForHeight now represent the node ids to the
        // left and the right of the descendant-range, respectively, in sorted order.
        // Performing the switch is O(1).
        lookupInfo.prefixesForHeight[newParentHeight].switchToRevision(numNonDescendantsToLeft);
        lookupInfo.suffixesForHeight[newParentHeight].switchToRevision(numNonDescendantsToRight);

        numNonDescendantHeightSum += (numNonDescendantsToLeft + numNonDescendantsToRight);
        numDescendantHeightSum += numDescendantsAtHeight;

        const auto numFromPriorNewParents = findIndexOfInPair(query.newParentNode->id() - 1, lookupInfo.prefixesForHeight[newParentHeight], lookupInfo.suffixesForHeight[newParentHeight]);
        indexInOriginalList += numFromPriorNewParents;

        // We now know indexInOriginalList.  Deal with mapping it to asIndexInRemaining, and marking this indexInOriginalList
        // as "removed".
        int numRemovedIndicesToLeft = 0;
        const auto removedIndicesToLeftInfo = findLastLessThanOrEqualTo(indexInOriginalList, removedIndices);
        numRemovedIndicesToLeft += removedIndicesToLeftInfo.second;
        if (removedIndicesToLeftInfo.first)
        {
            numRemovedIndicesToLeft++;
        }

        query.asIndexInRemaining = indexInOriginalList - numRemovedIndicesToLeft;
        assert(query.asIndexInRemaining >= 0);

        removedIndices.insertValue(indexInOriginalList);
    }
    cout << "numNonDescendantHeightSum: " << numNonDescendantHeightSum << endl;
    cout << "numDescendantHeightSum: " << numDescendantHeightSum << endl;
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

TestNode<NodeData>* buildGraphInLayers(TreeGenerator<NodeData>& treeGenerator, const int approxNumNodes, const std::map<int, double>& percentageProbOfNumChildrenBeforeSwitchOver, const int switchOverAfterNumNodes, const std::map<int, double>& percentageProbOfNumChildrenAfterSwitchOver)
{
    auto rootNode = treeGenerator.createNode();

    std::vector<TestNode<NodeData>*> leafNodes;
    leafNodes.push_back(rootNode);

    while (treeGenerator.numNodes() < approxNumNodes)
    {
        std::vector<TestNode<NodeData>*> nextLeafNodes;
        for (auto leafNode : leafNodes)
        {
            int numNewChildren = 0;
            if (treeGenerator.numNodes() < switchOverAfterNumNodes)
            {
                numNewChildren = chooseWithWeighting(percentageProbOfNumChildrenBeforeSwitchOver, 1).front();
            }
            else
            {
                numNewChildren = chooseWithWeighting(percentageProbOfNumChildrenAfterSwitchOver, 1).front();
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
    while (static_cast<int>(querySet.size()) < targetNumQueries)
    {
        auto nodeToReparent = rnd.nextFrom(allNodes);
        if (nodeToReparent->data.largestNonDescendantHeight == 0)
            continue;

        auto newParent = findRandomValidNewParent(nodeToReparent, allNodes, 0, nodeToReparent->data.largestNonDescendantHeight, lookupInfo);

        querySet.insert({nodeToReparent, newParent});
    }

    queries.assign(querySet.begin(), querySet.end());

    assert(set<TestQuery>(queries.begin(), queries.end()).size() == queries.size());
    assert(static_cast<int>(set<TestQuery>(queries.begin(), queries.end()).size()) == targetNumQueries);
}

void addWeightedQueries(const vector<TestNode<NodeData>*>& candidateNodesToReparent, const vector<TestNode<NodeData>*>& allNodes, vector<TestQuery>& queries, const int targetNumQueries, const double percentProbOfLargerNewParentHeight, LookupInfo& lookupInfo)
{
    assert(percentProbOfLargerNewParentHeight >= 0 && percentProbOfLargerNewParentHeight <= 100.0);
    int numNewQueries = 0;
    while (numNewQueries < targetNumQueries)
    {
        auto nodeToReparent = rnd.nextFrom(candidateNodesToReparent);
        if (nodeToReparent->data.largestNonDescendantHeight == 0)
            continue;
        const auto chooseLowerNode = (nodeToReparent->data.largestNonDescendantHeight >= nodeToReparent->data.height) && (rnd.next(0.0, 100.0) < percentProbOfLargerNewParentHeight);
        const auto newParentHeight = chooseLowerNode ? rnd.next(nodeToReparent->data.height, nodeToReparent->data.largestNonDescendantHeight) :
                                                       rnd.next(1, nodeToReparent->data.largestNonDescendantHeight);

        auto newParent = findRandomValidNewParent(nodeToReparent, allNodes, newParentHeight, newParentHeight, lookupInfo);
        queries.push_back({nodeToReparent, newParent});
        numNewQueries++;
    }
}

void addStrandsAndClumps(TreeGenerator<NodeData>& treeGenerator, const int numNodes, const int numClumpsAndStrandsToAdd, vector<vector<TestNode<NodeData>*>>& destStrands)
{
    // Roughly equal numbers of strands anad clumps.
    const int numClumps = rnd.next(0.4, 0.6) * numClumpsAndStrandsToAdd;
    const int numStrands = numClumpsAndStrandsToAdd - numClumps;
    const int numNodesInClumps = rnd.next(0.4, 0.6) * numNodes;
    const int numNodesInStrands = numNodes - numNodesInClumps;

    auto allNodes = treeGenerator.nodes();
    // Do the strands first; attaching clumps to strands rather than vice versa seems more interesting, somehow :)
    const auto lengthOfStrand = chooseRandomValuesWithSum(numStrands, numNodesInStrands, 1);
    for (int i = 0; i < numStrands; i++)
    {
        auto nodeToAddTo = rnd.nextFrom(allNodes);
        const auto strand = treeGenerator.addNodeChain(nodeToAddTo, lengthOfStrand[i]);
        destStrands.push_back(strand);
        allNodes.insert(allNodes.end(), strand.begin(), strand.end());
    }
    const auto lengthOfClump = chooseRandomValuesWithSum(numClumps, numNodesInClumps, 1);
    for (int i = 0; i < numClumps; i++)
    {
        auto nodeToAddTo = rnd.nextFrom(allNodes);
        for (int j = 0; j < lengthOfClump[i]; j++)
        {
            treeGenerator.createNode(nodeToAddTo);
        }
    }
}

/**
 * Add queries, with nodeToReparent being in one of the arms, that are covered by a strand.
 * A query q is set to be covered by a strand S if all of the  following hold:
 *
 *  * S is anchored at an ancestor of q.nodeToReparent in the same arm as q.nodeToReparent
 *  * q.newParent.height <= the max height of the nodes in S.
 *
 */
void addQueriesCoveredByStrands(const vector<vector<TestNode<NodeData>*>>& arms, const vector<vector<TestNode<NodeData>*>>& strands, const vector<TestNode<NodeData>*>& allNodes, const int numQueriesToAdd, vector<TestQuery>& queries, LookupInfo& lookupInfo)
{
    vector<std::pair<TestNode<NodeData>*, int>> nodeToReparentAndMaxHeights;

    for (const auto& strand : strands)
    {
        auto strandRoot = strand.front()->data.parent;
        const int maxHeightInStrand = strand.back()->data.height;

        for (const auto& arm : arms)
        {
            bool haveFoundRootInArm = false;
            for (auto nodeInArm : arm)
            {
                if (nodeInArm == strandRoot)
                {
                    haveFoundRootInArm = true;
                    continue;
                }
                if (haveFoundRootInArm && nodeInArm->data.height <= maxHeightInStrand)
                {
                    nodeToReparentAndMaxHeights.push_back({nodeInArm, maxHeightInStrand});
                    //cout << " node: " << nodeInArm->id() << " at height: " << nodeInArm->data.height << " is covered by strand rooted at " << strandRoot->id() << " which is at height: " << strandRoot->data.height << " with strand length: " << strand.size() << endl;
                }
            }
        }
    }
    //cout << "nodeToReparentAndMaxHeights.size(): " << nodeToReparentAndMaxHeights.size() << endl;
    for (int i = 0; i < numQueriesToAdd; i++)
    {
        const auto nodeToReparentAndMaxHeight = rnd.nextFrom(nodeToReparentAndMaxHeights);
        const auto nodeToReparent = nodeToReparentAndMaxHeight.first;
        const auto maxNewParentHeight = nodeToReparentAndMaxHeight.second;
        const auto newParentHeight = rnd.next(nodeToReparent->data.height, maxNewParentHeight);

        auto newParent = findRandomValidNewParent(nodeToReparent, allNodes, newParentHeight, newParentHeight, lookupInfo);

        //cout << " added strand-covered  query: nodeToReparent: " << nodeToReparent->id() << " newParent:" << newParent->id() << endl;

        queries.push_back({nodeToReparent, newParent});
    }
}

int main(int argc, char* argv[])
{
    TestSuite<SubtaskInfo> testsuite;
    testsuite.setTestFileVerifier(&verifyTestFile);

    // SUBTASK 1
    {
        {
            auto& testFile = testsuite.newTestFile(MC2TTestFileInfo().belongingToSubtask(subtask1)
                    .withSeed(20938851)
                    .withDescription("Sample"));
            {
                auto& testcase = testFile.newTestcase(MC2TTestCaseInfo().withDescription("Sample testcase 1of2"));


                TreeGenerator<NodeData> treeGenerator;
                const auto one = treeGenerator.createNode();
                const auto two = treeGenerator.createNode();
                const auto three = treeGenerator.createNode();
                const auto four = treeGenerator.createNode();
                const auto five = treeGenerator.createNode();

                treeGenerator.addEdge(one, three);
                treeGenerator.addEdge(one, two);
                treeGenerator.addEdge(two, four);
                treeGenerator.addEdge(two, five);

                const int numQueries = 3;
                vector<TestQuery> queries;
                auto lookupInfo = computeLookupInfo(treeGenerator);
                addRandomQueries(treeGenerator, queries, numQueries, lookupInfo);
                shuffle(queries.begin(), queries.end());

                writeTestCase(treeGenerator, testcase, queries);
            }
            {
                auto& testcase = testFile.newTestcase(MC2TTestCaseInfo().withDescription("Sample testcase 2of2"));


                TreeGenerator<NodeData> treeGenerator;
                const auto one = treeGenerator.createNode();
                const auto two = treeGenerator.createNode();
                const auto three = treeGenerator.createNode();
                const auto four = treeGenerator.createNode();
                const auto five = treeGenerator.createNode();
                const auto six = treeGenerator.createNode();
                const auto seven = treeGenerator.createNode();

                treeGenerator.addEdge(one, seven);
                treeGenerator.addEdge(two, one);
                treeGenerator.addEdge(six, one);
                treeGenerator.addEdge(five, four);
                treeGenerator.addEdge(three, five);
                treeGenerator.addEdge(one, five);

                const int numQueries = 5;
                vector<TestQuery> queries;
                auto lookupInfo = computeLookupInfo(treeGenerator);
                addRandomQueries(treeGenerator, queries, numQueries, lookupInfo);
                shuffle(queries.begin(), queries.end());

                writeTestCase(treeGenerator, testcase, queries);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(MC2TTestFileInfo().belongingToSubtask(subtask1)
                    .withSeed(23984723)
                    .withDescription("Misc tiny testcases; randomly generated"));
            cout << "subtask1.maxNodesPerTestcase: " << subtask1.maxNodesPerTestcase << endl;
            for (int i = 0; i < subtask1.maxNumTestcases; i++)
            {
                auto& testcase = testFile.newTestcase(MC2TTestCaseInfo());

                const int numNodes = subtask1.maxNodesPerTestcase - rnd.next(10);
                const int numQueries = subtask1.maxQueriesPerTestcase - rnd.next(10);
                cout << "numNodes: " << numNodes << " numQueries: " << numQueries << endl;

                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode();
                const int numNodesPhase1 = rnd.next(numNodes);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodesPhase1, rnd.next(1.0, 100.0));
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), rnd.next(1.0, 100.0));

                const auto allNodes = treeGenerator.nodes();
                computeLookupInfo(treeGenerator);

                vector<TestQuery> allPossibleQueries;
                for (auto nodeToReparent : allNodes)
                {
                    for (auto newParent : allNodes)
                    {
                        if (!newParent->data.isDescendantOf(*nodeToReparent))
                            allPossibleQueries.push_back({nodeToReparent, newParent});
                    }
                }

                assert(!allPossibleQueries.empty());

                const vector<TestQuery> queries = chooseKRandomFromNG(min<int>(allPossibleQueries.size(), numQueries), allPossibleQueries);
                assert(!queries.empty());

                scrambleAndwriteTestcase(treeGenerator, testcase, queries);
            }
        }
    }

    // SUBTASK 2
    {
        {
            auto& testFile = testsuite.newTestFile(MC2TTestFileInfo().belongingToSubtask(subtask2)
                    .withSeed(23985)
                    .withDescription("max testcases, mostly with about 900 nodes/ queries each"));

            for (int i = 0; i < subtask2.maxNumTestcases; i++)
            {
                cout << "Generating testcase " << i << " of " << subtask2.maxNumTestcases << endl;
                auto& testcase = testFile.newTestcase(MC2TTestCaseInfo());

                const int numNodes = subtask2.maxNodesPerTestcase - rnd.next(100);
                const int numQueries = subtask2.maxQueriesPerTestcase - rnd.next(100);
                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
                const int numNodesPhase1 = rnd.next(numNodes);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodesPhase1, rnd.next(1.0, 100.0));
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), rnd.next(1.0, 100.0));
                const auto allNodes = treeGenerator.nodes();
                assert(static_cast<int>(allNodes.size()) == numNodes);

                computeLookupInfo(treeGenerator);

                vector<TestQuery> allPossibleQueries;
                for (auto nodeToReparent : allNodes)
                {
                    for (auto newParent : allNodes)
                    {
                        if (!newParent->data.isDescendantOf(*nodeToReparent))
                            allPossibleQueries.push_back({nodeToReparent, newParent});
                    }
                }

                assert(!allPossibleQueries.empty());

                const vector<TestQuery> queries = chooseKRandomFromNG(min<int>(allPossibleQueries.size(), numQueries), allPossibleQueries);
                assert(!queries.empty());

                scrambleAndwriteTestcase(treeGenerator, testcase, queries);
            }
        }
    }

    // SUBTASK 3
    {
        {
            auto& testFile = testsuite.newTestFile(MC2TTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(9734)
                    .withDescription("Squat graph where all nodes have degree approximately 3 taking up ~120'000 nodes, then random nodes.  Some of the initial nodes of the squat graph are biased towards having a single child: this essentially increases the number of (nodeToReparent, newParentHeight) pairs where nodeToReparent has a large-ish number of descendants at height newParentHeight, which is good for making the cheat solutions time out."));
            {
                auto& testcase = testFile.newTestcase(MC2TTestCaseInfo());


                TreeGenerator<NodeData> treeGenerator;
                const int numNodes = subtask3.maxNodesOverAllTestcases;
                const int numQueries = 3 * subtask3.maxQueriesOverAllTestcases / 4;
                const int switchOverAfterNumNodes = 5000;
                // The first switchOverAfterNumNodes nodes are biased towards having a single child: this means that  they will likely have the same
                // set of descendants at a given height as their children: this effectively multiplies the number of (nodeToReparent, newParentHeight) pairs
                // that have a large number of descendants, which is necessarily to cause CHEAT_PHASE_THREE to TLE.
                buildGraphInLayers(treeGenerator, 180'000, { {1, 98.0}, {2, 2.0} }, switchOverAfterNumNodes, {
                        {2, 70.0},
                        {3, 5},
                        {1, 25.0}
                        });
                // Disguise the special structure of the first switchOverAfterNumNodes by dangling small chains/ strands from them, randomly.
                const int numStrandsToAdd = 800;
                const auto strandLengthForPreSwitchOverNode = chooseRandomValuesWithSum(numStrandsToAdd, numNodes - treeGenerator.numNodes(), 0);
                auto allNodes = treeGenerator.nodes();
                for (int i = 0; i < static_cast<int>(strandLengthForPreSwitchOverNode.size()); i++)
                {
                    treeGenerator.addNodeChain(allNodes[i], strandLengthForPreSwitchOverNode[i]);
                }
                allNodes = treeGenerator.nodes();


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
                            if (descendantRange.numInRange() > 0)
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
            auto& testFile = testsuite.newTestFile(MC2TTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(9734)
                    .withDescription("Similar to the previous squat graph, but the squat graph is smaller, and  there are two long arms appended to it"));
            {
                auto& testcase = testFile.newTestcase(MC2TTestCaseInfo());


                TreeGenerator<NodeData> treeGenerator;
                const int numNodes = subtask3.maxNodesOverAllTestcases;
                const int numQueries = subtask3.maxQueriesOverAllTestcases;
                const int switchOverAfterNumNodes = 5000;
                // The first switchOverAfterNumNodes nodes are biased towards having a single child: this means that  they will likely have the same
                // set of descendants at a given height as their children: this effectively multiplies the number of (nodeToReparent, newParentHeight) pairs
                // that have a large number of descendants, which is necessarily to cause CHEAT_PHASE_THREE to TLE.
                buildGraphInLayers(treeGenerator, 100'000, { {1, 98.0}, {2, 2.0} }, switchOverAfterNumNodes, {
                        {2, 70.0},
                        {3, 5},
                        {1, 25.0}
                        });
                int largestHeightInSquatPortion = -1;
                {
                    auto lookupInfo = computeLookupInfo(treeGenerator);
                    largestHeightInSquatPortion = lookupInfo.maxHeight;
                }
                cout << "largestHeightInSquatPortion: " << largestHeightInSquatPortion << endl;
                // Add a couple of long arms.
                auto allNodes = treeGenerator.nodes();
                vector<TestNode<NodeData>*> arms;
                const auto arm1Root = rnd.nextFrom(allNodes);
                const auto arm2Root = rnd.nextFrom(allNodes);
                const auto arm1 = treeGenerator.addNodeChain(arm1Root, rnd.next(35'000, 45'000));
                const auto arm2 = treeGenerator.addNodeChain(arm2Root, rnd.next(35'000, 45'000));
                arms.insert(arms.end(), arm1.begin(), arm1.end());
                arms.insert(arms.end(), arm2.begin(), arm2.end());

                allNodes = treeGenerator.nodes();
                // Disguise the special structure of the first switchOverAfterNumNodes by dangling small chains/ strands from them, randomly.
                const int numStrandsToAdd = 800;
                const auto strandLengthForPreSwitchOverNode = chooseRandomValuesWithSum(numStrandsToAdd, numNodes - treeGenerator.numNodes(), 0);
                for (int i = 0; i < static_cast<int>(strandLengthForPreSwitchOverNode.size()); i++)
                {
                    treeGenerator.addNodeChain(allNodes[i], strandLengthForPreSwitchOverNode[i]);
                }

                vector<TestQuery> queries;
                map<NodeAndHeightPair, double> numDescendantsForNodeAndHeight;
                allNodes = treeGenerator.nodes();
                {
                    auto lookupInfo = computeLookupInfo(treeGenerator);
                    for (auto nodeToReparent : allNodes)
                    {
                        if (nodeToReparent->data.largestNonDescendantHeight == 0)
                            continue;
                        bool haveFoundDescendants = false;
                        for (int newParentHeight = 0; newParentHeight <= min(nodeToReparent->data.largestNonDescendantHeight, largestHeightInSquatPortion); newParentHeight++)
                        {
                            const auto descendantRange = descendantRangeFor(nodeToReparent, newParentHeight, lookupInfo);
                            if (descendantRange.numInRange() > 0)
                            {
                                numDescendantsForNodeAndHeight[{nodeToReparent, newParentHeight}] = descendantRange.numInRange();
                                haveFoundDescendants = true;
                            }
                            else if (haveFoundDescendants)
                                break;
                        }
                    }
                    WeightedChooser<NodeAndHeightPair> nodeAndHeightPairChooser(numDescendantsForNodeAndHeight);
                    for (int i = 0; i < numQueries / 2; i++)
                    {
                        const auto nodeToReparentAndHeight = nodeAndHeightPairChooser.nextValue();
                        auto newParent = findRandomValidNewParent(nodeToReparentAndHeight.nodeToReparent, allNodes, nodeToReparentAndHeight.newParentHeight, nodeToReparentAndHeight.newParentHeight, lookupInfo);
                        queries.push_back({nodeToReparentAndHeight.nodeToReparent, newParent});
                    }
                    addWeightedQueries(arms, allNodes, queries, numQueries / 2, rnd.next(75.0, 85.0), lookupInfo);
                    addRandomQueries(treeGenerator, queries, numQueries, lookupInfo);
                }

                scrambleAndwriteTestcase(treeGenerator, testcase, queries);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(MC2TTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(9734)
                    .withDescription("Similar to the earlier 'squat graph' testcases.  Squat graph where all nodes have a strong bias towards one child (very rarely, two) for ~60'000, nodes, then switching towards a strong bias for two children.  This essentially increases the number of (nodeToReparent, newParentHeight) pairs where nodeToReparent has a large-ish number of descendants at height newParentHeight (even more than in the earlier squat graph testcases) which is good for making the cheat solutions time out."));
            {
                auto& testcase = testFile.newTestcase(MC2TTestCaseInfo());


                TreeGenerator<NodeData> treeGenerator;
                const int numNodes = subtask3.maxNodesOverAllTestcases;
                const int numQueries = 3 * subtask3.maxQueriesOverAllTestcases / 4;
                const int switchOverAfterNumNodes = 60'000;
                // The first switchOverAfterNumNodes nodes are heavily biased towards having a single child: this means that  they will likely have the same
                // set of descendants at a given height as their children: this effectively multiplies the number of (nodeToReparent, newParentHeight) pairs
                // that have a large number of descendants, which is necessarily to cause CHEAT_PHASE_THREE to TLE.
                buildGraphInLayers(treeGenerator, 180'000, { {1, 99.5}, {2, 0.5} }, switchOverAfterNumNodes, {
                        {2, 70.0},
                        {3, 5},
                        {1, 25.0}
                        });
                // Disguise the special structure of the first switchOverAfterNumNodes by dangling small chains/ strands from them, randomly.
                const int numStrandsToAdd = 800;
                const auto strandLengthForPreSwitchOverNode = chooseRandomValuesWithSum(numStrandsToAdd, numNodes - treeGenerator.numNodes(), 0);
                auto allNodes = treeGenerator.nodes();
                for (int i = 0; i < static_cast<int>(strandLengthForPreSwitchOverNode.size()); i++)
                {
                    treeGenerator.addNodeChain(allNodes[i], strandLengthForPreSwitchOverNode[i]);
                }
                allNodes = treeGenerator.nodes();

                vector<TestQuery> queries;
                map<NodeAndHeightPair, double> numDescendantsForNodeAndHeight;
                {
                    auto lookupInfo = computeLookupInfo(treeGenerator);
                    for (int height = 0; height <= lookupInfo.maxHeight; height++)
                    {
                        cout << "height: " << height << " #nodes at height: " << lookupInfo.nodesAtHeightLookup[height].size() << endl;
                    }
                    for (auto nodeToReparent : allNodes)
                    {
                        if (nodeToReparent->data.largestNonDescendantHeight == 0)
                            continue;
                        for (int newParentHeight = 0; newParentHeight <= nodeToReparent->data.largestNonDescendantHeight; newParentHeight++)
                        {
                            const auto descendantRange = descendantRangeFor(nodeToReparent, newParentHeight, lookupInfo);
                            if (descendantRange.numInRange() > 10)
                                numDescendantsForNodeAndHeight[{nodeToReparent, newParentHeight}] = descendantRange.numInRange();
                        }
                    }
                    WeightedChooser<NodeAndHeightPair> nodeAndHeightPairChooser(numDescendantsForNodeAndHeight);
                    set<NodeAndHeightPair> chosenNodeAndHeightPairs;
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
            auto& testFile = testsuite.newTestFile(MC2TTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(384732)
                    .withDescription("Two long arms, then random strands and clumps added.  This is intended to stress-test Phase Two."));
            {
                auto& testcase = testFile.newTestcase(MC2TTestCaseInfo());

                const int numNodes = subtask3.maxNodesOverAllTestcases;
                const int numQueries = subtask3.maxQueriesOverAllTestcases;

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode();
                const auto arm1 = treeGenerator.addNodeChain(rootNode, rnd.next(78'000, 82'000));
                const auto arm2 = treeGenerator.addNodeChain(rootNode, rnd.next(78'000, 82'000));
                vector<vector<TestNode<NodeData>*>> addedStrands;
                addStrandsAndClumps(treeGenerator, (numNodes - treeGenerator.numNodes()) / 2, 200, addedStrands);
                treeGenerator.createNodesWithRandomParent(numNodes - treeGenerator.numNodes());

                const auto allNodes = treeGenerator.nodes();

                vector<TestNode<NodeData>*> firstHalvesOfArms;
                firstHalvesOfArms.insert(firstHalvesOfArms.end(), arm1.begin(), arm1.begin() + arm1.size() / 2);
                firstHalvesOfArms.insert(firstHalvesOfArms.end(), arm2.begin(), arm2.begin() + arm2.size() / 2);

                vector<TestNode<NodeData>*> secondHalvesOfArms;
                secondHalvesOfArms.insert(secondHalvesOfArms.end(), arm1.begin() + arm1.size() / 2, arm1.end());
                secondHalvesOfArms.insert(secondHalvesOfArms.end(), arm2.begin() + arm2.size() / 2, arm2.end());

                vector<TestQuery> queries;
                {
                    auto lookupInfo = computeLookupInfo(treeGenerator);
                    addWeightedQueries(firstHalvesOfArms, allNodes, queries, 3 * numQueries / 4, rnd.next(75.0, 85.0), lookupInfo);
                    addWeightedQueries(secondHalvesOfArms, allNodes, queries, (numQueries - queries.size()) / 2, rnd.next(75.0, 85.0), lookupInfo);
                    const int numStrandCoveredQueries = 5000;
                    addWeightedQueries(secondHalvesOfArms, allNodes, queries, numQueries - queries.size() - numStrandCoveredQueries, rnd.next(75.0, 85.0), lookupInfo);
                    addQueriesCoveredByStrands({arm1, arm2}, addedStrands, allNodes, numStrandCoveredQueries, queries, lookupInfo);
                    addRandomQueries(treeGenerator, queries, numQueries, lookupInfo);
                }

                scrambleAndwriteTestcase(treeGenerator, testcase, queries);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(MC2TTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(4387543)
                    .withDescription("Three long arms, then random strands and clumps added.  This is intended to stress-test Phase Two."));
            {
                auto& testcase = testFile.newTestcase(MC2TTestCaseInfo());

                const int numNodes = subtask3.maxNodesOverAllTestcases;
                const int numQueries = subtask3.maxQueriesOverAllTestcases;

                TreeGenerator<NodeData> treeGenerator;
                auto rootNode = treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
                const auto arm1 = treeGenerator.addNodeChain(rootNode, rnd.next(48'000, 52'000));
                const auto arm2 = treeGenerator.addNodeChain(rootNode, rnd.next(48'000, 52'000));
                const int posOf3rdArmAlong1st = rnd.next(700, 800);
                const auto arm3 = treeGenerator.addNodeChain(arm1[posOf3rdArmAlong1st], rnd.next(48'000, 52'000));
                vector<vector<TestNode<NodeData>*>> addedStrands;
                addStrandsAndClumps(treeGenerator, (numNodes - treeGenerator.numNodes()) / 2, 200, addedStrands);
                treeGenerator.createNodesWithRandomParent(numNodes - treeGenerator.numNodes());

                const auto allNodes = treeGenerator.nodes();

                vector<TestNode<NodeData>*> firstHalvesOfArms;
                firstHalvesOfArms.insert(firstHalvesOfArms.end(), arm1.begin(), arm1.begin() + arm1.size() / 2);
                firstHalvesOfArms.insert(firstHalvesOfArms.end(), arm2.begin(), arm2.begin() + arm2.size() / 2);
                firstHalvesOfArms.insert(firstHalvesOfArms.end(), arm3.begin(), arm3.begin() + arm3.size() / 2);

                vector<TestNode<NodeData>*> secondHalvesOfArms;
                secondHalvesOfArms.insert(secondHalvesOfArms.end(), arm1.begin() + arm1.size() / 2, arm1.end());
                secondHalvesOfArms.insert(secondHalvesOfArms.end(), arm2.begin() + arm2.size() / 2, arm2.end());
                secondHalvesOfArms.insert(secondHalvesOfArms.end(), arm3.begin() + arm3.size() / 2, arm3.end());

                vector<TestQuery> queries;
                {
                    auto lookupInfo = computeLookupInfo(treeGenerator);
                    addWeightedQueries(firstHalvesOfArms, allNodes, queries, 3 * numQueries / 4, rnd.next(75.0, 85.0), lookupInfo);
                    addWeightedQueries(secondHalvesOfArms, allNodes, queries, (numQueries - queries.size()) / 2, rnd.next(75.0, 85.0), lookupInfo);
                    const int numStrandCoveredQueries = 5000;
                    addWeightedQueries(secondHalvesOfArms, allNodes, queries, numQueries - queries.size() - numStrandCoveredQueries, rnd.next(75.0, 85.0), lookupInfo);
                    addQueriesCoveredByStrands({arm1, arm2, arm3}, addedStrands, allNodes, numStrandCoveredQueries, queries, lookupInfo);

                    addRandomQueries(treeGenerator, queries, numQueries - 1, lookupInfo);
                    // Add one query that picks a random node n and chooses a query such that the result is the
                    // first reparenting that reparents n.  This tests an edge-case in the Phase One code:
                    // they must find the first numCanReparentToPrefixSum *strictly greater than* indexInOriginalList, rather
                    // than the first that is *greater than or equal to* indexInOriginalList.
                    // We scramble the graph now, as we want the node ids to be finalised for this, as the "last" reparenting
                    // depends on the node ids.
                    treeGenerator.scrambleNodeIdsAndReorder(rootNode /* Ensure that the rootNode keeps its id of 1 */);
                    treeGenerator.scrambleEdgeOrder();
                    // Need to recompute DFS info, else "isDescendantOf" won't work.
                    computeDFSInfo(rootNode);
                    auto nodeToReparent = rnd.nextFrom(allNodes);

                    auto findNodesCanReparentTo = [&allNodes](auto nodeToReparent)
                        {
                            vector<TestNode<NodeData>*> nodesCanReparentTo;
                            for (auto node : allNodes)
                            {
                                if (!node->data.isDescendantOf(*nodeToReparent))
                                    nodesCanReparentTo.push_back(node);
                            }
                            sort(nodesCanReparentTo.begin(), nodesCanReparentTo.end(), [](const auto& node1, const auto& node2)
                                    {
                                        if (node1->data.height != node2->data.height)
                                            return node1->data.height < node2->data.height;
                                        return node1->id() < node2->id();
                                    });
                            return nodesCanReparentTo;
                        };


                    const auto nodesCanReparentTo = findNodesCanReparentTo(nodeToReparent);
                    assert(!nodesCanReparentTo.empty());
                    auto newParentNode = nodesCanReparentTo.front();
                    queries.push_back({nodeToReparent, newParentNode});
                    cout << "magic: nodeToReparent: " << nodeToReparent->id() << " newParentNode: " << newParentNode->id() << endl;
                }

                // Do *NOT* use "scrambleAndwriteTestcase", as that would break the "final reparenting that reparents n"
                // query.  We must shuffle the queries ourselves.
                assert(queries.size() == numQueries);
                shuffle(queries.begin(), queries.end());
                writeTestCase(treeGenerator, testcase, queries);
            }
        }
        {
            auto& testFile = testsuite.newTestFile(MC2TTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(107466890)
                    .withDescription("Four long (~40k) arms originating at (or near) root; then random strands and clumps added.  This is (again) intended to stress-test Phase Two."));
            {
                auto& testcase = testFile.newTestcase(MC2TTestCaseInfo());

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
                vector<vector<TestNode<NodeData>*>> addedStrands;
                addStrandsAndClumps(treeGenerator, (numNodes - treeGenerator.numNodes()) / 2, rnd.next(200, 400), addedStrands);
                treeGenerator.createNodesWithRandomParent(numNodes - treeGenerator.numNodes());

                const auto allNodes = treeGenerator.nodes();

                vector<TestNode<NodeData>*> firstHalvesOfArms;
                firstHalvesOfArms.insert(firstHalvesOfArms.end(), arm1.begin(), arm1.begin() + arm1.size() / 2);
                firstHalvesOfArms.insert(firstHalvesOfArms.end(), arm2.begin(), arm2.begin() + arm2.size() / 2);
                firstHalvesOfArms.insert(firstHalvesOfArms.end(), arm3.begin(), arm3.begin() + arm3.size() / 2);
                firstHalvesOfArms.insert(firstHalvesOfArms.end(), arm4.begin(), arm4.begin() + arm4.size() / 2);

                vector<TestNode<NodeData>*> secondHalvesOfArms;
                secondHalvesOfArms.insert(secondHalvesOfArms.end(), arm1.begin() + arm1.size() / 2, arm1.end());
                secondHalvesOfArms.insert(secondHalvesOfArms.end(), arm2.begin() + arm2.size() / 2, arm2.end());
                secondHalvesOfArms.insert(secondHalvesOfArms.end(), arm3.begin() + arm3.size() / 2, arm3.end());
                secondHalvesOfArms.insert(secondHalvesOfArms.end(), arm4.begin() + arm4.size() / 2, arm4.end());

                vector<TestQuery> queries;
                {
                    auto lookupInfo = computeLookupInfo(treeGenerator);
                    addWeightedQueries(firstHalvesOfArms, allNodes, queries, 3 * numQueries / 4, rnd.next(75.0, 85.0), lookupInfo);
                    addWeightedQueries(secondHalvesOfArms, allNodes, queries, (numQueries - queries.size()) / 2, rnd.next(75.0, 85.0), lookupInfo);
                    const int numStrandCoveredQueries = 5000;
                    addWeightedQueries(secondHalvesOfArms, allNodes, queries, numQueries - queries.size() - numStrandCoveredQueries, rnd.next(75.0, 85.0), lookupInfo);
                    addQueriesCoveredByStrands({arm1, arm2, arm3, arm4}, addedStrands, allNodes, numStrandCoveredQueries, queries, lookupInfo);
                    addRandomQueries(treeGenerator, queries, numQueries, lookupInfo);
                }

                scrambleAndwriteTestcase(treeGenerator, testcase, queries);
            }

        }
        {
            auto& testFile = testsuite.newTestFile(MC2TTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(329845743)
                    .withDescription("Two testcases - one 100'000 nodes (and queries) which is a graph with 2% leaf preference; the other is 100'000 nodes (and queries!) and 98% leaf preference"));
            {
                auto& testcase = testFile.newTestcase(MC2TTestCaseInfo());

                const int numNodes = subtask3.maxNodesOverAllTestcases / 2;
                const int numQueries = subtask3.maxQueriesOverAllTestcases / 2;

                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode();
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, 2.0);


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
                            if (descendantRange.numInRange() > 0)
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
            {
                auto& testcase = testFile.newTestcase(MC2TTestCaseInfo());

                const int numNodes = subtask3.maxNodesOverAllTestcases / 2;
                const int numQueries = subtask3.maxQueriesOverAllTestcases / 2;

                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode();
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes((numNodes - treeGenerator.numNodes()) / 2, 98.0);


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
                            if (descendantRange.numInRange() > 0)
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
            auto& testFile = testsuite.newTestFile(MC2TTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(9734)
                    .withDescription("Small, skeletal graph with 5 nodes at root, then a few more layers where the number of children is mostly 1, sometimes 2. Then a large number of clumps are distributed around the nodes of this small graph, until we have 200'000 nodes.  200'000 mostly random queries."));
            {
                auto& testcase = testFile.newTestcase(MC2TTestCaseInfo());


                TreeGenerator<NodeData> treeGenerator;
                const int numNodes = subtask3.maxNodesOverAllTestcases;
                const int numQueries = subtask3.maxQueriesOverAllTestcases;
                // The first switchOverAfterNumNodes nodes are biased towards having a single child: this means that  they will likely have the same
                // set of descendants at a given height as their children: this effectively multiplies the number of (nodeToReparent, newParentHeight) pairs
                // that have a large number of descendants, which is necessarily to cause CHEAT_PHASE_THREE to TLE.
                buildGraphInLayers(treeGenerator, 40, { {5, 100.0} }, 5, {
                        {1, 90.0},
                        {2, 10}
                        });

                const auto nodesInSquatGraph = treeGenerator.nodes();
                const int numNodesInSquatGraph = nodesInSquatGraph.size();
                const auto numInClumpForNode = chooseRandomValuesWithSum(numNodesInSquatGraph, numNodes - numNodesInSquatGraph, 1);
                for (int i = 0; i < numNodesInSquatGraph; i++)
                {
                    for (int j = 0; j < numInClumpForNode[i]; j++)
                    {
                        treeGenerator.createNode(nodesInSquatGraph[i]);
                    }
                }
                assert(treeGenerator.numNodes() == numNodes);

                vector<TestQuery> queries;
                const auto allNodes = treeGenerator.nodes();
                map<NodeAndHeightPair, double> numDescendantsForNodeAndHeight;
                {
                    auto lookupInfo = computeLookupInfo(treeGenerator);
                    for (int height = 0; height <= lookupInfo.maxHeight; height++)
                    {
                        cout << " height: " << height << " nodesAtHeightLookup: " << lookupInfo.nodesAtHeightLookup[height].size() << endl;
                    }
                    for (auto nodeToReparent : allNodes)
                    {
                        if (nodeToReparent->data.largestNonDescendantHeight == 0)
                            continue;
                        bool haveFoundDescendants = false;
                        for (int newParentHeight = 0; newParentHeight <= nodeToReparent->data.largestNonDescendantHeight; newParentHeight++)
                        {
                            const auto descendantRange = descendantRangeFor(nodeToReparent, newParentHeight, lookupInfo);
                            if (descendantRange.numInRange() > 0)
                            {
                                numDescendantsForNodeAndHeight[{nodeToReparent, newParentHeight}] = descendantRange.numInRange();
                                haveFoundDescendants = true;
                            }
                            else if (haveFoundDescendants)
                                break;
                        }
                    }
                    WeightedChooser<NodeAndHeightPair> nodeAndHeightPairChooser(numDescendantsForNodeAndHeight);
                    for (int i = 0; i < 3 * numQueries / 4; i++)
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
            auto& testFile = testsuite.newTestFile(MC2TTestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(13984)
                    .withDescription("max testcases, mostly with about 200 nodes/ queries each but in total equalling maxNodesOverAllTestcases/ maxQueriesOverAllTestcases"));

            const auto numNodesForTestCase = chooseRandomValuesWithSum(subtask3.maxNumTestcases, subtask3.maxNodesOverAllTestcases, 2);
            const auto numQueriesForTestCase = chooseRandomValuesWithSum(subtask3.maxNumTestcases, subtask3.maxQueriesOverAllTestcases, 1);
            for (int i = 0; i < subtask3.maxNumTestcases; i++)
            {
                cout << "Generating testcase " << i << " of " << subtask3.maxNumTestcases << endl;
                auto& testcase = testFile.newTestcase(MC2TTestCaseInfo());

                const int numNodes = numNodesForTestCase[i];
                const int numQueries = numQueriesForTestCase[i];
                TreeGenerator<NodeData> treeGenerator;
                treeGenerator.createNode(); // Need to create at least one node for randomised generation of other nodes.
                const int numNodesPhase1 = rnd.next(numNodes);
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodesPhase1, rnd.next(1.0, 100.0));
                treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), rnd.next(1.0, 100.0));
                const auto allNodes = treeGenerator.nodes();
                assert(static_cast<int>(allNodes.size()) == numNodes);

                computeLookupInfo(treeGenerator);

                vector<TestQuery> allPossibleQueries;
                for (auto nodeToReparent : allNodes)
                {
                    for (auto newParent : allNodes)
                    {
                        if (!newParent->data.isDescendantOf(*nodeToReparent))
                            allPossibleQueries.push_back({nodeToReparent, newParent});
                    }
                }

                assert(!allPossibleQueries.empty());

                const vector<TestQuery> queries = chooseKRandomFromNG(min<int>(allPossibleQueries.size(), numQueries), allPossibleQueries);
                assert(!queries.empty());

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
    int64_t totalNumQueries = 0;
    const int64_t maxEncryptedKey = static_cast<int64_t>(1) << 36;

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
            assert(encryptedQuery < maxEncryptedKey);
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
