// Simon St James (ssjgz) - 2019-06-15
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <map>
#include <cassert>

#include <sys/time.h>

using namespace std;

int64_t solveBruteForce(const vector<int64_t>& a, int64_t r)
{
    const int n = a.size();
    int64_t numTriplets = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            for (int k = j + 1; k < n; k++)
            {
                if (((a[k] % a[j]) == 0) &&
                    (a[k] / a[j] == r) &&
                   (((a[j] % a[i]) == 0) &&
                    (a[j] / a[i] == r)))
                {
                    cout << "Triple i j k: " << i << ", " << j << ", " << k << " (" << a[i] << ", " << a[j] << ", " << a[k] << ")" << endl;
                    numTriplets++;
                }
            }
        }
    }
    return numTriplets;
}

int64_t solveOptimised(const vector<int64_t>& a, int64_t r)
{
    int64_t numTriplets = 0;
    map<int64_t, int> numRemainingOccurrencesOf;
    for (const auto x : a)
    {
        numRemainingOccurrencesOf[x]++;
    }
    const auto numRemainingOccurrencesOfOriginal = numRemainingOccurrencesOf;

    map<int64_t, int> numRemainingPairsBeginningWith;
    for (const auto x : a)
    {
        // The order is important, here: x * r == x if r == 1.
        numRemainingOccurrencesOf[x]--;
        numRemainingPairsBeginningWith[x] += numRemainingOccurrencesOf[x * r];

    }

    numRemainingOccurrencesOf = numRemainingOccurrencesOfOriginal;
    for (const auto x : a)
    {
        // The order is important, here: x * r == x if r == 1.
        numRemainingOccurrencesOf[x]--;
        numRemainingPairsBeginningWith[x] -= numRemainingOccurrencesOf[x * r];

        numTriplets += numRemainingPairsBeginningWith[x * r];


    }

    return numTriplets;
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int n = rand() % 100;
        const int r = rand() % 10 + 1;
        const int maxNum = rand() % 1000 + 1;

        cout << n << " " << r << endl;
        for (int i = 0; i < n; i++)
        {
            cout << (rand() % maxNum + 1) << " ";
        }
        cout << endl;

        return 0;
    }
    int n;
    cin >> n;
    int64_t r;
    cin >> r;

    vector<int64_t> a(n);
    for (int i = 0; i < n; i++)
    {
        cin >> a[i];
    }
    assert(cin);

#ifdef BRUTE_FORCE
    const auto solutionBruteForce = solveBruteForce(a, r);
    const auto solutionOptimiesd = solveOptimised(a, r);
    cout << "solutionBruteForce: " << solutionBruteForce << " solutionOptimiesd: " << solutionOptimiesd << endl;

    assert(solutionBruteForce == solutionOptimiesd);
#else
    cout << solveOptimised(a, r) << endl;
#endif
}

