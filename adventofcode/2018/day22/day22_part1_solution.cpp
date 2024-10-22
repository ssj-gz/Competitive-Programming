#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.
#include <iostream>
#include <regex>
#include <map>

#include <cassert>

using namespace std;

constexpr int erosionModulus = 20'183;

struct Coord
{
    int64_t x = -1;
    int64_t y = -1;
    auto operator<=>(const Coord& other) const = default;
};

int calcErosionLevel(const Coord& coord, map<Coord, int64_t>& lookup, const Coord& targetCoord, const int64_t depth)
{
    if (lookup.contains(coord))
        return lookup[coord];

    auto erosionLevelForGeologicIndex = [depth](int64_t geologicIndex)
    {
        geologicIndex %= erosionModulus;
        return (geologicIndex + depth) % erosionModulus;
    };

    if (coord == Coord{0, 0})
        return erosionLevelForGeologicIndex(0);
    if (coord == targetCoord)
        return erosionLevelForGeologicIndex(0);

    if (coord.y == 0)
        return erosionLevelForGeologicIndex(coord.x * 16807);

    if (coord.x == 0)
        return erosionLevelForGeologicIndex(coord.y * 48271);


    const int64_t result = erosionLevelForGeologicIndex((calcErosionLevel({coord.x - 1, coord.y}, lookup, targetCoord, depth) *
        calcErosionLevel({coord.x, coord.y - 1}, lookup, targetCoord, depth)));
    lookup[coord] = result;
    return result;
}


int main()
{
    string depthDescription;
    getline(cin, depthDescription);
    std::smatch regexMatch;
    int64_t depth = -1;
    if (std::regex_match(depthDescription, regexMatch, std::regex(R"(^depth:\s*(\d+)\s*$)")))
    {
        depth = std::stoll(regexMatch[1]);
    }
    else
    {
        assert(false);
    }

    string targetCoordDescription;
    getline(cin, targetCoordDescription);
    int64_t targetX = -1;
    int64_t targetY = -1;

    if (std::regex_match(targetCoordDescription, regexMatch, std::regex(R"(^target:\s*(\d+)\s*,\s*(\d+)\s*$)")))
    {
        targetX = std::stoll(regexMatch[1]);
        targetY = std::stoll(regexMatch[2]);
    }
    else
    {
        assert(false);
    }
    std::cout << "depth: " << depth << " targetX: " << targetX << " targetY: " << targetY << std::endl;
    const int64_t width = targetX + 1;
    const int64_t height = targetY + 1;

    vector<vector<int64_t>> erosionLevel(width, vector<int64_t>(height));
    map<Coord, int64_t> geologicIndexLookup;

    int64_t riskLevel = 0;
    for (int64_t x = 0; x <= targetX; x++)
    {
        for (int64_t y = 0; y <= targetY; y++)
        {
            erosionLevel[x][y] = calcErosionLevel({x, y}, geologicIndexLookup, {targetX, targetY}, depth);
            riskLevel += (erosionLevel[x][y] % 3);
            std::cout << "x: " << x << " y: " << y << " erosionLevel: " << erosionLevel[x][y] << std::endl;
        }
    }
    std::cout << "riskLevel: " << riskLevel << std::endl;
    return 0;
}
