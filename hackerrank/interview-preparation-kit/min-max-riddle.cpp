#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

#include <cassert>

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

vector<int> findIndexOfNextLowerThan(const vector<int>& heights)
{
    std::stack<int> buildingHeightIndices;
    const int n = heights.size();

    vector<int> indexOfNextLowerThan(n, -1);

    for (int index = 0; index < n; index++)
    {
        while (!buildingHeightIndices.empty() && heights[buildingHeightIndices.top()] > heights[index])
        {
            indexOfNextLowerThan[buildingHeightIndices.top()] = index;
            buildingHeightIndices.pop();
        }
        buildingHeightIndices.push(index);
    }

    return indexOfNextLowerThan;
}
vector<int> findIndexOfPrevLowerThan(const vector<int>& heights)
{
    // Leave the heavy-lifting to findIndexOfNextLowerThan after passing it
    // a reversed copy of heights.
    const int n = heights.size();
    const vector<int> reversedHeights(heights.rbegin(), heights.rend());
    auto indexOfPrevLowerThan = findIndexOfNextLowerThan(reversedHeights);

    // Correct for the fact that we gave findIndexOfNextLowerThan a reversed version 
    // of heights: Reverse order indexOfPrevLowerThan, and also "reverse" (flip from 
    // left to right) each index.
    for (auto& index : indexOfPrevLowerThan)
    {
        if (index != -1)
            index = (n - 1) - index;
    }
    reverse(indexOfPrevLowerThan.begin(), indexOfPrevLowerThan.end());

    return indexOfPrevLowerThan;
}


vector<int> solveOptimised(const vector<int>& arr)
{
    const int n = arr.size();
    vector<int> results;

    const auto indexOfNextLowerThan = findIndexOfNextLowerThan(arr);
    const auto indexOfPrevLowerThan = findIndexOfPrevLowerThan(arr);


    for (int index = 0; index < n; index++)
    {
        const int64_t distanceToLeftWhereWeAreMin = (indexOfPrevLowerThan[index] == -1 ? index : index - indexOfPrevLowerThan[index] - 1);
        const int64_t distanceToRightWhereWeAreMin = (indexOfNextLowerThan[index] == -1 ? n - 1 - index : indexOfNextLowerThan[index] - index - 1);
        const int64_t lengthOfRangeWhereWeAreMin = distanceToLeftWhereWeAreMin 
            + 1  // Include this building!
            + distanceToRightWhereWeAreMin;
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

    const auto solutionOptimised = solveOptimised(arr);
    cout << "optimised solution: ";
    for (const auto x : solutionOptimised)
    {
        cout << x << " ";
    }
    cout << endl;
    assert(solutionOptimised == solutionBruteForce);
}
