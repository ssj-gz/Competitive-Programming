#include <iostream>
#include <vector>
#include <map>
#include <cassert>

using std::cout;
using std::cin;
using std::endl;

using std::string;
using std::vector;
using std::map;

int main()
{
    enum Directions { East, West, NorthEast, NorthWest, SouthEast, SouthWest };
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
        auto operator<=>(const Coord& other) const = default;
    };
    // All tiles are white by default.
    map<Coord, bool> isTileBlack;

    for (const auto& directionsToTile : directionsToTiles)
    {
        int rowIndex = 0;
        int colIndex = 0;

        for (const auto direction : directionsToTile)
        {
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
        }

        isTileBlack[{rowIndex, colIndex}] = !isTileBlack[{rowIndex, colIndex}];
        cout << "Tile: row: " << rowIndex << " col: " << colIndex << endl;
    }
    int numBlackTiles = 0;
    for (const auto& [coord, isBlack] : isTileBlack)
    {
        if (isBlack)
            numBlackTiles++;
    }
    cout << "numBlackTiles: " << numBlackTiles << endl;
}
