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

        string colourRGB = digPlanMatch[3];
        const auto dirChar = colourRGB.back();

        // Remove dirChar and the leading "#" from colourRGB.
        colourRGB.erase(colourRGB.begin());
        colourRGB.pop_back();
        std::cout << "numToDig as hex: " << colourRGB << std::endl;
        int64_t numToDig = 0;
        for (const auto& hexDigit : colourRGB)
        {
            int digitalDecimal = 0;
            if (hexDigit >= '0' && hexDigit <= '9')
                digitalDecimal = hexDigit - '0';
            else if (hexDigit >= 'a' && hexDigit <= 'f')
                digitalDecimal = hexDigit - 'a' + 10;
            else
                assert(false);
            numToDig *= 16;
            numToDig += digitalDecimal;
        }

        std::cout << "dirChar: " << dirChar << " numToDig: " << numToDig << " colourRGB: " << colourRGB << std::endl;

        DigSection digSection;
        digSection.numToDig = numToDig;
        switch (dirChar)
        {
            case '0':
                digSection.direction = Right;
                break;
            case '1':
                digSection.direction = Down;
                break;
            case '2':
                digSection.direction = Left;
                break;
            case '3':
                digSection.direction = Up;
                break;
        }
        digPlan.push_back(digSection);
    }

    const int64_t resultOptimised = lagoonSizeOptimised(digPlan);
    std::cout << "result: " << resultOptimised << std::endl;


}
