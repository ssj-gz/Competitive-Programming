#include <iostream>
#include <vector>
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
    };


    vector<vector<Tile>> tiles(width, vector<Tile>(height));
    Tile* startTile = nullptr;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            const char cellContent = map[x][y];
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
                //cout << map[x][y];
            }
            else
            {
                furtherDistance = std::max(furtherDistance, tile->distanceFromStart);
                //cout << static_cast<char>('0' + tile->distanceFromStart);
            }
        }
        std::cout << endl;
    }

    std::cout << "furtherDistance: " << furtherDistance << std::endl;
}

