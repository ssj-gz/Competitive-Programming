#include <iostream>
#include <vector>
#include <utility>
#include <cassert>

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
