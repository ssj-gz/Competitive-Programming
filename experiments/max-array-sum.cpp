#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <cassert>

#include <sys/time.h>

using namespace std;

void solveBruteForce(int nextIndex, int64_t sumSoFar, const vector<int>& a, int64_t& bestSum)
{
    if (nextIndex >= a.size())
    {
        bestSum = max(bestSum, sumSoFar);
        return;
    }

    // Include a[nextIndex].
    solveBruteForce(nextIndex + 2, sumSoFar + a[nextIndex], a, bestSum);
    // Don't include a[nextIndex].
    solveBruteForce(nextIndex + 1, sumSoFar, a, bestSum);
}
int64_t solveBruteForce(const vector<int>& a)
{
    int64_t bestSum = std::numeric_limits<int64_t>::min();
    solveBruteForce(0, 0, a, bestSum);
    return bestSum;
}

int64_t solveOptimised(const vector<int>& a)
{
    const int n = a.size();
    if (n == 1)
    {
        return a[0] > 0 ? a[0] : 0;
    }

    vector<int64_t> maxUpToNUsingN(n);
    vector<int64_t> maxUpToNNotUsingN(n);
    maxUpToNUsingN[0] = a[0];
    maxUpToNNotUsingN[0] = 0;

    for (int i = 1; i < n; i++)
    {
        maxUpToNUsingN[i] = a[i] + maxUpToNNotUsingN[i - 1];
        maxUpToNNotUsingN[i] = max(maxUpToNUsingN[i - 1], maxUpToNNotUsingN[i - 1]);
    }
    return max(maxUpToNUsingN[n - 1], maxUpToNNotUsingN[n - 1]);
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));


        auto randInRange = [](const int l, const int r)
        {
            return l + rand() % (r - l + 1);
        };
        const int n = rand() % 20 + 1;
        int maxA = randInRange(-10'000, 10'000);
        int minA = randInRange(-10'000, 10'000);
        if (minA > maxA)
            std::swap(minA, maxA);
        cout << n << endl;
        for (int i = 0; i < n; i++)
        {
            cout << randInRange(minA, maxA) << " ";
        }
        cout << endl;
        return 0;
    }
    int n;
    cin >> n;

    vector<int> a(n);
    for (int i = 0; i < n; i++)
    {
        cin >> a[i];
    }
    const auto solutionBruteForce = solveBruteForce(a);
    const auto solutionOptimised = solveOptimised(a);
    cout << "solutionBruteForce: " << solutionBruteForce << " solutionOptimised: " << solutionOptimised << endl;
    assert(solutionOptimised == solutionBruteForce); 
    
}

