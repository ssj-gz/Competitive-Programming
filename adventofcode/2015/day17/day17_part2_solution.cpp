#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <limits>

#include <cassert>

using namespace std;

void calcMinimumContainers(int amount, int containerIndex, int numContainersUsed, const vector<int>& containerSizes, int& minContainers)
{
    const int numContainers = static_cast<int>(containerSizes.size());
    if (containerIndex == numContainers)
    {
        if (amount == 0)
        {
            if (numContainersUsed < minContainers)
            {
                minContainers = numContainersUsed;
                std::cout << "New minContainers: " << minContainers << std::endl;
            }
        }
        return;
    }

    // Use this container.
    calcMinimumContainers(amount - containerSizes[containerIndex], containerIndex + 1, numContainersUsed + 1, containerSizes, minContainers);
    // Don't use this container.
    calcMinimumContainers(amount, containerIndex + 1, numContainersUsed, containerSizes, minContainers);
}

int64_t countCombinations(int amount, int containerIndex, int numContainersUsed, const vector<int>& containerSizes, map<std::tuple<int, int, int>, int64_t>& lookup, const int minContainers)
{
    std::cout << "countCombinations: amount: " << amount << " containerIndex: " << containerIndex << std::endl;
    auto calcResult = [amount, containerIndex, numContainersUsed, &containerSizes, &lookup, minContainers]() -> int64_t
    {
        const int numContainers = static_cast<int>(containerSizes.size());
        if (amount < 0)
            return 0;
        if (containerIndex == numContainers)
            return (amount == 0 && numContainersUsed == minContainers) ? 1 : 0;
        if (lookup.contains({amount, containerIndex, numContainersUsed}))
            return lookup[{amount, containerIndex, numContainersUsed}];
        int64_t result = 0;
        // Use this container.
        result += countCombinations(amount - containerSizes[containerIndex], containerIndex + 1, numContainersUsed + 1, containerSizes, lookup, minContainers);
        // Don't use this container.
        result += countCombinations(amount, containerIndex + 1, numContainersUsed, containerSizes, lookup, minContainers);
        return result;
    };
    const auto result = calcResult();
    lookup[{amount, containerIndex, numContainersUsed}] = result;
    return result;
}

int main()
{
    vector<int> containerSizes;
    int size = -1;
    while (cin >> size)
    {
        containerSizes.push_back(size);
    }
    sort(containerSizes.begin(), containerSizes.end());
    constexpr auto amountRequired = 150;
    int minContainers = std::numeric_limits<int>::max();
    calcMinimumContainers(amountRequired, 0, 0, containerSizes, minContainers);
    std::cout << " minContainers: " << minContainers << std::endl;

    // lookup[{amount, containerIndex, numContainersUsed}] is the number of combinations
    // that use exactly amount litres, using precisely (minContainers - numContainersUsed) of the containers with
    // indices containerIndex, containerIndex +1, ... , numContainers - 1;
    map<std::tuple<int, int, int>, int64_t> lookup;

    const auto result = countCombinations(amountRequired, 0, 0, containerSizes, lookup, minContainers);
    std::cout << "result: " << result << std::endl;

    return 0;
}
