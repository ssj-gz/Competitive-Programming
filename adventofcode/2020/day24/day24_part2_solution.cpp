#include <iostream>
#include <vector>
#include <set>
#include <cassert>

using std::cout;
using std::cin;
using std::endl;

using std::string;
using std::vector;
using std::set;

int main()
{
    enum Directions { East, West, NorthEast, NorthWest, SouthEast, SouthWest };

    // Read in the list of directions for each tile to flip.
    string directionsToTileString;
    vector<vector<Directions>> directionsToTiles;
    while (std::getline(cin, directionsToTileString))
    {
        vector<Directions> directions;
        string::size_type dirParserPos = 0;
        while (dirParserPos < directionsToTileString.size())
        {
            switch (directionsToTileString[dirParserPos])
            {
                case 'n':
                   dirParserPos++; 
                   switch (directionsToTileString[dirParserPos])
                   {
                       case 'e':
                           directions.push_back(NorthEast);
                           break;
                       case 'w':
                           directions.push_back(NorthWest);
                           break;
                       default:
                           assert(false);
                   }
                   break;
                case 's':
                   dirParserPos++; 
                   switch (directionsToTileString[dirParserPos])
                   {
                       case 'e':
                           directions.push_back(SouthEast);
                           break;
                       case 'w':
                           directions.push_back(SouthWest);
                           break;
                       default:
                           assert(false);
                   }
                   break;

                case 'e':
                   directions.push_back(East);
                   break;
                case 'w':
                   directions.push_back(West);
                   break;
                default:
                   assert(false);
            }
           dirParserPos++; 
        }
        cout << "Read: " << directions.size() << " directions (source string len: " << directionsToTileString.size() << ")" << std::endl;
        directionsToTiles.push_back(directions);
    }

    struct Coord
    {
        int rowIndex = -1;
        int colIndex = -1;
        // The default <=> suffices for placing Coords in a set.
        auto operator<=>(const Coord& other) const = default;
    };
    // All tiles are white by default.
    set<Coord> blackTiles;

    auto tileInDirection = [](const Coord startTile, Directions direction)
    {
        int rowIndex = startTile.rowIndex;
        int colIndex = startTile.colIndex;

        switch(direction)
        {
            case East:
                colIndex++;
                break;
            case West:
                colIndex--;
                break;
            case NorthEast:
                if (abs(rowIndex) % 2 == 1)
                {
                    colIndex++;
                }
                rowIndex--;
                break;
            case NorthWest:
                if (abs(rowIndex) % 2 == 0)
                {
                    colIndex--;
                }
                rowIndex--;
                break;
            case SouthEast:
                if (abs(rowIndex) % 2 == 1)
                {
                    colIndex++;
                }
                rowIndex++;
                break;
            case SouthWest:
                if (abs(rowIndex) % 2 == 0)
                {
                    colIndex--;
                }
                rowIndex++;
                break;
            default:
                assert(false);
        }

        return Coord{rowIndex, colIndex};
    };

    // Follow the directions to the target tile, and flip its color.
    for (const auto& directionsToTile : directionsToTiles)
    {
        Coord currentTileCoord{0, 0};

        for (const auto direction : directionsToTile)
        {
            currentTileCoord = tileInDirection(currentTileCoord, direction);
        }

        if (blackTiles.contains(currentTileCoord))
        {
            blackTiles.erase(currentTileCoord);
        }
        else
        {
            blackTiles.insert(currentTileCoord);
        }
    }

    for (int day = 1; day <= 100; day++)
    {
        auto coordsOfAdjacentTiles = [&tileInDirection](const Coord& tileCoord)
        {
            vector<Coord> adjacentTiles;

            for (const auto direction : { East, West, NorthEast, NorthWest, SouthEast, SouthWest })
            {
                adjacentTiles.push_back(tileInDirection(tileCoord, direction));
            }

            return adjacentTiles;
        };
        // Implicit in the update rules: the only tiles we need to examine are those that satisfying either of:
        //  a) tile is black
        //  b) tile is adjacent to a black tile.
        set<Coord> tileCoordsOfInterest;
        for (const auto& blackTile : blackTiles)
        {
            tileCoordsOfInterest.insert(blackTile);

            const auto adjacentTiles = coordsOfAdjacentTiles(blackTile);
            tileCoordsOfInterest.insert(adjacentTiles.begin(), adjacentTiles.end());
        }
        // Update the tiles.
        set<Coord> nextBlackTiles = blackTiles;
        for (const auto& tileToUpdate : tileCoordsOfInterest)
        {
            const auto adjacentTiles = coordsOfAdjacentTiles(tileToUpdate);
            int numBlackAdjacent = 0;
            for (const auto& adjacentTileCoord :adjacentTiles)
            {
                if (blackTiles.contains(adjacentTileCoord))
                {
                    numBlackAdjacent++;
                }
            }
            if (blackTiles.contains(tileToUpdate))
            {
                // This tile is black.
                if (numBlackAdjacent == 0 || numBlackAdjacent > 2)
                {
                    // Flip.
                    nextBlackTiles.erase(tileToUpdate);
                }
            }
            else
            {
                // This tile is white.
                if (numBlackAdjacent == 2)
                {
                    // Flip.
                    nextBlackTiles.insert(tileToUpdate);
                }
            }
        }
        blackTiles = nextBlackTiles;
        cout << "Day " << day << " numBlackTiles: " << blackTiles.size() << endl;
    }
}
