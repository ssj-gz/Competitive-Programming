#define _GLIBCXX_DEBUG       // Iterator safety; out-of-bounds access for Containers, etc.
#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <limits>

#include <cassert>

using namespace std;

bool findAllWaysToMakeSum(const int requiredSum, vector<int>::iterator nextPackageIter, const vector<int>& packagesWeights, int sumSoFar, vector<int>& chosenSoFar, vector<vector<int>>& destSets)
{
    if (sumSoFar == requiredSum)
    {
        destSets.push_back(chosenSoFar);
        return true;
    }

    if (nextPackageIter == packagesWeights.end())
        return false;

    bool result = findAllWaysToMakeSum(requiredSum, std::next(nextPackageIter), packagesWeights, sumSoFar, chosenSoFar, destSets);

    chosenSoFar.push_back(*nextPackageIter);
    result = findAllWaysToMakeSum(requiredSum, std::next(nextPackageIter), packagesWeights, sumSoFar + *nextPackageIter, chosenSoFar, destSets) || result;
    chosenSoFar.pop_back();

    return result;
}

bool canMakeSum(const int requiredSum, vector<int>::iterator nextPackageIter, const vector<int>& packagesWeights, int sumSoFar)
{
    if (sumSoFar == requiredSum)
        return true;

    if (nextPackageIter == packagesWeights.end())
        return false;

    const bool result = canMakeSum(requiredSum, std::next(nextPackageIter), packagesWeights, sumSoFar) ||
        canMakeSum(requiredSum, std::next(nextPackageIter), packagesWeights, sumSoFar + *nextPackageIter);

    return result;

} 

bool canMakeSums(const int group1RequiredSum, const int group2RequiredSum, vector<int>::iterator nextPackageIter, const vector<int>& packagesWeights, int group1SumSoFar, int group2SumSoFar)
{
    if (group1SumSoFar == group1RequiredSum && group2SumSoFar == group2RequiredSum)
        return true;

    if (nextPackageIter == packagesWeights.end())
        return false;

    const bool result = canMakeSums(group1RequiredSum, group2RequiredSum, std::next(nextPackageIter), packagesWeights, group1SumSoFar, group2SumSoFar) ||
        canMakeSums(group1RequiredSum, group2RequiredSum, std::next(nextPackageIter), packagesWeights, group1SumSoFar + *nextPackageIter, group2SumSoFar) ||
        canMakeSums(group1RequiredSum, group2RequiredSum, std::next(nextPackageIter), packagesWeights, group1SumSoFar, group2SumSoFar +*nextPackageIter);

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
        std::cout << "firstGroupSize: " <<firstGroupSize << std::endl;
        while (true)
        {
            //for (int i = 0; i < firstGroupSize; i++)
                //std::cout << " firstGroupSize: " << firstGroupSize << " i : " << i << " groupChoiceIndices[i]: " << groupChoiceIndices[i] << std::endl;

            // We have our first group choice.
            int64_t weightOfGroup = 0;
            for (int i = 0; i < firstGroupSize; i++)
            {
                //std::cout << " chosen: " << groupChoiceIndices[i] << std::endl;
                weightOfGroup += packageWeights[groupChoiceIndices[i]];
            }
            //std::cout << " weightOfGroup: " << weightOfGroup << std::endl;
            if ((totalPackagesWeight % weightOfGroup == 0) && totalPackagesWeight / weightOfGroup == 4)
            {
                // Could be valid.  Let's check!
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
                //if (canMakeSum(weightOfGroup, remainingPackageWeights.begin(), remainingPackageWeights, 0))
                if (canMakeSums(weightOfGroup, weightOfGroup, remainingPackageWeights.begin(), remainingPackageWeights, 0, 0))
                {
                    std::cout << " woo-hoo: can make required first, second and third groups with firstGroupSize: " << firstGroupSize << std::endl;

#if 0
                    vector<int> chosenSoFar;
                    vector<vector<int>> destSets;
                    findAllWaysToMakeSum(weightOfGroup, remainingPackageWeights.begin(), remainingPackageWeights, 0, chosenSoFar, destSets);

                    std::cout << "  num ways: " << destSets.size() << std::endl;
#endif
                    int64_t entanglement = 1;
                    for (int i = 0; i < firstGroupSize; i++)
                    {
                        ///std::cout << " chosen: " << groupChoiceIndices[i] << std::endl;
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

            // Choose next group, keeping the chosen indices as low as possible.
            int choiceIndex = 0;
            while((choiceIndex + 1 < firstGroupSize && groupChoiceIndices[choiceIndex + 1] == groupChoiceIndices[choiceIndex] + 1) && choiceIndex != firstGroupSize - 1)
            {
                groupChoiceIndices[choiceIndex] = choiceIndex;
                choiceIndex++;
            }
            groupChoiceIndices[choiceIndex]++;
            if (groupChoiceIndices[choiceIndex] == numPackages)
            {
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
