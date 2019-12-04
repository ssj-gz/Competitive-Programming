#include <vector>
#include <memory>
#include <cassert>
#include <sstream>
#include <fstream>
#include <tuple>

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
        TestcaseInfo& withSeed(const int seed)
        {
            assert(m_seed == -1);
            m_seed = seed;
            return *this;
        }
        int seed() const
        {
            return m_seed;
        }
    private:
        std::string m_description;
        int m_seed = -1;
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
        void writeLine(Types... toPrint)
        {
            writeLineHelper(toPrint...);

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

        template <typename Head, typename... Tail>
        void writeLineHelper(const Head& head, Tail... tail)
        {
            m_contents << head << " ";
            writeLine(tail...);

        }
        template<typename Head>
        void writeLineHelper(const Head& head)
        {
            m_contents << head << std::endl;
        }

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
        TestFileInfo& withSeed(const int seed)
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
        int seed() const
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
        int m_seed = -1;
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
            m_description = testFileInfo.description();
        }
        Testcase<SubtaskInfo>& newTestcase(const TestcaseInfo<SubtaskInfo>& newTestcaseInfo)
        {
            m_testcases.push_back(std::make_unique<Testcase<SubtaskInfo>>(newTestcaseInfo));
            if (newTestcaseInfo.seed() != -1)
            {
                rnd.setSeed(newTestcaseInfo.seed());
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
};

template <typename SubtaskInfo>
class TestSuite
{
    public:
        TestFile<SubtaskInfo>& newTestFile(const TestFileInfo<SubtaskInfo>& newTestFileInfo)
        {
            assert(newTestFileInfo.containingSubtask() != nullptr);
            assert(newTestFileInfo.seed() != -1);

            rnd.setSeed(newTestFileInfo.seed());

            m_testFiles.push_back(std::make_unique<TestFile<SubtaskInfo>>(newTestFileInfo));
            return *m_testFiles.back();
        };
        void setTestFileVerifier(std::function<bool(std::istream&, const SubtaskInfo&)> testFileVerifier)
        {
            assert(!m_testFileVerifier);
            m_testFileVerifier = testFileVerifier;
        }
        void writeTestFiles()
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
            int testFileNum = 1;
            for (const auto& testFile : m_testFiles)
            {
                testFilesBySubtaskId[testFile->containingSubtask()->subtaskId].push_back(testFile.get());

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
                trimTrailingWhiteSpace(testFileContents);

                std::string paddedFileNumber = std::to_string(testFileNum);
                while (paddedFileNumber.length() < 3)
                    paddedFileNumber = '0' + paddedFileNumber;

                const std::string filename = "testfile-" + paddedFileNumber + "-subtask-" + std::to_string(testFile->containingSubtask()->subtaskId) + ".txt";
                fileNameForTestFile[testFile.get()] = filename;

                std::ofstream testFileOutFileStream(filename);
                assert(testFileOutFileStream.is_open());
                testFileOutFileStream << testFileContents;
                assert(testFileOutFileStream.flush());
                testFileOutFileStream.close();

                testFileNum++;
            }

            for (const auto& [subtaskId, testFilesForSubtask] : testFilesBySubtaskId)
            {
                std::cout << "Generated the following test files: " << std::endl;
                std::cout << "Subtask: " << subtaskId << std::endl;
                for (const auto testFile : testFilesForSubtask)
                {
                    std::cout << "   " << fileNameForTestFile[testFile] << " (" << (testFile->description().empty() ? "no description" : testFile->description()) << ")" << std::endl;

                    std::cout << "      " << testFile->numTestCases() << " test cases";

                    const auto& testcases = testFile->testcases();
                    if (std::count_if(testcases.begin(), testcases.end(), [](const auto& testcase) { return !testcase->description().empty();}) != 0)
                    {
                        std::cout << "; printing out ones with descriptions: " << std::endl;
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
                        std::cout << std::endl;
                    }
                }

            }
        }
    private:
        std::vector<std::unique_ptr<TestFile<SubtaskInfo>>> m_testFiles;
        std::function<bool(std::istream&, const SubtaskInfo&)> m_testFileVerifier;
};

class TestFileReader
{
    public:
        TestFileReader(std::istream& testFileInStream)
            : m_testFileInStream{testFileInStream}
        {
        }
        template<typename... Types>
        std::tuple<Types...> readLine()
        {
            const auto originalFlags = m_testFileInStream.flags();
            std::noskipws(m_testFileInStream);

            std::tuple<Types...> readValues;

            readLineHelper<decltype(readValues), 0, sizeof(Types)..., Types...>(readValues);

            m_testFileInStream.flags(originalFlags);

            return readValues;
        }
    private:
        std::istream& m_testFileInStream;

        template<typename ValuesType, std::size_t ValueIndex, std::size_t NumValues, typename Head, typename... Tail >
        void readLineHelper(ValuesType& readValues)
        {
            Head nextValue;
            m_testFileInStream >> nextValue;
            std::get<ValueIndex>(readValues) = nextValue;
        }
};
