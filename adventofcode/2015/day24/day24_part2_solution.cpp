#define _GLIBCXX_DEBUG       // Iterator safety; out-of-bounds access for Containers, etc.
#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <limits>

#include <cassert>

using namespace std;

bool canChooseTwoGroupsWithSum(const int groupsRequiredSum, vector<int>::iterator nextPackageIter, const vector<int>& packagesWeights, int group2SumSoFar, int group3SumSoFar)
{
    // Try to get (simultaneously) Group 2 to add up to groupsRequiredSum, and Group 3 to also add up to groupsRequiredSum.
    // Each package can be added to precisely one of the following:
    //  * Group 2
    //  * Group 3
    //  * Neither group
    if (group2SumSoFar == groupsRequiredSum && group3SumSoFar == groupsRequiredSum)
        return true;

    if (nextPackageIter == packagesWeights.end())
        return false;

    const bool result = canChooseTwoGroupsWithSum(groupsRequiredSum, std::next(nextPackageIter), packagesWeights, group2SumSoFar, group3SumSoFar) ||
        canChooseTwoGroupsWithSum(groupsRequiredSum, std::next(nextPackageIter), packagesWeights, group2SumSoFar + *nextPackageIter, group3SumSoFar) ||
        canChooseTwoGroupsWithSum(groupsRequiredSum, std::next(nextPackageIter), packagesWeights, group2SumSoFar, group3SumSoFar +*nextPackageIter);

    return result;

} 
int main()
{
    vector<int> packageWeights;
    int weight = -1;
    while (cin >> weight)
        packageWeights.push_back(weight);

    std::sort(packageWeights.begin(), packageWeights.end());

    int firstGroupSize = 1;
    const int numPackages = static_cast<int>(packageWeights.size());
    const auto totalPackagesWeight = std::accumulate(packageWeights.begin(), packageWeights.end(), 0);

    int64_t minEntanglement = std::numeric_limits<int64_t>::max();
    bool foundGroup = false;
    while (!foundGroup)
    {
        vector<int> groupChoiceIndices;
        for (int i = 0; i < firstGroupSize; i++)
            groupChoiceIndices.push_back(i);
        while (true)
        {
            // We have our first group choice; is it valid?
            int64_t weightOfGroup = 0;
            for (int i = 0; i < firstGroupSize; i++)
            {
                weightOfGroup += packageWeights[groupChoiceIndices[i]];
            }
            if ((totalPackagesWeight % weightOfGroup == 0) && totalPackagesWeight / weightOfGroup == 4)
            {
                // Passes the initial weight checks, but can we split the other 3 groups so that they
                // too each weig weightOfGroup?
                vector<int> remainingPackageWeights;
                int choiceIndex = 0;
                for (int packageIndex = 0; packageIndex < numPackages; packageIndex++)
                {
                    if ((choiceIndex >= firstGroupSize) || (packageIndex != groupChoiceIndices[choiceIndex]))
                        remainingPackageWeights.push_back(packageWeights[packageIndex]);
                    else
                        choiceIndex++;
                }
                assert(static_cast<int>(remainingPackageWeights.size()) == numPackages - firstGroupSize);
                if (canChooseTwoGroupsWithSum(weightOfGroup, remainingPackageWeights.begin(), remainingPackageWeights, 0, 0))
                {
                    std::cout << " woo-hoo: can make required first, second and third groups (and so, fourth) with firstGroupSize: " << firstGroupSize << std::endl;

                    int64_t entanglement = 1;
                    for (int i = 0; i < firstGroupSize; i++)
                    {
                        entanglement *= packageWeights[groupChoiceIndices[i]];
                    }
                    if (entanglement < minEntanglement)
                    {
                        minEntanglement = entanglement;
                        std::cout << "new best minEntanglement: " << minEntanglement << std::endl;
                    }
                    foundGroup = true;
                }
            }

            // Choose next candidate first group, keeping the chosen indices as low as possible.
            int choiceIndex = 0;
            while((choiceIndex + 1 < firstGroupSize && groupChoiceIndices[choiceIndex + 1] == groupChoiceIndices[choiceIndex] + 1) && choiceIndex != firstGroupSize - 1)
            {
                groupChoiceIndices[choiceIndex] = choiceIndex;
                choiceIndex++;
            }
            groupChoiceIndices[choiceIndex]++;
            if (groupChoiceIndices[choiceIndex] == numPackages)
            {
                // No remaining choices of size firstGroupSize possible.
                break;
            }
        }

        firstGroupSize++;
        if (firstGroupSize > numPackages)
            break;
    }
    std::cout << "result: " << minEntanglement << std::endl;

    return 0;
}
