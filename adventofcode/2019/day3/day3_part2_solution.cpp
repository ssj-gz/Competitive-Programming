#include <iostream>
#include <sstream>
#include <map>
#include <limits>
#include <cassert>

using namespace std;

struct Coord
{
    int x = -1;
    int y = -1;
    auto operator<=>(const Coord& other) const = default;
};

map<Coord, int64_t> getStepsForWireCoords(const string& wireDirections)
{
    istringstream wireDirectionsStream(wireDirections);;

    Coord currentCoord{0, 0};
    int64_t totalSteps = 0;
    map<Coord, int64_t> stepsForWireCoords;
    stepsForWireCoords[currentCoord] = totalSteps;

    while (true)
    {

        char direction;
        wireDirectionsStream >> direction;
        assert(wireDirectionsStream);
        int64_t numSteps;
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
            totalSteps++;

            if (!stepsForWireCoords.contains(currentCoord))
                stepsForWireCoords[currentCoord] = totalSteps;
        }

        char comma;
        wireDirectionsStream >> comma;
        if (!wireDirectionsStream)
            break;
        assert(comma == ',');
    }

    return stepsForWireCoords;
};

int main()
{
    string wire1Directions;
    getline(cin, wire1Directions);
    auto stepsForWire1Coords = getStepsForWireCoords(wire1Directions);
    string wire2Directions;
    getline(cin, wire2Directions);
    auto stepsForWire2Coords = getStepsForWireCoords(wire2Directions);

    int64_t closestOverLapStepSum = std::numeric_limits<int64_t>::max();
    for (auto& [wire2Coord, numSteps] : stepsForWire2Coords)
    {
        if (wire2Coord == Coord{0, 0})
            continue;
        if (stepsForWire1Coords.contains(wire2Coord))
        {
            const int64_t overlapStepSum = stepsForWire1Coords[wire2Coord] + 
                                           stepsForWire2Coords[wire2Coord];
            closestOverLapStepSum = min(closestOverLapStepSum, overlapStepSum);
        }
    }
    assert(closestOverLapStepSum != std::numeric_limits<int64_t>::max());
    cout << closestOverLapStepSum << endl;
}
