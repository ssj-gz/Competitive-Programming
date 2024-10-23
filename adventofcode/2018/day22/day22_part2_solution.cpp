#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.
#include <iostream>
#include <regex>
#include <map>
#include <set>

#include <cassert>

using namespace std;

constexpr int erosionModulus = 20'183;

struct Coord
{
    int64_t x = -1;
    int64_t y = -1;
    auto operator<=>(const Coord& other) const = default;
};

enum Equipment
{
    Torch,
    ClimbingGear,
    Neither
};

enum TerrainType
{
    Rocky,
    Wet,
    Narrow
};

int calcErosionLevel(const Coord& coord, map<Coord, int64_t>& lookup, const Coord& targetCoord, const int64_t depth)
{
    assert(coord.x >= 0 && coord.y >= 0);
    if (lookup.contains(coord))
        return lookup[coord];

    auto erosionLevelForGeologicIndex = [depth](int64_t geologicIndex)
    {
        geologicIndex %= erosionModulus;
        return (geologicIndex + depth) % erosionModulus;
    };

    if (coord == Coord{0, 0})
        return erosionLevelForGeologicIndex(0);
    if (coord == targetCoord)
        return erosionLevelForGeologicIndex(0);

    if (coord.y == 0)
        return erosionLevelForGeologicIndex(coord.x * 16807);

    if (coord.x == 0)
        return erosionLevelForGeologicIndex(coord.y * 48271);


    const int64_t result = erosionLevelForGeologicIndex((calcErosionLevel({coord.x - 1, coord.y}, lookup, targetCoord, depth) *
        calcErosionLevel({coord.x, coord.y - 1}, lookup, targetCoord, depth)));
    lookup[coord] = result;
    return result;
}


int main()
{
    string depthDescription;
    getline(cin, depthDescription);
    std::smatch regexMatch;
    int64_t depth = -1;
    if (std::regex_match(depthDescription, regexMatch, std::regex(R"(^depth:\s*(\d+)\s*$)")))
    {
        depth = std::stoll(regexMatch[1]);
    }
    else
    {
        assert(false);
    }

    string targetCoordDescription;
    getline(cin, targetCoordDescription);
    int64_t targetX = -1;
    int64_t targetY = -1;

    if (std::regex_match(targetCoordDescription, regexMatch, std::regex(R"(^target:\s*(\d+)\s*,\s*(\d+)\s*$)")))
    {
        targetX = std::stoll(regexMatch[1]);
        targetY = std::stoll(regexMatch[2]);
    }
    else
    {
        assert(false);
    }
    std::cout << "depth: " << depth << " targetX: " << targetX << " targetY: " << targetY << std::endl;

    map<Coord, int64_t> erosionLevelLookup;

#if 0
    int64_t riskLevel = 0;
    for (int64_t x = 0; x <= targetX; x++)
    {
        for (int64_t y = 0; y <= targetY; y++)
        {
            erosionLevel[x][y] = calcErosionLevel({x, y}, geologicIndexLookup, {targetX, targetY}, depth);
            riskLevel += (erosionLevel[x][y] % 3);
            std::cout << "x: " << x << " y: " << y << " erosionLevel: " << erosionLevel[x][y] << std::endl;
        }
    }
    std::cout << "riskLevel: " << riskLevel << std::endl;
#endif

    struct State
    {
        Coord coord;
        Equipment equipment;
        auto operator<=>(const State& other) const = default;
    };

    auto canUseEquipmentAt = [&erosionLevelLookup, &targetX, &targetY, depth](const Coord& regionCoord, Equipment equipment)
    {
        const auto regionType = static_cast<Equipment>(calcErosionLevel(regionCoord, erosionLevelLookup, {targetX, targetY}, depth) % 3);
        switch (regionType)
        {
            case Rocky:
                return ((equipment == ClimbingGear) || (equipment == Torch));
            case Wet:
                return ((equipment == ClimbingGear) || (equipment == Neither));
            case Narrow:
                return ((equipment == Torch) || (equipment == Neither));
        };
        assert(false);
        return false;
    };

    const State initialState = {{0, 0}, Torch};
    map<int, set<State>> toExploreAtTime = { {0, {initialState} } };
    map<State, int> minTimeToReachState;
    minTimeToReachState[initialState] = 0;
    constexpr int timeToSwitchEquipment = 7;
    map<State, State> predecessor;

    for (const auto& [minute, toExplore] : toExploreAtTime)
    {
        std::cout << "minute: " << minute << " #toExploreAtTime: " << toExploreAtTime.size() << std::endl;
        for (const auto& state : toExplore)
        {
            for (const auto& [dx, dy] : std::initializer_list<std::pair<int, int>>{ {-1, 0},
                    {+1, 0},
                    {0, -1},
                    {0, +1}
                    })
            {
                auto addSuccessorState = [&minTimeToReachState, &toExploreAtTime, &minute, &state, &predecessor](const State& neighbourState, int reachNeighbourAtTime)
                {
                    if (!minTimeToReachState.contains(neighbourState) || reachNeighbourAtTime < minTimeToReachState[neighbourState])
                    {
                        minTimeToReachState[neighbourState] = reachNeighbourAtTime;
                        assert(reachNeighbourAtTime > minute);
                        toExploreAtTime[reachNeighbourAtTime].insert(neighbourState);
                        predecessor[neighbourState] = state;
                    }
                };
                if (state.coord.x == targetX && state.coord.y == targetY)
                {
                    if (state.equipment == Torch)
                    {
                        std::cout << "Woo-hoo - " << minute << std::endl;
                        State currentState = {{targetX, targetY}, Torch};
                        while (true)
                        {
                            std::cout << "state: " << currentState.coord.x << ", " << currentState.coord.y << " " << static_cast<int>(currentState.equipment) << " time: " << minTimeToReachState[currentState] << std::endl;
                            if (currentState == initialState)
                                break;
                            currentState = predecessor[currentState];
                        }
                        return 0;
                    }
                    else
                    {
                        std::cout << "Reached target, but don't have torch" << std::endl;
                        const int reachFinalAtTime = minute + timeToSwitchEquipment;
                        State finalState = { state.coord, Torch };
                        addSuccessorState(finalState, reachFinalAtTime);
                    }
                }
                else
                {
                    State neighbourState = {{state.coord.x + dx, state.coord.y + dy}, Torch};
                    if (neighbourState.coord.x < 0 || neighbourState.coord.y < 0)
                        continue;
                    {
                        for (const auto equipmentAtNeighbour : { Torch, ClimbingGear, Neither })
                        {
                            if (canUseEquipmentAt(neighbourState.coord, equipmentAtNeighbour) && canUseEquipmentAt(state.coord, equipmentAtNeighbour))
                            {
                                neighbourState.equipment = equipmentAtNeighbour;
                                const int reachNeighbourAtTime = minute + 1 + ((equipmentAtNeighbour == state.equipment) ? 0 : timeToSwitchEquipment);
                                addSuccessorState(neighbourState, reachNeighbourAtTime);
                            }
                        }
                    }
                }
            }

        }
    }


    return 0;
}
