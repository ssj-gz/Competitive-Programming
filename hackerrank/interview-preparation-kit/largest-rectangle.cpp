// Simon St James (ssjgz) - 2019-04-05
#include <iostream>
#include <vector>
#include <stack>
#include <limits>
#include <algorithm>

#include <cassert>

using namespace std;

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

int64_t findLargestRectangle(const vector<int>& heights)
{
    int64_t largestRectangle = 0;
    const int n = heights.size();

    const auto indexOfNextLowerThan = findIndexOfNextLowerThan(heights);
    const auto indexOfPrevLowerThan = findIndexOfPrevLowerThan(heights);

    for (int index = 0; index < n; index++)
    {
        assert(indexOfNextLowerThan[index] == -1 || indexOfNextLowerThan[index] > index);
        assert(indexOfPrevLowerThan[index] == -1 || indexOfPrevLowerThan[index] < index);
        const int64_t distanceToLeftWhereWeAreMin = (indexOfPrevLowerThan[index] == -1 ? index : index - indexOfPrevLowerThan[index] - 1);
        const int64_t distanceToRightWhereWeAreMin = (indexOfNextLowerThan[index] == -1 ? n - 1 - index : indexOfNextLowerThan[index] - index - 1);
        const int64_t largestRectangleWhereWeAreMin = (distanceToLeftWhereWeAreMin + 1 + distanceToRightWhereWeAreMin) * heights[index];

        largestRectangle = max(largestRectangle, largestRectangleWhereWeAreMin);
    }

    return largestRectangle;
}

int main(int argc, char* argv[])
{
    int n;
    cin >> n;

    vector<int> heights(n);

    for (int i = 0; i < n; i++)
    {
        cin >> heights[i];
    }

    cout << findLargestRectangle(heights) << endl;
}

