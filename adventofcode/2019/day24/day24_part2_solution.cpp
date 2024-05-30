#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <cstdint>
#include <cassert>

using namespace std;

struct Coord
{
    int x = -1;
    int y = -1;
    auto operator<=>(const Coord& other) const = default;
};

ostream& operator<<(ostream& os, const Coord& coord)
{
    os << "(" << coord.x << "," << coord.y << ")";
    return os;
}

int main()
{
    vector<string> area;
    string row;
    while (std::getline(cin, row))
    {
        area.push_back(row);
    }
    area[2][2] = '?';

    const int areaWidth = area.front().size();
    const int areaHeight = area.size();

    struct Cell
    {
        Coord coord;
        int levelOffset = 0;
        auto operator<=>(const Cell& other) const = default;
    };

    // Build a lookup of adjacent cells for all cells on Level 0.
    // Cells on other levels can use this with minor changes.
    vector<vector<vector<Cell>>> adjacentCellsForCell(areaWidth, vector<vector<Cell>>(areaHeight));

    // Adjacencies on the same level.
    for (int x = 0; x < areaWidth; x++)
    {
        for (int y = 0; y < areaHeight; y++)
        {
            if (x == 2 && y == 2)
            {
                // Skip the middle cell, which is what we recurse into.
                continue;
            }
            const int directions[][2] = { {-1, 0}, { 1, 0}, { 0, -1}, {0, 1} };
            for (const auto dxdy : directions )
            {
                // Adjacencies on same level.
                const int adjacentX = x + dxdy[0];
                const int adjacentY = y + dxdy[1];
                if (adjacentX < 0 || adjacentX >= areaWidth)
                    continue;
                if (adjacentY < 0 || adjacentY >= areaHeight)
                    continue;
                if (adjacentX == 2 && adjacentY == 2)
                {
                    // Skip the middle cell, which is what we recurse into.
                    continue;
                }

                adjacentCellsForCell[x][y].push_back({{adjacentX, adjacentY}, 0});
            }
        }
    }
    // Adjacencies going upwards/ outwards i.e. to level -1.
    // Horizontal edges.
    for (int y = 0; y < areaHeight; y++)
    {
        adjacentCellsForCell[0][y].push_back({{1, 2}, -1});
        adjacentCellsForCell[areaWidth - 1][y].push_back({{3, 2}, -1});
    }
    // Vertical edges.
    for (int x = 0; x < areaWidth; x++)
    {
        adjacentCellsForCell[x][0].push_back({{2, 1}, -1});
        adjacentCellsForCell[x][areaHeight - 1].push_back({{2, 3}, -1});
    }
    // Adjacencies going downwards/ inwards i.e. to level +1.
    // Horizontal edges.
    for (int y = 0; y < areaHeight; y++)
    {
        adjacentCellsForCell[1][2].push_back({{0, y}, +1});
        adjacentCellsForCell[3][2].push_back({{areaWidth - 1, y}, +1});
    }
    // Vertical edges.
    for (int x = 0; x < areaWidth; x++)
    {
        adjacentCellsForCell[2][1].push_back({{x, 0}, +1});
        adjacentCellsForCell[2][3].push_back({{x, areaHeight - 1}, +1});
    }

    // Should be no duplicate adjacencies.
    for (int x = 0; x < areaWidth; x++)
    {
        for (int y = 0; y < areaHeight; y++)
        {
            assert(adjacentCellsForCell[y][x].size() == std::set(adjacentCellsForCell[y][x].begin(), adjacentCellsForCell[y][x].end()).size());
        }
    }

    vector<string> emptyArea(areaHeight, string(areaWidth, '.'));
    emptyArea[2][2] = '?';
    int maxLevelToCheck = 1;
    int minLevelToCheck = -1;
    std::map<int, vector<string>> areaForLevel = {{ minLevelToCheck, emptyArea}, {0, area}, {maxLevelToCheck, emptyArea}};
    int time = 0;
    while (true)
    {
        assert(areaForLevel[minLevelToCheck] == emptyArea);
        assert(areaForLevel[maxLevelToCheck] == emptyArea);

        std::map<int, vector<string>> newAreaForLevel(areaForLevel);
        for (int level = minLevelToCheck; level <= maxLevelToCheck; level++)
        {
            const auto& area = areaForLevel[level];
            if (!areaForLevel.contains(level - 1))
                areaForLevel[level - 1] = emptyArea;
            if (!areaForLevel.contains(level + 1))
                areaForLevel[level + 1] = emptyArea;
            for (int x = 0; x < areaWidth; x++)
            {
                for (int y = 0; y < areaHeight; y++)
                {
                    if (x == 2 && y == 2)
                        continue;
                    int numAdjacentBugs = 0;
                    for (const auto& adjacentCell : adjacentCellsForCell[x][y])
                    {
                        if (areaForLevel[adjacentCell.levelOffset + level][adjacentCell.coord.y][adjacentCell.coord.x] == '#')
                        {
                            numAdjacentBugs++;
                        }
                    }
                    if (area[y][x] == '#')
                    {
                        if (numAdjacentBugs != 1)
                        {
                            newAreaForLevel[level][y][x] = '.';
                        } 
                    }
                    else if (area[y][x] == '.')
                    {
                        if ((numAdjacentBugs == 1) || (numAdjacentBugs == 2))
                        {
                            newAreaForLevel[level][y][x] = '#';
                        }
                    }
                    else
                    {
                        assert(false);
                    }


                }
            }
        }
        areaForLevel = newAreaForLevel;
        if (areaForLevel[minLevelToCheck] != emptyArea) {
            minLevelToCheck--;
            areaForLevel[minLevelToCheck] = emptyArea;
        }
        if (areaForLevel[maxLevelToCheck] != emptyArea) {
            maxLevelToCheck++;
            areaForLevel[maxLevelToCheck] = emptyArea;
        }

        time++;
        if (time == 200)
        {
            int64_t numBugs = 0;
            for (const auto& [level, area] : areaForLevel)
            {
                for (const auto& row : area)
                {
                    numBugs += std::count(row.begin(), row.end(), '#');
                }
            }
            std::cout << "numBugs: " << numBugs << std::endl; 
            break;
        }
    }
}
