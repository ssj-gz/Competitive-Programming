#include <iostream>
#include <regex>

#include <cassert>

using namespace std;

int main()
{
    std::regex boxDimsRegex(R"(^(\d+)x(\d+)x(\d+)$)");
    string boxDimsLine;
    int64_t totalPaperRequired = 0;
    while (getline(cin, boxDimsLine))
    {
        std::smatch boxDimsMatch;
        const auto matchSuccessful = std::regex_match(boxDimsLine, boxDimsMatch, boxDimsRegex);
        assert(matchSuccessful);

        const auto l = std::stoi(boxDimsMatch[1]);
        const auto w = std::stoi(boxDimsMatch[2]);
        const auto h = std::stoi(boxDimsMatch[3]);

        int smallestSideArea = l * w;
        smallestSideArea = std::min(smallestSideArea, w * h);
        smallestSideArea = std::min(smallestSideArea, l * h);


        const int surfaceArea = 2 * (l * w + w * h + h * l);
        std::cout << "surfaceArea: " << surfaceArea << std::endl;
        std::cout << "smallestSideArea: " << smallestSideArea << std::endl;
        const int paperRequired = surfaceArea + smallestSideArea;
        std::cout << "paper required: " << paperRequired << std::endl;

        totalPaperRequired += paperRequired;
    }
    std::cout << "totalPaperRequired: " << totalPaperRequired << std::endl;
    return 0;
}
