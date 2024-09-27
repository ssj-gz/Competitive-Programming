#include "calc_lagoon_size.h"

#include <iostream>
#include <vector>
#include <regex>

#include <cassert>

using namespace std;

int main()
{
    std::regex digPlanRegex(R"(^([URDL])\s*(\d+)\s*\((.*)\)\s*$)");
    string digPlanLine;

    vector<DigSection> digPlan;
    while (getline(cin, digPlanLine))
    {
        std::smatch digPlanMatch;

        const bool matchSuccessful = std::regex_match(digPlanLine, digPlanMatch, digPlanRegex);
        assert(matchSuccessful);

        const auto dirChar = std::string(digPlanMatch[1])[0];
        const int64_t numToDig = std::stoll(digPlanMatch[2]);
        const string colourRGB = digPlanMatch[3];

        DigSection digSection;
        digSection.numToDig = numToDig;
        digSection.colourRGB;
        switch (dirChar)
        {
            case 'U':
                digSection.direction = Up;
                break;
            case 'R':
                digSection.direction = Right;
                break;
            case 'D':
                digSection.direction = Down;
                break;
            case 'L':
                digSection.direction = Left;
                break;
        }
        digPlan.push_back(digSection);
    }

    const int64_t resultOptimised = lagoonSizeOptimised(digPlan);
    std::cout << "result: " << resultOptimised << std::endl;

}
