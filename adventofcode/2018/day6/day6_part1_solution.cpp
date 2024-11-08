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

    auto closestCoordinate = [&coords](const Coord& coord)
    {
        int closestCoordDistance = std::numeric_limits<int>::max();
        Coord closestCoord;
        for (const auto& otherCoord : coords)
        {
            const int dist = abs(otherCoord.x - coord.x) + abs(otherCoord.y - coord.y);
            if (dist < closestCoordDistance)
            {
                closestCoord = otherCoord;
                closestCoordDistance = dist;
            }
            else if (dist == closestCoordDistance)
            {
                closestCoord = {-1, -1};
            }
        }
        return closestCoord;
    };

    int largestArea = -1;
    for (const auto& centreCoord : coords)
    {
        std::cout << "Looking at centreCoord: " << centreCoord.x << "," << centreCoord.y << std::endl;
        int areaSize = 1;
        int radius = 1;
        while (true)
        {
            std::cout << " radius: " << radius << std::endl;
            vector<Coord> perimeterCoords;
            for (int x = centreCoord.x - radius; x < centreCoord.x + radius; x++)
            {
                perimeterCoords.push_back({x, centreCoord.y - radius});
            }
            assert(perimeterCoords.size() == 2 * radius);
            for (int y = centreCoord.y - radius; y < centreCoord.y + radius; y++)
            {
                perimeterCoords.push_back({centreCoord.x + radius, y});
            }
            for (int x = centreCoord.x + radius; x > centreCoord.x - radius; x--)
            {
                perimeterCoords.push_back({x, centreCoord.y + radius});
            }
            for (int y = centreCoord.y + radius; y > centreCoord.y - radius; y--)
            {
                perimeterCoords.push_back({centreCoord.x - radius, y});
            }
            assert(static_cast<int>(perimeterCoords.size()) == 8 * radius);
            assert(set<Coord>(perimeterCoords.begin(), perimeterCoords.end()).size() == perimeterCoords.size());

            bool hasPerimeterCellClosestToCentre = false;
            bool escapedToInfinity = false;
            for (const auto& perimeterCell : perimeterCoords)
            {
                //std::cout << " perimeterCell: " << perimeterCell << std::endl;
                //if (perimeterCell.x < minX || perimeterCell.x > maxX || perimeterCell.y < minY || perimeterCell.y > maxY)
                //{
                //    escapedToInfinity = true;
                //    break;
                //}
                const auto closest = closestCoordinate(perimeterCell);
                //std::cout << "  closest: " << closest << std::endl;

                if (closest == centreCoord)
                {
                    if (perimeterCell.x < minX || perimeterCell.x > maxX || perimeterCell.y < minY || perimeterCell.y > maxY)
                    {
                        escapedToInfinity = true;
                        break;
                    }
                    else
                    {
                        hasPerimeterCellClosestToCentre = true;
                        areaSize++;
                    }
                }
            }

            if (escapedToInfinity)
            {
                std::cout << " area around cell: " << centreCoord << " is infinite" << std::endl;
                break;
            }

            if (!hasPerimeterCellClosestToCentre)
            {
                std::cout << " area around cell: " << centreCoord << " is " << areaSize << std::endl;
                largestArea = std::max(largestArea, areaSize);
                break;
            }

            radius++;
        }
    }
    std::cout << "largestArea: " << largestArea << std::endl;
    return 0;
}
