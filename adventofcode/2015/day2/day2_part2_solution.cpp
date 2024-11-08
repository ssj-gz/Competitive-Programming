#include <iostream>
#include <regex>

#include <cassert>

using namespace std;

int main()
{
    std::regex boxDimsRegex(R"(^(\d+)x(\d+)x(\d+)$)");
    string boxDimsLine;
    int64_t totalRibbonRequired = 0;
    while (getline(cin, boxDimsLine))
    {
        std::smatch boxDimsMatch;
        const auto matchSuccessful = std::regex_match(boxDimsLine, boxDimsMatch, boxDimsRegex);
        assert(matchSuccessful);

        const auto l = std::stoi(boxDimsMatch[1]);
        const auto w = std::stoi(boxDimsMatch[2]);
        const auto h = std::stoi(boxDimsMatch[3]);

        int sidesInOrder[] = {l, w, h};
        std::sort(std::begin(sidesInOrder),std::end(sidesInOrder));
        const int shortestPerimeterLen = 2 * (sidesInOrder[0] + sidesInOrder[1]);
        const int64_t volume = l * w * h;

        const int64_t ribbonRequired = shortestPerimeterLen + volume;
        std::cout << "ribbonRequired: " << ribbonRequired << std::endl;

        totalRibbonRequired += ribbonRequired;
    }
    std::cout << "totalRibbonRequired: " << totalRibbonRequired << std::endl;
    return 0;
}
