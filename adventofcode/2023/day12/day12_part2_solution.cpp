#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

#include <cassert>

using namespace std;

struct Lookup
{
    vector<vector<int64_t>> numArrangementsForPosAndGroupIndex;
    vector<int> spaceNeededForGroupIndex;
};

int64_t countValidArrangements(const string& arrangementTemplate, const vector<int>& requiredNumInGroup, int templatePos, int groupIndex, Lookup& lookup)
{
    if (templatePos >= arrangementTemplate.size())
    {
        return (groupIndex == requiredNumInGroup.size() ? 1: 0 );
    }
    if (groupIndex == requiredNumInGroup.size())
    {
        bool hasSuperfluousSprings = false;
        for (int i = templatePos; i < arrangementTemplate.size(); i++)
        {
            if (arrangementTemplate[i] == '#')
            {
                hasSuperfluousSprings = true;
                break;
            }
        }
        const int64_t result = (!hasSuperfluousSprings ? 1 : 0);
        lookup.numArrangementsForPosAndGroupIndex[templatePos][groupIndex] = result;
        return result;
    }
    if (lookup.numArrangementsForPosAndGroupIndex[templatePos][groupIndex] != -1)
        return lookup.numArrangementsForPosAndGroupIndex[templatePos][groupIndex];
    const int spaceRemaining = arrangementTemplate.size() - templatePos;
    if (spaceRemaining < lookup.spaceNeededForGroupIndex[groupIndex])
        return 0;
    const int beginningOfRun = templatePos;
    const int endOfRun = templatePos + requiredNumInGroup[groupIndex] - 1;
    bool canPlaceHere = true;
    assert(endOfRun < arrangementTemplate.size());
    if (endOfRun + 1 <= arrangementTemplate.size() && arrangementTemplate[endOfRun + 1] == '#')
    {
        canPlaceHere = false;
    }
    else if (beginningOfRun - 1 >= 0 && arrangementTemplate[beginningOfRun - 1] == '#')
    {
        canPlaceHere = false;
    }
    else
    {
        for (int i = beginningOfRun; i <= endOfRun; i++)
        {
            if (arrangementTemplate[i] == '.')
            {
                canPlaceHere = false;
                break;
            }
        }
    }
    int64_t result = 0;
    if (canPlaceHere)
    {
        result += countValidArrangements(arrangementTemplate, requiredNumInGroup, endOfRun + 2, groupIndex + 1, lookup);
    }
    const bool mustPlaceHere = (arrangementTemplate[templatePos] == '#');
    if (!mustPlaceHere)
        result += countValidArrangements(arrangementTemplate, requiredNumInGroup, templatePos + 1, groupIndex, lookup);

    lookup.numArrangementsForPosAndGroupIndex[templatePos][groupIndex] = result;
    return result;
}

int64_t countValidArrangements(const string& arrangementTemplate, const vector<int>& requiredNumInGroup)
{
    Lookup lookup;
    lookup.numArrangementsForPosAndGroupIndex = vector<vector<int64_t>>(arrangementTemplate.size() + 1, vector<int64_t>(requiredNumInGroup.size() + 1, -1));
    lookup.numArrangementsForPosAndGroupIndex[arrangementTemplate.size()][requiredNumInGroup.size()] = 1;

    lookup.spaceNeededForGroupIndex = vector<int>(requiredNumInGroup.size() + 1, 0);
    int spaceNeeded = 0;
    for (int groupIndex = requiredNumInGroup.size() - 1; groupIndex >= 0; groupIndex--)
    {
        spaceNeeded += requiredNumInGroup[groupIndex];
        if (groupIndex != requiredNumInGroup.size() - 1)
            spaceNeeded++;
        lookup.spaceNeededForGroupIndex[groupIndex] = spaceNeeded;
    }

    return countValidArrangements(arrangementTemplate, requiredNumInGroup, 0, 0, lookup);
}

int main()
{
    string arrangementLine;
    int64_t result = 0;
    while (getline(cin, arrangementLine))
    {
        istringstream arrangementStream(arrangementLine);
        string origArrangementTemplate;
        arrangementStream >> origArrangementTemplate;

        vector<int> origNumInGroup;
        while (true)
        {
            int num = -1;
            arrangementStream >> num;
            assert (arrangementStream);
            origNumInGroup.push_back(num);
            char comma;
            arrangementStream  >> comma;
            if (arrangementStream)
                assert(comma == ',');
            else
                break;
        }
        
        string arrangementTemplate;
        vector<int> numInGroup;
        constexpr auto numTimesToDuplicateArrangement = 5;
        for (int i = 1; i <= numTimesToDuplicateArrangement; i++)
        {
            arrangementTemplate.insert(arrangementTemplate.end(), origArrangementTemplate.begin(), origArrangementTemplate.end());
            if (i != numTimesToDuplicateArrangement)
                arrangementTemplate.insert(arrangementTemplate.end(), '?');
            numInGroup.insert(numInGroup.begin(), origNumInGroup.begin(), origNumInGroup.end());
        }

        std::cout << "arrangementTemplate: " << arrangementTemplate << " num in groups: " << std::endl;
        for (const auto x : numInGroup) cout << " " << x;
        std::cout << std::endl;

        const int64_t numValidArrangemnts = countValidArrangements(arrangementTemplate, numInGroup);
        std::cout << "numValidArrangemnts: " << numValidArrangemnts << std::endl;
        result += numValidArrangemnts;
    }
    std::cout << "result: " << result << std::endl;
}
