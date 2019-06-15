#include <iostream>
#include <vector>
#include <utility>
#include <cassert>

#include <sys/time.h>

using namespace std;

vector<int> solveBruteForce(const vector<int>& a, const vector<int>& queries)
{
    vector<int> results;
    for (const auto query : queries)
    {
        int largestXor = -1;
        for (const auto x : a)
        {
            largestXor = std::max(largestXor, x ^ query);
        }
        results.push_back(largestXor);
    }
    return results;
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int n = rand() % 100;
        const int m = rand() % 100;

        const int maxA = rand() % 10'000'000;
        const int maxQ = rand() % 10'000'000;

        cout << n << endl;
        for (int i = 0; i < n; i++)
        {
            cout << (rand() % maxA) << " ";
        }
        cout << endl;

        cout << m << endl;
        for (int i = 0; i < m; i++)
        {
            cout << (rand() % maxQ) << " ";
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

    int m;
    cin >> m;

    vector<int> queries(m);
    for (int i = 0; i < m; i++)
    {
        cin >> queries[i];
    }
    assert(cin);

    const auto bruteForceResults = solveBruteForce(a, queries);
    cout << "Brute force results: " << endl;
    for (const auto x : bruteForceResults)
    {
        cout << x << endl;
    }
}
