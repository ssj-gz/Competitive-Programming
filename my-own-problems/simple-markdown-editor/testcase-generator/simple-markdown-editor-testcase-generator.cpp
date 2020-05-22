#include <testlib.h>
#include "testcase-generator-lib.h"
#include "utils.h"

#include <iostream>
#include <cassert>

using namespace std;

constexpr int maxQueries = 200'000;

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

SubtaskInfo subtask1 = SubtaskInfo::create().withSubtaskId(1)
                                            .withScore(5)
                                            .withMaxQueriesPerTestcase(100)
                                            .withMaxQueriesOverAllTestcases(NoExplicitLimit)
                                            .withAllowsUndoRedo(false)
                                            .withMaxNumTestcases(10);
                                            
SubtaskInfo subtask2 = SubtaskInfo::create().withSubtaskId(2)
                                            .withScore(10)
                                            .withMaxQueriesPerTestcase(1000)
                                            .withMaxQueriesOverAllTestcases(NoExplicitLimit)
                                            .withAllowsUndoRedo(false)
                                            .withMaxNumTestcases(100);

SubtaskInfo subtask3 = SubtaskInfo::create().withSubtaskId(3)
                                            .withScore(85)
                                            .withMaxQueriesPerTestcase(maxQueries)
                                            .withMaxQueriesOverAllTestcases(maxQueries)
                                            .withAllowsUndoRedo(true)
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
                    cout << " writing InsertFormatting" << endl;
                    const auto insertionPos = query.insertionPos - 1;
                    formattingCharsTree.insertFormattingChar(insertionPos);
                    destTestcase.writeLine<char, int64_t>('F', (insertionPos + 1) ^ encryptionKey);
                }
                break;
            case TestQuery::InsertNonFormatting:
                {
                    cout << " writing InsertNonFormatting" << endl;
                    const auto insertionPos = query.insertionPos - 1;
                    const auto numToInsert = query.numToInsert;
                    formattingCharsTree.insertNonFormattingChars(insertionPos, numToInsert);
                    destTestcase.writeLine<char, int64_t, int64_t>('N', (insertionPos + 1) ^ encryptionKey, numToInsert ^ encryptionKey);
                }
                break;
            case TestQuery::IsRangeFormatted:
                {
                    cout << " writing IsRangeFormatted" << endl;
                    const auto queryPosition = query.queryPosition - 1;
                    auto queryAnswer = formattingCharsTree.distBetweenEnclosingFormattedChars(queryPosition);
                    destTestcase.writeLine<char, int64_t>('Q', (queryPosition + 1) ^ encryptionKey);

                    // Update encryptionKey for next queries.
                    if (queryAnswer == -1)
                        queryAnswer = 3'141'592;

                    encryptionKey = (encryptionKey + (queryAnswer * powerOf2) % Mod) % Mod;
                }
                break;
            case TestQuery::Undo:
                {
                    cout << " writing Undo" << endl;
                    const int numToUndo = query.numToUndo;
                    formattingCharsTree.undo(numToUndo);
                    destTestcase.writeLine<char, int64_t>('U', numToUndo ^ encryptionKey);
                }
                break;
            case TestQuery::Redo:
                {
                    cout << " writing Redo" << endl;
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

bool verifyTestFile(TestFileReader& testFileReader, const SubtaskInfo& containingSubtask);

int main(int argc, char* argv[])
{
    TestSuite<SubtaskInfo> testsuite;
    testsuite.setTestFileVerifier(&verifyTestFile);

    // SUBTASK 1
    {
        {
            auto& testFile = testsuite.newTestFile(SMETestFileInfo().belongingToSubtask(subtask1)
                    .withSeed(20938851)
                    .withDescription("Sample"));
            {
                auto& testcase = testFile.newTestcase(SMETestCaseInfo().withDescription("TODO"));

                // TODO - remove this - testing/ debugging only!
                const int T = 1;
                for (int t = 0; t < T; t++)
                {
                    string document;
                    int64_t decryptionKey = 0; 
                    int64_t powerOf2 = 2;

                    AVLTree formattingCharsTree(10'000);
                    // Add Sentinel node.
                    formattingCharsTree.insertFormattingChar(0);
                    formattingCharsTree.root()->isSentinelValue = true;

                    int queryNum = 1;
                    int numInsertionQueries = 0;
                    int numRangeQueries = 0;

                    vector<TestQuery> undoStack;
                    int undoStackPointer = -1;

                    const int numQueries = 500'000;
                    cout << "numQueries: " << numQueries << endl;
                    vector<TestQuery> queries;
                    bool allowsUndoRedo = false;
                    while (queries.size() < numQueries)
                    {
                        cout << "Generating query: " << queries.size() + 1 << " documentLength: " << formattingCharsTree.documentLength() << " numInsertionQueries: " << numInsertionQueries << " numRangeQueries: "<< numRangeQueries << endl;
                        bool haveQuery = false;
                        TestQuery query;
                        int numFormatting = count(document.begin(), document.end(), '*');
                        int numNonFormatting = count(document.begin(), document.end(), 'X');
                        //cout << "document:       " << document << endl;
                        //cout << "debug document: " << formattingCharsTree.documentString() << endl;
                        //assert(document == formattingCharsTree.documentString());
                        cout << "undoStackPointer:       " << formattingCharsTree.undoStackPointer() << endl;
                        cout << "debug undoStackPointer: " << undoStackPointer << endl;

                        cout << "undoStackSize:       " << formattingCharsTree.undoStackSize() << endl;
                        cout << "debug undoStackSize: " << undoStack.size() << endl;

                        assert(undoStackPointer == formattingCharsTree.undoStackPointer());
                        assert(undoStack.size() == formattingCharsTree.undoStackSize());

                        while (!haveQuery)
                        {
                            const int queryType = rand() % 5;
                            query.type = static_cast<TestQuery::Type>(queryType);
                            cout << "candidate queryType: " << query.type << endl;
                            if (queryType == TestQuery::Undo)
                            {
                                if (!allowsUndoRedo || (rand() % 4 >= 1))
                                    continue; // Undos should be fairly rare.
                                if (undoStackPointer == -1)
                                    continue;
                                else
                                {
                                    const int numToUndo = 1 + rand() % (undoStackPointer + 1);
                                    query.numToUndo = numToUndo;
                                    haveQuery = true;
                                }
                            }
                            if (queryType == TestQuery::Redo)
                            {
                                if (!allowsUndoRedo || (rand() % 4 >= 1))
                                    continue; // Redos should be fairly rare.
                                if (undoStackPointer + 1 == undoStack.size())
                                    continue;
                                else
                                {
                                    const int numToRedo = 1 + rand() % (undoStack.size() - 1 - undoStackPointer);
                                    query.numToRedo = numToRedo;
                                    haveQuery = true;
                                }
                            }
                            if (queryType == TestQuery::InsertFormatting)
                            {
                                const int pos = rand() % (document.size() + 1);
                                query.insertionPos = pos + 1;
                                haveQuery = true;
                            }
                            if (queryType == TestQuery::InsertNonFormatting)
                            {
                                const int pos = rand() % (document.size() + 1);
                                query.insertionPos = pos + 1;
                                const int num = 1 + rand() % 1000;
                                query.numToInsert = num;
                                haveQuery = true;
                            }
                            if (queryType == TestQuery::IsRangeFormatted)
                            {
                                if (numNonFormatting == 0)
                                    continue;
                                else
                                {
                                    int nonFormattedToPick = rand() % numNonFormatting;
                                    int numNonFormattedSoFar = 0;
                                    int position = -1;
                                    for (int i = 0; i < document.size(); i++)
                                    {
                                        if (document[i] == 'X')
                                        {
                                            if (numNonFormattedSoFar == nonFormattedToPick)
                                            {
                                                position = i;
                                                break;
                                            }
                                            numNonFormattedSoFar++;
                                        }
                                    }
                                    assert(position != -1);
                                    query.queryPosition = position + 1;
                                    haveQuery = true;
                                }

                            }

                        }
                        cout << "Keeping query with type: " << query.type << endl;

                        queries.push_back(query);
                        switch (query.type)
                        {
                            case TestQuery::InsertFormatting:
                                {
                                    const int insertionPos = query.insertionPos - 1;
                                    //cerr << "InsertFormatting at " << insertionPos << endl;
                                    document.insert(document.begin() + insertionPos, '*');
                                    undoStackPointer++;
                                    undoStack.erase(undoStack.begin() + undoStackPointer, undoStack.end());
                                    TestQuery undoQuery = query;
                                    undoQuery.insertionPos = insertionPos + 1;
                                    undoStack.push_back(undoQuery);
                                    formattingCharsTree.insertFormattingChar(insertionPos);
                                    numInsertionQueries++;
                                }
                                break;
                            case TestQuery::InsertNonFormatting:
                                {
                                    const int insertionPos = query.insertionPos - 1;
                                    const int numToInsert = query.numToInsert;
                                    //cerr << "InsertNonFormatting " << numToInsert << " at " << insertionPos << endl;
                                    const string charsToInsert(numToInsert, 'X');
                                    document.insert(insertionPos, charsToInsert);
                                    undoStackPointer++;
                                    undoStack.erase(undoStack.begin() + undoStackPointer, undoStack.end());
                                    TestQuery undoQuery = query;
                                    undoQuery.insertionPos = insertionPos + 1;
                                    undoQuery.numToInsert = numToInsert;
                                    undoStack.push_back(undoQuery);
                                    formattingCharsTree.insertNonFormattingChars(insertionPos, numToInsert);
                                    numInsertionQueries++;
                                }
                                break;
                            case TestQuery::IsRangeFormatted:
                                {
                                    const int queryPosition = query.queryPosition - 1;
                                    assert(document[queryPosition] == 'X');
                                    //cerr << "IsRangeFormatted at " << queryPosition << endl;
                                    int queryAnswer = -1;
                                    {
                                        int openingFormatPos = -1;
                                        for (int pos = 0; pos < document.size(); pos++)
                                        {
                                            if (document[pos] == '*')
                                            {
                                                if (openingFormatPos == -1)
                                                {
                                                    // Open formatting.
                                                    openingFormatPos = pos;
                                                }
                                                else
                                                {
                                                    // Close formatting.
                                                    if (openingFormatPos < queryPosition && queryPosition < pos)
                                                    {
                                                        queryAnswer = pos - openingFormatPos - 1;
                                                    }
                                                    openingFormatPos = -1;
                                                }
                                            }
                                        }
                                    }
                                    {
                                        // TODO - the below is not yet implemented.
                                        // Choose a position purely at random will bias in favour of long runs of 
                                        // non-formatting chars: instead, pick a formatted char (that has at least one
                                        // non-formatting char in the run to its left) and then pick a random position
                                        // in that run.  Although - TODO - maybe we should bias towards the ends of the range
                                        // as that makes it more likely to detect errors in the submitter's implementation?
                                        // We include sentinel, here, otherwise we won't include the very last run of non-formatting chars.
                                        const auto documentWithSentinel = document + "*";
                                        const int numFormattingWithNonFormattingToLeft = formattingCharsTree.root()->totalFormattedDescendantsWithNonFormattedToLeft;
                                        const int numFormattingWithoutNonFormattingToLeft = (numFormatting + 1 /*Sentinel*/) - numFormattingWithNonFormattingToLeft;
                                        int dbgNumFormattingWithNonFormattingToLeft = 0;
                                        int dbgNumFormattingWithoutNonFormattingToLeft = 0;
                                        for (int i = 0; i < documentWithSentinel.size(); i++)
                                        {
                                            if (documentWithSentinel[i] == '*')
                                            {
                                                if (i > 0 && documentWithSentinel[i - 1] == 'X')
                                                    dbgNumFormattingWithNonFormattingToLeft++;
                                                else
                                                    dbgNumFormattingWithoutNonFormattingToLeft++;
                                            }
                                        }
                                        cout << "numFormattingWithNonFormattingToLeft: " << numFormattingWithNonFormattingToLeft << " dbgNumFormattingWithNonFormattingToLeft: " << dbgNumFormattingWithNonFormattingToLeft << endl;
                                        cout << "numFormattingWithoutNonFormattingToLeft: " << numFormattingWithoutNonFormattingToLeft << " dbgNumFormattingWithoutNonFormattingToLeft: " << dbgNumFormattingWithoutNonFormattingToLeft << endl;
                                        assert(numFormattingWithoutNonFormattingToLeft == dbgNumFormattingWithoutNonFormattingToLeft); 
                                        assert(numFormattingWithNonFormattingToLeft == dbgNumFormattingWithNonFormattingToLeft); 
                                    }

                                    if (queryAnswer == -1)
                                        queryAnswer = 3'141'592;
                                    decryptionKey = (decryptionKey + (queryAnswer * powerOf2) % Mod) % Mod;
                                    auto dbgQueryAnswer = formattingCharsTree.distBetweenEnclosingFormattedChars(queryPosition);
                                    if (dbgQueryAnswer == -1)
                                        dbgQueryAnswer = 3'141'592;

                                    cout << "queryAnswer: " << queryAnswer << endl;
                                    cout << "dbgQueryAnswer: " << dbgQueryAnswer << endl;
                                    assert(queryAnswer == dbgQueryAnswer);
                                    numRangeQueries++;
                                }
                                break;
                            case TestQuery::Undo:
                                {
                                    const int numToUndo = query.numToUndo;
                                    //cerr << "Undo " << numToUndo << endl;
                                    for (int i = 0; i < numToUndo; i++)
                                    {
                                        const auto& queryToUndo = undoStack[undoStackPointer];
                                        const auto removalPosition = queryToUndo.insertionPos - 1;
                                        const auto numToRemove = (queryToUndo.type == TestQuery::InsertNonFormatting ? queryToUndo.numToInsert : 1);
                                        document.erase(document.begin() + removalPosition, document.begin() + removalPosition + numToRemove);
                                        undoStackPointer--;
                                    }
                                    formattingCharsTree.undo(numToUndo);
                                }
                                break;
                            case TestQuery::Redo:
                                {
                                    const int numToRedo = query.numToRedo;
                                    //cerr << "Redo " << numToRedo << endl;
                                    for (int i = 0; i < numToRedo; i++)
                                    {
                                        undoStackPointer++;
                                        const auto& queryToUndo = undoStack[undoStackPointer];
                                        const auto insertionPos = queryToUndo.insertionPos - 1;
                                        const auto charToInsert = queryToUndo.type == TestQuery::InsertNonFormatting ? 'X' : '*';
                                        const auto numToInsert = queryToUndo.type == TestQuery::InsertNonFormatting ? queryToUndo.numToInsert : 1;
                                        document.insert(insertionPos, string(numToInsert, charToInsert));
                                    }
                                    formattingCharsTree.redo(numToRedo);

                                }
                                break;
                        }

                    }
                    writeTestCase(testcase, queries);
                }


            }
        }
    }

    // SUBTASK 2
    {
        {
            auto& testFile = testsuite.newTestFile(SMETestFileInfo().belongingToSubtask(subtask2)
                    .withSeed(23985)
                    .withDescription("TODO"));

            auto& testcase = testFile.newTestcase(SMETestCaseInfo());
                vector<TestQuery> queries;

            writeTestCase(testcase, queries);
        }
    }

    // SUBTASK 3
    {
        {
            auto& testFile = testsuite.newTestFile(SMETestFileInfo().belongingToSubtask(subtask3)
                    .withSeed(9734)
                    .withDescription("TODO"));
            {
                auto& testcase = testFile.newTestcase(SMETestCaseInfo());
                vector<TestQuery> queries;

                writeTestCase(testcase, queries);
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


    for (int t = 0; t < T; t++)
    {
        const auto [numQueries] = testFileReader.readLine<int>();
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
                // Two args.
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
        for (const auto& query : queries)
        {
            switch (query.type)
            {
                case TestQuery::InsertFormatting:
                    {
                        const auto insertionPos = (query.encryptedArgument ^ decryptionKey) - 1;
                        formattingCharsTree.insertFormattingChar(insertionPos);
                    }
                    break;
                case TestQuery::InsertNonFormatting:
                    {
                        const auto insertionPos = (query.encryptedArgument ^ decryptionKey) - 1;
                        const auto numToInsert = (query.encryptedArgument2 ^ decryptionKey);
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
                            queryAnswer = 3'141'592;
                        decryptionKey = (decryptionKey + (queryAnswer * powerOf2) % Mod) % Mod;

                        queryResults.push_back(queryAnswer);
                    }
                    break;
                case TestQuery::Undo:
                    {
                        const int numToUndo = (query.encryptedArgument ^ decryptionKey);
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

            queryNum++;
            powerOf2 = (2 * powerOf2) % Mod;

        }

        cout << "verifier: final encryption key: " << decryptionKey << endl;

        testFileReader.markTestcaseAsValidated();

    }
    testFileReader.markTestFileAsValidated();


    return true;
}
