#include <testlib.h>
#include <random-utilities.h>
#include "testcase-generator-lib.h"
#include "utils.h"

#include <iostream>
#include <fstream>

#include <cassert>

using namespace std;

constexpr int maxQueries = 500'000;

const int64_t Mod = 1'000'000'007;

struct SubtaskInfo
{
    int subtaskId = -1;
    int score = -1;

    int maxQueriesPerTestcase = -1;
    int maxQueriesOverAllTestcases = -1;
    int maxNumTestcases = -1;
    int64_t maxDocLength = -1;
    bool allowsUndoRedo = false;

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
    SubtaskInfo& withMaxQueriesPerTestcase(int maxQueriesPerTestcase)
    {
        this->maxQueriesPerTestcase = maxQueriesPerTestcase;
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
    SubtaskInfo& withMaxNumDocLength(int64_t maxDocLength)
    {
        this->maxDocLength = maxDocLength;
        return *this;
    }
    SubtaskInfo& withAllowsUndoRedo(bool allowsUndoRedo)
    {
        this->allowsUndoRedo = allowsUndoRedo;
        return *this;
    }
};

const int NoExplicitLimit = std::numeric_limits<int>::max();
const auto maxDocLength = 1e18;

SubtaskInfo subtask1 = SubtaskInfo::create().withSubtaskId(1)
                                            .withScore(5)
                                            .withMaxQueriesPerTestcase(100) // TODO - should this be 1000?
                                            .withMaxQueriesOverAllTestcases(NoExplicitLimit)
                                            .withAllowsUndoRedo(true)
                                            .withMaxNumDocLength(1000)
                                            .withMaxNumTestcases(10);
                                            
SubtaskInfo subtask2 = SubtaskInfo::create().withSubtaskId(2)
                                            .withScore(40)
                                            .withMaxQueriesPerTestcase(maxQueries)
                                            .withMaxQueriesOverAllTestcases(maxQueries)
                                            .withAllowsUndoRedo(false)
                                            .withMaxNumDocLength(maxDocLength)
                                            .withMaxNumTestcases(1000);

SubtaskInfo subtask3 = SubtaskInfo::create().withSubtaskId(3)
                                            .withScore(55)
                                            .withMaxQueriesPerTestcase(maxQueries)
                                            .withMaxQueriesOverAllTestcases(maxQueries)
                                            .withAllowsUndoRedo(true)
                                            .withMaxNumDocLength(maxDocLength)
                                            .withMaxNumTestcases(1000);
                                        
std::ostream& operator<<(std::ostream& outStream, const SubtaskInfo& subtaskInfo)
{
    outStream << "Constraints: " << std::endl;
    outStream << " Maximum queries per testcase (i.e. Q): " << subtaskInfo.maxQueriesPerTestcase << std::endl;
    if (subtaskInfo.maxQueriesOverAllTestcases != NoExplicitLimit)
        outStream << " Maximum sum of Q over all testcases: " << subtaskInfo.maxQueriesOverAllTestcases << std::endl;
    outStream << " Maximum document length at any point: " << subtaskInfo.maxDocLength << std::endl;
    outStream << " Allows undo/ redo: " << (subtaskInfo.allowsUndoRedo ? "true" : "false") << std::endl;
    outStream << " Max num testcases (i.e. T): " << subtaskInfo.maxNumTestcases << std::endl;

    return outStream;
};

using SMETestCaseInfo = TestcaseInfo<SubtaskInfo>;
using SMETestFileInfo = TestFileInfo<SubtaskInfo>;

struct TestQuery
{
    enum Type { InsertFormatting, InsertNonFormatting, IsRangeFormatted, Undo, Redo };
    Type type;
    int64_t insertionPos = -1;
    int64_t numToInsert = -1;
    int64_t queryPosition = -1;
    int64_t numToUndo = -1;
    int64_t numToRedo = -1;

    int64_t encryptedArgument = -1;
    int64_t encryptedArgument2 = -1;
};

void writeTestCase(Testcase<SubtaskInfo>& destTestcase, const std::vector<TestQuery>& queries)
{
    destTestcase.writeLine<int>(queries.size());
    int64_t encryptionKey = 0;
    int64_t powerOf2 = 2;

    AVLTree formattingCharsTree(10'000);
    // Add Sentinel node.
    formattingCharsTree.insertFormattingChar(0);
    formattingCharsTree.root()->isSentinelValue = true;

    int queryNum = 1;
    for (const auto& query : queries)
    {
        switch (query.type)
        {
            case TestQuery::InsertFormatting:
                {
                    const auto insertionPos = query.insertionPos - 1;
                    formattingCharsTree.insertFormattingChar(insertionPos);
                    destTestcase.writeLine<char, int64_t>('F', (insertionPos + 1) ^ encryptionKey);
                }
                break;
            case TestQuery::InsertNonFormatting:
                {
                    const auto insertionPos = query.insertionPos - 1;
                    const auto numToInsert = query.numToInsert;
                    formattingCharsTree.insertNonFormattingChars(insertionPos, numToInsert);
                    destTestcase.writeLine<char, int64_t, int64_t>('N', (insertionPos + 1) ^ encryptionKey, numToInsert ^ encryptionKey);
                }
                break;
            case TestQuery::IsRangeFormatted:
                {
                    const auto queryPosition = query.queryPosition - 1;
                    auto queryAnswer = formattingCharsTree.distBetweenEnclosingFormattedChars(queryPosition);
                    destTestcase.writeLine<char, int64_t>('Q', (queryPosition + 1) ^ encryptionKey);

                    // Update encryptionKey for next queries.
                    if (queryAnswer == -1)
                        queryAnswer = 3'141'592;

                    encryptionKey = (encryptionKey + ((queryAnswer % Mod) * powerOf2) % Mod) % Mod;
                }
                break;
            case TestQuery::Undo:
                {
                    const int numToUndo = query.numToUndo;
                    formattingCharsTree.undo(numToUndo);
                    destTestcase.writeLine<char, int64_t>('U', numToUndo ^ encryptionKey);
                }
                break;
            case TestQuery::Redo:
                {
                    const int numToRedo = query.numToRedo;
                    formattingCharsTree.redo(numToRedo);
                    destTestcase.writeLine<char, int64_t>('R', numToRedo ^ encryptionKey);
                }
                break;
        }

        queryNum++;
        powerOf2 = (2 * powerOf2) % Mod;

    }
    cout << "final encryption key: " << encryptionKey << endl;
}

class QueryGenUtils
{
    public:
        QueryGenUtils()
        {
            // Add Sentinel node.
            formattingCharsTree.insertFormattingChar(0);
            formattingCharsTree.root()->isSentinelValue = true;
        }
        void setUndoAndRedoAllowed(bool isAllowed)
        {
            m_undoAndRedoAllowed = isAllowed;
        }

        void addInsertFormattingCharQuery()
        {
            const auto numFormattingChars = formattingCharsTree.root()->totalFormattedDescendants; // Includes sentinel.
            const auto formattedCharIndexToChoose = rnd.next(numFormattingChars);

            const auto chosenFormattingCharIter = formattingCharsTree.findKthFormattingChar(formattedCharIndexToChoose);
            const auto pos = chosenFormattingCharIter.currentNodePosition() - rnd.next(chosenFormattingCharIter.currentNode()->leftNonFormattedRunSize + 1);
            assert(pos >= 0);
            TestQuery newQuery;
            newQuery.type = TestQuery::InsertFormatting;
            newQuery.insertionPos = pos + 1;

            addQuery(newQuery);
        }

        void addInsertFormattingCharQuery(int64_t insertionPos) /* insertionPos is 0-relative */
        {
            TestQuery newQuery;
            newQuery.type = TestQuery::InsertFormatting;
            newQuery.insertionPos = insertionPos + 1;

            addQuery(newQuery);
        }

        void addInsertNonFormattingCharQuery(int64_t numToInsert)
        {
            const auto numFormattedCharsWithoutNonFormattingToLeft = (formattingCharsTree.root()->totalFormattedDescendants) -  formattingCharsTree.root()->totalFormattedDescendantsWithNonFormattedToLeft; // Includes sentinel, if sentinel has no formatting chars to left.
            const bool chooseFormattedCharWithoutNonFormattingToLeft = (numFormattedCharsWithoutNonFormattingToLeft > 0 && rnd.next(0.0, 100.0) <= 80.0);
            AVLTreeIterator chosenFormattingCharIter(nullptr);
            if (chooseFormattedCharWithoutNonFormattingToLeft)
            {
                const auto chosenFormattingWithoutNonFormattingIndex = rnd.next(numFormattedCharsWithoutNonFormattingToLeft);
                chosenFormattingCharIter = formattingCharsTree.findKthFormattingCharWithoutNonFormattingToLeft(chosenFormattingWithoutNonFormattingIndex);
            }
            else
            {
                const auto numFormattingChars = formattingCharsTree.root()->totalFormattedDescendants; // Includes sentinel.
                const auto chosenFormattedCharIndex = rnd.next(numFormattingChars);
                chosenFormattingCharIter = formattingCharsTree.findKthFormattingChar(chosenFormattedCharIndex);

            }
            assert(chosenFormattingCharIter.currentNode() != nullptr);
            const auto pos = chosenFormattingCharIter.currentNodePosition() - rnd.next(chosenFormattingCharIter.currentNode()->leftNonFormattedRunSize + 1);
            assert(pos >= 0);
            TestQuery newQuery;
            newQuery.type = TestQuery::InsertNonFormatting;
            newQuery.insertionPos = pos + 1;
            newQuery.numToInsert = numToInsert;

            addQuery(newQuery);
        }

        void addInsertNonFormattingCharQuery(int64_t insertionPos, int64_t numToInsert) /* insertionPos is 0-relative */
        {
            TestQuery newQuery;
            newQuery.type = TestQuery::InsertNonFormatting;
            newQuery.insertionPos = insertionPos + 1;
            newQuery.numToInsert = numToInsert;

            addQuery(newQuery);
        }
        void addIsRangeFormattedQuery()
        {
            addIsRangeFormattedQueryAtOrAfterPos(0);
        }

        void addIsRangeFormattedQuery(int64_t queryPosition)
        {
            TestQuery newQuery;
            newQuery.type = TestQuery::IsRangeFormatted;
            newQuery.queryPosition = queryPosition + 1;
            
            addQuery(newQuery);
        }

        void addIsRangeFormattedQueryBiasingTowardsAfterInsertionPos()
        {
            const bool chooseAfterLastInsertionPos = (lastInsertionPos != -1) && (rnd.next(100) <= 95);
            if (chooseAfterLastInsertionPos)
            {
                cout << "Choose IsRangeFormatted after " << lastInsertionPos << endl;
                addIsRangeFormattedQueryAtOrAfterPos(lastInsertionPos);
            }
            else
            {
                addIsRangeFormattedQuery();
            }
        }

        void addIsRangeFormattedQueryAtOrAfterPos(int64_t minPos)
        {
            assert(canRangeQuery());
            // Choosing a position purely at random will bias in favour of long runs of 
            // non-formatting chars: instead, pick a formatted char (that has at least one
            // non-formatting char in the run to its left) and then pick a random position
            // in that run.  Further, bias towards queries that are within a formatted range.
            //
            // We include the sentinel as a valid formatting char, here, otherwise we won't include 
            // the very last run of non-formatting chars in queries.
            const auto totalNumFormattingWithNonFormattingToLeft = formattingCharsTree.root()->totalFormattedDescendantsWithNonFormattedToLeft;
            const auto numFormattingWithNonFormattingToLeftUpToMinPos = formattingCharsTree.numFormattingCharWithNonFormattingCharsAtOrToLeft(minPos);
            const auto numFormattingWithNonFormattingToLeftAfterMinPos = totalNumFormattingWithNonFormattingToLeft - numFormattingWithNonFormattingToLeftUpToMinPos;
            //const auto numFormattingWithoutNonFormattingToLeft = (formattingCharsTree.numFormattingChars() + 1 /* The "+ 1" is for the Sentinel*/) - numFormattingWithNonFormattingToLeft;
            const int minNonFormattingWithNonFormattingToLeftIndex = numFormattingWithNonFormattingToLeftUpToMinPos;
            const int maxNonFormattingWithNonFormattingToLeftIndex = totalNumFormattingWithNonFormattingToLeft - 1;
            if (minNonFormattingWithNonFormattingToLeftIndex > maxNonFormattingWithNonFormattingToLeftIndex)
            {
                //cout << "Can't choose! minPos: " << minPos << " minNonFormattingWithNonFormattingToLeftIndex: " << minNonFormattingWithNonFormattingToLeftIndex << " maxNonFormattingWithNonFormattingToLeftIndex: " << maxNonFormattingWithNonFormattingToLeftIndex << " document: " << formattingCharsTree.documentString() << endl;
                // Can't choose one after this minPos: ignore minPos.
                addIsRangeFormattedQueryAtOrAfterPos(0);
                return;
            }
            auto validFormattingToChoose = rnd.next(minNonFormattingWithNonFormattingToLeftIndex, maxNonFormattingWithNonFormattingToLeftIndex);
            auto formattedCharIter = formattingCharsTree.findKthFormattingCharWithNonFormattingToLeft(validFormattingToChoose);
            // Queries in a non-formatted range are boring (always have answer 3'141'592) - bias heavily towards ranges
            // that are formatted.
            const bool forcePickFormattedRange = numFormattingWithNonFormattingToLeftAfterMinPos >= 2 && rnd.next(100) <= 95;
            if (forcePickFormattedRange && formattedCharIter.numFormattingCharsToLeft() % 2 == 0 && !formattedCharIter.currentNode()->isSentinelValue)
            {
                // Pick the next formatted char - if it has leftNonFormattedRunSize > 0, prefer it.
                // Otherwise, just stick with this being a query in an unformatted range - it's not worth the effot
                // of hunting for a better one.
                const auto nextFormattedCharIter = formattingCharsTree.findFirstNodeAtOrToRightOf(formattedCharIter.currentNodePosition() + 1);
                if (nextFormattedCharIter.currentNode()->leftNonFormattedRunSize != 0)
                    formattedCharIter = nextFormattedCharIter;
            }

            // We've chosen the formatting char; now choose the position of the non-formatting char in the run to its left.
            // TODO - bias towards the beginning/ end of range, to make it harder for people to get the correct answer.
            const auto formattedCharPos = formattedCharIter.currentNodePosition();
            const auto numNonFormattedCharsToChooseFrom = min(formattedCharIter.currentNode()->leftNonFormattedRunSize, formattedCharPos - minPos);
            WeightedChooser2<int64_t> distFromEdgeChooser({
                    {1 , 3},
                    {2 , 2},
                    {3 , 3},
                    {4 , 4},
                    {5 , 5},
                    {6 , 6},
                    {-1 , 10},
                    });

            const auto distFromFormattedCharChoice = distFromEdgeChooser.nextValue();
            int64_t distFromFormattedChar = -1;
            if (distFromFormattedCharChoice != -1)
            {
                if (rnd.next(2) == 0)
                {
                    distFromFormattedChar = distFromFormattedCharChoice;
                }
                else
                {
                    // Let distFromFormattedCharChoice represent distance *into* the run of formatted char,
                    // not from the end of it.
                    distFromFormattedChar = numNonFormattedCharsToChooseFrom - distFromFormattedCharChoice;
                }
                distFromFormattedChar = max<int64_t>(distFromFormattedChar, 1);
                distFromFormattedChar = min<int64_t>(distFromFormattedChar, numNonFormattedCharsToChooseFrom);
            }
            else
            {
                // Choose any value.
                distFromFormattedChar = (rnd.next(numNonFormattedCharsToChooseFrom)) + 1;
            }
            if (formattedCharPos <= minPos)
            {
                cout << "formattedCharPos: " << formattedCharPos << " minPos: " << minPos << " validFormattingToChoose: " << validFormattingToChoose << " minNonFormattingWithNonFormattingToLeftIndex: " << minNonFormattingWithNonFormattingToLeftIndex << " maxNonFormattingWithNonFormattingToLeftIndex: " << maxNonFormattingWithNonFormattingToLeftIndex << " numFormattingWithNonFormattingToLeftUpToMinPos: " << numFormattingWithNonFormattingToLeftUpToMinPos << endl;
                cout << "document: " << formattingCharsTree.documentString()  << endl;
                cout << "          " << string(minPos, ' ') << '^' << endl;
            }
            assert(formattedCharPos > minPos);
            const auto queryPosition = formattedCharPos - distFromFormattedChar;
            assert(queryPosition >= minPos);
            assert(formattingCharsTree.findFirstNodeAtOrToRightOf(queryPosition) == formattedCharIter);
            assert(queryPosition != formattedCharIter.currentNodePosition() && "Chosen IsRangeFormatted queryPosition at a formatting char!");
            TestQuery newQuery;
            newQuery.type = TestQuery::IsRangeFormatted;
            newQuery.queryPosition = queryPosition + 1;
            
            addQuery(newQuery);
        }

        void addUndoQuery()
        {
            assert(canUndo());
            TestQuery newQuery;
            const auto numToUndo = rnd.next(1, formattingCharsTree.undoStackPointer());
            newQuery.type = TestQuery::Undo;
            newQuery.numToUndo = numToUndo;
                
            addQuery(newQuery);
        }

        void addUndoQuery(int numToUndo)
        {
            assert(canUndo());
            TestQuery newQuery;
            newQuery.type = TestQuery::Undo;
            newQuery.numToUndo = numToUndo;
                
            addQuery(newQuery);
        }

        void addRedoQuery()
        {
            assert(canRedo());
            TestQuery newQuery;
            const int numToRedo = rnd.next(1, formattingCharsTree.undoStackSize() - 1 - formattingCharsTree.undoStackPointer() - 1);
            newQuery.type = TestQuery::Redo;
            newQuery.numToRedo = numToRedo;

            addQuery(newQuery);
        }

        void addRedoQuery(int numToRedo)
        {
            assert(canRedo());
            TestQuery newQuery;
            newQuery.type = TestQuery::Redo;
            newQuery.numToRedo = numToRedo;

            addQuery(newQuery);
        }

        void addQuery(const TestQuery& query)
        {
            queries.push_back(query);
            switch (query.type)
            {
                case TestQuery::InsertFormatting:
                    {
                        const auto insertionPos = query.insertionPos - 1;
                        lastInsertionPos = insertionPos;
                        formattingCharsTree.insertFormattingChar(insertionPos);
                        numInsertionQueries++;
                    }
                    break;
                case TestQuery::InsertNonFormatting:
                    {
                        const auto insertionPos = query.insertionPos - 1;
                        const auto numToInsert = query.numToInsert;
                        lastInsertionPos = insertionPos;

                        formattingCharsTree.insertNonFormattingChars(insertionPos, numToInsert);
                        numInsertionQueries++;
                    }
                    break;
                case TestQuery::IsRangeFormatted:
                    {
                        const auto queryPosition = query.queryPosition - 1;
                        auto queryAnswer = formattingCharsTree.distBetweenEnclosingFormattedChars(queryPosition);

                        // Update encryptionKey for next queries.
                        if (queryAnswer == -1)
                            queryAnswer = 3'141'592;

                        encryptionKey = (encryptionKey + ((queryAnswer % Mod) * powerOf2) % Mod) % Mod;


                        numRangeQueries++;
                    }
                    break;
                case TestQuery::Undo:
                    {
                        const auto numToUndo = query.numToUndo;
                        lastInsertionPos = -1; // lastInsertionPos is meaningless after Undo.
                        formattingCharsTree.undo(numToUndo);
                    }
                    break;
                case TestQuery::Redo:
                    {
                        const auto numToRedo = query.numToRedo;
                        lastInsertionPos = -1; // lastInsertionPos is meaningless after Redo.
                        formattingCharsTree.redo(numToRedo);

                    }
                    break;
            }
            powerOf2 = (powerOf2 * 2) % Mod;
        }

        bool canUndo() const
        {
            if (!m_undoAndRedoAllowed)
                return false;
            if (formattingCharsTree.undoStackPointer() == -1)
                return false;
            return true;
        }
        bool canRedo() const
        {
            if (!m_undoAndRedoAllowed)
                return false;
            if (formattingCharsTree.undoStackPointer() + 1 == formattingCharsTree.undoStackSize())
                return false;
            return true;
        }
        bool canRangeQuery() const
        {
            return formattingCharsTree.numNonFormattingChars() > 0;
        }

        vector<TestQuery> queries;
        AVLTree formattingCharsTree{10'000};
        int numInsertionQueries = 0;
        int numRangeQueries = 0;
        int64_t lastInsertionPos = -1;
        int64_t encryptionKey = 0;
        int64_t powerOf2 = 2;
    private:
        bool m_undoAndRedoAllowed = true;
};

void addRandomishQueries(QueryGenUtils& testcaseGenUtils, const int numQueriesToAdd, const int64_t maxDocLength)
{
    // At least one query per other query (undo/ redo, insert etc)
    const auto numIsRangeFormattedQueries = static_cast<int>(rnd.next(51.0, 55.0) * numQueriesToAdd / 100.0 + 0.5); // At least half IsRangeFormatted queries.
    const auto numNonIsRangeFormattedQueries = numQueriesToAdd - numIsRangeFormattedQueries;
    assert(numIsRangeFormattedQueries >= numNonIsRangeFormattedQueries);
    const auto numUndoOrRedoQueries = static_cast<int>(numNonIsRangeFormattedQueries * rnd.next(3.0, 7.0) / 100.0);
    const auto numInsertionQueries = numNonIsRangeFormattedQueries - numUndoOrRedoQueries;
    cout << "numNonIsRangeFormattedQueries: " << numNonIsRangeFormattedQueries << " numInsertionQueries: " << numInsertionQueries << " numUndoOrRedoQueries: " << numUndoOrRedoQueries << " numQueriesToAdd: " << numQueriesToAdd << endl;
    int numUndoOrRedoRuns = 0;
    if (numUndoOrRedoQueries > 0)
    {
        numUndoOrRedoRuns = rnd.next(1, numUndoOrRedoQueries);
    }
    const auto numInsertionRuns = numUndoOrRedoRuns > 0 ? numUndoOrRedoRuns : 1;
    const auto numIsRangeQueryRuns = numNonIsRangeFormattedQueries;
    assert(numIsRangeQueryRuns <= numIsRangeFormattedQueries);

    cout << "numInsertionRuns: " << numInsertionRuns << " numInsertionQueries: " << numInsertionQueries << endl;
    const auto numInsertionQueriesInRun = chooseRandomValuesWithSum3(numInsertionRuns, numInsertionQueries, 1);
    vector<int64_t> numUndoOrRedoQueriesInRun;
    if (numUndoOrRedoRuns > 0)
        numUndoOrRedoQueriesInRun = chooseRandomValuesWithSum3(numUndoOrRedoRuns, numUndoOrRedoQueries, 1);
    cout << "numIsRangeQueryRuns: " << numIsRangeQueryRuns << " numIsRangeFormattedQueries: " << numIsRangeFormattedQueries << endl;
    const auto numIsRangeFormattedQueriesInRun = chooseRandomValuesWithSum3(numIsRangeQueryRuns, numIsRangeFormattedQueries, 1);

    int insertionQueriesRunIndex = 0;
    int undoOrRedoQueriesRunIndex = 0;
    int isRangeFormattedQueriesRunIndex = 0;

    bool addedQuery = false;
    do 
    {
        addedQuery = false;
        if (insertionQueriesRunIndex != numInsertionRuns)
        {
            cout << "Adding insertion run" << endl;
            const auto numInsertionQueriesToAdd = numInsertionQueriesInRun[insertionQueriesRunIndex];
            const auto targetDocumentLength = rnd.next(testcaseGenUtils.formattingCharsTree.documentLength() + 1, maxDocLength);
            const auto numCharsToAddToReachTargetLength = targetDocumentLength - testcaseGenUtils.formattingCharsTree.documentLength();
            cout << "numInsertionQueriesToAdd: " << numInsertionQueriesToAdd << " targetDocumentLength: " << targetDocumentLength << " numCharsToAddToReachTargetLength: " << numCharsToAddToReachTargetLength << endl;
            assert(numCharsToAddToReachTargetLength >= numInsertionQueriesToAdd);
            auto numNonFormattedInsertions = rnd.next(static_cast<int64_t>(0), numInsertionQueriesToAdd);
            if (!testcaseGenUtils.canRangeQuery() && numNonFormattedInsertions == 0)
                numNonFormattedInsertions++;
            const auto numFormattedInsertions = numInsertionQueriesToAdd - numNonFormattedInsertions;
            vector<int64_t> numNonFormattedCharsToAddForQuery;
            if (numNonFormattedInsertions > 0)
            {
                cout << "numNonFormattedInsertions: " << numNonFormattedInsertions << " numFormattedInsertions: " << numFormattedInsertions << " numCharsToAddToReachTargetLength: " << numCharsToAddToReachTargetLength << endl;
                numNonFormattedCharsToAddForQuery = chooseRandomValuesWithSum3(numNonFormattedInsertions, numCharsToAddToReachTargetLength - numFormattedInsertions, 1);
            }

            vector<TestQuery::Type> insertionTypes;
            for (int i = 0; i < numFormattedInsertions; i++)
                insertionTypes.push_back(TestQuery::InsertFormatting);
            for (int i = 0; i < numNonFormattedInsertions; i++)
                insertionTypes.push_back(TestQuery::InsertNonFormatting);

            cout << "numNonFormattedInsertions: " << numNonFormattedInsertions << " numFormattedInsertions: " << numFormattedInsertions << endl;
            ::shuffle(insertionTypes.begin(), insertionTypes.end());
            if (!testcaseGenUtils.canRangeQuery() && insertionTypes.front() != TestQuery::InsertNonFormatting)
            {
                const auto firstNonFormattingIter = std::find(insertionTypes.begin(), insertionTypes.end(), TestQuery::InsertNonFormatting);
                assert(firstNonFormattingIter != insertionTypes.end());
                iter_swap(insertionTypes.begin(), firstNonFormattingIter);
            }


            int formattedInsertionIndex = 0;
            for (const auto& insertionType : insertionTypes)
            {
                if (insertionType == TestQuery::InsertNonFormatting)
                {
                    testcaseGenUtils.addInsertNonFormattingCharQuery(numNonFormattedCharsToAddForQuery[formattedInsertionIndex]);
                    formattedInsertionIndex++;
                }
                else
                {
                    testcaseGenUtils.addInsertFormattingCharQuery();
                }
                // Follow each insertion with a range query.
                assert(isRangeFormattedQueriesRunIndex < static_cast<int>(numIsRangeFormattedQueriesInRun.size()));
                for (int j = 0; j < numIsRangeFormattedQueriesInRun[isRangeFormattedQueriesRunIndex]; j++)
                {
                    testcaseGenUtils.addIsRangeFormattedQueryBiasingTowardsAfterInsertionPos();
                }
                isRangeFormattedQueriesRunIndex++;
                addedQuery = true;
            }
            insertionQueriesRunIndex++;
        }
        if (undoOrRedoQueriesRunIndex != numUndoOrRedoRuns)
        {
            cout << "Adding Undo/ Redo run" << endl;
            cout << "undo stack size: " << testcaseGenUtils.formattingCharsTree.undoStackSize() << endl;
            cout << "undo stack pointer: " << testcaseGenUtils.formattingCharsTree.undoStackPointer() << endl;
            for (int i = 0; i < numUndoOrRedoQueriesInRun[undoOrRedoQueriesRunIndex]; i++)
            {
                assert(testcaseGenUtils.formattingCharsTree.undoStackSize() >= 1);
                const auto currentUndoStackIndex = testcaseGenUtils.formattingCharsTree.undoStackPointer(); // 0-relative
                int newUndoStackIndex = -1;
                while (true)
                {
                    newUndoStackIndex = rnd.next(-1, testcaseGenUtils.formattingCharsTree.undoStackSize() - 1);
                    cout << "newUndoStackIndex: " << newUndoStackIndex << endl;
                    if (newUndoStackIndex != currentUndoStackIndex)
                        break;
                }
                if (newUndoStackIndex < currentUndoStackIndex)
                {
                    testcaseGenUtils.addUndoQuery(currentUndoStackIndex - newUndoStackIndex);
                }
                else
                {
                    testcaseGenUtils.addRedoQuery(newUndoStackIndex - currentUndoStackIndex);
                }
                // Follow each undo/ redo with a range query.
                if (testcaseGenUtils.canRangeQuery()) // It should be moderately rare that this *isn't* the case - we'll just add any "missed" queries at the end.
                {
                    assert(isRangeFormattedQueriesRunIndex < static_cast<int>(numIsRangeFormattedQueriesInRun.size()));
                    for (int j = 0; j < numIsRangeFormattedQueriesInRun[isRangeFormattedQueriesRunIndex]; j++)
                    {
                        testcaseGenUtils.addIsRangeFormattedQueryBiasingTowardsAfterInsertionPos();
                    }
                }
                isRangeFormattedQueriesRunIndex++;
                addedQuery = true;
            }
            undoOrRedoQueriesRunIndex++;
        }
    } while (addedQuery);

    // Make up and shortfall in generated queries with IsRangeFormatted queries.
    while (static_cast<int>(testcaseGenUtils.queries.size()) < numQueriesToAdd)
    {
        if (!testcaseGenUtils.canRangeQuery())
            testcaseGenUtils.addInsertNonFormattingCharQuery(rnd.next(static_cast<int64_t>(1), maxDocLength - testcaseGenUtils.formattingCharsTree.documentLength()));
        if (static_cast<int>(testcaseGenUtils.queries.size()) < numQueriesToAdd)
            testcaseGenUtils.addIsRangeFormattedQueryBiasingTowardsAfterInsertionPos();
    }
    assert(static_cast<int>(testcaseGenUtils.queries.size()) == numQueriesToAdd);
}

void addRandomishQueriesNoUndosOrRedos(QueryGenUtils& testcaseGenUtils, const int numQueriesToAdd, const int64_t finalDocLength)
{
    const auto queryMakeUp = chooseRandomValuesWithSum3(3, numQueriesToAdd, 1);
    const auto numInsertFormattingQueries = queryMakeUp[0];
    const auto numInsertNonFormattingQueries = queryMakeUp[1];
    const auto numIsRangeFormattedQueries = queryMakeUp[2];
    const auto totalNumUnformattedCharsToAdd = finalDocLength - numInsertFormattingQueries;

    // Compute the number of unformatted chars to add for each InsertNonFormatting query.
    const auto numNonformattedCharToAddForQuery = chooseRandomValuesWithSum3(numInsertNonFormattingQueries, totalNumUnformattedCharsToAdd, 1);

    vector<TestQuery::Type> queryTypes;
    for (int i = 0; i < numInsertFormattingQueries; i++)
        queryTypes.push_back(TestQuery::Type::InsertFormatting);
    for (int i = 0; i < numInsertNonFormattingQueries; i++)
        queryTypes.push_back(TestQuery::Type::InsertNonFormatting);
    for (int i = 0; i < numIsRangeFormattedQueries; i++)
        queryTypes.push_back(TestQuery::Type::IsRangeFormatted);
    assert(static_cast<int>(queryTypes.size()) == numQueriesToAdd);

    ::shuffle(queryTypes.begin(), queryTypes.end());

    // Can't have a IsRangeFormatted before an InsertNonFormatting query, as IsRangeFormatted's
    // queryPosition must point at at a non-formatting char.
    int firstNonFormattedQueryIndex = -1;
    for (int queryIndex = 0; queryIndex < numQueriesToAdd; queryIndex++)
    {
        if (queryTypes[queryIndex] == TestQuery::InsertNonFormatting)
        {
            firstNonFormattedQueryIndex = queryIndex;
            break;
        }
    }
    assert(firstNonFormattedQueryIndex != -1);
    for (int queryIndex = 0; queryIndex < firstNonFormattedQueryIndex; queryIndex++)
    {
        if (queryTypes[queryIndex] == TestQuery::IsRangeFormatted)
        {
            swap(queryTypes[queryIndex], queryTypes[firstNonFormattedQueryIndex]);
            break;
        }
    }

    // Prefer to end with a IsRangeFormatted - else the preceding insertion queries are "wasted".
    if (queryTypes.back() != TestQuery::IsRangeFormatted)
    {
        for (auto& queryType : queryTypes)
        {
            if (queryType == TestQuery::IsRangeFormatted)
            {
                swap(queryType, queryTypes.back());
                break;
            }
        }
    }
    assert(queryTypes.back() == TestQuery::IsRangeFormatted);

    int numNonFormattingCharInsertionsQueriesAdded = 0;
    for (int queryIndex = 0; queryIndex < numQueriesToAdd; queryIndex++)
    {
        switch (queryTypes[queryIndex])
        {
            case TestQuery::Type::InsertFormatting:
                testcaseGenUtils.addInsertFormattingCharQuery();
                break;
            case TestQuery::Type::InsertNonFormatting:
                testcaseGenUtils.addInsertNonFormattingCharQuery(numNonformattedCharToAddForQuery[numNonFormattingCharInsertionsQueriesAdded]);
                numNonFormattingCharInsertionsQueriesAdded++;
                break;
            case TestQuery::Type::IsRangeFormatted:
                testcaseGenUtils.addIsRangeFormattedQueryBiasingTowardsAfterInsertionPos();
                break;
            default:
                assert(false);
        }
    }
    assert(testcaseGenUtils.formattingCharsTree.documentLength() == finalDocLength);

}


bool verifyTestFile(TestFileReader& testFileReader, const SubtaskInfo& containingSubtask);

int main(int argc, char* argv[])
{
    TestSuite<SubtaskInfo> testsuite;
    testsuite.setTestFileVerifier(&verifyTestFile);

    // SUBTASK 1
    {
        {
            auto& testFile = testsuite.newTestFile(SMETestFileInfo().belongingToSubtask(subtask1)
                    .withSeed(23985)
                    .withDescription("sample testfile"));

            auto& testcase = testFile.newTestcase(SMETestCaseInfo());

            QueryGenUtils testcaseGenUtils;
            testcaseGenUtils.addInsertNonFormattingCharQuery(0, 9);
            testcaseGenUtils.addInsertFormattingCharQuery(3);
            testcaseGenUtils.addInsertFormattingCharQuery(6);
            testcaseGenUtils.addIsRangeFormattedQuery(4);
            testcaseGenUtils.addInsertNonFormattingCharQuery(7, 3);
            testcaseGenUtils.addInsertFormattingCharQuery(9);
            testcaseGenUtils.addInsertFormattingCharQuery(0);
            testcaseGenUtils.addIsRangeFormattedQuery(2);
            testcaseGenUtils.addUndoQuery(3);
            testcaseGenUtils.addIsRangeFormattedQuery(9);
            testcaseGenUtils.addRedoQuery(1);
            testcaseGenUtils.addInsertFormattingCharQuery(3);
            testcaseGenUtils.addInsertFormattingCharQuery(9);
            testcaseGenUtils.addIsRangeFormattedQuery(8);
            testcaseGenUtils.addUndoQuery(3);
            testcaseGenUtils.addInsertNonFormattingCharQuery(4, 5);
            testcaseGenUtils.addIsRangeFormattedQuery(6);

            writeTestCase(testcase, testcaseGenUtils.queries);
        }
        {
            auto& testFile = testsuite.newTestFile(SMETestFileInfo().belongingToSubtask(subtask1)
                    .withSeed(39784432)
                    .withDescription("maxNumTestcases 'random' testcases"));

            for (int t = 0; t < subtask1.maxNumTestcases; t++)
            {
                auto& testcase = testFile.newTestcase(SMETestCaseInfo());
                QueryGenUtils testcaseGenUtils;
                const auto maxDocLength = subtask1.maxDocLength;
                const auto numQueries = rnd.next(subtask1.maxQueriesPerTestcase - 50, subtask1.maxQueriesPerTestcase);
                addRandomishQueries(testcaseGenUtils, numQueries, maxDocLength);
                writeTestCase(testcase, testcaseGenUtils.queries);
            }
        }
    }

    // SUBTASK 2
    {
        {
            auto& testFile = testsuite.newTestFile(SMETestFileInfo().belongingToSubtask(subtask2)
                    .withSeed(23985)
                    .withDescription("100 testcases with each testcase resulting in a doc with maxDocLength, and total queries across all testcases equal to maxQueriesOverAllTestcases.  Approx equal number of InsertNonFormatting, InsertFormatting, and IsRangeFormatted.  No undos or redos, of course."));

            const auto numTestcases = 100;
            const auto numQueriesForTestcases = chooseRandomValuesWithSum3(numTestcases, subtask2.maxQueriesOverAllTestcases, 1);
            
            for (int t = 0; t < numTestcases; t++)
            {
                auto& testcase = testFile.newTestcase(SMETestCaseInfo());
                QueryGenUtils testcaseGenUtils;
                testcaseGenUtils.setUndoAndRedoAllowed(false);

                const auto numQueries = numQueriesForTestcases[t];
                const auto finalDocLength = rnd.next(subtask2.maxDocLength - 100, subtask2.maxDocLength);

                addRandomishQueriesNoUndosOrRedos(testcaseGenUtils, numQueries, finalDocLength);

                writeTestCase(testcase, testcaseGenUtils.queries);
            }

        }
        {
            auto& testFile = testsuite.newTestFile(SMETestFileInfo().belongingToSubtask(subtask2)
                    .withSeed(48759843)
                    .withDescription("Single testcase with maxQueriesPerTestcase testcases"));

            auto& testcase = testFile.newTestcase(SMETestCaseInfo());
            QueryGenUtils testcaseGenUtils;
            testcaseGenUtils.setUndoAndRedoAllowed(false);

            const auto numQueries = subtask2.maxQueriesPerTestcase;
            const auto finalDocLength = subtask2.maxDocLength;

            addRandomishQueriesNoUndosOrRedos(testcaseGenUtils, numQueries, finalDocLength);

            writeTestCase(testcase, testcaseGenUtils.queries);

        }
        {
            auto& testFile = testsuite.newTestFile(SMETestFileInfo().belongingToSubtask(subtask2)
                    .withSeed(348754)
                    .withDescription("1000 testcases with each testcase resulting in a doc with maxDocLength, and total queries across all testcases equal to maxQueriesOverAllTestcases.  Approx equal number of InsertNonFormatting, InsertFormatting, and IsRangeFormatted.  No undos or redos, of course."));

            const auto numTestcases = subtask2.maxNumTestcases;
            const auto numQueriesForTestcases = chooseRandomValuesWithSum3(numTestcases, subtask2.maxQueriesOverAllTestcases, 5);
            
            for (int t = 0; t < numTestcases; t++)
            {
                auto& testcase = testFile.newTestcase(SMETestCaseInfo());
                QueryGenUtils testcaseGenUtils;
                testcaseGenUtils.setUndoAndRedoAllowed(false);

                const auto numQueries = numQueriesForTestcases[t];
                const auto finalDocLength = rnd.next(subtask2.maxDocLength - 100, subtask2.maxDocLength);

                addRandomishQueriesNoUndosOrRedos(testcaseGenUtils, numQueries, finalDocLength);

                writeTestCase(testcase, testcaseGenUtils.queries);
            }

        }
    }

    // SUBTASK 3
    {
        {
            auto& testFile = testsuite.newTestFile(SMETestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(20938851)
                    .withDescription("Max testcases worth of 'random' queries, with total queries equalling maxQueriesOverAllTestcases"));
            {

                const auto T = subtask3.maxNumTestcases;
                const auto numQueriesForTestcase = chooseRandomValuesWithSum3(T, subtask3.maxQueriesOverAllTestcases, 5);
                const auto maxDocLength = subtask3.maxDocLength;


                for (int t = 0; t < T; t++)
                {
                    auto& testcase = testFile.newTestcase(SMETestCaseInfo());
                    const int numQueries = numQueriesForTestcase[t];

                    QueryGenUtils testcaseGenUtils;
                    testcaseGenUtils.setUndoAndRedoAllowed(true);
                    addRandomishQueries(testcaseGenUtils, numQueries, maxDocLength);
                    writeTestCase(testcase, testcaseGenUtils.queries);
                }
            }
        }
        {
            auto& testFile = testsuite.newTestFile(SMETestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(3498537)
                    .withDescription("100 testcases worth of 'random' queries, with total queries equalling maxQueriesOverAllTestcases"));
            {

                const auto T = 100;
                const auto numQueriesForTestcase = chooseRandomValuesWithSum3(T, subtask3.maxQueriesOverAllTestcases, 5);
                const auto maxDocLength = subtask3.maxDocLength;

                for (int t = 0; t < T; t++)
                {
                    auto& testcase = testFile.newTestcase(SMETestCaseInfo());
                    const int numQueries = numQueriesForTestcase[t];

                    QueryGenUtils testcaseGenUtils;
                    testcaseGenUtils.setUndoAndRedoAllowed(true);
                    addRandomishQueries(testcaseGenUtils, numQueries, maxDocLength);
                    writeTestCase(testcase, testcaseGenUtils.queries);
                }
            }
        }
        {
            auto& testFile = testsuite.newTestFile(SMETestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(98645)
                    .withDescription("125'000 insertions (biasing slightly towards Formatted chars, resulting in a string of length maxDocLength) each followed by a range query followed by 125'000 random Undo/ Redo operations, each followed by a range query (so 500'000 queries in total)."));
            {

                const auto maxDocLength = subtask3.maxDocLength;
                const auto numInsertionQueries = subtask3.maxQueriesPerTestcase / 4;
                const auto numFormattedInsertions = static_cast<int>(rnd.next(55.0, 65.0) * numInsertionQueries / 100.0);
                const auto numNonFormattedInsertions = numInsertionQueries - numFormattedInsertions;

                vector<TestQuery::Type> insertionTypes;
                for (int i = 0; i < numNonFormattedInsertions; i++)
                    insertionTypes.push_back(TestQuery::Type::InsertNonFormatting);
                for (int i = 0; i < numFormattedInsertions; i++)
                    insertionTypes.push_back(TestQuery::Type::InsertFormatting);

                ::shuffle(insertionTypes.begin(), insertionTypes.end());
                // Ensure the first insertion is a InsertNonFormatting, so that we can always add a IsRangeFormatted.
                const auto firstNonFormattingIter = std::find(insertionTypes.begin(), insertionTypes.end(), TestQuery::Type::InsertNonFormatting);
                assert(firstNonFormattingIter != insertionTypes.end());
                iter_swap(insertionTypes.begin(), firstNonFormattingIter);

                QueryGenUtils testcaseGenUtils;
                auto& testcase = testFile.newTestcase(SMETestCaseInfo());

                const auto numNonFormattedCharsToAddForQuery = chooseRandomValuesWithSum3(numNonFormattedInsertions, maxDocLength - numFormattedInsertions, 1);
                int nonFormattedInsertionIndex = 0;
                int dbg = 0;
                for (const auto& insertionType : insertionTypes)
                {
                    if (insertionType == TestQuery::Type::InsertFormatting)
                    {
                        testcaseGenUtils.addInsertFormattingCharQuery();
                    }
                    else if (insertionType == TestQuery::Type::InsertNonFormatting)
                    {
                        testcaseGenUtils.addInsertNonFormattingCharQuery(numNonFormattedCharsToAddForQuery[nonFormattedInsertionIndex]);
                        nonFormattedInsertionIndex++;
                        dbg++;
                    }
                    else
                    {
                        assert(false);
                    }
                    testcaseGenUtils.addIsRangeFormattedQueryBiasingTowardsAfterInsertionPos();
                }
                cout << "Num phase 1 queries: " << testcaseGenUtils.queries.size() << endl;
                assert(nonFormattedInsertionIndex == static_cast<int>(numNonFormattedCharsToAddForQuery.size()));
                assert(testcaseGenUtils.formattingCharsTree.documentLength() == maxDocLength);
                const int numUndoOrRedoQueries = (subtask3.maxQueriesPerTestcase - testcaseGenUtils.queries.size()) / 2;

                cout << "numUndoOrRedoQueries: " << numUndoOrRedoQueries << endl;

                for (int i = 0; i < numUndoOrRedoQueries; i++)
                {
                    const auto currentUndoStackIndex = testcaseGenUtils.formattingCharsTree.undoStackPointer(); // 0-relative
                    int newUndoStackIndex = -1;
                    while (true)
                    {
                        newUndoStackIndex = rnd.next(-1, testcaseGenUtils.formattingCharsTree.undoStackSize() - 1);
                        if (newUndoStackIndex != currentUndoStackIndex)
                            break;
                    }
                    if (newUndoStackIndex < currentUndoStackIndex)
                    {
                        testcaseGenUtils.addUndoQuery(currentUndoStackIndex - newUndoStackIndex);
                    }
                    else
                    {
                        testcaseGenUtils.addRedoQuery(newUndoStackIndex - currentUndoStackIndex);
                    }
                    // Follow each undo/ redo with a range query.
                    if (testcaseGenUtils.canRangeQuery()) // It should be moderately rare that this *isn't* the case - we'll just add any "missed" queries at the end.
                    {
                        testcaseGenUtils.addIsRangeFormattedQuery();
                    }
                }
                // Make up and shortfall in generated queries with IsRangeFormatted queries.
                while (static_cast<int>(testcaseGenUtils.queries.size()) < subtask3.maxQueriesPerTestcase)
                {
                    if (!testcaseGenUtils.canRangeQuery())
                        testcaseGenUtils.addInsertNonFormattingCharQuery(rnd.next(static_cast<int64_t>(1), maxDocLength - testcaseGenUtils.formattingCharsTree.documentLength()));
                    if (static_cast<int>(testcaseGenUtils.queries.size()) < subtask3.maxQueriesPerTestcase)
                        testcaseGenUtils.addIsRangeFormattedQueryBiasingTowardsAfterInsertionPos();
                }
                cout << "# queries: " << testcaseGenUtils.queries.size() << endl;
                assert(static_cast<int>(testcaseGenUtils.queries.size()) == subtask3.maxQueriesPerTestcase);
                writeTestCase(testcase, testcaseGenUtils.queries);

            }
        }
        {
            auto& testFile = testsuite.newTestFile(SMETestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(425435430)
                    .withDescription("Phase 1: Approx 200k insertions interspersed with ~30k range queries - creating a document with size within 90% of maxDocLength. Then Phase 2: ~10 'waves' - each wave has a phase where we add a block of insertions, again bringing us close to maxDocLength, followed by a block of undos and redos.  The final undo/redo in a block brings undoStackPointer within 1000 or so of undoStackSize, so the document is still 'big' at the end of a wave.  All insertions/ undos/ redos in the waves are followed by at least one range query.  This stress-tests Undo and Redo, especially in the face of continuing insertions"));
            {

                const auto maxDocLength = subtask3.maxDocLength;

                QueryGenUtils testcaseGenUtils;
                auto& testcase = testFile.newTestcase(SMETestCaseInfo());
                
                // Phase 1.
                {
                    const auto numInsertionQueries = 220'000;
                    const auto numRangeQueries = 30'000;
                    const auto numFormattedInsertions = static_cast<int>(rnd.next(55.0, 65.0) * numInsertionQueries / 100.0);
                    const auto numNonFormattedInsertions = numInsertionQueries - numFormattedInsertions;

                    vector<TestQuery::Type> insertionTypes;
                    for (int i = 0; i < numNonFormattedInsertions; i++)
                        insertionTypes.push_back(TestQuery::Type::InsertNonFormatting);
                    for (int i = 0; i < numFormattedInsertions; i++)
                        insertionTypes.push_back(TestQuery::Type::InsertFormatting);
                    for (int i = 0; i < numRangeQueries; i++)
                        insertionTypes.push_back(TestQuery::Type::IsRangeFormatted);

                    ::shuffle(insertionTypes.begin(), insertionTypes.end());
                    // Ensure the first insertion is a InsertNonFormatting, so that we can always add a IsRangeFormatted.
                    const auto firstNonFormattingIter = std::find(insertionTypes.begin(), insertionTypes.end(), TestQuery::Type::InsertNonFormatting);
                    assert(firstNonFormattingIter != insertionTypes.end());
                    iter_swap(insertionTypes.begin(), firstNonFormattingIter);

                    const auto numNonFormattedCharsToAddForQuery = chooseRandomValuesWithSum3(numNonFormattedInsertions, rnd.next(90.0, 95.0) * maxDocLength / 100.0 - numFormattedInsertions, 1);
                    int nonFormattedInsertionIndex = 0;
                    for (const auto& insertionType : insertionTypes)
                    {
                        if (insertionType == TestQuery::Type::InsertFormatting)
                        {
                            testcaseGenUtils.addInsertFormattingCharQuery();
                        }
                        else if (insertionType == TestQuery::Type::InsertNonFormatting)
                        {
                            testcaseGenUtils.addInsertNonFormattingCharQuery(numNonFormattedCharsToAddForQuery[nonFormattedInsertionIndex]);
                            nonFormattedInsertionIndex++;
                        }
                        else if (insertionType == TestQuery::Type::IsRangeFormatted)
                        {
                            testcaseGenUtils.addIsRangeFormattedQueryBiasingTowardsAfterInsertionPos();
                        }
                        else
                        {
                            assert(false);
                        }
                    }
                    cout << "Num phase 1 queries: " << testcaseGenUtils.queries.size() << endl;
                    assert(nonFormattedInsertionIndex == static_cast<int>(numNonFormattedCharsToAddForQuery.size()));
                    cout << "Phase 1 doc length: " << testcaseGenUtils.formattingCharsTree.documentLength() << endl;
                }
                // Phase 2.
                {
                    const int numWaves = 10;
                    const int numRemainingQueries = subtask3.maxQueriesPerTestcase - testcaseGenUtils.queries.size();
                    const int numIsRangeFormattedQueries = numRemainingQueries * rnd.next(55.0, 60.0) / 100.0;
                    const int numInsertions = (numRemainingQueries - numIsRangeFormattedQueries) * rnd.next(45.0, 55.0) / 100.0;
                    const int numUndoOrRedoQueries = numRemainingQueries - numIsRangeFormattedQueries - numInsertions;

                    const auto numInsertionsForWaves = chooseRandomValuesWithSum3(numWaves, numInsertions, 1);
                    const auto numRangeQueriesForWaves = chooseRandomValuesWithSum3(numWaves, numIsRangeFormattedQueries, 1);
                    const auto numUndoOrRedoQueriesForWaves = chooseRandomValuesWithSum3(numWaves, numUndoOrRedoQueries, 1);
                    const auto numRangeQueriesInRun = chooseRandomValuesWithSum3(numInsertions + numUndoOrRedoQueries, numIsRangeFormattedQueries, 1);

                    int rangeQueryRunIndex = 0;

                    for (int waveIndex = 0; waveIndex < numWaves; waveIndex++)
                    {
                        cout << "Beginning wave; currentUndoStackSize: " << testcaseGenUtils.formattingCharsTree.undoStackSize() << " current undoStackPointer: " << testcaseGenUtils.formattingCharsTree.undoStackPointer() << " numFormattedChars: " << testcaseGenUtils.formattingCharsTree.numFormattingChars() << endl;
                        // Insertions.
                        const auto numInsertionsThisWave = numInsertionsForWaves[waveIndex];
                        const int numFormattedCharInsertions = rnd.next(55.0, 60.0) / 100.0 * numInsertionsThisWave;
                        const int numNonFormattedCharInsertions = numInsertionsThisWave - numFormattedCharInsertions;
                        const auto currentDocLength = testcaseGenUtils.formattingCharsTree.documentLength();
                        int64_t targetDocLength = 0;
                        while (targetDocLength <= currentDocLength + numFormattedCharInsertions)
                        {
                            targetDocLength = maxDocLength * rnd.next(90.0, 95.0) / 100.0;
                        }
                        const auto numNonFormattedCharsToAddForQuery = chooseRandomValuesWithSum3(numNonFormattedCharInsertions, targetDocLength - currentDocLength, 1);
                        vector<TestQuery::Type> insertionTypes;
                        for (int i = 0; i < numFormattedCharInsertions; i++)
                            insertionTypes.push_back(TestQuery::Type::InsertFormatting);
                        for (int i = 0; i < numNonFormattedCharInsertions; i++)
                            insertionTypes.push_back(TestQuery::Type::InsertNonFormatting);
                        ::shuffle(insertionTypes.begin(), insertionTypes.end());
                        int nonFormattedInsertionIndex = 0;
                        for (const auto& insertionType : insertionTypes)
                        {
                            if (insertionType == TestQuery::Type::InsertFormatting)
                            {
                                testcaseGenUtils.addInsertFormattingCharQuery();
                            }
                            else if (insertionType == TestQuery::Type::InsertNonFormatting)
                            {
                                testcaseGenUtils.addInsertNonFormattingCharQuery(numNonFormattedCharsToAddForQuery[nonFormattedInsertionIndex]);
                                nonFormattedInsertionIndex++;
                            }
                            else
                            {
                                assert(false);
                            }
                            assert(rangeQueryRunIndex < numRangeQueriesInRun.size());
                            for (int i = 0; i < numRangeQueriesInRun[rangeQueryRunIndex]; i++)
                            {
                                testcaseGenUtils.addIsRangeFormattedQueryBiasingTowardsAfterInsertionPos();
                            }
                            rangeQueryRunIndex++;
                        }
                        // Undos/ Redos.
                        const auto numUndoOrRedoQueriesThisWave = numUndoOrRedoQueriesForWaves[waveIndex];
                        for (int i = 0; i < numUndoOrRedoQueriesThisWave; i++)
                        {
                            const auto currentUndoStackIndex = testcaseGenUtils.formattingCharsTree.undoStackPointer(); // 0-relative
                            int newUndoStackIndex = -1;
                            while (true)
                            {
                                const int undoStackSize = testcaseGenUtils.formattingCharsTree.undoStackSize();
                                if (i == numUndoOrRedoQueriesThisWave - 1)
                                {
                                    newUndoStackIndex = static_cast<int>(rnd.next(96.0, 99.0) / 100.0 * (undoStackSize - 1));
                                }
                                else
                                {
                                    newUndoStackIndex = rnd.next(0, undoStackSize - 1);
                                }
                                if (newUndoStackIndex != currentUndoStackIndex)
                                    break;
                            }
                            if (newUndoStackIndex < currentUndoStackIndex)
                            {
                                testcaseGenUtils.addUndoQuery(currentUndoStackIndex - newUndoStackIndex);
                            }
                            else
                            {
                                testcaseGenUtils.addRedoQuery(newUndoStackIndex - currentUndoStackIndex);
                            }
                            cout << "Current stack index: " << testcaseGenUtils.formattingCharsTree.undoStackPointer() << endl;
                            cout << "Adding range queries: " << numRangeQueriesInRun[rangeQueryRunIndex] << endl;
                            assert(rangeQueryRunIndex < numRangeQueriesInRun.size());
                            for (int i = 0; i < numRangeQueriesInRun[rangeQueryRunIndex]; i++)
                            {
                                testcaseGenUtils.addIsRangeFormattedQueryBiasingTowardsAfterInsertionPos();
                            }
                            rangeQueryRunIndex++;
                        }
                    }
                    cout << "# 'wave' testcase queries: " << testcaseGenUtils.queries.size() << endl;
                    assert(static_cast<int>(testcaseGenUtils.queries.size()) == subtask3.maxQueriesPerTestcase);
                }
                writeTestCase(testcase, testcaseGenUtils.queries);

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
    const auto [T] = testFileReader.readLine<int>();

    cout << "maxDocLength: " << containingSubtask.maxDocLength << endl;
    testFileReader.addErrorUnless(T >= 1, "Expected T >= 1, got T = " + to_string(T));
    testFileReader.addErrorUnless(T <= containingSubtask.maxNumTestcases, "Expected T <= " + to_string(containingSubtask.maxNumTestcases) + " got T = " + to_string(T));


    int64_t totalNumQueries = 0;
    for (int t = 0; t < T; t++)
    {
        const auto [numQueries] = testFileReader.readLine<int>();

        testFileReader.addErrorUnless(numQueries >= 1, "Expected numQueries >= 1, got numQueries = " + to_string(numQueries));
        testFileReader.addErrorUnless(numQueries <= containingSubtask.maxQueriesPerTestcase, "Expected numQueries <= " + to_string(containingSubtask.maxQueriesPerTestcase) + " got numQueries = " + to_string(numQueries));
        totalNumQueries += numQueries;

        vector<TestQuery> queries;
        for (int i = 0; i < numQueries; i++)
        {
            const auto nextLine = testFileReader.peekLine();
            TestQuery query;
            char queryTypeChar = '\0';
            if (nextLine[0] == 'N')
            {
                // Two args.
                const auto [queryTypeCharTmp, encryptedArgumentTmp, encryptedArgument2Tmp] = testFileReader.readLine<char, int64_t, int64_t>();
                queryTypeChar = queryTypeCharTmp;
                query.encryptedArgument = encryptedArgumentTmp;
                query.encryptedArgument2 = encryptedArgument2Tmp;
            }
            else
            {
                // Single arg.
                const auto [queryTypeCharTmp, encryptedArgumentTmp] = testFileReader.readLine<char, int64_t>();
                queryTypeChar = queryTypeCharTmp;
                query.encryptedArgument = encryptedArgumentTmp;
            }

            switch (queryTypeChar)
            {
                case 'F':
                    query.type = TestQuery::InsertFormatting;
                    break;
                case 'N':
                    query.type = TestQuery::InsertNonFormatting;
                    break;
                case 'Q':
                    query.type = TestQuery::IsRangeFormatted;
                    break;
                case 'U':
                    query.type = TestQuery::Undo;
                    break;
                case 'R':
                    query.type = TestQuery::Redo;
                    break;
                default:
                    testFileReader.addError("Unrecognised query type char: " + queryTypeChar);
                    break;
            }
            queries.push_back(query);
        }

        int64_t decryptionKey = 0;
        int64_t powerOf2 = 2;

        vector<int64_t> queryResults;
        AVLTree formattingCharsTree(10'000);
        // Add Sentinel node.
        formattingCharsTree.insertFormattingChar(0);
        formattingCharsTree.root()->isSentinelValue = true;

        int queryNum = 1;
        int64_t numQueriesInFormattedRange = 0;
        int64_t numQueriesInNonFormattedRange = 0;
        for (const auto& query : queries)
        {
            switch (query.type)
            {
                case TestQuery::InsertFormatting:
                    {
                        const auto insertionPos = (query.encryptedArgument ^ decryptionKey) - 1;
                        // We can insert "one past" the document length, but no further than that.
                        testFileReader.addErrorUnless(insertionPos < formattingCharsTree.documentLength() + 1, "Attempt to insert formatting at 0-relative index " + to_string(insertionPos) + " when document length is " + to_string(formattingCharsTree.documentLength()));
                        formattingCharsTree.insertFormattingChar(insertionPos);
                    }
                    break;
                case TestQuery::InsertNonFormatting:
                    {
                        const auto insertionPos = (query.encryptedArgument ^ decryptionKey) - 1;
                        const auto numToInsert = (query.encryptedArgument2 ^ decryptionKey);
                        // We can insert "one past" the document length, but no further than that.
                        testFileReader.addErrorUnless(insertionPos < formattingCharsTree.documentLength() + 1, "Attempt to insert non-formatting at 0-relative index " + to_string(insertionPos) + " when document length is " + to_string(formattingCharsTree.documentLength()));
                        testFileReader.addErrorUnless(numToInsert >= 1, "Attempt to insert " + to_string(numToInsert) + " non-formatting chars, which is < 1!");
                        formattingCharsTree.insertNonFormattingChars(insertionPos, numToInsert);
                    }
                    break;
                case TestQuery::IsRangeFormatted:
                    {
                        const auto queryPosition = (query.encryptedArgument ^ decryptionKey) - 1;
                        const auto nextAtOrAfterQueryPos = formattingCharsTree.findFirstNodeAtOrToRightOf(queryPosition);
                        testFileReader.addErrorUnless(nextAtOrAfterQueryPos.currentNodePosition() != queryPosition, "IsRangeFormatted where queryPosition points to a formatting char");
                        auto queryAnswer = formattingCharsTree.distBetweenEnclosingFormattedChars(queryPosition);
                        if (queryAnswer == -1)
                        {
                            queryAnswer = 3'141'592;
                            numQueriesInNonFormattedRange++;
                        }
                        else
                        {
                            numQueriesInFormattedRange++;
                        }

                        decryptionKey = (decryptionKey + ((queryAnswer % Mod) * powerOf2) % Mod) % Mod;

                        queryResults.push_back(queryAnswer);
                    }
                    break;
                case TestQuery::Undo:
                    {
                        const int numToUndo = (query.encryptedArgument ^ decryptionKey);
                        testFileReader.addErrorUnless(numToUndo >= 1, "Tried to undo " + to_string(numToUndo) + " steps, which is less than 1!");
                        testFileReader.addErrorUnless(formattingCharsTree.undoStackPointer() + 1 - numToUndo >= 0, "Tried to undo " + to_string(numToUndo) + " steps but can do at most " + to_string(formattingCharsTree.undoStackPointer() + 1) + " steps!");
                        formattingCharsTree.undo(numToUndo);
                    }
                    break;
                case TestQuery::Redo:
                    {
                        const int numToRedo = (query.encryptedArgument ^ decryptionKey);
                        formattingCharsTree.redo(numToRedo);
                    }
                    break;
            }

            testFileReader.addErrorUnless(formattingCharsTree.documentLength() <= containingSubtask.maxDocLength, "Document length should be <= " + to_string(containingSubtask.maxDocLength) + ", not " + to_string(formattingCharsTree.documentLength()));

            queryNum++;
            powerOf2 = (2 * powerOf2) % Mod;

        }

        cout << "verifier: final encryption key: " << decryptionKey << endl;
        cout << "numQueriesInNonFormattedRange: " << numQueriesInNonFormattedRange << endl;
        cout << "numQueriesInFormattedRange: " << numQueriesInFormattedRange << endl;

        testFileReader.markTestcaseAsValidated();

    }
    testFileReader.addErrorUnless(totalNumQueries <= containingSubtask.maxQueriesOverAllTestcases, "Expected total numQueries over all testcases <= " + to_string(containingSubtask.maxQueriesOverAllTestcases) + " got total num queries = " + to_string(totalNumQueries));
    testFileReader.markTestFileAsValidated();


    return true;
}
