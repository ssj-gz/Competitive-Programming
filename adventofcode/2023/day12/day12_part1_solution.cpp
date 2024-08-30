#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

#include <cassert>

using namespace std;

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

int64_t countValidArrangements(const string& arrangementTemplate, const vector<int>& requiredNumInGroup)
{
    std::cout << "countValidArrangements: " << arrangementTemplate << std::endl;
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

int main()
{
    string arrangementLine;
    int64_t result = 0;
    while (getline(cin, arrangementLine))
    {
        istringstream arrangementStream(arrangementLine);
        string arrangementTemplate;
        arrangementStream >> arrangementTemplate;

        vector<int> numInGroup;
        while (true)
        {
            int num = -1;
            arrangementStream >> num;
            assert (arrangementStream);
            numInGroup.push_back(num);
            char comma;
            arrangementStream  >> comma;
            if (arrangementStream)
                assert(comma == ',');
            else
                break;
        }
        

        std::cout << "arrangementTemplate: " << arrangementTemplate << " num in groups: " << std::endl;
        for (const auto x : numInGroup) cout << " " << x << std::endl;

        const int64_t numValidArrangemnts = countValidArrangements(arrangementTemplate, numInGroup);
        std::cout << " numValidArrangemnts: " << numValidArrangemnts << std::endl;
        result += numValidArrangemnts;
    }
    std::cout << "result: " << result << std::endl;
}
