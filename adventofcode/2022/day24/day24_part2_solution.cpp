#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>

using namespace std;

struct Blizzard
{
    char dirChar = '\0';
    int x = -1;
    int y = -1;
    auto operator<=>(const Blizzard& other) const = default;
};

struct Pos
{
    int x = -1;
    int y = -1;
    auto operator<=>(const Pos& other) const = default;
};

int main()
{
    string rawMapLine;
    vector<string> blankMap;
    int y = 0;
    set<Blizzard> blizzards;
    while (getline(cin, rawMapLine))
    {
        for (int x = 0; x < rawMapLine.size(); x++)
        {
            if (rawMapLine[x] == '<' || rawMapLine[x] == '^' || rawMapLine[x] == '>' || rawMapLine[x] == 'v')
            {
                blizzards.insert({rawMapLine[x], x, y});

                rawMapLine[x] = '.';
            }
        }
        blankMap.push_back(rawMapLine);
        y++;
    }
#if 0
    const int width = rawMap.front().size();
    const int height = rawMap.size();

    vector<vector<char>> map(width, vector<char>(height, ' '));
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            map[x][y] = rawMap[y][x];
        }
    }
#endif
    const int width = blankMap.front().size();
    const int height = blankMap.size();

    auto printMap = [&blankMap, &blizzards, width, height]()
    {
        vector<string> map(blankMap);
        for (const auto& blizzard : blizzards)
        {
            auto& mapCell = map[blizzard.y][blizzard.x];

            if (mapCell == '.')
                mapCell = blizzard.dirChar;
            else
            {
                if (mapCell == '<' || mapCell == '>' || mapCell == '^' || mapCell == 'v')
                {
                    mapCell = '2';
                }
                else
                {
                    mapCell++;
                }
            }
        }

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                cout << map[y][x];
            }
            cout << endl;
        }

    };

    cout << "Initial state: " << endl;
    printMap();

    Pos initialPos = { 1, 0 };
    Pos finalPos = { width - 2, height - 1 };
    int minute = 1;

    for (int tripNum = 1; tripNum <= 3; tripNum++)
    {
        assert(blankMap[finalPos.y][finalPos.x] == '.');
        std::cout << "finalPos: " << finalPos.x << ", " << finalPos.y << std::endl;
        set<Pos> positionsToExplore = { initialPos };

        while (true)
        {
            set<Blizzard> nextBlizzards;
            for (const auto& blizzard : blizzards)
            {
                Blizzard nextBlizzard = blizzard;
                switch (blizzard.dirChar)
                {
                    case '>':
                        nextBlizzard.x++;
                        if (blankMap[nextBlizzard.y][nextBlizzard.x] == '#')
                        {
                            assert(nextBlizzard.x == width - 1);
                            nextBlizzard.x = 1;
                        }
                        break;
                    case '<':
                        nextBlizzard.x--;
                        if (blankMap[nextBlizzard.y][nextBlizzard.x] == '#')
                        {
                            assert(nextBlizzard.x == 0);
                            nextBlizzard.x = width - 2;
                        }
                        break;
                    case 'v':
                        nextBlizzard.y++;
                        if (blankMap[nextBlizzard.y][nextBlizzard.x] == '#')
                        {
                            assert(nextBlizzard.y == height - 1);
                            nextBlizzard.y = 1;
                        }
                        break;
                    case '^':
                        nextBlizzard.y--;
                        if (blankMap[nextBlizzard.y][nextBlizzard.x] == '#')
                        {
                            assert(nextBlizzard.y == 0);
                            nextBlizzard.y = height - 2;
                        }
                        break;
                }
                nextBlizzards.insert(nextBlizzard);
            } 

            blizzards = nextBlizzards;
            set<Pos> blizzardPositions;
            for (const auto blizzard : blizzards)
            {
                blizzardPositions.insert({blizzard.x, blizzard.y});
            }

            set<Pos> nextPositionsToExplore;
            std::cout << "positionsToExplore: " << std::endl;
            for (const auto pos : positionsToExplore)
            {
                std::cout << " " << pos.x << ", " << pos.y << std::endl;
            }

            for (const auto pos : positionsToExplore)
            {
                std::cout << " checking pos: " << pos.x << ", " << pos.y << std::endl;
                for (int nextX = pos.x - 1; nextX <= pos.x + 1; nextX++)
                {
                    for (int nextY = pos.y - 1; nextY <= pos.y + 1; nextY++)
                    {
                        std::cout << "  nextX: " << nextX << " nextY: " << nextY << std::endl;
                        if (nextX < 0 || nextY < 0 || nextX >= width || nextY >= height)
                        {
                            std::cout << "   out of range" << std::endl;
                            continue;
                        }
                        if (abs(nextX - pos.x) + abs(nextY - pos.y) > 1)
                        {
                            // No diagonal movements.
                            std::cout << "   diagonal" << std::endl;
                            continue;
                        }
                        if (blankMap[nextY][nextX] != '.')
                        {
                            std::cout << "   wall" << std::endl;
                            continue;
                        }
                        if (blizzardPositions.contains({nextX, nextY}))
                        {
                            std::cout << "   blizzard" << std::endl;
                            continue;
                        }

                        std::cout << "   Adding!" << std::endl;
                        nextPositionsToExplore.insert({nextX, nextY});
                    }
                }
            }

            std::cout << "After minute " << minute << std::endl;
            printMap();
            positionsToExplore = nextPositionsToExplore;
            minute++;
            if (std::find(positionsToExplore.begin(), positionsToExplore.end(), finalPos) != positionsToExplore.end())
            {
                break;
            }
        }
        std::swap(initialPos, finalPos);
    }
    std::cout << "time: " << (minute - 1) << std::endl;
}

