#include "../shared/intcodecomputer.h"

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <limits>

using namespace std;

    struct Coord
    {
        int x = -1;
        int y = -1;
        auto operator<=>(const Coord&) const = default;
    };


void drawWorldMap(const Coord& currentCoord,  const map<Coord, char>& worldMap)
{
    int minX = currentCoord.x;
    int minY = currentCoord.y;
    int maxX = currentCoord.x;
    int maxY = currentCoord.y;
    for (const auto& [coord, contents] : worldMap)
    {
        minX = min(minX, coord.x);
        minY = min(minY, coord.y);
        maxX = max(maxX, coord.x);
        maxY = max(maxY, coord.y);
    }
    const int width = maxX - minX + 1;
    const int height = maxY - minY + 1;
    vector<string> worldDisplay(height, string(width, ' '));
    for (const auto& [coord, contents] : worldMap)
    {
        const int row = coord.y - minY;
        const int col = coord.x - minX;
        const char code = contents;
        assert(code != '\0');
        worldDisplay[row][col] = code;
    }

    worldDisplay[currentCoord.y - minY][currentCoord.x - minX] = 'D';
    worldDisplay[0 - minY][0 - minX] = 'X';

    for (const auto& worldRow : worldDisplay)
        cout << worldRow << endl;
}

void explore(const Coord& currentCoord,  map<Coord, char>& worldMap, IntCodeComputer& intCodeComputer, set<Coord>& visited)
{
    cout << "Explore: " << currentCoord.x << "," << currentCoord.y << endl;
    if (visited.contains(currentCoord))
    {
        cout << " already explored" << endl;
        // Already explored.
        return;
    }
    visited.insert(currentCoord);
    drawWorldMap(currentCoord, worldMap);
    for (int direction = 1; direction <= 4; direction++)
    {
        intCodeComputer.addInput(direction);
        const auto status = intCodeComputer.run();
        assert(status == IntCodeComputer::WaitingForInput);
        const auto& output = intCodeComputer.takeOutput();
        assert(output.size() == 1);

        Coord nextCoord{currentCoord};
        int oppositeDirection = -1;
        switch (direction)
        {
            case 1:
                // North.
                nextCoord.y--;
                oppositeDirection = 2;
                break;
            case 2:
                // South.
                nextCoord.y++;
                oppositeDirection = 1;
                break;
            case 3:
                // West.
                nextCoord.x--;
                oppositeDirection = 4;
                break;
            case 4:
                // East.
                nextCoord.x++;
                oppositeDirection = 3;
                break;
        }
        switch(output[0])
        {
            case 0:
                worldMap[nextCoord] = '#';
                break;
            case 1:
                worldMap[nextCoord] = '.';
                break;
            case 2:
                worldMap[nextCoord] = '*';
                break;
        }
        cout << "Looks like " << nextCoord.x << "," << nextCoord.y << " is a " << output[0] << endl;
        if (output[0] != 0)
        {
            explore(nextCoord, worldMap, intCodeComputer, visited);
            // Backtrack.
            intCodeComputer.addInput(oppositeDirection);
            const auto status = intCodeComputer.run();
            assert(status == IntCodeComputer::WaitingForInput);
            intCodeComputer.takeOutput();
        }
    }
}

int main()
{
    vector<int64_t> program;
    int64_t programInput;
    while (true)
    {
        cin >> programInput;
        program.push_back(programInput);
        assert(cin);
        char comma;
        cin >> comma;
        if (!cin)
            break;
        assert(comma == ',');
    }

    IntCodeComputer intCodeComputer(program);

    map<Coord, char> worldMap;
    set<Coord> visited;
    const Coord initialCoord{0, 0};

    // Build world map.
    explore(initialCoord, worldMap, intCodeComputer, visited);
    cout << "Final world map:" << endl;
    drawWorldMap(initialCoord, worldMap);

    // Find shortest path (BFS).
    visited.clear();
    vector<Coord> coordsToExplore = { initialCoord };
    int distanceTravelled = 0;
    int answer = -1;
    while (!coordsToExplore.empty() && answer == -1)
    {
        cout << "# coordsToExplore: " << coordsToExplore.size() << endl;
        vector<Coord> nextCoordsToExplore;
        for (const auto& coord : coordsToExplore)
        {
            const std::pair<int, int> directions[] =
            {
                { -1,  0 },
                { +1,  0 },
                {  0, -1 },
                {  0, +1 }
            };
            for (const auto& [dx, dy] : directions)
            {
                Coord nextCoord{coord};
                nextCoord.x += dx;
                nextCoord.y += dy;
                if (worldMap[nextCoord] == '*' && answer == -1)
                {
                    answer = distanceTravelled + 1;
                    break;
                }
                else if (worldMap[nextCoord] == '.')
                {
                    if (!visited.contains(nextCoord))
                    {
                        visited.insert(nextCoord);
                        nextCoordsToExplore.push_back(nextCoord);
                    }
                }
            }
        }
        coordsToExplore = nextCoordsToExplore;
        distanceTravelled++;
    }
    cout << "answer: " << answer << endl;

}

