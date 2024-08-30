#include <iostream>
#include <vector>

using namespace std;

int main()
{
    vector<string> map;
    string mapLine;
    while (getline(cin, mapLine))
    {
        map.push_back(mapLine);
    }
    std::cout << "Original map: " << std::endl;
    for (const auto& row : map) cout << row << std::endl;
    int width = map.front().size();
    int height = map.size();
    for (int row = 0; row < height; row++)
    {
        if (map[row].find('#') == string::npos)
        {
            map.insert(map.begin() + row, string(width, '.'));
            row++;
            height++;
        }
    }
    std::cout << "After inserting rows: " << std::endl;
    for (const auto& row : map) cout << row << std::endl;
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
            std::cout << "inserting row" << std::endl;
            for (int row = 0; row < height; row++)
            {
                map[row].insert(map[row].begin() + col, '.');
            }
            col++;
            width++;
        }

    }
    std::cout << "After inserting cols: " << std::endl;
    for (const auto& row : map) cout << row << std::endl;

    struct Coord
    {
        int row = -1;
        int col = -1;
        auto operator<=>(const Coord& other) const = default;
    };

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
            result += abs(galaxy2.row - galaxy1.row) + abs(galaxy2.col - galaxy1.col);
        }
    }
    std::cout << result << std::endl;

}

