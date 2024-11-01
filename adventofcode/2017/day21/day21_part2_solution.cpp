#define _GLIBCXX_DEBUG       // Iterator safety; out-of-bounds access for Containers, etc.
#include <iostream>
#include <map>
#include <vector>
#include <optional>
#include <sstream>
#include <limits>
#include <algorithm>

#include <cassert>

using namespace std;

vector<vector<char>> resultOfApplyingRule(const vector<vector<char>>& startConfigOrig, map<string, string>& ruleLookup)
{

    vector<vector<char>> startConfig = startConfigOrig;
    const int size = startConfigOrig.size();
#if 0
    std::cout <<"resultOfApplyingRule called with startConfigOrig: " << std::endl;
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            std::cout << startConfigOrig[x][y];
        }
        std::cout << std::endl;
    }
#endif
    std::optional<vector<vector<char>>> result;
    for (int flipNum = 0; flipNum < 2; flipNum++)
    {
        // Try all rotations.
        for (int rotationNum = 0; rotationNum < 4; rotationNum++)
        {
            vector<vector<char>> rotatedConfig(size, vector<char>(size, ' '));
            for (int x = 0; x < size; x++)
            {
                for (int y = 0; y < size; y++)
                {
                    rotatedConfig[y][size - 1 - x] = startConfig[x][y];
                }

            }
            startConfig = rotatedConfig;
            // We have one of the possible rotation+flips of startConfigOrig; does it
            // have a corresponding endConfig?
            string startConfigFlatterned;
            for (int y = 0; y < size; y++)
            {
                for (int x = 0; x < size; x++)
                {
                    startConfigFlatterned += startConfig[x][y];
                }
            }
            //std::cout << " startConfigFlatterned: " << startConfigFlatterned << std::endl;
            if (ruleLookup.contains(startConfigFlatterned))
            {
                // Yes! Retrieve and unflattern it.
                const auto endConfigFlatterned = ruleLookup[startConfigFlatterned];
                const int endConfigSize = size + 1;
                assert(static_cast<int>(endConfigFlatterned.size()) == (endConfigSize * endConfigSize));
                vector<vector<char>> endConfig(endConfigSize, vector<char>(endConfigSize, ' '));
                for (int y = 0; y < endConfigSize; y++)
                {
                    for (int x = 0; x < endConfigSize; x++)
                    {
                        endConfig[x][y] = endConfigFlatterned[x + y * endConfigSize];
                    }
                }
#if 0
                std::cout << " got matching endConfig: " << std::endl;
                for (int y = 0; y < endConfigSize; y++)
                {
                    for (int x = 0; x < endConfigSize; x++)
                    {
                        std::cout << endConfig[x][y];
                    }
                    std::cout << std::endl;
                }
#endif
                assert(!result.has_value() || result.value() == endConfig);
                result = endConfig;
            }
        }

        // Flip.
        for (int x = 0; x < size; x++)
        {
            std::reverse(startConfig[x].begin(), startConfig[x].end());
        }

    }

    assert(result.has_value());
    return result.value();
}

int main()
{
    string ruleLine;
    map<string, string> ruleLookup;
    while (getline(cin, ruleLine))
    {
        istringstream ruleStream{ruleLine};
        string startConfig;
        ruleStream >> startConfig;
        std::cout << " startConfig: " << startConfig << std::endl;
        string dummy;
        ruleStream >> dummy;
        std::cout << " dummy: " << dummy << std::endl;
        assert(dummy == "=>");
        string endConfig;
        ruleStream >> endConfig;
        std::cout << " endConfig: " << endConfig << std::endl;
        assert(ruleStream);

        startConfig.erase(std::remove(startConfig.begin(), startConfig.end(), '/'), startConfig.end());
        endConfig.erase(std::remove(endConfig.begin(), endConfig.end(), '/'), endConfig.end());
        assert((startConfig.size() == 4 && endConfig.size() == 9) || ((startConfig.size() == 9 && endConfig.size() == 16)));
        ruleLookup[startConfig] = endConfig;
    }
    struct Coord
    {
        int x = -1;
        int y = -1;
        auto operator<=>(const Coord& other) const = default;
    };

    const string startPattern[] = { 
        ".#.",
        "..#",
        "###" };

    map<Coord, bool> grid;
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 3; y++)
        {
            grid[{x, y}] = (startPattern[y][x] == '#');
        }
    }

    auto createNextGrid = [&grid, &ruleLookup](const int currentGridSize, const int startConfigTileSize)
    {
        int minX = std::numeric_limits<int>::max();
        int maxX = std::numeric_limits<int>::min();
        int minY = std::numeric_limits<int>::max();
        int maxY = std::numeric_limits<int>::min();
        for (const auto [coord, isOn] : grid)
        {
            minX = std::min(minX, coord.x);
            maxX = std::max(maxX, coord.x);
            minY = std::min(minY, coord.y);
            maxY = std::max(maxY, coord.y);
        }

        map<Coord, bool> nextGrid;

        for (int tileX = 0; tileX < currentGridSize / startConfigTileSize; tileX++)
        {
            for (int tileY = 0; tileY < currentGridSize / startConfigTileSize; tileY++)
            {
                // Read the startConfig tile from grid.
                vector<vector<char>> tileStartConfig(startConfigTileSize, vector<char>(startConfigTileSize, ' '));
                {
                    const int tileStartX = minX + tileX * startConfigTileSize;
                    for (int x = tileStartX; x < tileStartX + startConfigTileSize; x++)
                    {
                        const int tileStartY = minY + tileY * startConfigTileSize;
                        for (int y = tileStartY; y < tileStartY + startConfigTileSize; y++)
                        {
                            tileStartConfig[x - tileStartX][y - tileStartY] = (grid[{x,y}] ? '#' : '.');
                        }
                    }
                }
                const auto tileEndConfig = resultOfApplyingRule(tileStartConfig, ruleLookup);
                const int endConfigTileSize = startConfigTileSize + 1;
                assert(tileEndConfig.size() == endConfigTileSize);
                assert(tileEndConfig.front().size() == endConfigTileSize);
                // Write the endConfig tile to nextGrid.
                {
                    const int tileStartX = minX + tileX * endConfigTileSize;
                    for (int x = tileStartX; x < tileStartX + endConfigTileSize; x++)
                    {
                        const int tileStartY = minY + tileY * endConfigTileSize;
                        for (int y = tileStartY; y < tileStartY + endConfigTileSize; y++)
                        {
                            nextGrid[{x, y}] = (tileEndConfig[x - tileStartX][y - tileStartY] == '#');

                        }
                    }
                }
            }

        }
        return nextGrid;
    };

    auto printGrid = [&grid]()
    {
        int minX = std::numeric_limits<int>::max();
        int maxX = std::numeric_limits<int>::min();
        int minY = std::numeric_limits<int>::max();
        int maxY = std::numeric_limits<int>::min();
        for (const auto [coord, isOn] : grid)
        {
            minX = std::min(minX, coord.x);
            maxX = std::max(maxX, coord.x);
            minY = std::min(minY, coord.y);
            maxY = std::max(maxY, coord.y);
        }
        for (int y = minY; y <= maxY; y++)
        {
            for (int x = minX; x <= maxX; x++)
            {
                std::cout << (grid[{x, y}] ? '#' : '.');

            }
            std::cout << std::endl;
        }
    };

    auto gridSize =[&grid]()
    {
        int minX = std::numeric_limits<int>::max();
        int maxX = std::numeric_limits<int>::min();
        int minY = std::numeric_limits<int>::max();
        int maxY = std::numeric_limits<int>::min();
        for (const auto [coord, isOn] : grid)
        {
            minX = std::min(minX, coord.x);
            maxX = std::max(maxX, coord.x);
            minY = std::min(minY, coord.y);
            maxY = std::max(maxY, coord.y);
        }
        return maxX - minX + 1;
    };

    for (int count = 1; count <= 18; count++)
    {
        std::cout << "Beginning iteration: " << count << std::endl;
        const int size = gridSize();
        std::cout << "size: "<< size << std::endl;
        if ((size % 2) == 0)
        {
            const int startConfigTileSize = 2;
            grid = createNextGrid(size, startConfigTileSize);
        }
        else if ((size % 3) == 0)
        {
            const int startConfigTileSize = 3;
            grid = createNextGrid(size, startConfigTileSize);
        }
        else 
            assert(false);

    }
    std::cout << "final config: " << std::endl;

    int numOn = 0;
    for (const auto [coord, isOn] : grid)
    {
        if (isOn)
            numOn++;
    }
    std::cout << "numOn: " << numOn << std::endl;
    return 0;
}
