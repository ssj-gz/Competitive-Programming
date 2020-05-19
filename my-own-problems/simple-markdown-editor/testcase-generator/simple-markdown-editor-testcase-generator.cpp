#include <testlib.h>
#include "testcase-generator-lib.h"
#include "utils.h"

#include <iostream>
#include <cassert>

using namespace std;

constexpr int maxQueries = 200'000;

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

    int64_t encryptedArgument1 = -1;
    int64_t encryptedArgument2 = -1;
};

void writeTestCase(Testcase<SubtaskInfo>& destTestcase, const std::vector<TestQuery>& originalQueries)
{
    assert(false); // TODO
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
                vector<TestQuery> queries;

                writeTestCase(testcase, queries);
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
    // TODO
    return false;
}
