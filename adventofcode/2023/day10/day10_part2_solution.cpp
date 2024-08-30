#include <iostream>
#include <vector>
#include <set>
#include <limits>

#include <cassert>

using namespace std;

int main()
{
    string mapLine;
    vector<string> mapRaw;
    while (getline(cin, mapLine))
    {
        mapRaw.push_back(mapLine);
    }
    const int width = mapRaw.front().size();
    const int height = mapRaw.size();

    vector<vector<char>> map(width, vector<char>(height, ' '));
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            map[x][y] = mapRaw[y][x];
        }
    }

    struct Tile
    {
        vector<Tile*> neighbourTiles;
        int distanceFromStart = std::numeric_limits<int>::max();
        int x = -1;
        int y = -1;
    };


    vector<vector<Tile>> tiles(width, vector<Tile>(height));
    Tile* startTile = nullptr;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            const char cellContent = map[x][y];
            tiles[x][y].x = x;
            tiles[x][y].y = y;
            auto addNeighbour = [&tiles, x, y, width, height](int dx, int dy)
            {
                const int neighbourX = x + dx;
                const int neighbourY = y + dy;
                if (neighbourX < 0 || neighbourX >= width || neighbourY < 0 || neighbourY >= height)
                    return;
                tiles[x][y].neighbourTiles.push_back(&tiles[neighbourX][neighbourY]);
            };
            switch (cellContent)
            {
                case '|':
                    addNeighbour(0, -1);
                    addNeighbour(0, +1);
                    break;
                case '-':
                    addNeighbour(-1, 0);
                    addNeighbour(+1, 0);
                    break;
                case 'L':
                    addNeighbour(0, -1);
                    addNeighbour(+1, 0);
                    break;
                case 'J':
                    addNeighbour(-1, 0);
                    addNeighbour(0, -1);
                    break;
                case '7':
                    addNeighbour(-1, 0);
                    addNeighbour(0, +1);
                    break;
                case 'F':
                    addNeighbour(+1, 0);
                    addNeighbour(0, +1);
                    break;
                case 'S':
                    assert(startTile == nullptr);
                    startTile = &(tiles[x][y]);
                    break;
                case '.':
                    // Do nothing.
                    break;
                default:
                    assert(false);
            };
        }
    }
    assert(startTile);
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            Tile *tile = &(tiles[x][y]);
            for (auto* neighbourTile : tile->neighbourTiles)
            {
                if (neighbourTile == startTile)
                    startTile->neighbourTiles.push_back(tile);
            }
        }
    }

    vector<Tile*> toExplore = { startTile };
    int numSteps = 0;
    while (!toExplore.empty())
    {
        std::cout << "numSteps: " << numSteps << " #toExplore: " << toExplore.size() << std::endl;
        vector<Tile*> nextToExplore;
        for (auto* tile : toExplore)
        {
            if (tile->distanceFromStart != std::numeric_limits<int>::max())
                continue;
            tile->distanceFromStart = numSteps;
            for (auto* neighbourTile : tile->neighbourTiles)
            {
                nextToExplore.push_back(neighbourTile);
            }
        }

        numSteps++;
        toExplore = nextToExplore;
    }

    std::cout << "distance map: " << std::endl;
    int furtherDistance = -1;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Tile *tile = &(tiles[x][y]);
            if (tile->distanceFromStart == std::numeric_limits<int>::max())
            {
                cout << '.';
            }
            else
            {
                furtherDistance = std::max(furtherDistance, tile->distanceFromStart);
                cout << 'X';
            }
        }
        std::cout << endl;
    }

    const int detailedMapWidth = 2 * width + 1;
    const int detailedMapHeight = 2 * height + 1;
    vector<vector<char>> detailedMap(detailedMapWidth, vector<char>(detailedMapHeight, ' '));
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            Tile *tile = &(tiles[x][y]);
            const int tileDetailX = tile->x * 2 + 1;
            const int tileDetailY = tile->y * 2 + 1;
            if (tile->distanceFromStart == std::numeric_limits<int>::max())
            {

                detailedMap[tileDetailX][tileDetailY] = '.';
                continue;
            }
            for (auto* neighbourTile : tile->neighbourTiles)
            {
                const int neighbourDetailX = neighbourTile->x * 2 + 1;
                const int neighbourDetailY = neighbourTile->y * 2 + 1;
                assert(tileDetailX >= 0 && tileDetailX < detailedMapWidth);
                assert(tileDetailY >= 0 && tileDetailY < detailedMapHeight);
                assert(neighbourDetailX >= 0 && neighbourDetailX < detailedMapWidth);
                assert(neighbourDetailY >= 0 && neighbourDetailY < detailedMapHeight);
                auto sgn = [](int x)
                {
                    if (x == 0)
                        return 0;
                    else if (x < 0)
                        return -1;
                    else 
                        return +1;
                };
                const int dx = sgn(neighbourDetailX - tileDetailX);
                const int dy = sgn(neighbourDetailY - tileDetailY);
                for (int i = 0; i < 2; i++)
                {
                    const int detailX = tileDetailX + i * dx;
                    const int detailY = tileDetailY + i * dy;
                    assert(detailX >= 0 && detailX < detailedMapWidth);
                    assert(detailY >= 0 && detailY < detailedMapHeight);
                    detailedMap[detailX][detailY] = 'X';
                }
            }
        }
    }

    std::cout << "Detailed map: " << std::endl;
    for (int y = 0; y < detailedMapHeight; y++)
    {
        for (int x = 0; x < detailedMapWidth; x++)
        {
            cout << detailedMap[x][y];
        }
        std::cout << endl;
    }

    struct Coord
    {
        int x = -1;
        int y = -1;
        auto operator<=>(const Coord& other) const = default;
    };

    {
        set<Coord> toExplore = { { 0, 0 } };
        set<Coord> visited;
        while (!toExplore.empty())
        {
            std::cout << "#toExplore: " << toExplore.size() << std::endl;
            set<Coord> nextToExplore;
            for (const auto [x, y] : toExplore)
            {
                if (detailedMap[x][y] == '.')
                    detailedMap[x][y] = 'O';

                for (const auto& [dx, dy] : std::initializer_list<std::pair<int, int>>{ {-1, 0},
                        {+1, 0},
                        {0, -1},
                        {0, +1}
                        })
                {
                    const int neighbourX = x + dx;
                    const int neighbourY = y + dy;
                    if (neighbourX < 0 || neighbourX >= detailedMapWidth || neighbourY < 0 || neighbourY >= detailedMapHeight)
                        continue;
                    if (detailedMap[neighbourX][neighbourY] == 'X')
                        continue;
                    if (!visited.contains({neighbourX, neighbourY}))
                    {
                        nextToExplore.insert({neighbourX, neighbourY});
                        visited.insert({neighbourX, neighbourY});
                    }
                }

            }
            toExplore = nextToExplore;
        }
    }
    int numInsideTiles = 0;
    for (int y = 0; y < detailedMapHeight; y++)
    {
        for (int x = 0; x < detailedMapWidth; x++)
        {
            if (detailedMap[x][y] == '.')
            {
                detailedMap[x][y] = 'I';
                numInsideTiles++;
            }
        }
    }

    std::cout << "Detailed map (after filling): " << std::endl;
    for (int y = 0; y < detailedMapHeight; y++)
    {
        for (int x = 0; x < detailedMapWidth; x++)
        {
            cout << detailedMap[x][y];
        }
        std::cout << endl;
    }

    std::cout << "numInsideTiles: " << numInsideTiles << std::endl;
}

