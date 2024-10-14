#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <limits>

#include <cassert>

using namespace std;

struct Coord
{
    int64_t x = -1;
    int64_t y = -1;
    auto operator<=>(const Coord& other) const = default;
};

ostream& operator<<(ostream& os, const Coord& coord)
{
    os << "(" << coord.x << ", " << coord.y << ")";
    return os;
}

vector<vector<int64_t>> shortestPaths(const Coord& startCoord, const vector<vector<char>>& map)
{
    const int width = map.size();
    const int height = map.front().size();
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
                if (map[neighbourX][neighbourY] != '.')
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

int main()
{
    string rawMapLine;
    vector<string> rawMap;
    while(getline(cin, rawMapLine))
    {
        rawMap.push_back(rawMapLine);
    }
    const int width = rawMap.front().size();
    const int height = rawMap.size();

    struct Unit
    {
        int id = -1;
        enum Type { Goblin, Elf };
        Type type;
        Coord coord;
        bool isAlive = true;
        int hitPoints = 200;
    };

    vector<Unit> units;
    vector<vector<char>> landMap(width, vector<char>(height, ' '));
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            const char origCellContents = rawMap[y][x];
            const int nextUnitId = static_cast<int>(units.size());
            if (origCellContents == 'G')
            {
                units.push_back({nextUnitId, Unit::Goblin, {x, y}});
                landMap[x][y] = '.';
            }
            else if (origCellContents == 'E')
            {
                units.push_back({nextUnitId, Unit::Elf, {x, y}});
                landMap[x][y] = '.';
            }
            else
                landMap[x][y] = origCellContents;

        }
    }
    const int initialNumElves = std::count_if(units.begin(), units.end(), [](const auto& unit) { return unit.isAlive && unit.type == Unit::Elf; });
    const auto initialUnits = units;

    // Directions are in reading order.
    auto directions = std::initializer_list<std::pair<int, int>>{ 
        {0, -1}, // Up.
            {-1, 0}, // Left.
            {+1, 0}, // Right.
            {0, +1}  // Down.
    };

    constexpr int goblinAttackPower = 3;
    int elfAttackPower = 1;
    while (true)
    {
        std::cout << "Trying with elfAttackPower: " << elfAttackPower << std::endl;
        units = initialUnits;
        bool combatEnded = false;
        int64_t outcome = -1;
        int round = 1;
        while (!combatEnded)
        {
            std::cout << "Beginning round: " << round << std::endl;
            // Arrange Units into "reading order".
            sort(units.begin(), units.end(), [](const auto& lhsUnit, const auto& rhsUnit)
                    {
                    if (lhsUnit.coord.y != rhsUnit.coord.y)
                    return lhsUnit.coord.y < rhsUnit.coord.y;
                    return lhsUnit.coord.x < rhsUnit.coord.x;
                    });

            for (auto unitIter = units.begin(); unitIter != units.end(); unitIter++)
            {
                auto& unit = *unitIter;
                if (!unit.isAlive)
                {
                    std::cout << " skipping dead Unit# " << unit.id << std::endl;
                    continue;
                }
                else
                {
                    std::cout << " Unit# " << unit.id << " " << unit.coord << " takes turn" << std::endl;
                }
                std::cout << "  full map: " << std::endl;
                vector<vector<char>> fullMap = landMap;
                for (const auto& unit : units)
                {
                    if (unit.isAlive)
                        fullMap[unit.coord.x][unit.coord.y] = (unit.type == Unit::Goblin ? 'G' : 'E');
                }
                for (int y = 0; y < height; y++)
                {
                    for (int x = 0; x < width; x++)
                    {
                        std::cout << fullMap[x][y];
                    }
                    std::cout << std::endl;
                }
                for (const auto& unit : units)
                {
                    std::cout << "unit: " << unit.id << "(" << (unit.type == Unit::Goblin ? "G" : "E") << ")" << " @ " << unit.coord << " hitPoints: " << unit.hitPoints << std::endl;
                }

                bool isInRange = false;

                auto otherLiveUnits = [&units, &unit]()
                {
                    vector<Unit*> ret;
                    for (auto& otherUnit : units)
                    {
                        if (otherUnit.isAlive && &otherUnit != &unit)
                            ret.push_back(&otherUnit);
                    }
                    return ret;
                };

                bool foundTarget = false;
                for (const auto* enemyUnit : otherLiveUnits())
                {
                    if (enemyUnit->type == unit.type)
                        continue;
                    foundTarget = true;
                }
                if (!foundTarget)
                {
                    std::cout << " combat ends; " << (round - 1) << " full rounds were played" << std::endl;
                    std::cout << " combat ends; " << (round - 1) << " full rounds were played" << std::endl;
                    int64_t totalHitPoints = 0;
                    for (const auto& unit : units)
                    {
                        if (unit.isAlive)
                        {
                            std::cout << "Adding " << unit.hitPoints << " to hit points "<< std::endl;
                            totalHitPoints += unit.hitPoints;
                        }
                    }
                    outcome = (round - 1) * totalHitPoints;
                    std::cout << "result: " << outcome << std::endl;

                    combatEnded = true;
                    break;
                }

                for (const auto& [dx, dy] : directions)
                {
                    for (const auto* enemyUnit : otherLiveUnits())
                    {
                        if (enemyUnit->type == unit.type)
                            continue;
                        if ((enemyUnit->coord.x == unit.coord.x + dx) && (enemyUnit->coord.y == unit.coord.y + dy))
                            isInRange = true;
                    }
                }
                if (!isInRange)
                {
                    // Need to move.
                    std::cout << "  not yet in range; attempting to move" << std::endl;
                    const auto shortestPathsFromUnit = shortestPaths(unit.coord, fullMap);
                    vector<Coord> potentialTargets;
                    for (const auto* enemyUnit : otherLiveUnits())
                    {
                        if (enemyUnit->type == unit.type)
                            continue;
                        for (const auto& [dx, dy] : directions)
                        {
                            const Coord inRangeCoord = {enemyUnit->coord.x + dx, enemyUnit->coord.y + dy};
                            if (shortestPathsFromUnit[inRangeCoord.x][inRangeCoord.y] != -1)
                            {
                                potentialTargets.push_back(inRangeCoord);
                            }
                        }
                    }

                    sort(potentialTargets.begin(), potentialTargets.end(), [&shortestPathsFromUnit](const auto& lhsCoord, const auto& rhsCoord)
                            {
                            if (shortestPathsFromUnit[lhsCoord.x][lhsCoord.y] != shortestPathsFromUnit[rhsCoord.x][rhsCoord.y])
                            return shortestPathsFromUnit[lhsCoord.x][lhsCoord.y] < shortestPathsFromUnit[rhsCoord.x][rhsCoord.y];
                            if (lhsCoord.y != rhsCoord.y)
                            return lhsCoord.y < rhsCoord.y;
                            return lhsCoord.x < rhsCoord.x;
                            });
                    if (!potentialTargets.empty())
                    {
                        const auto targetCoord = *potentialTargets.begin();
                        const auto distToTarget = shortestPathsFromUnit[targetCoord.x][targetCoord.y];
                        fullMap[unit.coord.x][unit.coord.y] = '.';
                        const auto shortestPathsFromTarget = shortestPaths(targetCoord, fullMap);
                        bool moved = false;
                        std::cout << "targetCoord: " << targetCoord << " distToTarget: " << distToTarget << std::endl;
                        for (const auto& [dx, dy] : directions)
                        {
                            const Coord nextStepCoord = {unit.coord.x + dx, unit.coord.y + dy};
                            if (shortestPathsFromTarget[nextStepCoord.x][nextStepCoord.y] != -1 && shortestPathsFromTarget[nextStepCoord.x][nextStepCoord.y] == distToTarget - 1)
                            {
                                // This is the next square we should move to.
                                unit.coord.x += dx;
                                unit.coord.y += dy;
                                std::cout << "  moved to " << unit.coord << std::endl;
                                moved = true;
                                break;
                            }
                        }
                        assert(moved);
                        if (unit.coord == targetCoord)
                            isInRange = true;
                    }
                    std::cout << "   no coords in range" << std::endl;
                }

                if (isInRange)
                {
                    std::cout << "  attacking - choosing target" << std::endl;
                    // Either started in range, or moved into range this turn.
                    // Either way, begin attacking.
                    vector<Unit*> targets;
                    for (const auto& [dx, dy] : directions)
                    {
                        const Coord targetCoord = { unit.coord.x + dx, unit.coord.y + dy };
                        for (auto* enemyUnit : otherLiveUnits())
                        {
                            if (enemyUnit->type == unit.type)
                                continue;
                            if (enemyUnit->coord == targetCoord)
                                targets.push_back(enemyUnit);
                        }
                    }

                    sort(targets.begin(), targets.end(), [](const auto* lhsUnit, const auto* rhsUnit)
                            {
                            if (lhsUnit->hitPoints != rhsUnit->hitPoints)
                            return lhsUnit->hitPoints < rhsUnit->hitPoints;
                            if (lhsUnit->coord.y != rhsUnit->coord.y)
                            return lhsUnit->coord.y < rhsUnit->coord.y;
                            return lhsUnit->coord.x < rhsUnit->coord.x;
                            });
                    if (!targets.empty())
                    {
                        const int attackPower = (unit.type == Unit::Elf ? elfAttackPower : goblinAttackPower);
                        auto* target = targets.front();
                        std::cout << "   chosen unit #" << target->id << " as target" << target->id << std::endl;
                        target->hitPoints -= attackPower;
                        if (target->hitPoints <= 0)
                        {
                            target->isAlive = false;
                            std::cout << "   killed unit #" << target->id << target->id << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "  no targets???" << std::endl;
                        assert(false);
                    }
                }
            }

            round++;
        }
        const int numElvesRemaining = std::count_if(units.begin(), units.end(), [](const auto& unit) { return unit.isAlive && unit.type == Unit::Elf; });
        if (numElvesRemaining != initialNumElves)
        {
            std::cout << "No good; elves died" << std::endl;
            elfAttackPower++;
        }
        else
        {
            std::cout << "Hooray: elfAttackPower: " << elfAttackPower << " will do: outcome: " << outcome << std::endl;
            break;
        }
    }

    return 0;
}
