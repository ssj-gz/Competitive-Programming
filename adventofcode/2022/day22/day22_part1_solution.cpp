#include <iostream>
#include <vector>
#include <sstream>
#include <ranges>
#include <algorithm>
#include <cassert>

using namespace std;

int main()
{
    vector<string> rawMapData;
    string rawMapLine;
    while (std::getline(cin, rawMapLine) && !rawMapLine.empty())
    {
        rawMapData.push_back(rawMapLine);
    }

    const int width = std::ranges::max(rawMapData | std::ranges::views::transform([](const auto& line) { return line.size(); }));
    const int height = rawMapData.size();
    const int startX = rawMapData.front().find('.');

    // Convert the map to a more convenient representation.
    vector<vector<char>> map(width, vector<char>(height, ' '));
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < std::min<int>(width, rawMapData[y].size()); x++)
        {
            {
                map[x][y] = rawMapData[y][x];
            }
        }
    }

    string path;
    cin >> path;

    int x = startX;
    int y = 0;
    int direction = 0; // Facing right.

    istringstream pathStream(path);
    while (true)
    {
        int numToMove = 0;
        pathStream >> numToMove;
        if (!pathStream)
            break;
        for (int step = 0; step < numToMove; step++)
        {
            const int origX = x;
            const int origY = y;
            // Proceed one step, taking into account
            // "walking off edge of the map".
            while (true)
            {
                switch (direction)
                {
                    case 0:
                        x++;
                        break;
                    case 1:
                        y++;
                        break;
                    case 2:
                        x--;
                        break;
                    case 3:
                        y--;
                        break;
                };
                if (x < 0)
                    x = width -1;
                if (x >= width)
                    x = 0;
                if (y < 0)
                    y = height - 1;
                if (y >= height)
                    y = 0;
                if (map[x][y] != ' ')
                    break;
            }
            assert(x >= 0 && x < width && y >= 0 && y < height);
            if (map[x][y] == '#')
            {
                x = origX;
                y = origY;
                break;
            }
            else
            {
                assert(map[x][y] == '.');
            }
        }
        char turnDir = '\0';
        pathStream >> turnDir; 
        if (!turnDir)
            break;
        assert((turnDir == 'L') || (turnDir == 'R'));
        if (turnDir == 'R')
            direction = (direction + 1) % 4;
        else
            direction = (4 + direction - 1) % 4;
    }

    std::cout << "password: " << ((y + 1) * 1000) + ((x + 1) * 4) + direction << std::endl;


}

