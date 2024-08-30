#include <iostream>
#include <vector>
#include <set>

#include <cassert>

using namespace std;

const int64_t emptyLineDistMultiplier = 1'000'000;

int main()
{
    vector<string> map;
    string mapLine;
    while (getline(cin, mapLine))
    {
        map.push_back(mapLine);
    }
    struct Coord
    {
        int row = -1;
        int col = -1;
        auto operator<=>(const Coord& other) const = default;
    };

    const int width = map.front().size();
    const int height = map.size();
    set<int> emptyRowIndices;
    for (int row = 0; row < height; row++)
    {
        if (map[row].find('#') == string::npos)
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
            if (map[row][col] == '#')
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
            if (map[row][col] == '#')
                galaxyCoords.push_back({row, col});
        }
    }

    int64_t result = 0;
    for (int galaxy1Index = 0; galaxy1Index < galaxyCoords.size(); galaxy1Index++)
    {
        for (int galaxy2Index = galaxy1Index + 1; galaxy2Index < galaxyCoords.size(); galaxy2Index++)
        {
            const auto galaxy1 = galaxyCoords[galaxy1Index];
            const auto galaxy2 = galaxyCoords[galaxy2Index];

            int64_t distance = 0;
            {
                int xStart = galaxy1.col;
                int xEnd = galaxy2.col;
                if (xEnd < xStart)
                    swap(xStart, xEnd);
                for (int x = xStart; x < xEnd; x++)
                {
                    if (emptyColIndices.contains(x))
                    {
                        distance += emptyLineDistMultiplier;
                    }
                    else
                    {
                        distance++;
                    }
                }
            }
            {
                int yStart = galaxy1.row;
                int yEnd = galaxy2.row;
                if (yEnd < yStart)
                    swap(yStart, yEnd);
                for (int y = yStart; y < yEnd; y++)
                {
                    if (emptyRowIndices.contains(y))
                    {
                        distance += emptyLineDistMultiplier;
                    }
                    else
                    {
                        distance++;
                    }
                }
            }
            result += distance;
        }
    }
    std::cout << "result: " << result << std::endl;
}

