#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

int64_t countCombinations(int amount, int containerIndex, const vector<int>& containerSizes, map<int, map<int, int64_t>>& lookup )
{
    std::cout << "countCombinations: amount: " << amount << " containerIndex: " << containerIndex << std::endl;
    const int numContainers = static_cast<int>(containerSizes.size());
    if (amount < 0)
        return 0;
    if (containerIndex == numContainers)
        return amount == 0 ? 1 : 0;
    if (lookup.contains(amount)  && lookup[amount].contains(containerIndex))
        return lookup[amount][containerIndex];
    int64_t result = 0;
    // Use this container.
    result += countCombinations(amount - containerSizes[containerIndex], containerIndex + 1, containerSizes, lookup);
    // Don't use this container.
    result += countCombinations(amount, containerIndex + 1, containerSizes, lookup);

    lookup[amount][containerIndex] = result;
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
    // lookup[amount][containerIndex] is the number of combinations
    // that use exactly amount litres, using only the containers with
    // indices containerIndex, containerIndex +1, ... , numContainers - 1;
    map<int, map<int, int64_t>> lookup;

    const auto result = countCombinations(150, 0, containerSizes, lookup);
    std::cout << "result: " << result << std::endl;

    return 0;
}
