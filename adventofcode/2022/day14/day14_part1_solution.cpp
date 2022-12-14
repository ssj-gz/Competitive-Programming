#include <iostream>
#include <sstream>
#include <map>

#include <cassert>

using namespace std;

int main()
{
    enum TileType { Empty, Rock, Sand };
    struct Coord
    {
        int x = -1;
        int y = -1;
        auto operator<=>(const Coord& other) const = default;
    };
    std::map<Coord, TileType> tileTypeOfCoord;
    // 498,4 -> 498,6 -> 496,6
    // 503,4 -> 502,4 -> 502,9 -> 494,9
    string rockPathLine;
    while (std::getline(cin, rockPathLine))
    {
        istringstream rockPathStream(rockPathLine);
        std::cout << "Parsing: " << rockPathLine << std::endl;

        Coord previousRockCoord;
        bool havePreviousRockCoord = false;
        while (true)
        {
            Coord rockPathCoord;
            rockPathStream >> rockPathCoord.x;
            char comma;
            rockPathStream >> comma;
            assert(comma == ',');
            rockPathStream >> rockPathCoord.y;

            assert(rockPathStream);

            string arrow;
            rockPathStream >> arrow;

            std::cout << " rockPathX: " << rockPathCoord.x << std::endl;
            std::cout << " rockPathY: " << rockPathCoord.y << std::endl;

            if (havePreviousRockCoord)
            {
                if (previousRockCoord.x == rockPathCoord.x)
                {
                    const int minY = std::min(previousRockCoord.y, rockPathCoord.y);
                    const int maxY = std::max(previousRockCoord.y, rockPathCoord.y);
                    for (int y = minY; y <= maxY; y++)
                    {
                        //std::cout << "Blee: " << rockPathCoord.x << "," <<  y << std::endl;
                        tileTypeOfCoord[Coord{rockPathCoord.x, y}] = Rock;
                    }
                }
                else if (previousRockCoord.y == rockPathCoord.y)
                {
                    const int minX = std::min(previousRockCoord.x, rockPathCoord.x);
                    const int maxX = std::max(previousRockCoord.x, rockPathCoord.x);
                    for (int x = minX; x <= maxX; x++)
                    {
                        //std::cout << "Bloo: " << x << "," <<  rockPathCoord.y << std::endl;
                        tileTypeOfCoord[Coord{x, rockPathCoord.y}] = Rock;
                    }
                }
                else
                {
                    assert(false);
                }

            }

            if (!rockPathStream)
                break;
            assert(arrow == "->");

            previousRockCoord = rockPathCoord;
            havePreviousRockCoord = true;
        }
    }

    const Coord SandRockOrigin{500, 0};

    int minX = SandRockOrigin.x;
    int minY = SandRockOrigin.y;
    int maxX = SandRockOrigin.x;
    int maxY = SandRockOrigin.y;

    for (const auto& [coord, type] : tileTypeOfCoord)
    {
        minY = std::min(minY, coord.y);
        maxY = std::max(maxY, coord.y);
        minX = std::min(minX, coord.x);
        maxX = std::max(maxX, coord.x);
    }
    const int bottomlessYBegin = maxY + 1;
    std::cout << "minX: " << minX << " minY: " << minY << " maxX: " << maxX << " maxY: " << maxY << std::endl;
    std::cout << "bottomlessYBegin: " << bottomlessYBegin << std::endl;

    auto printGrid = [&]()
    {
        int minX = SandRockOrigin.x;
        int minY = SandRockOrigin.y;
        int maxX = SandRockOrigin.x;
        int maxY = SandRockOrigin.y;

        for (const auto& [coord, type] : tileTypeOfCoord)
        {
            minY = std::min(minY, coord.y);
            maxY = std::max(maxY, coord.y);
            minX = std::min(minX, coord.x);
            maxX = std::max(maxX, coord.x);
        }

        for (int y = minY; y <= maxY; y++)
        {
            for (int x = minX; x <= maxX; x++)
            {
                if (x == SandRockOrigin.x && y == SandRockOrigin.y)
                {
                    cout << "+";
                }
                else
                {
                    switch (tileTypeOfCoord[{x, y}])
                    {
                        case Empty:
                            cout << ".";
                            break;
                        case Rock:
                            cout << "#";
                            break;
                        case Sand:
                            cout << "O";
                            break;
                        default:
                            assert(false);

                    }
                }

            }
            std::cout << std::endl;
        }
    };

    printGrid();

    int numSpawnedSands = 0;
    bool haveFallenIntoAbyss = false;
    while (!haveFallenIntoAbyss)
    {
        std::cout << "Spawning new sand" << std::endl;
        Coord sandCoord = SandRockOrigin;
        numSpawnedSands++;
        while (true)
        {
            sandCoord.y++;
            if (tileTypeOfCoord[sandCoord] != Empty)
            {
                sandCoord.x--;
                if (tileTypeOfCoord[sandCoord] != Empty)
                {
                    sandCoord.x += 2;
                    if (tileTypeOfCoord[sandCoord] != Empty)
                    {
                        // Settle.
                        sandCoord.x--;
                        sandCoord.y--;
                        tileTypeOfCoord[sandCoord] = Sand;
                        break;
                    }

                }
            }
            if (sandCoord.y >= bottomlessYBegin)
            {
                // Fell into the abyss.
                haveFallenIntoAbyss = true;
                break;
            }
        }
        std::cout << "After spawning " << numSpawnedSands << " sands: " << std::endl;
        printGrid();
    }
    std::cout << "# sands before abyss: " << (numSpawnedSands - 1) << std::endl;


}
