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

#if 0
    for (const auto& rawMapLine : rawMapData)
    {
        std::cout << rawMapLine << std::endl;
    }
#endif


    const int width = std::ranges::max(rawMapData | std::ranges::views::transform([](const auto& line) { return line.size(); }));
    std::cout << "width: " << width << std::endl;
    const int height = rawMapData.size();
    const int startX = rawMapData.front().find('.');

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
    vector<vector<char>> pathDisplayMap = map;

    string path;
    cin >> path;

    std::cout << "path: " << path << std::endl;
    std::cout << "startX: " << startX << std::endl;

    istringstream pathStream(path);
    int x = startX;
    int y = 0;
    int direction = 0; // Facing right.

    auto markPositionOnDisplayMap = [&pathDisplayMap, &x, &y, &direction]()
    {
        char directionDisplayChar = '\0';
        switch (direction)
        {
            case 0:
                directionDisplayChar = '>';
                break;
            case 1:
                directionDisplayChar = 'V';
                break;
            case 2:
                directionDisplayChar = '<';
                break;
            case 3:
                directionDisplayChar = '^';
                break;
        };
        assert(directionDisplayChar != '\0');
        pathDisplayMap[x][y] = directionDisplayChar;
    };

    auto printDisplayMap = [&pathDisplayMap, width, height]()
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                std::cout << pathDisplayMap[x][y];
            }
            std::cout << std::endl;
        }
    };

    markPositionOnDisplayMap();
    printDisplayMap();
    while (true)
    {
        int numToMove = 0;
        pathStream >> numToMove;
        if (!pathStream)
            break;
        std::cout << "Forward " << numToMove << std::endl;
        for (int step = 0; step < numToMove; step++)
        {
            const int origX = x;
            const int origY = y;
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
            }
            else
            {
                printDisplayMap();
                std::cout << "Ended up at: " << x << ", " << y << " blah: " << map[x][y] << " width: " << width << " height: " << height << std::endl;
                assert(map[x][y] == '.');
                markPositionOnDisplayMap();
            }
        }
        std::cout << "Now at: " << x << ", " << y << " facing: " << direction << std::endl;
        char turnDir = '\0';
        pathStream >> turnDir; 
        if (!turnDir)
            break;
        assert((turnDir == 'L') || (turnDir == 'R'));
        std::cout << "Turn: " << turnDir << std::endl;
        if (turnDir == 'R')
            direction = (direction + 1) % 4;
        else
            direction = (4 + direction - 1) % 4;
        std::cout << "Now at: " << x << ", " << y << " facing: " << direction << std::endl;
        markPositionOnDisplayMap();
    }

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            std::cout << pathDisplayMap[x][y];
        }
        std::cout << std::endl;
    }

    std::cout << "password: " << ((y + 1) * 1000) + ((x + 1) * 4) + direction << std::endl;


}

