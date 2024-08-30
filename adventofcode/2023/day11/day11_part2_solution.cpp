#include <iostream>
#include <vector>
#include <set>

#include <cassert>

using namespace std;

const int64_t multiplier = 1'000'000;

int main()
{
    vector<string> debugMap;
    string mapLine;
    while (getline(cin, mapLine))
    {
        debugMap.push_back(mapLine);
    }
    const auto origMap = debugMap;
    struct Coord
    {
        int row = -1;
        int col = -1;
        auto operator<=>(const Coord& other) const = default;
    };
#if 0
    std::cout << "Original map: " << std::endl;
    for (const auto& row : debugMap) cout << row << std::endl;
    int width = debugMap.front().size();
    int height = debugMap.size();
    for (int row = 0; row < height; row++)
    {
        if (debugMap[row].find('#') == string::npos)
        {
            const int numRowsToAdd = multiplier - 1;
            for (int i = 0; i < numRowsToAdd; i++)
            {
                debugMap.insert(debugMap.begin() + row, string(width, '.'));
            }
            row += numRowsToAdd;
            height += numRowsToAdd;
        }
    }
    std::cout << "After inserting rows: " << std::endl;
    for (const auto& row : debugMap) cout << row << std::endl;
    for (int col = 0; col < width; col++)
    {
        bool hasGalaxyInColumn = false;
        for (int row = 0; row < height; row++)
        {
            if (debugMap[row][col] == '#')
            {
                hasGalaxyInColumn = true;
                break;
            }
        }
        if (!hasGalaxyInColumn)
        {
            std::cout << "inserting row" << std::endl;
            const int numColsToAdd = multiplier - 1;
            for (int row = 0; row < height; row++)
            {
                for (int i = 0; i < numColsToAdd; i++)
                {
                    debugMap[row].insert(debugMap[row].begin() + col, '.');
                }
            }
            col += numColsToAdd;
            width += numColsToAdd;
        }

    }
    std::cout << "After inserting cols: " << std::endl;
    for (const auto& row : debugMap) cout << row << std::endl;


    int64_t debugResult = 0;
    {
        vector<Coord> galaxyCoords;
        for (int col = 0; col < width; col++)
        {
            for (int row = 0; row < height; row++)
            {
                if (debugMap[row][col] == '#')
                    galaxyCoords.push_back({row, col});
            }
        }


        for (int galaxy1Index = 0; galaxy1Index < galaxyCoords.size(); galaxy1Index++)
        {
            for (int galaxy2Index = galaxy1Index + 1; galaxy2Index < galaxyCoords.size(); galaxy2Index++)
            {
                const auto galaxy1 = galaxyCoords[galaxy1Index];
                const auto galaxy2 = galaxyCoords[galaxy2Index];
                const int distance = abs(galaxy2.row - galaxy1.row) + abs(galaxy2.col - galaxy1.col);
                std::cout << "Debug Distance between galaxies: " << galaxy1Index << " and " << galaxy2Index << " = " << distance << std::endl;
                debugResult += distance;
            }
        }
    }
#endif

    const int width = origMap.front().size();
    const int height = origMap.size();
    int64_t result = 0;
    set<int> emptyRowIndices;
    for (int row = 0; row < height; row++)
    {
        if (origMap[row].find('#') == string::npos)
        {
            emptyRowIndices.insert(row);
        }
    }
    set<int> emptyColIndices;
    for (int col = 0; col < width; col++)
    {
        bool hasGalaxyInColumn = false;
        for (int row = 0; row < height; row++)
        {
            if (origMap[row][col] == '#')
            {
                hasGalaxyInColumn = true;
                break;
            }
        }
        if (!hasGalaxyInColumn)
        {
            emptyColIndices.insert(col);
        }
    }

    vector<Coord> galaxyCoords;
    for (int col = 0; col < width; col++)
    {
        for (int row = 0; row < height; row++)
        {
            if (origMap[row][col] == '#')
                galaxyCoords.push_back({row, col});
        }
    }


    for (int galaxy1Index = 0; galaxy1Index < galaxyCoords.size(); galaxy1Index++)
    {
        for (int galaxy2Index = galaxy1Index + 1; galaxy2Index < galaxyCoords.size(); galaxy2Index++)
        {
            const auto galaxy1 = galaxyCoords[galaxy1Index];
            const auto galaxy2 = galaxyCoords[galaxy2Index];
            std::cout << "Computing Distance between galaxies: " << galaxy1Index << " and " << galaxy2Index << std::endl;

            int64_t distance = 0;
            {
                int xStart = galaxy1.col;
                int xEnd = galaxy2.col;
                if (xEnd < xStart)
                    swap(xStart, xEnd);
                std::cout << " xStart: " << xStart << " xEnd: " << xEnd << std::endl;
                for (int x = xStart; x < xEnd; x++)
                {
                    //std::cout << "  x: " << x << std::endl;
                    if (emptyColIndices.contains(x))
                    {
                        //std::cout << "    adding: " << multiplier << std::endl;
                        distance += multiplier;
                    }
                    else
                    {
                        //std::cout << "    adding: " << 1 << std::endl;
                        distance++;
                    }
                }
            }
            {
                int yStart = galaxy1.row;
                int yEnd = galaxy2.row;
                if (yEnd < yStart)
                    swap(yStart, yEnd);
                std::cout << " yStart: " << yStart << " yEnd: " << yEnd << std::endl;
                for (int y = yStart; y < yEnd; y++)
                {
                    //std::cout << "  y: " << y << std::endl;
                    if (emptyRowIndices.contains(y))
                    {
                        //std::cout << "    adding: " << multiplier << std::endl;
                        distance += multiplier;
                    }
                    else
                    {
                        //std::cout << "    adding: " << 1 << std::endl;
                        distance++;
                    }
                }
            }
            std::cout << "Optimised Distance between galaxies: " << galaxy1Index << " and " << galaxy2Index << " = " << distance << std::endl;
            result += distance;
        }
    }
    //std::cout << "debugResult: " << debugResult << std::endl;
    std::cout << "result: " << result << std::endl;
    //assert(debugResult == result);
}

