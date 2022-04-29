#include <vector>
#include <map>
#include <memory>
#include <cassert>
#include <sstream>
#include <fstream>
#include <functional>

#include <testlib.h>

template <typename SubtaskInfo>
class TestcaseInfo
{
    public:
        TestcaseInfo& withDescription(const std::string& description)
        {
            assert(m_description.empty());
            m_description = description;
            return *this;
        }
        std::string description() const
        {
            return m_description;
        }
        TestcaseInfo& withSeed(const int64_t seed)
        {
            assert(m_seed == -1);
            m_seed = seed;
            return *this;
        }
        int64_t seed() const
        {
            return m_seed;
        }
    private:
        std::string m_description;
        int64_t m_seed = -1;
};

template <typename SubtaskInfo>
class Testcase
{
    public:
        Testcase(const TestcaseInfo<SubtaskInfo>& testcaseInfo)
        {
            m_description = testcaseInfo.description();
        }
        template <typename... Types>
        void writeLine(Types&&... toPrint)
        {
            static_assert(sizeof...(toPrint) >= 1);

            ((m_contents << std::forward<Types>(toPrint) << " "), ...);

            m_contents.seekp(m_contents.tellp() - 1); // Pop the superflous trailing " ".
            m_contents << std::endl;
        }
        template <typename FwdIter>
        void writeObjectsAsLine(FwdIter begin, FwdIter end)
        {
            FwdIter current = begin;
            while (current != end)
            {
                m_contents << *current;
                current++;
                if (current != end)
                    m_contents << ' ';
            }
            m_contents << std::endl;
        }
        std::string contents() const
        {
            return m_contents.str();
        }

        std::string description() const
        {
            return m_description;
        }
    private:
        // Write in binary mode - don't want '\r's added to newlines if we
        // generate on e.g. windows.
        std::ostringstream m_contents{std::ios_base::out | std::ios_base::binary};
        std::string m_description;
};

template <typename SubtaskInfo>
class TestFileInfo
{
    public:
        TestFileInfo& belongingToSubtask(const SubtaskInfo& containingSubtask)
        {
            assert(m_containingSubtask == nullptr);
            m_containingSubtask = &containingSubtask;
            return *this;
        }
        TestFileInfo& withSeed(const int64_t seed)
        {
            assert(m_seed == -1);
            m_seed = seed;
            return *this;
        }
        TestFileInfo& withDescription(const std::string& description)
        {
            assert(m_description.empty());
            m_description = description;
            return *this;
        }
        int64_t seed() const
        {
            return m_seed;
        }

        const SubtaskInfo* containingSubtask() const
        {
            return m_containingSubtask;
        }

        std::string description() const
        {
            return m_description;
        }
    private:
        int64_t m_seed = -1;
        const SubtaskInfo* m_containingSubtask = nullptr;
        std::string m_description;
};

template <typename SubtaskInfo>
class TestFile
{
    public:
        TestFile(const TestFileInfo<SubtaskInfo>& testFileInfo)
        {
            assert(testFileInfo.containingSubtask() != nullptr);
            m_containingSubtask = testFileInfo.containingSubtask();
            m_currentSeedForTestFile = testFileInfo.seed();
            m_description = testFileInfo.description();
        }
        Testcase<SubtaskInfo>& newTestcase(const TestcaseInfo<SubtaskInfo>& newTestcaseInfo)
        {
            m_testcases.push_back(std::make_unique<Testcase<SubtaskInfo>>(newTestcaseInfo));
            if (newTestcaseInfo.seed() != -1)
            {
                rnd.setSeed(newTestcaseInfo.seed());
                m_currentSeedForTestFile = newTestcaseInfo.seed();
            }
            else
            {
                assert(m_currentSeedForTestFile != -1 && "Creating a new Testcase with no seed when no seed has yet been set for this TestFile is forbidden");
            }
            return *m_testcases.back();
        };

        const SubtaskInfo* containingSubtask() const
        {
            return m_containingSubtask;
        }

        std::string description() const
        {
            return m_description;
        }

        int numTestCases() const
        {
            return m_testcases.size();
        }

        std::vector<const Testcase<SubtaskInfo>*> testcases() const
        {
            std::vector<const Testcase<SubtaskInfo>*> testcases;
            for (const auto& testcase : m_testcases)
            {
                testcases.push_back(testcase.get());
            }
            return testcases;
        }
        
    private:
        std::vector<std::unique_ptr<Testcase<SubtaskInfo>>> m_testcases;
        const SubtaskInfo* m_containingSubtask = nullptr;
        std::string m_description;
        int m_currentSeedForTestFile = -1;
};

/**
 * Very, very strict testfile line reader/ parser.  The two readLine() overloads
 * read the next line of the testfile and parse the requested types from it.
 * While doing this, it tests that, amongst other things:
 *  
 *   i) No lines are empty.
 *   ii) No lines have leading whitespace.
 *   iii) There is precisely one space between each value in the line.
 *   iv) All requested values can be parsed successfully.
 *   v) There is no trailing data in the line.
 *   vi) There is no trailing data in the testfile (checked upon the (mandatory) call
 *       to markTestcaseAsValidated()).
 *
 * Writing your testcases using Testcase::writeLine/ writeObjectsAsLine should automatically
 * ensure this, but I'm very much a "belts and braces" kind of guy :)
 */
class TestFileReader
{
    public:
        TestFileReader(const std::string& testFileContents)
            : m_testFileInStream{testFileContents}
        {
        }
        template<typename... Types>
        std::tuple<Types...> readLine()
        {
            const auto originalFlags = m_testFileInStream.flags();
            std::noskipws(m_testFileInStream);

            std::tuple<Types...> readValues;

            const auto line = readLine();
            std::istringstream lineStream(line);
            std::noskipws(lineStream);
            readLineHelper<decltype(readValues), 0, Types...>(readValues, lineStream);

            m_testFileInStream.flags(originalFlags);

            return readValues;
        }
        template<typename ValueType>
        std::vector<ValueType> readLineOfValues(int numValuesToRead)
        {
            std::vector<ValueType> readValues;

            const auto line = readLine();
            std::istringstream lineStream(line);
            std::noskipws(lineStream);

            for (int i = 0; i < numValuesToRead; i++)
            {
                ValueType value = readAndValidateValue<ValueType>(lineStream, i == numValuesToRead - 1, i);
                readValues.push_back(value);
            }

            return readValues;
        }

        std::string peekLine()
        {
            const auto originalPos = m_testFileInStream.tellg();
            const auto line = readLine();
            const bool restoreToOriginalPosSuccess = m_testFileInStream.seekg(originalPos).good();
            assert(restoreToOriginalPosSuccess);
            return line;
        }

        void markTestcaseAsValidated()
        {
            assert(!m_isTestFileMarkedAsValidated);
            if (!hasErrors())
                m_testcaseNum++;
        }
        int numTestcasesValidated() const
        {
            return m_testcaseNum;
        }
        void markTestFileAsValidated()
        {
            assert(!m_isTestFileMarkedAsValidated);
            if (!hasErrors())
                m_isTestFileMarkedAsValidated = true;
            if (hasUnreadData())
            {
                addError("Found trailing data at end of file");
            }
        }
        bool isTestFileMarkedAsValidated() const
        {
            return m_isTestFileMarkedAsValidated;
        }

        void addError(const std::string& errorMessage)
        {
            m_errorMessages.push_back(errorMessage + " at line " + std::to_string(m_numLinesRead) + ", testcase: " + std::to_string(m_testcaseNum));
        }
        void addErrorUnless(bool condition, const std::string& errorMessage)
        {
            if (!condition)
            {
                addError(errorMessage);
            }
        }
        std::vector<std::string> errorMessages() const
        {
            return m_errorMessages;
        }
        bool hasErrors() const
        {
            return !m_errorMessages.empty();
        }

    private:
        std::istringstream m_testFileInStream;
        std::vector<std::string> m_errorMessages;
        int m_numLinesRead = 0;
        int m_testcaseNum = 0;
        bool m_isTestFileMarkedAsValidated = false;

        std::string readLine()
        {
            std::string line;
            while (true)
            {
                const int nextCharValue = m_testFileInStream.get();
                if (nextCharValue == std::istringstream::traits_type::eof())
                {
                    addError("Could not read line: reached eof before '\\n'!");
                    return "";
                }
                const char character = static_cast<char>(nextCharValue);
                if (character == '\r')
                {
                    addError("Got a '\\r' character while reading line!");
                    return "";
                }
                if (character == '\n')
                    break;

                line += character;
            }

            m_numLinesRead++;

            if (line.empty())
            {
                addError("Got an empty line");
                return "";
            }

            if (isspace(line.front()))
            {
                addError("Got leading whitespace");
                return "";
            }

            return line;
        }

        template<typename ValuesType, std::size_t ValueIndex, typename Head, typename... Tail >
        void readLineHelper(ValuesType& readValues, std::istream& lineStream)
        {
            constexpr auto isLastOnLine = (sizeof...(Tail) == 0);
            if constexpr(isLastOnLine)
            {
                std::get<ValueIndex>(readValues) = readAndValidateValue<Head>(lineStream, true, ValueIndex);
            }
            else
            {
                std::get<ValueIndex>(readValues) = readAndValidateValue<Head>(lineStream, false, ValueIndex);
                readLineHelper<ValuesType, ValueIndex + 1, Tail...>(readValues, lineStream);
            }
        }
        template <typename ValueType>
        ValueType readAndValidateValue(std::istream& lineStream, bool isLastOnLine, int index)
        {
            ValueType value{};
            if (std::isspace(lineStream.peek()))
            {
                addError("Got superfluous space before value with index " + std::to_string(index));
                return value;
            }
                    
            lineStream >> value;
            if (!lineStream)
            {
                addError("Failed to read value with index " + std::to_string(index));
                return value;
            }

            if (!isLastOnLine)
            {
                assert((lineStream.flags() & std::ios::skipws) == 0);
                char followingChar = '\0';
                lineStream >> followingChar;
                if (followingChar != ' ')
                {
                    addError(std::string("Expecting a space after value with index " + std::to_string(index) + "; got '") + followingChar + "' instead");
                    return value;
                }
            }
            else
            {
                assert((lineStream.flags() & std::ios::skipws) == 0);
                if (lineStream.peek() != std::istringstream::traits_type::eof())
                {
                    addError("Got trailing characters after reading last value");
                    return value;
                }
            }

            return value;
        }

        bool hasUnreadData()
        {
            return m_testFileInStream.peek() != std::istringstream::traits_type::eof();
        }
};

template <typename SubtaskInfo>
class TestSuite
{
    public:
        TestFile<SubtaskInfo>& newTestFile(const TestFileInfo<SubtaskInfo>& newTestFileInfo)
        {
            assert(newTestFileInfo.containingSubtask() != nullptr);

            std::cout << "Beginning creation of new TestFile (subtask " << newTestFileInfo.containingSubtask()->subtaskId << ")";
            if (!newTestFileInfo.description().empty())
            {
                std::cout << " with description: " << newTestFileInfo.description();
            }
            std::cout << std::endl;


            if (newTestFileInfo.seed() != -1)
                rnd.setSeed(newTestFileInfo.seed());

            m_testFiles.push_back(std::make_unique<TestFile<SubtaskInfo>>(newTestFileInfo));
            return *m_testFiles.back();
        };
        int numTestFiles() const
        {
            return m_testFiles.size();
        }
        void setTestFileVerifier(std::function<bool(TestFileReader&, const SubtaskInfo&)> testFileVerifier)
        {
            assert(!m_testFileVerifier);
            m_testFileVerifier = testFileVerifier;
        }
        bool writeTestFiles()
        {
            auto trimTrailingWhiteSpace = [](std::string& stringToTrim)
            {
                while (!stringToTrim.empty() && isspace(stringToTrim.back()))
                {
                    stringToTrim.pop_back();
                }
            };
            std::map<int, std::vector<const TestFile<SubtaskInfo>*>> testFilesBySubtaskId;
            std::map<const TestFile<SubtaskInfo>*, std::string> fileNameForTestFile;
            std::map<int, const SubtaskInfo*> subtasksById;
            bool hasAnyValidationErrors = false;
            for (const auto& testFile : m_testFiles)
            {
                auto containingSubtask = testFile->containingSubtask();
                testFilesBySubtaskId[containingSubtask->subtaskId].push_back(testFile.get());

                if (subtasksById[containingSubtask->subtaskId] != nullptr)
                {
                    if (subtasksById[containingSubtask->subtaskId] != containingSubtask)
                    {
                        hasAnyValidationErrors = true;
                        std::cerr << "Error: Duplicate subtaskId: two Subtasks have the same id of " << containingSubtask->subtaskId << std::endl;
                    }
                }
                subtasksById[containingSubtask->subtaskId] = containingSubtask;
            }

            // Verify subtask ids are sequential.
            const int numSubTasks = subtasksById.size();
            for (int subtaskId = 1; subtaskId <= numSubTasks; subtaskId++)
            {
                if (subtasksById[subtaskId] == nullptr)
                {
                    hasAnyValidationErrors = true;
                    std::cerr << "Error: Subtask ids should be sequential, starting from 1: got: " << std::endl;
                    for (const auto& [subtaskId, subtask] : subtasksById)
                    {
                        (void)subtask; // Suppress "unused variable" warning.
                        std::cerr << subtaskId << " ";
                    }
                    std::cerr << std::endl;
                }
            }

            // Verify subtask scores sum to 100.
            int totalSubtaskScores = 0;
            for (int subtaskId = 1; subtaskId <= numSubTasks; subtaskId++)
            {
                const int subtaskScore = subtasksById[subtaskId]->score;
                if (subtaskScore <= 0 || subtaskScore > 100)
                {
                    hasAnyValidationErrors = true;
                    std::cerr << "Error: Subtask with id " << subtaskId << " has score not in range [1,100]: "  << subtaskScore << std::endl;
                }
                totalSubtaskScores += subtasksById[subtaskId]->score;
            }
            if (totalSubtaskScores != 100)
            {
                hasAnyValidationErrors = true;
                std::cerr << "Error: Total scores over all Subtasks should sum to 100, not " << totalSubtaskScores << std::endl;
            }

            // Verify and write out testfiles.
            int testFileNum = 0; // Start at "testfile-000*.in".
            for (const auto& [subtaskId, testFilesForSubtask] : testFilesBySubtaskId)
            {
                (void)subtaskId; // Suppress "unused variable" warning.
                for (const auto& testFile : testFilesForSubtask)
                {
                    const auto& testcases = testFile->testcases();
                    const int numTestCases = testcases.size();
                    assert(numTestCases > 0);

                    std::ostringstream testFileOutStream;
                    testFileOutStream << numTestCases << std::endl;
                    for (const auto& testcase : testcases)
                    {
                        auto testCaseContents = testcase->contents();
                        trimTrailingWhiteSpace(testCaseContents);
                        assert(!testCaseContents.empty());
                        testFileOutStream << testCaseContents << std::endl;
                    }

                    auto testFileContents = testFileOutStream.str();

                    std::string paddedFileNumber = std::to_string(testFileNum);
                    while (paddedFileNumber.length() < 3)
                        paddedFileNumber = '0' + paddedFileNumber;

                    const std::string filename = "testfile-" + paddedFileNumber + "-subtask-" + std::to_string(testFile->containingSubtask()->subtaskId) + ".in";
                    fileNameForTestFile[testFile] = filename;

                    if (m_testFileVerifier)
                    {
                        std::cout << "Verifying testfile " << filename << " ... " << std::endl;
                        TestFileReader testFileReader(testFileContents);
                        m_testFileVerifier(testFileReader, *testFile->containingSubtask());
                        // Check that the Verifier has been paying at least *some* attention, and not just rubber-stamping stuff!
                        if (testFileReader.numTestcasesValidated() != numTestCases)
                        {
                            testFileReader.addError("Only " + std::to_string(testFileReader.numTestcasesValidated()) + " of " + std::to_string(numTestCases) + " were marked as validated");
                        }
                        if (!testFileReader.isTestFileMarkedAsValidated())
                        {
                            testFileReader.addError("The testfile was not marked as validated!");
                        }

                        if (testFileReader.hasErrors())
                        {
                            hasAnyValidationErrors = true;
                            std::cerr << "** The testfile with description (" << (testFile->description().empty() ? "no description" : testFile->description()) << ") and filename " << filename << "  has validation errors:" << std::endl;
                            for (const auto& errorMessage : testFileReader.errorMessages())
                            {
                                std::cerr << "     " << errorMessage << std::endl;
                            }
                        }
                        std::cout << "Done verifying testfile " << filename << " ... " << std::endl;
                    }


                    std::ofstream testFileOutFileStream(filename);
                    assert(testFileOutFileStream.is_open());
                    testFileOutFileStream << testFileContents;
                    assert(testFileOutFileStream.flush());
                    testFileOutFileStream.close();

                    testFileNum++;
                }
            }

            std::cout << "Generated the following test files: " << std::endl;
            testFileNum = 0; // Start at "testfile #0".
            for (const auto& [subtaskId, testFilesForSubtask] : testFilesBySubtaskId)
            {
                assert(!testFilesBySubtaskId.empty());
                const SubtaskInfo& subTaskInfo = *testFilesForSubtask.front()->containingSubtask();
                std::cout << "Subtask: " << subtaskId << " (score: " << subTaskInfo.score << ")" << std::endl;
                std::cout << subTaskInfo << std::endl << std::endl;
                for (const auto testFile : testFilesForSubtask)
                {
                    std::cout << "   Testfile # " << testFileNum << " filename: " << fileNameForTestFile[testFile] << " (" << (testFile->description().empty() ? "no description" : testFile->description()) << ")" << std::endl;

                    std::cout << "      " << testFile->numTestCases() << " test cases";

                    const auto& testcases = testFile->testcases();
                    if (std::count_if(testcases.begin(), testcases.end(), [](const auto& testcase) { return !testcase->description().empty();}) != 0)
                    {
                        std::cout << "; printing out ones with descriptions: " << std::endl << std::endl;
                        int testcaseNum = 1;

                        for (const auto testcase : testcases)
                        {
                            if (!testcase->description().empty())
                            {
                                std::cout << "         Testcase # " << testcaseNum << ": " << testcase->description() << std::endl;
                            }

                            testcaseNum++;
                        }
                    }
                    else
                    {
                        std::cout << "; none with individual descriptions." << std::endl;
                    }
                    testFileNum++;
                    std::cout << std::endl;
                }

                std::cout << std::endl;
            }

            // Print some handy information for filling in some of the "Edit Problem" fields.
            std::cout << "In the Edit Problem page, the contents of the field 'Problem test sequence' should be: " << std::endl;
            for (int i = 0; i < numTestFiles(); i++)
            {
                std::cout << "#" << i << " ";
            }
            std::cout << std::endl << std::endl;

            std::cout << "In the Edit Problem page, the contents of the field 'Auxiliary data passed to masterjudge' should be: " << std::endl;
            std::cout << numSubTasks << " ";
            for (int subtaskId = 1; subtaskId <= numSubTasks; subtaskId++)
            {
                std::cout << subtasksById[subtaskId]->score << " ";
            }
            for (const auto& testFile : m_testFiles)
            {
                std::cout << testFile->containingSubtask()->subtaskId << " ";
            }
            std::cout << std::endl << std::endl;

            if (hasAnyValidationErrors)
            {
                std::cerr << "\n** Error - some testfiles had some validation errors (see above for more details) - the generated testfiles should *NOT* be submitted for use in a Contest!!! **" << std::endl;
                return false;
            }

            return true;
        }
    private:
        std::vector<std::unique_ptr<TestFile<SubtaskInfo>>> m_testFiles;
        std::function<bool(TestFileReader&, const SubtaskInfo&)> m_testFileVerifier;
};
