#include <iostream>
#include <map>
#include <regex>

#include <cassert>

using namespace std;

int main()
{
    std::regex claimRegex(R"(^#(\d+)\s*@\s*(\d+),(\d+):\s*(\d+)x(\d+)$)");
    string claimLine;
    struct Coord
    {
        int64_t x = -1;
        int64_t y = -1;
        auto operator<=>(const Coord& other) const = default;
    };
    std::map<Coord, int> numClaimsOnCoord;
    while (getline(cin, claimLine))
    {
        std::cout << "claimLine: " << claimLine << std::endl;
        std::smatch claimMatch;
        const bool matchSuccessful = std::regex_match(claimLine, claimMatch, claimRegex);
        assert(matchSuccessful);
        const int64_t tlX = stoll(claimMatch[2]);
        const int64_t tlY = stoll(claimMatch[3]);
        const int64_t width = stoll(claimMatch[4]);
        const int64_t height = stoll(claimMatch[5]);
        for (int64_t x = tlX; x < tlX + width; x++)
        {
            for (int64_t y = tlY; y < tlY + height; y++)
            {
                numClaimsOnCoord[{x, y}]++;
            }
        }
    }

    int64_t numCoordsWithMultipleClaims = 0;
    for (const auto& [coord, numClaims] : numClaimsOnCoord)
    {
        if (numClaims > 1)
            numCoordsWithMultipleClaims++;
    }
    std::cout << numCoordsWithMultipleClaims << std::endl;
    return 0;
}
