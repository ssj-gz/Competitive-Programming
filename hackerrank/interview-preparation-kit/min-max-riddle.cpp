#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

vector<int> solveBruteForce(const vector<int>& arr)
{
    const int n = arr.size();
    vector<int> results;

    for (int windowSize = 1; windowSize <= n; windowSize++)
    {
        int result = 0;
        for (int start = 0; start + windowSize <= n; start++)
        {
            const auto minInWindow = *std::min_element(arr.begin() + start, arr.begin() + start + windowSize);
            result = max(result, minInWindow);
        }
        results.push_back(result);
    }
    return results;
}

int main()
{
    int n;
    cin >> n;

    vector<int> arr(n);
    for (int i = 0; i < n; i++)
    {
        cin >> arr[i];
    }

    const auto solutionBruteForce = solveBruteForce(arr);
    cout << "brute force solution: ";
    for (const auto x : solutionBruteForce)
    {
        cout << x << " ";
    }
    cout << endl;
}
