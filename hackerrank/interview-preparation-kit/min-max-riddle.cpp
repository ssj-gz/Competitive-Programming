// Simon St James (ssjgz) - 2019-04-05
#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

#include <cassert>

using namespace std;

vector<int> findIndexOfNextLowerThan(const vector<int>& arr)
{
    std::stack<int> indices;
    const int n = arr.size();

    vector<int> indexOfNextLowerThan(n, -1);

    for (int index = 0; index < n; index++)
    {
        while (!indices.empty() && arr[indices.top()] > arr[index])
        {
            indexOfNextLowerThan[indices.top()] = index;
            indices.pop();
        }
        indices.push(index);
    }

    return indexOfNextLowerThan;
}
vector<int> findIndexOfPrevLowerThan(const vector<int>& arr)
{
    // Leave the heavy-lifting to findIndexOfNextLowerThan after passing it
    // a reversed copy of arr.
    const int n = arr.size();
    const vector<int> reversedHeights(arr.rbegin(), arr.rend());
    auto indexOfPrevLowerThan = findIndexOfNextLowerThan(reversedHeights);

    // Correct for the fact that we gave findIndexOfNextLowerThan a reversed version 
    // of arr: Reverse order indexOfPrevLowerThan, and also "reverse" (flip from 
    // left to right) each index.
    for (auto& index : indexOfPrevLowerThan)
    {
        if (index != -1)
            index = (n - 1) - index;
    }
    reverse(indexOfPrevLowerThan.begin(), indexOfPrevLowerThan.end());

    return indexOfPrevLowerThan;
}


vector<int> findMaxMinsForWindowSizes(const vector<int>& arr)
{
    const int n = arr.size();

    const auto indexOfNextLowerThan = findIndexOfNextLowerThan(arr);
    const auto indexOfPrevLowerThan = findIndexOfPrevLowerThan(arr);

    struct ElementInfo
    {
        int value = -1;
        int lengthOfRangeWhereElementIsMin = -1;
    };

    vector<ElementInfo> elementInfos;

    for (int index = 0; index < n; index++)
    {
        const int distanceToLeftWhereWeAreMin = (indexOfPrevLowerThan[index] == -1 ? index : index - indexOfPrevLowerThan[index] - 1);
        const int distanceToRightWhereWeAreMin = (indexOfNextLowerThan[index] == -1 ? n - 1 - index : indexOfNextLowerThan[index] - index - 1);
        const int lengthOfRangeWhereElementIsMin = distanceToLeftWhereWeAreMin 
            + 1  // Include this element!
            + distanceToRightWhereWeAreMin;

        elementInfos.push_back({arr[index], lengthOfRangeWhereElementIsMin});
    }

    vector<int> maxMinForWindowSize(n + 1, -1);

    sort(elementInfos.begin(), elementInfos.end(), [](const ElementInfo& lhs, const ElementInfo& rhs)
            {
                if (lhs.value != rhs.value)
                    return lhs.value > rhs.value;
                return lhs.lengthOfRangeWhereElementIsMin > rhs.lengthOfRangeWhereElementIsMin;

            });

    for (const ElementInfo& elementInfo : elementInfos)
    {
        int windowSize = elementInfo.lengthOfRangeWhereElementIsMin;
        // If we find a maxMinForWindowSize[windowSize] != -1, we can stop
        // marking the lower window sizes - they've always been marked
        // by a value at least equal to elementInfo.value.
        // The total contribution of this while-loop across all elementInfos
        // is just O(n).
        while (windowSize > 0 && maxMinForWindowSize[windowSize] == -1)
        {
            maxMinForWindowSize[windowSize] = elementInfo.value;
            windowSize--;
        }
    }

    // Ditch the "window size 0" case.
    maxMinForWindowSize.erase(maxMinForWindowSize.begin());

    return maxMinForWindowSize;
}

int main(int argc, char* argv[])
{
    int n;
    cin >> n;

    vector<int> arr(n);
    for (int i = 0; i < n; i++)
    {
        cin >> arr[i];
    }

    const auto maxMinsForWindowSizes = findMaxMinsForWindowSizes(arr);
    for (const auto x : maxMinsForWindowSizes)
    {
        cout << x << " ";
    }
    cout << endl;
}
