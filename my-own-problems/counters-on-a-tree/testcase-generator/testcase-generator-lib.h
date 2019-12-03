#include <vector>
#include <memory>
#include <cassert>

#include <testlib.h>

template <typename SubtaskInfo>
struct Testcase
{
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
        TestcaseInfo& withSeed(const int seed)
        {
            assert(m_seed == -1);
            m_seed = seed;
            return *this;
        }
        TestcaseInfo& belongingToSubtask(const SubtaskInfo& containingSubtask)
        {
            assert(m_containingSubtask.empty());
            m_containingSubtask = containingSubtask;
            return *this;
        }

        std::string description() const
        {
            return m_description;
        }
        int seed() const
        {
            return m_seed;
        }
        SubtaskInfo* containingSubtask() const
        {
            return m_containingSubtask;
        }
    private:
        std::string m_description;
        int m_seed = -1;
        SubtaskInfo* m_containingSubtask = nullptr;
};

template <typename SubtaskInfo>
class Testfile
{
    public:
        Testcase<SubtaskInfo>& newTestcase(const TestcaseInfo<SubtaskInfo>& newTestcaseInfo)
        {
            assert(newTestcaseInfo.seed() != -1);
            assert(newTestcaseInfo.containingSubtask() != nullptr);

            m_testcases.push_back(std::make_unique<Testcase<SubtaskInfo>>());
            return *m_testcases.back();
        };
        
    private:
        std::vector<std::unique_ptr<Testcase<SubtaskInfo>>> m_testcases;
};
