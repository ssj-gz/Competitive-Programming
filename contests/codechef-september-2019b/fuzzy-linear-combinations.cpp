// Simon St James (ssjgz) - 2019-09-06
// 
// Solution to: https://www.codechef.com/SEPT19B/problems/FUZZYLIN
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

int gcd(int a, int b)
{
    while (true)
    {
        if (a > b)
            swap(a, b);
        if (a == 0)
            break;
        const int nextA = a;
        const int nextB = b - a;

        a = nextA;
        b = nextB;
    }
    return b;
}


#if 1
vector<int64_t> solveBruteForce(const vector<int64_t>& a, const vector<int>& queries)
{
    vector<int64_t> results;
    const int n = a.size();

    for (const auto query : queries)
    {
        int64_t queryResult = 0;
        for (int i = 0; i < n; i++)
        {
            int64_t subarrayGcd = a[i];
            for (int j = i; j < n; j++)
            {
                subarrayGcd = gcd(subarrayGcd, a[j]);
            }
            if ((query % subarrayGcd) == 0)
            {
                queryResult++;
            }
        }
        results.push_back(queryResult);
    }

    return results;
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

    vector<int64_t> a(N);
    for (auto& x : a)
    {
        x = read<int64_t>();
    }

    const int Q = read<int>();
    vector<int> queries(Q);
    for (auto& x : queries)
    {
        x = read<int>();
    }

#ifdef BRUTE_FORCE
#if 1
    const auto solutionBruteForce = solveBruteForce(a, queries);
    cout << "solutionBruteForce: " << endl;
    for (const auto& x : solutionBruteForce)
    {
        cout << x << endl;
    }
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

    assert(cin);
}
