// Simon St James (ssjgz) - 2019-11-06
// 
// Solution to: https://www.codechef.com/NOV19B/problems/CAMC
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

struct OccurencesOfValue
{
    int value = -1;
    int numOccurences = 0;
}

vector<OccurencesOfValue> buildOccurrenceList(const vector<int64_t>& values)
{
    vector<OccurencesOfValue> occurrencesOfValue;
    vector<int64_t> valuesSorted;
    sort(valuesSorted.begin(), valuesSorted.end());
    int previousValue = valuesSorted[0];
    int numOfValue = 0;
    for (const auto& x : valuesSorted)
    {
        if (x != previousValue)
        {
            occurrencesOfValue.push_back({x, numOfValue});
            numOfValue = 0;
        }
        numOfValue++;
    }
    occurrencesOfValue.push_back({valuesSorted.back(), numOfValue}); // Pick up the stray.

    return occurrencesOfValue;
}

#if 1
int solveBruteForce(int N, int M, const vector<int64_t>& a)
{
    // Ok - if c1, c2, ... , cm are the colours, then any
    // valid colouring is of the form:
    //
    // π(c1)π(c2)...π(cm)π(c1)π(c2)...π(cm)...π(c1)π(c2)...
    //
    // where π is a permutation of c1, c2, ... , cm i.e.
    // is it some permutation of c1, c2, ... , cm.
    //
    // Similarly, a choice is valid for such a π if and only if
    // it is valid for the colouring:
    //
    // c1c2...cmc1c2...cm...c1c2...
    //
    // The colours at i and j are different if and only if i != j mod m,
    // so we merely need to find, for each k ... 1, 2, ..., m, the 
    // maximum a[i] such that i mod m = k and the minimum a[j] such that
    // j mod m != k.


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
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        const int M = read<int>();

        vector<int64_t> a(N);
        for (auto& x : a)
            x = read<int64_t>();

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(N, M, a);
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
