#include <vector>
#include <memory>
#include <cassert>
#include <sstream>

#include <testlib.h>

template <typename SubtaskInfo>
class Testcase
{
    public:
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
        std::string contents()
        {
            m_contents.flush();
            return m_contents.str();
        }
    private:
        // Write in binary mode - don't want '\r's added to newlines if we
        // generate on e.g. windows.
        std::ostringstream m_contents{std::ios_base::out | std::ios_base::binary};

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
        int seed() const
        {
            return m_seed;
        }

        const SubtaskInfo* containingSubtask() const
        {
            return m_containingSubtask;
        }

    private:
        int m_seed = -1;
        const SubtaskInfo* m_containingSubtask = nullptr;
};

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
class TestFile
{
    public:
        Testcase<SubtaskInfo>& newTestcase(const TestcaseInfo<SubtaskInfo>& newTestcaseInfo)
        {
            m_testcases.push_back(std::make_unique<Testcase<SubtaskInfo>>());
            return *m_testcases.back();
        };
        
    private:
        std::vector<std::unique_ptr<Testcase<SubtaskInfo>>> m_testcases;
};

template <typename SubtaskInfo>
class TestSuite
{
    public:
        TestFile<SubtaskInfo>& newTestFile(const TestFileInfo<SubtaskInfo>& newTestFileInfo)
        {
            assert(newTestFileInfo.containingSubtask() != nullptr);
            assert(newTestFileInfo.seed() != -1);

            m_testFiles.push_back(std::make_unique<TestFile<SubtaskInfo>>());
            return *m_testFiles.back();
        };
        
    private:
        std::vector<std::unique_ptr<TestFile<SubtaskInfo>>> m_testFiles;
};
