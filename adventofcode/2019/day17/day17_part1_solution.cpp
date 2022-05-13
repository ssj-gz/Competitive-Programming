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
    const auto status = intCodeComputer.run();
    assert(status == IntCodeComputer::Terminated);

    const auto output = intCodeComputer.takeOutput();

    vector<string> map;
    string currentLine;
    for (const auto ascii : output)
    {
        if (ascii == 10)
        {
            map.push_back(currentLine);
            currentLine.clear();
        }
        else 
            currentLine.push_back(static_cast<char>(ascii));
    }
    for (const auto& line : map)
    {
        cout << line << endl;
    }
    while(map.back().empty())
    {
        map.pop_back();
    }
    const int height = static_cast<int>(map.size());
    const int width = static_cast<int>(map[0].size());
    cout << "width: " << width << " height: " << height << endl;

    int alignmentParameterSum = 0;
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            if (map[row][col] != '#')
                continue;

            const std::pair<int, int> directions[] = 
            {
                { -1,  0},
                { +1,  0},
                {  0, -1},
                {  0, +1}
            };

            bool isIntersection = true;
            for (const auto& [dx, dy] : directions)
            {
                const int neighbourRow = row + dy;
                const int neighbourCol = col + dx;
                if (neighbourRow < 0 || neighbourRow >= height)
                    continue;
                if (neighbourCol < 0 || neighbourCol >= width)
                    continue;
                if (map[neighbourRow][neighbourCol] != '#')
                    isIntersection = false;
            }
            if(isIntersection)
            {
                map[row][col] = 'O';
                alignmentParameterSum += row * col;
            }

        }
    }
    for (const auto& line : map)
    {
        cout << line << endl;
    }
    cout << "alignmentParameterSum: " << alignmentParameterSum << endl;



}

