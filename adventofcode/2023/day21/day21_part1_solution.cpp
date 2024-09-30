#include <iostream>
#include <vector>
#include <set>

using namespace std;

int main()
{
    string mapLineRaw;
    vector<string> rawMap;
    while (getline(cin, mapLineRaw))
    {
        rawMap.push_back(mapLineRaw);
    }
    const int width = rawMap.front().size();
    const int height = rawMap.size();

    vector<vector<char>> map(width, vector<char>(height, '.'));
    struct Coord
    {
        int x = -1;
        int y = -1;
        auto operator<=>(const Coord& other) const = default;
    };
    Coord startCoord;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            map[x][y] = rawMap[y][x];
            if (map[x][y] == 'S')
            {
                map[x][y] = '.';
                startCoord = {x, y};
            }
        }
    }

    set<Coord> reachableCoords = { startCoord };
    for (int numSteps = 1; numSteps <= 64; numSteps++)
    {
        set<Coord> nextReachableCoords;
        for (const auto& coord : reachableCoords)
        {
            for (const auto& [dx, dy] : std::initializer_list<std::pair<int, int>>{ {-1, 0},
                    {+1, 0},
                    {0, -1},
                    {0, +1}
                    })
            {
                int neighbourX = coord.x + dx;
                int neighbourY = coord.y + dy;
                if (neighbourX < 0 ||  neighbourX >= width || neighbourY < 0 || neighbourY >= height)
                    continue;
                if (map[neighbourX][neighbourY] == '#')
                    continue;
                nextReachableCoords.insert({neighbourX, neighbourY});
            }
        }
        reachableCoords = nextReachableCoords;
        auto mapToPrint = map;
        for (const auto& coord : reachableCoords)
        {
            mapToPrint[coord.x][coord.y] = 'O';
        }
        std::cout << "After " << numSteps << " steps: " << numSteps << " #reachable: " << reachableCoords.size() << std::endl;
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                std::cout << mapToPrint[x][y];
            }
            std::cout << endl;
        }
    }
        return 0;
}
