#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

#include <cassert>

//#define BRUTE_FORCE

using namespace std;

#ifdef BRUTE_FORCE
bool isArrangementValid(const string& arrangement, const vector<int>& requiredNumInGroup)
{
    int requiredGroupIndex = 0;
    int numInGroup = -1;
    for (int pos = 0; pos < arrangement.size(); pos++)
    {
        if (numInGroup == -1)
        {
            if (arrangement[pos] == '#')
                numInGroup = 0;
        }
        if (numInGroup != -1)
        {
            if (arrangement[pos] == '#')
            {
                numInGroup++;
            }
            if (arrangement[pos] != '#' || pos == arrangement.size() - 1)
            {
                if (numInGroup != requiredNumInGroup[requiredGroupIndex])
                    return false;

                requiredGroupIndex++;
                numInGroup = -1;
            }

        }
    }
    return (requiredGroupIndex == requiredNumInGroup.size());
}

int64_t countValidArrangementsBruteForce(const string& arrangementTemplate, const vector<int>& requiredNumInGroup)
{
    //std::cout << "countValidArrangements: " << arrangementTemplate << std::endl;
    int64_t result = 0;
    const int numUnknowns = std::count(arrangementTemplate.begin(), arrangementTemplate.end(), '?');
        vector<string> jokerReplacements;
    vector<char> replacedUnknowns(numUnknowns, '.');
    while (true)
    {
        int replaceWithUnknownIndex = 0;
        string arrangement;
        for (int pos = 0; pos < arrangementTemplate.size(); pos++)
        {
            if (arrangementTemplate[pos] != '?')
                arrangement.push_back(arrangementTemplate[pos]);
            else
            {
                arrangement.push_back(replacedUnknowns[replaceWithUnknownIndex]);
                replaceWithUnknownIndex++;
            }
        }
        //std::cout << " arrangement: " << arrangement << std::endl;
        if (isArrangementValid(arrangement, requiredNumInGroup))
            result++;

        // Next.
        int unknownIndex = 0;
        while (unknownIndex < numUnknowns && replacedUnknowns[unknownIndex] == '#')
        {
            replacedUnknowns[unknownIndex] = '.';
            unknownIndex++;
        }
        if (unknownIndex == numUnknowns)
            break;
        replacedUnknowns[unknownIndex] = '#';
    }

    return result;
}

#endif

struct Lookup
{
    vector<vector<int64_t>> numArrangementsForPosAndGroupIndex;
    vector<int> spaceNeededForGroupIndex;

};

int64_t countValidArrangements(const string& arrangementTemplate, const vector<int>& requiredNumInGroup, int templatePos, int groupIndex, Lookup& lookup)
{
    //std::cout << " countValidArrangements: templatePos: " << templatePos << " groupIndex: " << groupIndex << std::endl;
    if (templatePos >= arrangementTemplate.size())
    {
        if (groupIndex == requiredNumInGroup.size())
            return 1;
        else
            return 0;
    }
    if (groupIndex == requiredNumInGroup.size())
    {
        int64_t result = 0;
        bool hasSuperfluousSprings = false;
        for (int i = templatePos; i < arrangementTemplate.size(); i++)
        {
            if (arrangementTemplate[i] == '#')
            {
                hasSuperfluousSprings = true;
                break;
            }
        }
        if (!hasSuperfluousSprings)
        {
            //std::cout << "   " << " no superflous spring" << std::endl;
            result = 1;
        }
        else
        {
            //std::cout << "   " << " superflous spring" << std::endl;
        }
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
    //std::cout << "  beginningOfRun: " << beginningOfRun << " endOfRun: " << endOfRun << std::endl;
    bool canPlaceHere = true;
    int64_t result = 0;
    assert(endOfRun < arrangementTemplate.size());
    if (endOfRun + 1 <= arrangementTemplate.size() && arrangementTemplate[endOfRun + 1] == '#')
    {
        //std::cout << "   can't place here as followed by '#'" << std::endl;
        canPlaceHere = false;
    }
    else if (beginningOfRun - 1 >= 0 && arrangementTemplate[beginningOfRun - 1] == '#')
    {
        //std::cout << "   can't place here as preceded by '#'" << std::endl;
        canPlaceHere = false;
    }
    else
    {
        for (int i = beginningOfRun; i <= endOfRun; i++)
        {
            if (arrangementTemplate[i] == '.')
            {
                canPlaceHere = false;
                //std::cout << "   can't place here due to '.' in the way" << std::endl;
                break;
            }
        }
    }
    //std::cout << "  canPlaceHere: " << canPlaceHere << std::endl;
    if (canPlaceHere)
    {
        result += countValidArrangements(arrangementTemplate, requiredNumInGroup, endOfRun + 2, groupIndex + 1, lookup);
    }
    if (arrangementTemplate[templatePos] != '#')
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

#ifdef BRUTE_FORCE
int64_t compareResults(const string& arrangementTemplate, const vector<int>& requiredNumInGroup)
{
    std::cout << "Compare results; arrangementTemplate: " << arrangementTemplate << " requiredNumInGroup: " << std::endl;
    for (const auto x : requiredNumInGroup) cout << " " << x << std::endl;
    const int64_t numValidArrangemnts = countValidArrangements(arrangementTemplate, requiredNumInGroup);
    std::cout << "numValidArrangemnts: " << numValidArrangemnts << std::endl;
    const int64_t numValidArrangemntsBruteForce = countValidArrangementsBruteForce(arrangementTemplate, requiredNumInGroup);
    std::cout << "numValidArrangemntsBruteForce: " << numValidArrangemntsBruteForce << std::endl;
    assert(numValidArrangemntsBruteForce == numValidArrangemnts);
    return numValidArrangemnts;
}
#endif

int main()
{
#ifdef BRUTE_FORCE
    if (false)
    {
        compareResults("?", {1});
        compareResults("#", {1});
        compareResults(".#", {1});
        compareResults("?#", {1});
        compareResults("????", {1,3});
        compareResults("?????", {1,3});
        compareResults("??????", {1,3});
        compareResults("????????????", {1,3});
        compareResults("?????#??????", {1});
        compareResults("?????#??????", {1,3});
        compareResults("?????#?????#", {1,3});
        compareResults("?????#?????#", {1,3,1});
        compareResults("?????#?????#", {1,3,1});
        compareResults("????????????", {1,1,1});
        compareResults("#??????????#", {1,1,1});
        compareResults("#?????#????#", {1,1,1});
        compareResults("#?????#????#", {1,6,1});

    }
#endif
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
        for (int i = 0; i < 5; i++)
        {
            arrangementTemplate.insert(arrangementTemplate.end(), origArrangementTemplate.begin(), origArrangementTemplate.end());
            if (i != 4)
                arrangementTemplate.insert(arrangementTemplate.end(), '?');
            numInGroup.insert(numInGroup.begin(), origNumInGroup.begin(), origNumInGroup.end());
        }

        std::cout << "arrangementTemplate: " << arrangementTemplate << " num in groups: " << std::endl;
        for (const auto x : numInGroup) cout << " " << x << std::endl;

        const int64_t numValidArrangemnts = countValidArrangements(arrangementTemplate, numInGroup);
        std::cout << "numValidArrangemnts: " << numValidArrangemnts << std::endl;
        result += numValidArrangemnts;
#ifdef BRUTE_FORCE
        const int64_t numValidArrangemntsBruteForce = countValidArrangementsBruteForce(arrangementTemplate, numInGroup);
        std::cout << "numValidArrangemntsBruteForce: " << numValidArrangemntsBruteForce << std::endl;
        assert(numValidArrangemntsBruteForce == numValidArrangemnts);
#endif

        //const int64_t numValidArrangemnts = countValidArrangements(arrangementTemplate, numInGroup);
        //std::cout << " numValidArrangemnts: " << numValidArrangemnts << std::endl;
        //result += numValidArrangemnts;
    }
    std::cout << "result: " << result << std::endl;
}
