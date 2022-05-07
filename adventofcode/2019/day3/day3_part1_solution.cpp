#include <iostream>
#include <sstream>
#include <set>
#include <limits>
#include <cassert>

using namespace std;

struct Coord
{
    int x = -1;
    int y = -1;
    auto operator<=>(const Coord& other) const = default;
};

set<Coord> getWireCoords(const string& wireDirections)
{
    istringstream wireDirectionsStream(wireDirections);;

    Coord currentCoord{0, 0};
    set<Coord> wireCoords = {currentCoord};

    while (true)
    {

        char direction;
        wireDirectionsStream >> direction;
        assert(wireDirectionsStream);
        int numSteps;
        wireDirectionsStream >> numSteps;
        assert(wireDirectionsStream);

        int dx = 0;
        int dy = 0;
        switch (direction)
        {
            case 'R':
                dx = 1;
                break;
            case 'L':
                dx = -1;
                break;
            case 'U':
                dy = -1;
                break;
            case 'D':
                dy = 1;
                break;
        }
        for (int i = 0; i < numSteps; i++)
        {
            currentCoord.x += dx;
            currentCoord.y += dy;
            wireCoords.insert(currentCoord);
        }

        char comma;
        wireDirectionsStream >> comma;
        if (!wireDirectionsStream)
            break;
        assert(comma == ',');
    }

    return wireCoords;
};

int main()
{
    string wire1Directions;
    getline(cin, wire1Directions);
    const auto wire1Coords = getWireCoords(wire1Directions);
    string wire2Directions;
    getline(cin, wire2Directions);
    const auto wire2Coords = getWireCoords(wire2Directions);

    int64_t closestOverLapDistance = std::numeric_limits<int64_t>::max();
    for (const auto wire2Coord : wire2Coords)
    {
        if (wire2Coord == Coord{0, 0})
            continue;
        if (wire1Coords.contains(wire2Coord))
        {
            const int64_t overlapDistance = abs(wire2Coord.x) + abs(wire2Coord.y);
            closestOverLapDistance = min(closestOverLapDistance, overlapDistance);
        }
    }
    assert(closestOverLapDistance != std::numeric_limits<int64_t>::max());
    cout << closestOverLapDistance << endl;
}
