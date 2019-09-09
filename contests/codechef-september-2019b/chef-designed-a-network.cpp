// Simon St James (ssjgz) - 2019-09-09
// 
// Solution to: https://www.codechef.com/SEPT19B/problems/CHEFK1
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

#if 1
int solveBruteForce(int64_t N, int64_t M)
{
    // TODO - make sure we're dealing with N == 1 and 2.  Also M == 0.
    if (M == 0 && N == 1)
    {
        // Graph consisting of just the one node with no edges presumably
        // counts as connected.
        return 0;
    }
    if (M < N - 1)
    {
        // Can't make a connected graph.
        return -1;
    }
    if (M > (N * (N + 1)) / 2)
    {
        // More edges than will fit on a complete
        // graph on N vertices (plus one self-loop per vertex).
        // NB: N <= 10^6, so N * N + N will not overflow an int64_t.
        return -1;
    }
    if (M == N - 1)
    {
        // Just connect the vertices in a line.
        if (N == 2)
        {
            // A pair of vertices connected together.
            return 1;
        }
        else
        {
            // At least one of the vertices will have degree 2.
            return 2;
        }
    }
    if (M == N)
    {
        if (N >= 2)
        {
            // For N > 2, a cycle through all vertices.  At least one will have degree 2.
            // For N == 2, connect the pair and add a self-loop from one to itself.
            // Exactly one will have degree 2.
            return 2;
        }
        else if (N == 1)
        {
            // For N == 1, a loop from the vertex to itself: this vertex will have degree 1.
            return 1;
        }
    }
    if (M > N && M <= 2 * N)
    {
        // For N > 2, a cycle through all vertices, then a self-loop from each vertex to iself
        // until M is exhausted.  At least one self-loop will be required; this will have
        // degree 3.  Vertices without self-loops will have degree 2.
        if (N > 2)
        {
            return 3;
        }
        if (N == 2)
        {
            // M == 3 is the only possibility.
            if (M == 3)
            {
                // Line connecting them, plus self-loop for each vertex.
                return 2;
            }

        }
    }
    // M > 2 * N.
    // For M == 2 * N, we have a cycle through all vertice and a self-loop through
    // each vertex.



    
    bool result = false;
    
    return result;
}
#endif

#if 0
bool solveOptimised()
{
    bool result = false;
    
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

        for (int t = 0; t < T; t++)
        {
        }

        return 0;
    }

#if 0
    const int N = 4;
    for (int M = 2 * N + 1; M <= ((N + 1) * N) / 2; M++)
    {
        const int blah = ((M - (2 * N + 1)) / (N / 2)) + 4;
        cout << "N: " << N << " M: " << M << " blah: " << blah << endl;
    }
#endif
    const int N = 13;
    vector<int> nodeDegree(N);
    for (int i = 0; i < N; i++)
    {
        nodeDegree[i] = 3;
    }
    for (int M = 2 * N + 1; M <= ((N + 1) * N) / 2; M++)
    {
        vector<int> nodeIndicesByDegree;
        for (int i = 0; i < N; i++)
        {
            nodeIndicesByDegree.push_back(i);
        }
        sort(nodeIndicesByDegree.begin(), nodeIndicesByDegree.end(), [&nodeDegree](const auto nodeIndex1, const auto nodeIndex2)
                {
                    if (nodeDegree[nodeIndex1] < nodeDegree[nodeIndex2])
                        return true;
                    return nodeIndex1 < nodeIndex2;
                });

        nodeDegree[nodeIndicesByDegree[0]]++;
        nodeDegree[nodeIndicesByDegree[1]]++;
        const int maxNodeDegree = *max_element(nodeDegree.begin(), nodeDegree.end());
        cout << "N: " << N << " M: " << M << " maxNodeDegree: " << maxNodeDegree << endl;

        if ((N % 2) == 0)
        {
            assert(maxNodeDegree == (M - 2 * N - 1) / (N / 2) + 4);
        }
        else
        {
            const int blah = (M - 2 * N - 1) / N;
            const int blah2 = (M - 2 * N - 1) % N;
            cout << "blah: " << blah << " blah2: " << blah << endl;
            const int wee = (4 + 2 * blah + ((blah2 >= (N - 1) / 2) ? 1 : 0));
            cout << "wee: " << wee << endl;;
            assert(wee == maxNodeDegree);
            
        }
    }

    return 0; // TODO - remove this.
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto N = read<int64_t>();
        const auto M = read<int64_t>();

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(N, M);
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
