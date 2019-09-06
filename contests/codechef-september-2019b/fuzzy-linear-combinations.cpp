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
        cout << "query: " << query << endl;
        int64_t queryResult = 0;
        for (int i = 0; i < n; i++)
        {
            for (int j = i; j < n; j++)
            {
                int64_t subarrayGcd = a[i];
                for (int k = i; k <= j; k++)
                {
                    subarrayGcd = gcd(subarrayGcd, a[k]);
                }
                cout << "i: " << i << " j: " << j << " gcd[i,j] = " << subarrayGcd << endl;
                if ((query % subarrayGcd) == 0)
                {
                    queryResult++;
                }
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
        const int N = rand() % 100 + 1;
        const int maxA = rand() % 1000 + 1;

        cout << N << endl;

        for (int i = 0; i < N; i++)
        {
            cout << ((rand() % 1000 + 1)) << " ";
        }
        cout << endl;

        const int Q = rand() % 100 + 1;
        cout << Q << endl;

        for (int i = 0; i < Q; i++)
        {
            cout << ((rand() % (20 * maxA))) << " ";
        }
        cout << endl;

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
    cout << "solution for query: " << endl;
    for (const auto& x : solutionBruteForce)
    {
        cout << "solutionBruteForce: " << x << endl;
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
