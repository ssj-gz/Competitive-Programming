#include <iostream>
#include <regex>
#include <map>
#include <limits>

#include <cassert>

using namespace std;

int main()
{
    std::regex pointRegex(R"(^position=<\s*([-0-9]+)\s*,\s*([-0-9]+)\s*> velocity=<\s*([-0-9]+)\s*,\s*([-0-9]+)\s*>$)");
    string pointLine;

    struct Point
    {
        int x = -1;
        int y = -1;
        int dx = -1;
        int dy = -1;
    };
    struct Coord
    {
        int x = -1;
        int y = -1;
        auto operator<=>(const Coord& other) const = default;
    };
    vector<Point> points;
    while (getline(cin, pointLine))
    {
        std::smatch pointMatch;
        const bool matchSuccessful = std::regex_match(pointLine, pointMatch, pointRegex);
        assert(matchSuccessful);
        Point point;
        point.x = std::stoi(pointMatch[1]);
        point.y = std::stoi(pointMatch[2]);
        point.dx = std::stoi(pointMatch[3]);
        point.dy = std::stoi(pointMatch[4]);
        points.push_back(point);
    }

    int prevHeight = -1;
    map<Coord, bool> prevHasPointAtCoord;
    int prevMinX;
    int prevMinY;
    int prevMaxX;
    int prevMaxY;
    while (true)
    {
        map<Coord, bool> hasPointAtCoord;
        int minX = std::numeric_limits<int>::max();
        int minY = std::numeric_limits<int>::max();
        int maxX = std::numeric_limits<int>::min();
        int maxY = std::numeric_limits<int>::min();
        for (auto& point : points)
        {
            hasPointAtCoord[{point.x, point.y}] = true;
            minX = std::min(minX, point.x);
            minY = std::min(minY, point.y);
            maxX = std::max(maxX, point.x);
            maxY = std::max(maxY, point.y);
        }
        const int height = maxY - minY + 1;
        std::cout << "height: " << (maxY - minY + 1) << std::endl;
        if (prevHeight != -1 && height > prevHeight)
        {
            for (int y = prevMinY; y <= prevMaxY; y++)
            {
                for (int x = prevMinX; x <= prevMaxX; x++)
                {
                    if (prevHasPointAtCoord[{x, y}])
                        std::cout << '#';
                    else
                        std::cout << '.';
                }
                std::cout << std::endl;
            }
            break;
        }

        for (auto& point : points)
        {
            point.x += point.dx;
            point.y += point.dy;
        }
        prevHeight = height;
        prevHasPointAtCoord = hasPointAtCoord;
        prevMinX = minX;
        prevMinY = minY;
        prevMaxX = maxX;
        prevMaxY = maxY;
    }

    return 0;
}
