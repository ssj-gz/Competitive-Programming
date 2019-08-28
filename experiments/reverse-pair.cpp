// Simon St James (ssjgz) - 2019-XX-XX
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

// Typical SegmentTree - you can find similar implementations all over the place :)
class SegmentTree
{
    public:
        SegmentTree() = default;
        SegmentTree(int maxId)
            : m_maxId{maxId},
            m_numElements{2 * maxId},
            m_elements(m_numElements + 1)
            {
            }

        void addValueAt(int pos, int value)
        {
            const auto n = m_numElements;
            auto elements = m_elements.data();
            pos = pos + 1; // Make 1-relative.
            while(pos <= n)
            {
                elements[pos] += value;
                assert(elements[pos] >= 0);
                pos += (pos & (pos * -1));
            }
        }

        // Find the number in the given range (inclusive) in O(log2(maxId)).
        int numInRange(int start, int end) const
        {
            start++; // Make 1-relative.  start and end are inclusive.
            end++;
            int sum = 0;
            auto elements = m_elements.data();
            while(end > 0)
            {
                sum += elements[end];
                end -= (end & (end*-1));
            }
            start--;
            while(start > 0)
            {
                sum -= elements[start];
                start -= (start & (start*-1));
            }
            return sum;
        }
    private:
        int m_maxId;
        int m_numElements;
        vector<int> m_elements;

};


#if 1
int64_t solveBruteForce(const vector<int>& nums)
{
    int64_t result = 0;
    const int n = nums.size();

    for (int j = 0; j < n; j++)
    {
        for (int i = 0; i < j; i++)
        {
            if (nums[i] > 2 * nums[j])
                result++;
        }
    }

    return result;
}
#endif

#if 0
int64_t solveOptimised()
{
    int64_t result;

    return result;
}
#endif


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        return 0;
    }

    const int N = read<int>();

    vector<int> nums(N);
    for (auto& x : nums)
    {
        x = read<int>();
    }


#ifdef BRUTE_FORCE
#if 1
    const auto solutionBruteForce = solveBruteForce(nums);
    cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 0
    const auto solutionOptimised = solveOptimised();
    cout << "solutionOptimised: " << solutionOptimised << endl;

    assert(solutionOptimised == solutionBruteForce);
#endif
#else
    const auto solutionOptimised = solveOptimised();
    cout << solutionOptimised << endl;
#endif

    assert(cin);
}
