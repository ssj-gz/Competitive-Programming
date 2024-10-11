#include <iostream>
#include <limits>
#include <regex>
#include <set>

#include <cassert>

using namespace std;

struct Coord
{
    int x = -1;
    int y = -1;
    auto operator<=>(const Coord& other) const = default;
};

ostream& operator<<(ostream& os, const Coord& other)
{
    os << "(" << other.x << ", " << other.y << ")";
    return os;
}

int main()
{
    std::regex coordRegex(R"(^(\d+)\s*,\s*(\d+)$)");
    string coordLine;
    vector<Coord> coords;
    int maxX = std::numeric_limits<int>::min();
    int maxY = std::numeric_limits<int>::min();
    int minX = std::numeric_limits<int>::max();
    int minY = std::numeric_limits<int>::max();
    while (getline(cin, coordLine))
    {
        std::smatch coordMatch;
        const bool matchSuccessful = std::regex_match(coordLine, coordMatch, coordRegex);
        assert(matchSuccessful);

        Coord coord = {std::stoi(coordMatch[1]), std::stoi(coordMatch[2])};
        maxX = std::max(maxX, coord.x);
        maxY = std::max(maxY, coord.y);
        minX = std::min(minX, coord.x);
        minY = std::min(minY, coord.y);
        coords.push_back(coord);
    }
    std::cout << "minX: " << minX << std::endl;
    std::cout << "minY: " << minY << std::endl;
    std::cout << "maxX: " << maxX << std::endl;
    std::cout << "maxY: " << maxY << std::endl;

    auto sumOfDistancesTo = [&coords](const Coord& targetCoord)
    {
        int64_t sumOfDistances = 0;
        for (const auto& coord : coords)
        {
            sumOfDistances += abs(coord.x - targetCoord.x) + abs(coord.y - targetCoord.y);
        }
        return sumOfDistances;
    };

    // Get some sensible, but not optimal, bounds.
    constexpr int64_t distanceBound = 10'000;
    //constexpr int64_t distanceBound = 32;
    Coord coord = { minX - 1, minY - 1 };
    while (sumOfDistancesTo(coord) <= distanceBound)
        coord.y--;
    const int beginY = coord.y;
    coord = { minX - 1, minY - 1 };
    while (sumOfDistancesTo(coord) <= distanceBound)
        coord.x--;
    const int beginX = coord.x;
    coord = { maxX + 1, maxY + 1 };
    while (sumOfDistancesTo(coord) <= distanceBound)
        coord.x++;
    const int endX = coord.x;
    coord = { maxX + 1, maxY + 1 };
    while (sumOfDistancesTo(coord) <= distanceBound)
        coord.y++;
    const int endY = coord.y;

    std::cout << "beginX: " << beginX << " beginY: " << beginY << " endX: " << endX << " endY: " << endY << std::endl;
    int64_t regionSize = 0;
    for (int x = beginX; x <= endX; x++)
    {
        for (int y = beginY; y <= endY; y++)
        {
            if (sumOfDistancesTo({x,y}) < distanceBound)
                regionSize++;
        }
    }
    std::cout << "regionSize: " << regionSize << std::endl;

    return 0;
}
