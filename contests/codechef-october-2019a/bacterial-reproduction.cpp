// Simon St James (ssjgz) - 2019-XX-XX
// 
// Solution to: TODO - problem link here!
//
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
class NodeIdTracker
{
    public:
        NodeIdTracker() = default;
        NodeIdTracker(int maxId)
            : m_maxId{maxId},
            m_numElements{2 * maxId},
            m_elements(m_numElements + 1)
            {
            }
        int numIdsGreaterThan(int nodeId)
        {
            return numInRange(nodeId + 1, m_maxId);
        }
        int64_t numIdsLessThan(int nodeId)
        {
            return numInRange(0, nodeId - 1);
        }

        void registerNodeId(int nodeId, int64_t blop)
        {
            const auto n = m_numElements;
            auto elements = m_elements.data();
            int pos = nodeId + 1; // Make 1-relative.
            while(pos <= n)
            {
                elements[pos] += blop;
                assert(elements[pos] >= 0);
                pos += (pos & (pos * -1));
            }
        }
    private:
        int m_maxId;
        int m_numElements;
        vector<int64_t> m_elements;

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
};


#if 0
SolutionType solveBruteForce()
{
    SolutionType result;
    
    return result;
}
#endif

#if 0
SolutionType solveOptimised()
{
    SolutionType result;
    
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
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
        }

        return 0;
    }
    struct timeval time;
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

    const int maxID = 500'000;
    NodeIdTracker nodeIdTracker(maxID);
    int64_t sum = 0;
    for (int i = 0; i < 500'000; i++)
    {

        nodeIdTracker.registerNodeId(rand() % maxID, rand() % 1000);
        const int blah = rand() % maxID;
        const auto blee = nodeIdTracker.numIdsLessThan(blah);
        const auto bloo = nodeIdTracker.numIdsGreaterThan(rand() % maxID);
        sum += blee + bloo;
        //cout << "i: " << i << " blah: " << blah << " blee: " << blee << endl;

    }
    cout << "Done!" << sum << endl;
    return 0;


    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {

#ifdef BRUTE_FORCE
#if 0
        const auto solutionBruteForce = solveBruteForce();
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 0
        const auto solutionOptimised = solveOptimised();
        cout << "solutionOptimised:  " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised();
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
