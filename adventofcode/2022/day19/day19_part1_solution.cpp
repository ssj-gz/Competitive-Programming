#include <iostream>
#include <vector>
#include <set>
#include <array>
#include <regex>
#include <limits>
#include <cassert>

using namespace std;

enum Resource { Ore = 0, Clay = 1, Obsidian = 2, Geode = 3 };
constexpr int numResourceTypes = 4;
std::string_view resourceTypeNames[] = { "Ore", "Clay", "Obsidian", "Geode" };

struct Blueprint
{
    std::array<int64_t, numResourceTypes> costOfRobot[numResourceTypes] = {};
    int blueprintNumber = -1;
};

struct State
{
    int64_t amountOfResource[numResourceTypes] = {};
    int64_t amountOfRobot[numResourceTypes] = {};
    //bool isRobotBeingBuilt[numResourceTypes]= {};
    auto operator<=>(const State& other) const = default;
};

ostream& operator<<(ostream& os, const State& state)
{
    os << "State: amount of: ";
    for (int resourceType = 0; resourceType < numResourceTypes; resourceType++)
    {
        os << " " << resourceTypeNames[resourceType] << " = " << state.amountOfResource[resourceType];
    }
    os << " amount of robot: ";
    for (int resourceType = 0; resourceType < numResourceTypes; resourceType++)
    {
        os << " " << resourceTypeNames[resourceType] << " = " << state.amountOfRobot[resourceType];
    }
#if 0
    os << " building robot? ";
    for (int resourceType = 0; resourceType < numResourceTypes; resourceType++)
    {
        os << " " << resourceTypeNames[resourceType] << " = " << state.isRobotBeingBuilt[resourceType];
    }
#endif
    return os;
};

int64_t estimateMaxPossibleExtraGeodes(int initialNumObsidianRobots, int initialObsidian, int initialNumGeodeRobots, int timeRemaining, const Blueprint& bluePrint)
{
    if (timeRemaining == -1)
        return 0;
    static std::map<std::tuple<int, int, int, int, int>, int> lookup;
    std::tuple<int, int, int, int, int> paramsAsTuple = { initialNumObsidianRobots, initialObsidian, initialNumGeodeRobots, timeRemaining, bluePrint.blueprintNumber };
    if (lookup.contains(paramsAsTuple))
        return lookup[paramsAsTuple];

    const int64_t numNewGeodes = initialNumGeodeRobots;
    // Assume we can buy an Obsidian robot every minute.
    const int64_t extraFromBuyingObsidiean = estimateMaxPossibleExtraGeodes(initialNumObsidianRobots + 1, initialObsidian + initialNumObsidianRobots, initialNumGeodeRobots, timeRemaining - 1, bluePrint);
    int64_t extraFromBuyingGeode = 0;
    if (initialObsidian >= bluePrint.costOfRobot[Geode][Obsidian])
    {
        //std::cout << "timeRemaining: " << timeRemaining << " can buy extra geode robot!" << std::endl;
        extraFromBuyingGeode = estimateMaxPossibleExtraGeodes(initialNumObsidianRobots, initialObsidian - bluePrint.costOfRobot[Geode][Obsidian] + initialNumObsidianRobots, initialNumGeodeRobots + 1, timeRemaining - 1, bluePrint);
        //std::cout << " extraFromBuyingGeode: " <<  extraFromBuyingGeode  << std::endl;
    }
    else
    {
        //std::cout << "timeRemaining: " << timeRemaining << " CAN'T buy extra geode robot - costs " << bluePrint.costOfRobot[Geode][Obsidian] << " but only have: " << initialObsidian << std::endl;
    }

    int64_t result = numNewGeodes + std::max(extraFromBuyingGeode, extraFromBuyingObsidiean);
    lookup[paramsAsTuple] = result;
    return result;
}

int main()
{


    vector<Blueprint> blueprints;
    string blueprintInfoLine;
    std::regex blueprintInfoRegex("^Blueprint\\s*(\\d+):\\sEach ore robot costs (\\d+) ore.\\s*Each clay robot costs (\\d+) ore.\\s*Each obsidian robot costs (\\d+) ore and (\\d+) clay.\\s*Each geode robot costs (\\d+) ore and (\\d+) obsidian\\.?\\s*$");
    while (std::getline(cin, blueprintInfoLine))
    {
        std::cout << "blueprintInfoLine: " << blueprintInfoLine << std::endl;
        std::smatch blueprintInfoMatch;
        const bool matchSuccessful = std::regex_match(blueprintInfoLine, blueprintInfoMatch, blueprintInfoRegex);
        assert(matchSuccessful);
        Blueprint blueprintInfo;
        blueprintInfo.costOfRobot[Ore] = { std::stoll(blueprintInfoMatch[2]), 0, 0, 0 };
        blueprintInfo.costOfRobot[Clay] = { std::stoll(blueprintInfoMatch[3]), 0, 0, 0 };
        blueprintInfo.costOfRobot[Obsidian] = { std::stoll(blueprintInfoMatch[4]), std::stoll(blueprintInfoMatch[5]), 0, 0 };
        blueprintInfo.costOfRobot[Geode] = { std::stoll(blueprintInfoMatch[6]), 0, std::stoll(blueprintInfoMatch[7]), 0 };
        blueprintInfo.blueprintNumber = blueprints.size() + 1;

        blueprints.push_back(blueprintInfo);
    }
    // blueprint: 1time: 22 mostGeodes: 5 maxPossibleGeodes: 0 minCompetitorGeodes: 16 pruning state: State: amount of:  Ore = 4 Clay = 19 Obsidian = 11 Geode = 0 amount of robot:  Ore = 2 Clay = 5 Obsidian = 3 Geode = 0
    std::cout << "blah: " << estimateMaxPossibleExtraGeodes(3, 11, 0, 2, blueprints[0]) << std::endl;
    //return 0;

    int64_t qualitySum = 0;
    constexpr int timeLimit = 24;
    for (const auto& bluePrint : blueprints)
    {
        int64_t maxAmountOfResourceToBuyRobot[numResourceTypes] = {};
            for (int resourceType = 0; resourceType < numResourceTypes; resourceType++)
        {
            for (int robotType = 0; robotType < numResourceTypes; robotType++)
            {
                maxAmountOfResourceToBuyRobot[resourceType] = std::max<int64_t>(maxAmountOfResourceToBuyRobot[resourceType], bluePrint.costOfRobot[robotType][resourceType]);
            }
        }
        for (int resourceType = 0; resourceType < numResourceTypes; resourceType++)
        {
            std::cout << "Max of " << resourceTypeNames[resourceType] << " to buy a robot: " << maxAmountOfResourceToBuyRobot[resourceType] << std::endl;
        }
        // We never spend Geodes, so don't want to cap the amount of Geode-producing robots.
        maxAmountOfResourceToBuyRobot[Geode] = std::numeric_limits<int64_t>::max();

        State initialState;
        initialState.amountOfRobot[Ore] = 1;

        vector<State> toExplore = { initialState };
        set<State> seenStates;
        int time = 0;
        int64_t mostGeodes = 0;
        int64_t earliestTimeOfMostGeodes = -1;
        while (!toExplore.empty())
        {
            std::cout << "time: " << time << " #toExplore: " << toExplore.size() << std::endl;
            vector<State> nextToExplore;
            for (const auto& state : toExplore)
            {
                if (seenStates.contains(state))
                    continue;
                // Prune.
                // For maxPossibleGeodes, assume we can buy a Geode-producing Robot every minute until the timeLimit.
                const int64_t timeRemaining = timeLimit - time;
                if ((mostGeodes > 0) && timeRemaining <= 10)
                {
                    const int64_t maxPossibleGeodes = state.amountOfResource[Geode] + estimateMaxPossibleExtraGeodes(state.amountOfRobot[Obsidian], state.amountOfResource[Obsidian], state.amountOfRobot[Geode], timeRemaining, bluePrint);
                    const int64_t minCompetitorGeodes = mostGeodes + (timeLimit - earliestTimeOfMostGeodes);
                    if (maxPossibleGeodes <= minCompetitorGeodes)
                    {
                        //std::cout << " blueprint: " << blueprintNumber << " time: " << time << " most (competitor) Geodes: " << mostGeodes << " earliestTimeOfMostGeodes: " << earliestTimeOfMostGeodes << " maxPossibleGeodes: " << maxPossibleGeodes << " minCompetitorGeodes: " << minCompetitorGeodes << " pruning state: " << state << std::endl;
                        continue;
                    }
                }
                seenStates.insert(state);
                State nextState = state;
                if (rand() % 100'000 == 0)
                {
                    cout << "time: " << time << " blueprint: " << bluePrint.blueprintNumber << " " << state << std::endl;
                }
                //cout << state << endl;
                // Collect.
                for (int resourceType = 0; resourceType < numResourceTypes; resourceType++)
                {
                    nextState.amountOfResource[resourceType] += nextState.amountOfRobot[resourceType];
                }
                // Cap - we don't need more of a given resource R than the cost to buy the most expensive
                // resource (in terms of R) every single minute (including this one) until the end.
                for (int resourceType = 0; resourceType < numResourceTypes; resourceType++)
                {
                    nextState.amountOfResource[resourceType] = std::min(nextState.amountOfResource[resourceType], (timeLimit - time + 1) * maxAmountOfResourceToBuyRobot[resourceType]);
                }

#if 0
                // Finish building.
                for (int resourceType = 0; resourceType < numResourceTypes; resourceType++)
                {
                    if (state.isRobotBeingBuilt[resourceType])
                    {
                        nextState.amountOfRobot[resourceType]++;
                        nextState.isRobotBeingBuilt[resourceType] = false;
                    }
                }
#endif
                // Build?
                for (int robotType = 0; robotType < numResourceTypes; robotType++)
                {
                    bool canBuild = true;
                    for (int resourceType = 0; resourceType < numResourceTypes; resourceType++)
                    {
                        if (state.amountOfResource[resourceType] < bluePrint.costOfRobot[robotType][resourceType])
                            canBuild = false;
                    }
                    if (canBuild)
                    {
                        State buildingRobotState = nextState;
                        //buildingRobotState.isRobotBeingBuilt[robotType] = true;
                        buildingRobotState.amountOfRobot[robotType]++;
                        for (int resourceType = 0; resourceType < numResourceTypes; resourceType++)
                        {
                            buildingRobotState.amountOfResource[resourceType] -= bluePrint.costOfRobot[robotType][resourceType];
                            assert(buildingRobotState.amountOfResource[resourceType] >= 0);
                        }
                        // Cap - we don't need more robots than are necessary to build a robot each minute.
                        buildingRobotState.amountOfRobot[robotType] = std::min(buildingRobotState.amountOfRobot[robotType], maxAmountOfResourceToBuyRobot[robotType]);
                        nextToExplore.push_back(buildingRobotState);
                    }
                }
                // Don't build.
                nextToExplore.push_back(nextState);

                if (mostGeodes < nextState.amountOfResource[Geode])
                {
                    mostGeodes = nextState.amountOfResource[Geode];
                    earliestTimeOfMostGeodes = time;
                    std::cout << "New mostGeodes: " << mostGeodes << std::endl;
                }
            }

            toExplore = nextToExplore;
            time++;
            if (time == timeLimit)
            {
                std::cout << "mostGeodes: " << mostGeodes << std::endl;
                qualitySum += mostGeodes * bluePrint.blueprintNumber;
                break;
            }
        }


    }
    std::cout << "qualitySum: " << qualitySum << std::endl;

}
