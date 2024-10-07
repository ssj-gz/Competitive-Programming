#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <iomanip>
#include <limits>

#include <cassert>

//#define BRUTE_FORCE

using namespace std;

struct Coord
{
    int64_t x = -1;
    int64_t y = -1;
    auto operator<=>(const Coord& other) const = default;
};

namespace std
{
    template<>
        class hash<Coord>
        {
            public:
                size_t operator()(const Coord& coord) const
                {
                    return 1'000'003 * coord.x + coord.y;
                }
        };
};

int64_t mod(int64_t x, int64_t m)
{
    if (x < 0)
    {
        x = x + ((-x / m) + 1) * m;
    }
    const auto result = x % m;
    return result;
}

int64_t computeUltraBruteForce(const vector<vector<char>>& tileMap, int64_t numStepsToTake)
{
    const int tileWidth = tileMap.size();
    const int tileHeight = tileMap.front().size();
    std::cout << "tileWidth: " << tileWidth << " tileHeight: " << tileHeight << std::endl;
    Coord startCoord;
    for (int x = 0; x < tileWidth; x++)
    {
        for (int y = 0; y < tileHeight; y++)
        {
            if (tileMap[x][y] == 'S')
            {
                startCoord = {x, y};
            }
        }
    }
    assert((startCoord != Coord({-1, -1})));
    set<Coord> reachableCoords = { startCoord };
    map<Coord, int64_t> firstSeenEven;
    firstSeenEven[startCoord] = 0;
    map<Coord, int64_t> firstSeenOdd;
    map<Coord, Coord> predecessor;
    for (int64_t stepsTaken = 1; stepsTaken <= numStepsToTake; stepsTaken++)
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
                const int64_t neighbourX = coord.x + dx;
                const int64_t neighbourY = coord.y + dy;
                int64_t neighbourXInTile = mod(neighbourX, tileWidth);
                assert(neighbourXInTile >= 0 && neighbourXInTile < tileWidth);
                int64_t neighbourYInTile = mod(neighbourY, tileHeight);
                assert(neighbourYInTile >= 0 && neighbourYInTile < tileHeight);
                if (tileMap[neighbourXInTile][neighbourYInTile] == '#')
                    continue;
#if 1
                if (stepsTaken % 2 == 0)
                {
                    if (!firstSeenEven.contains({neighbourX, neighbourY}))
                    {
                        firstSeenEven[{neighbourX, neighbourY}] = stepsTaken;
                        //std::cout << "First seen even: " << neighbourX << ", " << neighbourY << " @: " << stepsTaken << std::endl;
                    }
                }
                else
                {
                    if (!firstSeenOdd.contains({neighbourX, neighbourY}))
                    {
                        firstSeenOdd[{neighbourX, neighbourY}] = stepsTaken;
                        //std::cout << "First seen odd: " << neighbourX << ", " << neighbourY << " @: " << stepsTaken << std::endl;
                    }
                }
#endif

                if (!predecessor.contains({neighbourX, neighbourY}))
                {
                    predecessor[{neighbourX, neighbourY}] = coord;
                    //std::cout << "Predecessor (modulo) of " << mod(neighbourX, tileWidth) << ", " << mod(neighbourY, tileHeight) << " is: " << mod(coord.x, tileWidth) << ", " << mod(coord.y, tileHeight) << std::endl;
                }
                nextReachableCoords.insert({neighbourX, neighbourY});
            }
        }
        reachableCoords = nextReachableCoords;
        std::cout << "After " << stepsTaken << " steps #reachable: " << reachableCoords.size() << std::endl;
#if 0
        auto mapToPrint = map;
        for (const auto& coord : reachableCoords)
        {
            mapToPrint[coord.x][coord.y] = 'O';
        }
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                std::cout << mapToPrint[x][y];
            }
            std::cout << endl;
        }
#endif
    }
#if 1
    for (const auto& [coord, steps] : firstSeenOdd)
    {
        assert(!firstSeenEven.contains(coord));
    }
    for (const auto& [coord, steps] : firstSeenEven)
    {
        assert(!firstSeenOdd.contains(coord));
    }
#endif
    return reachableCoords.size();
}

int64_t computeBruteForce(const vector<vector<char>>& tileMap, int64_t numStepsToTake)
{
    int64_t result = (numStepsToTake % 2 == 0 ? 1 : 0); // Conditionally include "S" (only if even).
    const int tileWidth = tileMap.size();
    const int tileHeight = tileMap.front().size();
    std::cout << "tileWidth: " << tileWidth << " tileHeight: " << tileHeight << std::endl;
    Coord startCoord;
    for (int x = 0; x < tileWidth; x++)
    {
        for (int y = 0; y < tileHeight; y++)
        {
            if (tileMap[x][y] == 'S')
            {
                startCoord = {x, y};
            }
        }
    }
    assert((startCoord != Coord({-1, -1})));
    unordered_set<Coord> toExplore = { startCoord };
    unordered_set<Coord> seen = { startCoord };
    for (int64_t stepsTaken = 1; stepsTaken <= numStepsToTake; stepsTaken++)
    {
        if (stepsTaken % 100 == 0)
            std::cout << "stepsTaken: " << stepsTaken << " #toExplore: " << toExplore.size() << std::endl;
        unordered_set<Coord> nextToExplore;
        for (const auto& coord : toExplore)
        {
            for (const auto& [dx, dy] : std::initializer_list<std::pair<int, int>>{ {-1, 0},
                    {+1, 0},
                    {0, -1},
                    {0, +1}
                    })
            {
                const int64_t neighbourX = coord.x + dx;
                const int64_t neighbourY = coord.y + dy;
                int64_t neighbourXInTile = mod(neighbourX, tileWidth);
                assert(neighbourXInTile >= 0 && neighbourXInTile < tileWidth);
                int64_t neighbourYInTile = mod(neighbourY, tileHeight);
                assert(neighbourYInTile >= 0 && neighbourYInTile < tileHeight);
                if (tileMap[neighbourXInTile][neighbourYInTile] == '#')
                    continue;

                if (!seen.contains({neighbourX, neighbourY}))
                {
                    if (stepsTaken % 2 == numStepsToTake % 2)
                    {
                        result++;
                    }
                    nextToExplore.insert({neighbourX, neighbourY});
                    seen.insert({neighbourX, neighbourY});
                }
            }
        }
        toExplore = nextToExplore;
        //std::cout << "After " << stepsTaken << " steps #reachable: " << reachableCoords.size() << std::endl;
    }
    return result;
}

vector<vector<int64_t>> shortestPathInTileMap(const Coord& startCoord, const vector<vector<char>>& tileMap)
{
    const int width = tileMap.size();
    const int height = tileMap.front().size();
    vector<vector<int64_t>> shortestPaths = vector<vector<int64_t>>(width, vector<int64_t>(height, -1));
    shortestPaths[startCoord.x][startCoord.y] = 0;
    set<Coord> toExplore = { startCoord };
    int stepsTaken = 1;
    while (!toExplore.empty())
    {
        set<Coord> nextToExplore;
        for (const auto& coord : toExplore)
        {
            for (const auto& [dx, dy] : std::initializer_list<std::pair<int, int>>{ {-1, 0},
                    {+1, 0},
                    {0, -1},
                    {0, +1}
                    })
            {
                const int64_t neighbourX = coord.x + dx;
                const int64_t neighbourY = coord.y + dy;
                if (neighbourX < 0 || neighbourX >= width || neighbourY < 0 || neighbourY >= height)
                    continue;
                if (tileMap[neighbourX][neighbourY] == '#')
                    continue;
                if (shortestPaths[neighbourX][neighbourY] == -1)
                {
                    shortestPaths[neighbourX][neighbourY] = stepsTaken;
                    nextToExplore.insert({neighbourX, neighbourY});
                }
            }
        }
        toExplore = nextToExplore;
        stepsTaken++;
    }
    return shortestPaths;
}

int64_t doBottomRightBruteForce(const vector<vector<char>>& tileMap, int64_t numStepsToTake)
{
    std::cout << "doBottomRightBruteForce" << std::endl;
    int64_t result = 0;
    const int tileWidth = tileMap.size();
    const int tileHeight = tileMap.front().size();
    Coord startCoord;
    for (int x = 0; x < tileWidth; x++)
    {
        for (int y = 0; y < tileHeight; y++)
        {
            if (tileMap[x][y] == 'S')
            {
                startCoord = {x, y};
            }
        }
    }
    assert((startCoord != Coord({-1, -1})));
    unordered_set<Coord> toExplore = { startCoord };
    unordered_set<Coord> seen = { startCoord };

    for (int64_t stepsTaken = 1; stepsTaken <= numStepsToTake; stepsTaken++)
    {
        unordered_set<Coord> nextToExplore;
        for (const auto& coord : toExplore)
        {
            for (const auto& [dx, dy] : std::initializer_list<std::pair<int, int>>{ {-1, 0},
                    {+1, 0},
                    {0, -1},
                    {0, +1}
                    })
            {
                const int64_t neighbourX = coord.x + dx;
                const int64_t neighbourY = coord.y + dy;
                if (neighbourX < 0)
                    continue;
                if (neighbourY < 0)
                    continue;
                const int64_t neighbourXInTile = mod(neighbourX, tileWidth);
                assert(neighbourXInTile >= 0 && neighbourXInTile < tileWidth);
                const int64_t neighbourYInTile = mod(neighbourY, tileHeight);
                assert(neighbourYInTile >= 0 && neighbourYInTile < tileHeight);
                const int64_t neighbourTileX = neighbourX / tileWidth;
                const int64_t neighbourTileY = neighbourY / tileHeight;
                if (tileMap[neighbourXInTile][neighbourYInTile] == '#')
                    continue;
                assert(neighbourTileX >= 0 && neighbourTileY >= 0);

                if (!seen.contains({neighbourX, neighbourY}))
                {
                    if ((neighbourTileX >= 1) && (neighbourTileY >= 1) && (stepsTaken % 2 == numStepsToTake % 2))
                    {
                        //std::cout << " reached: " << neighbourX << ", " << neighbourY << " at step: " << stepsTaken << std::endl;
                        result++;
                    }
                    nextToExplore.insert({neighbourX, neighbourY});
                    seen.insert({neighbourX, neighbourY});
                }
            }
        }
        toExplore = nextToExplore;
    }
    return result;

}
int64_t doVerticalStripeBruteForce(const vector<vector<char>>& tileMap, int64_t numStepsToTake)
{
    int64_t result = 0;
    const int tileWidth = tileMap.size();
    const int tileHeight = tileMap.front().size();
    Coord startCoord;
    for (int x = 0; x < tileWidth; x++)
    {
        for (int y = 0; y < tileHeight; y++)
        {
            if (tileMap[x][y] == 'S')
            {
                startCoord = {x, y};
            }
        }
    }
    assert((startCoord != Coord({-1, -1})));
    unordered_set<Coord> toExplore = { startCoord };
    unordered_set<Coord> seen = { startCoord };

    for (int64_t stepsTaken = 1; stepsTaken <= numStepsToTake; stepsTaken++)
    {
        //if (stepsTaken % 100 == 0)
            //std::cout << "stepsTaken: " << stepsTaken << " #toExplore: " << toExplore.size() << std::endl;
        unordered_set<Coord> nextToExplore;
        for (const auto& coord : toExplore)
        {
            for (const auto& [dx, dy] : std::initializer_list<std::pair<int, int>>{ {-1, 0},
                    {+1, 0},
                    {0, -1},
                    {0, +1}
                    })
            {
                const int64_t neighbourX = coord.x + dx;
                const int64_t neighbourY = coord.y + dy;
                if (neighbourX < 0 || neighbourX >= tileWidth)
                    continue;
                if (neighbourY < 0)
                    continue;
                const int64_t neighbourXInTile = mod(neighbourX, tileWidth);
                assert(neighbourXInTile >= 0 && neighbourXInTile < tileWidth);
                const int64_t neighbourYInTile = mod(neighbourY, tileHeight);
                assert(neighbourYInTile >= 0 && neighbourYInTile < tileHeight);
                const int64_t neighbourTileX = neighbourX / tileWidth;
                const int64_t neighbourTileY = neighbourY / tileHeight;
                if (tileMap[neighbourXInTile][neighbourYInTile] == '#')
                    continue;
                assert(neighbourTileX == 0);


                if (!seen.contains({neighbourX, neighbourY}))
                {
                    if (neighbourTileY >= 1 && stepsTaken % 2 == numStepsToTake % 2)
                    {
                        result++;
                    }
                    nextToExplore.insert({neighbourX, neighbourY});
                    seen.insert({neighbourX, neighbourY});
                }
            }
        }
        toExplore = nextToExplore;
    }
    return result;

}

int64_t doVerticalStripe(const vector<vector<char>>& tileMap, int64_t numStepsToTake, const Coord& startCoord)
{
    const auto shortestPaths = shortestPathInTileMap({0, 0}, tileMap);
    const int tileWidth = tileMap.size();
    const int tileHeight = tileMap.front().size();
    int64_t numReachableSpacesInTile = tileWidth * tileHeight;
    for (int y = 0; y < tileHeight; y++)
    {
        for (int x = 0; x < tileWidth; x++)
        {
            if (shortestPaths[x][y] == -1)
            {
                //std::cout << " unreachable: " << " x: " << x << " y: " << y << std::endl;
                numReachableSpacesInTile --;
            }
        }
    }
    std::cout << "numReachableSpacesInTile: " << numReachableSpacesInTile << std::endl;
    std::cout << "doVerticalStripe" << std::endl;
    //int64_t result = (numStepsToTake % 2 == 0 ? 1 : 0); // Conditionally include "S" (only if even).
    std::cout << "tileWidth: " << tileWidth << " tileHeight: " << tileHeight << std::endl;
    map<Coord, vector<vector<int64_t>>> minsDistForTile;
    map<Coord, int64_t> numUnknownDistsForTile;
    assert((startCoord != Coord({-1, -1})));
    unordered_set<Coord> toExplore = { startCoord };
    unordered_set<Coord> seen = { startCoord };

    vector<vector<int64_t>> previousDiffsBetweenAdjacentTiles;
    bool diffRepetitionFound = false;
    vector<vector<int64_t>> repetitionDiffPattern;
    int64_t repetitionBaseTileY = -1;

    for (int64_t stepsTaken = 1; (stepsTaken <= numStepsToTake && !diffRepetitionFound); stepsTaken++)
    {
        //if (stepsTaken % 100 == 0)
            //std::cout << "stepsTaken: " << stepsTaken << " #toExplore: " << toExplore.size() << std::endl;
        unordered_set<Coord> nextToExplore;
        for (const auto& coord : toExplore)
        {
            for (const auto& [dx, dy] : std::initializer_list<std::pair<int, int>>{ {-1, 0},
                    {+1, 0},
                    {0, -1},
                    {0, +1}
                    })
            {
                const int64_t neighbourX = coord.x + dx;
                const int64_t neighbourY = coord.y + dy;
                if (neighbourX < 0 || neighbourX >= tileWidth)
                    continue;
                if (neighbourY < 0)
                    continue;
                const int64_t neighbourXInTile = mod(neighbourX, tileWidth);
                assert(neighbourXInTile >= 0 && neighbourXInTile < tileWidth);
                const int64_t neighbourYInTile = mod(neighbourY, tileHeight);
                assert(neighbourYInTile >= 0 && neighbourYInTile < tileHeight);
                const int64_t neighbourTileX = neighbourX / tileWidth;
                const int64_t neighbourTileY = neighbourY / tileHeight;
                if (tileMap[neighbourXInTile][neighbourYInTile] == '#')
                    continue;

                assert(neighbourTileX == 0);
                if (!minsDistForTile.contains({neighbourTileX, neighbourTileY}))
                {
                    minsDistForTile[{neighbourTileX, neighbourTileY}] = vector<vector<int64_t>>(tileWidth, vector<int64_t>(tileHeight, -1));
                    numUnknownDistsForTile[{neighbourTileX, neighbourTileY}] = numReachableSpacesInTile;
                }

                auto& minDistsForNeighbourTile = minsDistForTile[{neighbourTileX, neighbourTileY}];
                if (minDistsForNeighbourTile[neighbourXInTile][neighbourYInTile] == -1)
                {
                    minDistsForNeighbourTile[neighbourXInTile][neighbourYInTile] = stepsTaken;
                    numUnknownDistsForTile[{neighbourTileX, neighbourTileY}]--;
                    //std::cout << " reduced numUnknownDistsForTile " << neighbourXInTile << ", " << neighbourYInTile << " down to " << numUnknownDistsForTile[{neighbourTileX, neighbourTileY}] << " in tile: " << neighbourTileX << ", " << neighbourTileY << std::endl;
                    if (numUnknownDistsForTile[{neighbourTileX, neighbourTileY}] == 0)
                    {
                        //std::cout << "Completed tile: " << neighbourTileX << "," << neighbourTileY << std::endl;
                        if (neighbourTileY > 0)
                        {
                            assert((numUnknownDistsForTile[{neighbourTileX, neighbourTileY - 1}] == 0)); 
                            //std::cout << " diffs from previous: " << std::endl;
                            auto diffsFromPreviousTile = vector<vector<int64_t>>(tileWidth, vector<int64_t>(tileHeight, -1));
                            for (int y = 0; y < tileHeight; y++)
                            {
                                for (int x = 0; x < tileWidth; x++)
                                {
                                    diffsFromPreviousTile[x][y] = minsDistForTile[{neighbourTileX, neighbourTileY}][x][y] - minsDistForTile[{neighbourTileX, neighbourTileY - 1}][x][y];
                                    //std::cout << setw(3) << diffsFromPreviousTile[x][y]<< " ";
                                }
                                //std::cout << endl;
                            }
                            if (diffsFromPreviousTile == previousDiffsBetweenAdjacentTiles)
                            {
                                std::cout << "Repetition found: stepsTaken: " << stepsTaken << std::endl;
                                diffRepetitionFound = true;
                                repetitionDiffPattern = diffsFromPreviousTile;
                                repetitionBaseTileY = neighbourTileY - 1;
                            }
                            else 
                            {
                                assert(!diffRepetitionFound);
                            }
                            previousDiffsBetweenAdjacentTiles = diffsFromPreviousTile;
                        }
                    }
                }

                if (!seen.contains({neighbourX, neighbourY}))
                {
                    if (stepsTaken % 2 == numStepsToTake % 2)
                    {
                        //result++;
                    }
                    nextToExplore.insert({neighbourX, neighbourY});
                    seen.insert({neighbourX, neighbourY});
                }
            }
        }
        toExplore = nextToExplore;
        //std::cout << "After " << stepsTaken << " steps #reachable: " << reachableCoords.size() << std::endl;
    }
    assert(diffRepetitionFound);
    int64_t result = 0;
    std::cout << "Sausage" << std::endl;
    int64_t minDistsInBaseTile = std::numeric_limits<int64_t>::max();
    int64_t maxDistsInBaseTile = std::numeric_limits<int64_t>::min();
    int64_t numOddDistances = 0;
    int64_t numEvenDistances = 0;
    const auto& repetitionBaseTileDists = minsDistForTile[{0, repetitionBaseTileY}];
    for (int y = 0; y < tileHeight; y++)
    {
        for (int x = 0; x < tileWidth; x++)
        {
            assert(repetitionDiffPattern[x][y] == 0 || repetitionDiffPattern[x][y] == tileHeight);
            if (repetitionBaseTileDists[x][y] != -1)
            {
                minDistsInBaseTile = std::min(minDistsInBaseTile, repetitionBaseTileDists[x][y]);
                maxDistsInBaseTile = std::max(maxDistsInBaseTile, repetitionBaseTileDists[x][y]);

                if (repetitionBaseTileDists[x][y] % 2 == 0)
                    numEvenDistances++;
                else
                    numOddDistances++;
            }

        }
    }
    assert(numOddDistances + numEvenDistances == numReachableSpacesInTile);
    int tileY = 1;
    while (true)
    {
        //std::cout << "tileY: " << tileY << std::endl;
        if (tileY < repetitionBaseTileY)
        {
            auto& tileMinDists = minsDistForTile[{0, tileY}];
            for (int y = 0; y < tileHeight; y++)
            {
                for (int x = 0; x < tileWidth; x++)
                {
                    if (tileMinDists[x][y] != -1 && tileMinDists[x][y] % 2 == numStepsToTake % 2)
                    {
                        result++;
                    }
                }
            }
        }
        else
        {
            const int64_t maxDistInTile = maxDistsInBaseTile + tileHeight * (tileY - repetitionBaseTileY);
            const int64_t minDistInTile = minDistsInBaseTile + tileHeight * (tileY - repetitionBaseTileY);
            auto& minDistsForThisTile = minsDistForTile[{0, tileY}];
#if 0
            int64_t dbgNumOdd = 0;
            int64_t dbgNumEven = 0;
            for (int y = 0; y < tileHeight; y++)
            {
                for (int x = 0; x < tileWidth; x++)
                {
                    if (minDistsForThisTile[x][y] != -1)
                    {
                        if (minDistsForThisTile[x][y] % 2 == 0)
                            dbgNumEven++;
                        else
                            dbgNumOdd++;
                    }
                }
            }
            assert(dbgNumOdd == numOddDistances);
            assert(dbgNumEven == numEvenDistances);
#endif
            if (maxDistInTile <= numStepsToTake)
            {
                // All cells in this tile are reachable in <= numStepsToTake.
                result += (numStepsToTake % 2 == 0) ? numEvenDistances : numOddDistances;
            }
            else if (minDistInTile > numStepsToTake) 
            {
                // No lower tiles are reachable in numStepsToTake steps.
                std::cout << "Bailing at: " << tileY << std::endl;
                break;
            }
            else
            {
                // Some of the cells in this tile are reachable.
                for (int y = 0; y < tileHeight; y++)
                {
                    for (int x = 0; x < tileWidth; x++)
                    {
                        if (repetitionBaseTileDists[x][y] != -1)
                        {
                            const int64_t minDistToCell = repetitionBaseTileDists[x][y] + tileHeight * (tileY - repetitionBaseTileY);
                            if (minDistToCell <= numStepsToTake && (minDistToCell % 2 == numStepsToTake % 2) )
                            {
                                result++;
                            }
                        }
                    }
                }
            }

            if (tileHeight % 2 != 0)
            {
                swap(numOddDistances, numEvenDistances);
            }
        }

        tileY++;
    }

    return result;
}

int64_t doBottomRight(const vector<vector<char>>& tileMap, int64_t numStepsToTake, const Coord& startCoord)
{
    std::cout << "doBottomRight" << std::endl;
    const int tileWidth = tileMap.size();
    const int tileHeight = tileMap.front().size();
    const auto shortestPathsFromStart = shortestPathInTileMap(startCoord, tileMap);
    const int64_t shortestDistToNextTileTL = shortestPathsFromStart[tileWidth - 1][tileHeight - 1] + 1 /* Down one */ + 1 /* Rightwards one */;
    std::cout << "shortestDistToNextTileTL: " << shortestDistToNextTileTL << std::endl;
    const auto shortestPathsFromTL = shortestPathInTileMap({0, 0}, tileMap);
    int64_t minDistInBaseTile = std::numeric_limits<int64_t>::max();
    int64_t maxDistInBaseTile = std::numeric_limits<int64_t>::min();
    int64_t numOddDistancesInBaseTile = 0;
    int64_t numEvenDistancesInBaseTile = 0;
    for (int x = 0; x < tileWidth; x++)
    {
        for (int y = 0; y < tileHeight; y++)
        {
            if (shortestPathsFromTL[x][y] != -1)
            {
                const int64_t distToCell = shortestPathsFromTL[x][y] + shortestDistToNextTileTL;
                minDistInBaseTile = std::min(minDistInBaseTile, distToCell);
                maxDistInBaseTile = std::max(maxDistInBaseTile, distToCell);
                if (distToCell % 2 == 0)
                    numEvenDistancesInBaseTile++;
                else
                    numOddDistancesInBaseTile++;
            }
        }
    }
    std::cout << "numOddDistancesInBaseTile: " << numOddDistancesInBaseTile << std::endl;
    std::cout << "numEvenDistancesInBaseTile: " << numEvenDistancesInBaseTile << std::endl;
    std::cout << "minDistInBaseTile: " << minDistInBaseTile << std::endl;
    std::cout << "maxDistInBaseTile: " << maxDistInBaseTile << std::endl;
#if 0
    int64_t dbgResult = 0;
    int tileY = 1;
    map<int64_t, int64_t> dbgResultIncreaseForDistOffset;
    std::cout << " less optimised" << std::endl;
    while (true)
    {
        int64_t distanceOffset = (tileY - 1) * tileHeight;
        std::cout << "tileY: " << tileY << " distanceOffset: " << distanceOffset << " minDistInBaseTile: " << minDistInBaseTile << std::endl;
        if (distanceOffset + minDistInBaseTile > numStepsToTake)
        {
            std::cout << " bailing at tileY: " << tileY << std::endl;
            break;
        }

        
        int tileX = 1;
        while (true)
        {
            //std::cout << " tileX: " << tileX << " distanceOffset: " << distanceOffset << std::endl;
            const int64_t numOddDistancesInTile = ((tileX + tileY) % 2 == 0) ? numOddDistancesInBaseTile : numEvenDistancesInBaseTile;
            const int64_t numEvenDistancesInTile = ((tileX + tileY) % 2 == 0) ? numEvenDistancesInBaseTile : numOddDistancesInBaseTile;
            //std::cout << "  numOddDistancesInTile: " << numOddDistancesInTile << " numEvenDistancesInTile: " << numEvenDistancesInTile << std::endl;
            const auto oldResult = dbgResult;

            if (distanceOffset + minDistInBaseTile > numStepsToTake)
            {
                // No cells in this tile are reachable in <= numStepsToTake.
                //std::cout << "   No cells in this tile are reachable in <= numStepsToTake." << std::endl;
                break;
            }
            else if (distanceOffset + maxDistInBaseTile <= numStepsToTake)
            {
                // All cells in this tile are reachable in <= numStepsToTake.
                //std::cout << "   All cells in this tile are reachable in <= numStepsToTake." << std::endl;
                dbgResult += (numStepsToTake % 2 == 0) ? numEvenDistancesInTile : numOddDistancesInTile;
            }
            else
            {
                // Some cells in this tile are reachable in <= numStepsToTake.
                std::cout << "   Some cells in this tile are reachable in <= numStepsToTake." << std::endl;
                std::cout << "    old dbgResult: " << dbgResult << std::endl;
                for (int x = 0; x < tileWidth; x++)
                {
                    for (int y = 0; y < tileHeight; y++)
                    {
                        if (shortestPathsFromTL[x][y] != -1)
                        {
                            const int64_t minDistToCell = shortestDistToNextTileTL + shortestPathsFromTL[x][y] + distanceOffset;
                            //std::cout << "     x: " << x << " y: " << y << " shortestPathsFromTL[x][y]: " << shortestPathsFromTL[x][y] << std::endl;
                            if ((minDistToCell <= numStepsToTake) && (minDistToCell % 2 == numStepsToTake % 2))
                            {
                                //std::cout << "      reached: " << (tileX * tileWidth + x) << ", " << (tileY * tileHeight + y) << " x in tile: " << x << " y in tile: " << y << " shortestPathsFromTL: " << shortestPathsFromTL[x][y] << " minDistToCell: " << minDistToCell << std::endl;
                                dbgResult++;
                            }
                        }
                    }
                }
                std::cout << "    new dbgResult: " << dbgResult << std::endl;
            }

            dbgResultIncreaseForDistOffset[distanceOffset] += (dbgResult - oldResult);

            tileX++;
            distanceOffset += tileWidth;
        }

        tileY++;
    }
#endif

    int64_t result = 0;
    int64_t distanceOffset = 0;
    assert(tileWidth == tileHeight);
    const int64_t tileLen = tileWidth;
    std::cout << " more optimised" << std::endl;
    while (true)
    {
        const int64_t numTilesWithThisDistOffset = distanceOffset / tileLen + 1;
        //int64_t numWaysToReachTile = (static_cast<int64_t>(1) << (distanceOffset / tileLen)) - (distanceOffset / tileLen);
        //int64_t numWaysToReachTile = (distanceOffset / tileLen + 1) * (distanceOffset / tileLen + 1);
        std::cout << "distanceOffset: " << distanceOffset << " numTilesWithThisDistOffset: " << numTilesWithThisDistOffset << std::endl;
        //std::cout << "numTimesEncounteredDistOffset: " << numTimesEncounteredDistOffset[distanceOffset] << std::endl;
        //assert(numTilesWithThisDistOffset == numTimesEncounteredDistOffset[distanceOffset]); 
        assert(distanceOffset % tileLen == 0);
        const int64_t numOddDistancesInTile = ((distanceOffset / tileLen) % 2 == 0) ? numOddDistancesInBaseTile : numEvenDistancesInBaseTile;
        const int64_t numEvenDistancesInTile = ((distanceOffset / tileLen) % 2 == 0) ? numEvenDistancesInBaseTile : numOddDistancesInBaseTile;
        const auto oldResult = result;

        if (distanceOffset + minDistInBaseTile > numStepsToTake)
        {
            // No cells in this tile are reachable in <= numStepsToTake.
            //std::cout << "   No cells in this tile are reachable in <= numStepsToTake." << std::endl;
            break;
        }
        else if (distanceOffset + maxDistInBaseTile <= numStepsToTake)
        {
            // All cells in this tile are reachable in <= numStepsToTake.
            //std::cout << "   All cells in this tile are reachable in <= numStepsToTake." << std::endl;
            result += ((numStepsToTake % 2 == 0) ? numEvenDistancesInTile : numOddDistancesInTile) * numTilesWithThisDistOffset;
        }
        else
        {
            // Some cells in this tile are reachable in <= numStepsToTake.
            std::cout << "   Some cells in this tile are reachable in <= numStepsToTake." << std::endl;
            std::cout << "    old result: " << result << std::endl;
            for (int x = 0; x < tileWidth; x++)
            {
                for (int y = 0; y < tileHeight; y++)
                {
                    if (shortestPathsFromTL[x][y] != -1)
                    {
                        const int64_t minDistToCell = shortestDistToNextTileTL + shortestPathsFromTL[x][y] + distanceOffset;
                        //std::cout << "     x: " << x << " y: " << y << " shortestPathsFromTL[x][y]: " << shortestPathsFromTL[x][y] << std::endl;
                        if ((minDistToCell <= numStepsToTake) && (minDistToCell % 2 == numStepsToTake % 2))
                        {
                            //std::cout << "      reached: " << (tileX * tileWidth + x) << ", " << (tileY * tileHeight + y) << " x in tile: " << x << " y in tile: " << y << " shortestPathsFromTL: " << shortestPathsFromTL[x][y] << " minDistToCell: " << minDistToCell << std::endl;
                            result += numTilesWithThisDistOffset;
                        }
                    }
                }
            }
            //std::cout << "    new result: " << result << std::endl;
        }

        //std::cout << " distanceOffset: " << distanceOffset << " dbgResultIncreaseForDistOffset: " << dbgResultIncreaseForDistOffset[distanceOffset] << " result increase: " << (result - oldResult) << std::endl;
        //assert(dbgResultIncreaseForDistOffset[distanceOffset] == (result - oldResult));

        distanceOffset += tileLen;
    }

    return result;
}

int64_t computeOptimised(const vector<vector<char>>& tileMapOrig, int64_t numStepsToTake)
{
    vector<vector<char>> tileMap = tileMapOrig;
    const int tileWidth = tileMap.size();
    const int tileHeight = tileMap.front().size();
    Coord startCoord;
    for (int x = 0; x < tileWidth; x++)
    {
        for (int y = 0; y < tileHeight; y++)
        {
            if (tileMap[x][y] == 'S')
            {
                startCoord = {x, y};
                tileMap[x][y] = '.';
            }
        }
    }
    int64_t result = 0;
    assert((startCoord != Coord({-1, -1})));
    const auto shortestPathsFromStart = shortestPathInTileMap(startCoord, tileMap);
    for (int x = 0; x < tileWidth; x++)
    {
        for (int y = 0; y < tileHeight; y++)
        {
            if (shortestPathsFromStart[x][y] != -1 && (shortestPathsFromStart[x][y] % 2 == numStepsToTake % 2))
            {
                result++;
            }

        }
    }

    assert(tileWidth == tileHeight);
    for (int rotationNum = 1; rotationNum <= 4; rotationNum++)
    {
        std::cout << "rotationNum: " << rotationNum << std::endl;
        std::cout << "tileMap: " << std::endl;
        for (int y = 0; y < tileHeight; y++)
        {
            for (int x = 0; x < tileWidth; x++)
            {
                if (Coord({x, y}) != startCoord)
                    std::cout << tileMap[x][y];
                else
                    std::cout << "S";
            }
            std::cout << std::endl;
        }
        result += doVerticalStripe(tileMap, numStepsToTake, startCoord);
        result += doBottomRight(tileMap, numStepsToTake, startCoord);
        std::cout << "result: " << result << std::endl;

        vector<vector<char>> tilesRotated(tileWidth, vector<char>(tileHeight, ' '));
        for (int x = 0; x < tileWidth; x++)
        {
            for (int y = 0; y < tileHeight; y++)
            {
                tilesRotated[tileWidth - 1 - y][x] = tileMap[x][y];
            }
        }
        startCoord = { tileWidth - 1 - startCoord.y, startCoord.x };
        tileMap = tilesRotated;

    }

    return result;
}

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
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            map[x][y] = rawMap[y][x];
        }
    }


#if 0
    set<Coord> topBoarderCoords;
    set<Coord> bottomBoarderCoords;
    for (int x = 0; x < width; x++)
    {
        topBoarderCoords.insert({x, 0});
        bottomBoarderCoords.insert({x, height - 1});
    }
#if 0
    for (int y = 0; y < height; y++)
    {
        boarderCoords.insert({0, y});
        boarderCoords.insert({width - 1, y});
    }
#endif
    for (const auto& coord1 : topBoarderCoords)
    {
        for (const auto& coord2 : bottomBoarderCoords)
        {
            if (coord1 != coord2)
            {
                std::cout << "Dist between border coords: " << shortestPath(coord1, coord2, map) << std::endl;
            }
        }
    }
#endif
#if 0
    for (int x = 1; x <= width - 2; x++)
    {
        bool hasClearPath = true;
        for (int y = 1; y <= height - 2; y++)
        {
            if (map[x][y] == '#')
            {
                hasClearPath = false;
                break;
            }
        }
        if (hasClearPath)
        {
            std::cout << "x: " << x << std::endl;
        }
        assert(!hasClearPath);
    }
    for (int y = 1; y <= height - 2; y++)
    {
        bool hasClearPath = true;
        for (int x = 1; x <= width - 2; x++)
        {
            if (map[x][y] == '#')
            {
                hasClearPath = false;
                break;
            }
        }
        assert(!hasClearPath);
    }
#endif

    //const auto resultUltraBruteForce = computeUltraBruteForce(map, 340);
    //std::cout << "resultUltraBruteForce: " << resultUltraBruteForce << std::endl;
    const auto shortestPaths = shortestPathInTileMap({0, 0}, map);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (shortestPaths[x][y] == -1 && (map[x][y] != '#'))
            {
                std::cout << " unreachable: " << " x: " << x << " y: " << y << std::endl;
            }
        }
    }
#if 0
    for (int numVerticalSteps = 10'000; numVerticalSteps <= 10'500; numVerticalSteps++)
    {
        std::cout << "numVerticalSteps: " << numVerticalSteps << std::endl;
        const int64_t verticalStripeBruteForce = doVerticalStripeBruteForce(map, numVerticalSteps);
        const int64_t verticalStripeOptimised = doVerticalStripe(map, numVerticalSteps);
        std::cout << "verticalStripeBruteForce: " << verticalStripeBruteForce << std::endl;
        std::cout << "verticalStripeOptimised: " << verticalStripeOptimised << std::endl;
        assert(verticalStripeBruteForce == verticalStripeOptimised);
    }
#endif

#if 0
    for (int numBottomRightSteps = 1; numBottomRightSteps <= 1000; numBottomRightSteps++)
    {
        std::cout << "numBottomRightSteps: " << numBottomRightSteps << std::endl;
        const int64_t bottomRightBruteForce = doBottomRightBruteForce(map, numBottomRightSteps);
        const int64_t bottomRightOptimised = doBottomRight(map, numBottomRightSteps);
        std::cout << "bottomRightBruteForce: " << bottomRightBruteForce << std::endl;
        std::cout << "bottomRightOptimised: " << bottomRightOptimised << std::endl;
        assert(bottomRightOptimised == bottomRightBruteForce);
    }
#endif
#if 1
    const int64_t numStepsToTake = 26501365;
    const auto resultOptimised = computeOptimised(map, numStepsToTake);
    std::cout << "resultOptimised: " << resultOptimised << std::endl;
#ifdef BRUTE_FORCE
    const auto resultBruteForce = computeBruteForce(map, numStepsToTake);
    std::cout << "resultOptimised: " << resultOptimised << std::endl;
    std::cout << "resultBruteForce: " << resultBruteForce << std::endl;
    assert(resultBruteForce == resultOptimised);
#endif
#endif
    return 0;
}
