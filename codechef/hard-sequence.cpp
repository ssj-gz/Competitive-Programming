// Simon St James (ssjgz) - 2019-11-11
// 
// Solution to: https://www.codechef.com/problems/HRDSEQ
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>

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

int solveBruteForce(int N)
{
    int result = 0;
    
    vector<int> elements;
    elements.push_back(0);

    for (int i = 0; i < N - 1; i++)
    {
        cout << "i: " << i << endl;

        cout << "elements: " << endl;
        for (const auto x : elements)
        {
            cout << x << " ";
        }
        cout << endl;
        const auto lastElement = elements.back();
        cout << "i: " << i << " lastElement: " << lastElement << endl;
        int lastOccurence = -1;

        if (elements.size() > 1)
        {
            for (int index = elements.size() - 2;  index >= 0; index--)
            {
                if (elements[index] == lastElement)
                {
                    if (lastOccurence == -1)
                    {
                        lastOccurence = index;
                        break;
                    }
                }
            }
        }

        cout << "lastOccurence: " << lastOccurence << " elements.size(): " << elements.size() << endl;

        if (lastOccurence != -1)
        {
            elements.push_back(elements.size() - 1 - lastOccurence);
        }
        else
        {
            elements.push_back(0);
        }
    }

        cout << "elements final: " << endl;
        for (const auto x : elements)
        {
            cout << x << " ";
        }
        cout << endl;

    
    return count(elements.begin(), elements.end(), elements.back());
}

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
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(N);
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
