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
    // Fairly easy one.  Imagine if, for each element x, we could find the size
    // of the range surrounding x such  that x has the minimum value in that range 
    // (lengthOfRangeWhereElementIsMin): this is easily accomplished in O(n) using
    // indexOfPrevLowerThan/ indexOfNextLowerThan, which are copy-and-pasted from
    // the "Largest Rectangle" challenge.  Store x and its corresponding lengthOfRangeWhereElementIsMin
    // in the elementInfos array.
    //
    // Set maxMinsForWindowSizes to be an array of n values (well - n + 1) all initially set to 0.
    // To get maxMinsForWindowSizes to be correct, and so solve the problem, the following pseudocode
    // would do - it uses the fact that since element x has a range of lengthOfRangeWhereElementIsMin(x)
    // where x is minimum, then all the windows sizes
    //
    //    w = 1, 2, .... lengthOfRangeWhereElementIsMin(x) 
    //
    // have maxMinsForWindowSizes[w] >= x:
    //
    //    for elementInfo in elementInfos:
    //       for windowSize = 1, 2, ..., elementInfo.lengthOfRangeWhereElementIsMin:
    //          maxMinsForWindowSizes[windowSize] = max(maxMinsForWindowSizes[windowSize], elementInfo.value)
    //
    // Easy-peasy, but unfortunately this is O(n^2) - there are O(n) elementInfos, and each can cause lengthOfRangeWhereElementIsMin
    // iterations of the inner loop, and O(lengthOfRangeWhereElementIsMin) == o(n) :/
    //
    // Fear not, though: let's instead sort the element infos by decreasing value, and then (if two values are the same), by decreasing
    // lengthOfRangeWhereElementIsMin.  Let us also now count windowSize backwards down to 1.  Hopefully it's clear
    // that the following pseudocode accomplishes the same as the above:
    //
    //    for elementInfo in (sorted) elementInfos:
    //       for windowSize = elementInfo.lengthOfRangeWhereElementIsMin, elementInfo.lengthOfRangeWhereElementIsMin -1, ... , 1.
    //          maxMinsForWindowSizes[windowSize] = max(maxMinsForWindowSizes[windowSize], elementInfo.value)
    //
    // No gain there, alas.  But wait: imagine, when we were counting down our windowSize, we encountered a maxMinsForWindowSizes[windowSize]
    // which has already been set.  We could *stop counting down windowSize* immediately: if maxMinsForWindowSizes[windowSize] has been set,
    // then so has windowSize - 1, windowSize - 2, ... , 1 and moreover, it was set by an "earlier" elementInfo in our sorted elementInfos
    // i.e. by a previous element info whose value was at least equal to our current value.  Thus, maxMinsForWindowSizes[windowSize],
    // maxMinsForWindowSizes[windowSize - 1], ... maxMinsForWindowSizes[1] are already >= elementInfo.value, so 
    // the "maxMinsForWindowSizes[windowSize] = max(..." part of the algorithm would be a no-op, so no point continuing.  Thus
    // the following pseudocode is equivalent to the original:
    //
    //    for elementInfo in (sorted) elementInfos:
    //       for windowSize = elementInfo.lengthOfRangeWhereElementIsMin, elementInfo.lengthOfRangeWhereElementIsMin -1, ... , 1.
    //          if maxMinsForWindowSizes[windowSize] is already set:
    //             break
    //          maxMinsForWindowSizes[windowSize] = max(maxMinsForWindowSizes[windowSize], elementInfo.value)
    //
    // But now, each iteration of the inner (windowSize) loop now either: sets an element of maxMinsForWindowSizes for the first time
    // (can only happen O(n) times), or stops iterating; thus, its total contribution to the whole runtime, over all elementInfos,
    // is now O(n) instead of O(n^2).
    //
    // And that will do - finding each elementInfo.lengthOfRangeWhereElementIsMin takes O(n); sorting elementInfos takes O(n log2 n);
    // and finally updating maxMinsForWindowSizes takes O(n), so just O(n log2 n) in total.   Hooray!
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
