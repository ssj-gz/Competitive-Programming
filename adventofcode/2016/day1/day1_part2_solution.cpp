#include <iostream>
#include <algorithm>
#include <set>
#include <sstream>

#include <cassert>

using namespace std;

int main()
{
    string directionsList;
    std::getline(cin, directionsList);
    std::replace(directionsList.begin(), directionsList.end(), ',', ' ');
    istringstream directionsStream(directionsList);

    enum Direction { Up, Right, Down, Left};
    Direction direction = Up;
    struct Coord
    {
        int x = -1;
        int y = -1;
        auto operator<=>(const Coord& other) const = default;
    };

    Coord pos = {0, 0};
    set<Coord> visitedCoords = { pos };

    string directionDescr;
    while (directionsStream >> directionDescr)
    {
        if (directionDescr.front() == 'L')
        {
            direction = static_cast<Direction>((static_cast<int>(direction) + 3) % 4);
        } 
        else if (directionDescr.front() == 'R')
        {
            direction = static_cast<Direction>((static_cast<int>(direction) + 1) % 4);
        }
        else
            assert(false);

        std::cout << "directionDescr: " << directionDescr << std::endl;
        const int numSteps = std::stoi(directionDescr.substr(1));
        int dx = 0;
        int dy = 0;
        switch (direction)
        {
            case Up:
                dy = -1;
                break;
            case Right:
                dx = +1;
                break;
            case Down:
                dy = +1;
                break;
            case Left:
                dx = -1;
                break;
        }
        for (int step = 0; step < numSteps; step++)
        {
            pos.x += dx;
            pos.y += dy;
            std::cout << "pos: " << pos.x << ", " << pos.y << std::endl;
            if (visitedCoords.contains(pos))
            {
                std::cout << "distance: " << (abs(pos.x) + abs(pos.y)) << std::endl;
                return 0;
            }
            visitedCoords.insert(pos);
        }
    }

    return 0;
}
