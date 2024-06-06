#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <regex>
#include <ranges>
#include <cassert>

using namespace std;

struct Valve
{
    string label;
    vector<Valve*> neighbours;
    int flowRate = 0;
    int valveIndex = -1;

    vector<string> neighbourLabels;
};

ostream& operator<<(ostream& os, const Valve* valve)
{
    if (!valve)
        os << "NONE";
    else
        os << valve->label;
    return os;
}

void updateMinDistances(const Valve* startValve, map<const Valve*, map<const Valve*, int>>& minDistanceBetweenValves)
{
    vector<const Valve*> toExplore = { startValve };
    set<const Valve*> seen = { startValve };
    int distance = 1;
    while (!toExplore.empty())
    {
        vector<const Valve*> nextToExplore;

        for (const auto* valve : toExplore)
        {
            for (const auto* neigbourValve : valve->neighbours)
            {
                if (!seen.contains(neigbourValve))
                {
                    if (neigbourValve->flowRate > 0)
                    {
                        minDistanceBetweenValves[startValve][neigbourValve] = distance;
                        minDistanceBetweenValves[neigbourValve][startValve] = distance;
                    }
                    seen.insert(neigbourValve);
                    nextToExplore.push_back(neigbourValve);
                }
            }
        }

        distance++;
        toExplore = nextToExplore;
    }
}

class ValveSet
{
    public:
        ValveSet& addValve(const Valve* valveToAdd)
        {
            assert(!hasValve(valveToAdd));
            m_valveBits |= bitForValve(valveToAdd);
            m_totalFlowRate += valveToAdd->flowRate;
            return *this;
        };
        bool hasValve(const Valve* valveToCheck) const
        {
            return m_valveBits & bitForValve(valveToCheck);
        }
        int64_t totalFlowRate() const
        {
            return m_totalFlowRate;
        }
        auto operator<=>(const ValveSet& other) const = default;
    private:
        uint64_t m_valveBits = 0;
        int64_t m_totalFlowRate = 0;

        uint64_t bitForValve(const Valve* valve) const
        {
            assert(valve != nullptr);
            uint64_t keyBit = static_cast<uint64_t>(1) << static_cast<uint64_t>(valve->valveIndex);
            return keyBit;
        }
};


int main()
{
    vector<Valve> valves;
    string valveInfoLine;
    std::regex valveInfoRegex("^Valve ([^ ]+) has flow rate=([-\\d]+); tunnels? leads? to valves? (.*)$");
    int valveIndex = 0;
    int maxPossibleFlowRate = 0;
    while (std::getline(cin, valveInfoLine))
    {
        std::smatch valveInfoMatch;
        const bool matchSuccessful = std::regex_match(valveInfoLine, valveInfoMatch, valveInfoRegex);
        assert(matchSuccessful);
        Valve valveInfo;
        valveInfo.valveIndex = valveIndex;
        valveInfo.label = valveInfoMatch[1];
        valveInfo.flowRate = std::stoll(valveInfoMatch[2]);
        // Trimming and splitting a string is still clunky as hell, here in YToOL 2024.
        auto neighbourLabelsRange = std::views::split(std::string(valveInfoMatch[3]), ',');
        for (const auto& neighbourLabel : neighbourLabelsRange)
        {
            string neighbourLabelTrimmed(neighbourLabel.begin(), neighbourLabel.end());
            std::erase(neighbourLabelTrimmed, ' ');
            valveInfo.neighbourLabels.push_back(neighbourLabelTrimmed);
        }
        valves.push_back(valveInfo);

        maxPossibleFlowRate += valveInfo.flowRate;

        valveIndex++;
    }

    // Populate each Valve::neighbours.
    Valve* startingValve = nullptr;
    for (auto& valve : valves)
    {
        if (valve.label == "AA")
            startingValve = &valve;
        for (const auto& neighbourLabel : valve.neighbourLabels)
        {
            const auto neighbourIter = std::find_if(valves.begin(), valves.end(), [neighbourLabel](const auto& otherValve) { return otherValve.label == neighbourLabel; });
            assert(neighbourIter != valves.end());
            valve.neighbours.push_back(&(*neighbourIter));
        }
    }
    assert(startingValve != nullptr);

    // Build minDistanceBetweenValves map.
    map<const Valve*, map<const Valve*, int>> minDistanceBetweenValves;
    for (const auto& valve : valves)
    {
        if (valve.flowRate != 0)
            updateMinDistances(&valve, minDistanceBetweenValves);
    }
    updateMinDistances(startingValve, minDistanceBetweenValves);
    for (const auto [valve, minDistanceFromValve] : minDistanceBetweenValves)
    {
        for (const auto [otherValve, distance] : minDistanceFromValve)
        {
            std::cout << "Min distance between " << valve->label << " and " << otherValve->label << " is: " << distance << std::endl;
        }
    }
    vector<const Valve*> valvesWithNonZeroRate;
    for (const auto& valve : valves)
    {
        if (valve.flowRate > 0)
            valvesWithNonZeroRate.push_back(&valve);
    }
    std::cout << "#valvesWithNonZeroRate: " << valvesWithNonZeroRate.size() << std::endl;

    std::cout << "maxPossibleFlowRate: " << maxPossibleFlowRate << std::endl;
    
    constexpr int numActors = 2; // Man and elephant.
    struct State
    {
        const Valve* actorTargetValves[numActors] = { nullptr, nullptr };
        int actorDistsToTargetValves[numActors] = { -1, -1 };
        ValveSet openValves;
        // Pressure released is deliberately omitted; instead, it's 
        // stored externally, in highestPressureForStateAtMinute.
        auto operator<=>(const State& other) const = default;
    };
    //const Valve* actorTargetValves[numActors] = { nullptr, nullptr };
    //actorTargetValves[0] = nullptr;

    State initialState = { {startingValve, startingValve}, {0, 0} };
    initialState.openValves.addValve(startingValve); // A bit of a hack - just want to prevent attempting to open the startingValve.

    set<ValveSet> valveSets;
    constexpr int timeLimit = 26;
    vector<map<State, int>> highestPressureForStateAtMinute(timeLimit + 1);
    map<State, int> highestPressureForState;
    highestPressureForStateAtMinute[1] = { { initialState, 0} };
    int time = 1;
    int64_t highestPressureReleased = 0;
    int numSkipped = 0;
    while (time <= timeLimit)
    {
        auto& toExplore = highestPressureForStateAtMinute[time];
        std::cout << "time: " << time << " #toExplore: " << toExplore.size() << " #valveSets: "<< valveSets.size() << " numSkipped: " << numSkipped << " highestPressureReleased: " << highestPressureReleased << std::endl;
        for (auto& [origState, pressureReleased] : toExplore)
        {
            //std::cout << "Exploring a state: targetValve[0]: " << origState.actorTargetValves[0] << " targetValve[1]: " << origState.actorTargetValves[1] << " actorDistsToTargetValves[0]: " << origState.actorDistsToTargetValves[0] << " actorDistsToTargetValves[1]: " << origState.actorDistsToTargetValves[1] << std::endl; 
            assert(origState.actorDistsToTargetValves[0] >= 0);
            assert(origState.actorDistsToTargetValves[1] >= 0);
            if (highestPressureForState.contains(origState) && highestPressureForState[origState] >= pressureReleased)
            {
                //std::cout << "Skipping" << std::endl;
                numSkipped++;
                //continue;
            }
            else
            {
                highestPressureForState[origState] = pressureReleased;
            }
            auto handleNewState = [&highestPressureForStateAtMinute, &parentState = origState, pressureReleased, parentStateTime = time, &valveSets](State& newState, const int newStateTime)
            {
                // Assumes newState.pressureReleased has not yet been updated.
                assert(parentStateTime < newStateTime);
                const int64_t pressureReleasedAtNewState = pressureReleased + (newStateTime - parentStateTime) * parentState.openValves.totalFlowRate();
                auto& currentHighestPressureForNewState = highestPressureForStateAtMinute[newStateTime][newState];
                if (currentHighestPressureForNewState < pressureReleasedAtNewState)
                {
                    currentHighestPressureForNewState = pressureReleasedAtNewState;
                    valveSets.insert(newState.openValves);
                }
            };
            if (time != timeLimit)
            {
                // Yuck.
                State state = origState;
                const Valve* actorPositions[] = { nullptr, nullptr };
                for (int actorIndex = 0; actorIndex < numActors; actorIndex++)
                {
                    if (state.actorDistsToTargetValves[actorIndex] == 0)
                    {
                        actorPositions[actorIndex] = state.actorTargetValves[actorIndex];
                        state.actorTargetValves[actorIndex] = nullptr;
                    }
                }
                for (bool actor1OpensValve : { true, false })
                {
                    for (bool actor2OpensValve : { true, false })
                    {
                        //std::cout << "actor1OpensValve: " << actor1OpensValve << " actor2OpensValve: " << actor2OpensValve << std::endl;
                        bool actorOpensValve[] = { actor1OpensValve, actor2OpensValve };
                        bool invalidCombination = false;
                        for (int actorIndex = 0; actorIndex < numActors; actorIndex++)
                        {
                            if (actorOpensValve[actorIndex])
                            {
                                if ((actorPositions[actorIndex] == nullptr) || state.openValves.hasValve(actorPositions[actorIndex]))
                                {
                                    // Can't open this valve, so this combination is invalid.
                                    invalidCombination = true;
                                }
                            }
                        }
                        if (actor1OpensValve && actor2OpensValve && (actorPositions[0] == actorPositions[1]))
                        {
                            // Can't both open the same valve, so this combination is invalid.
                            invalidCombination = true;
                        }
                        if (invalidCombination)
                        {
                            //std::cout << "invalid combination of valve openings" << std::endl;
                            continue; // Move on to the next combination.
                        }
                        else
                        {
                            //std::cout << "Valid combination of valve openings :)" << std::endl;
                        }
                        State nextState = state;
                        if (actor1OpensValve)
                        {
                            nextState.openValves.addValve(actorPositions[0]);
                        }
                        if (actor2OpensValve)
                        {
                            nextState.openValves.addValve(actorPositions[1]);
                        }
                        if (!actor1OpensValve && !actor2OpensValve)
                        {
                            //std::cout << "Neither opens valve, so both must move" << std::endl;
                        }
                        vector<State> actorNextStates[numActors];
                        for (int actorIndex = 0; actorIndex < numActors; actorIndex++)
                        {
                            if (actorOpensValve[actorIndex])
                            {
                                for (const auto* targetValve : valvesWithNonZeroRate)
                                {
                                    if ((targetValve == actorPositions[actorIndex]) || nextState.openValves.hasValve(targetValve))
                                        continue;

                                    assert(actorPositions[actorIndex]);
                                    State stateToAdd = nextState;
                                    stateToAdd.actorDistsToTargetValves[actorIndex] = minDistanceBetweenValves[actorPositions[actorIndex]][targetValve];
                                    stateToAdd.actorTargetValves[actorIndex] = targetValve;
                                    actorNextStates[actorIndex].push_back(stateToAdd);
                                }
                            }
                            else if (nextState.actorTargetValves[actorIndex] == nullptr)
                            {
                                for (const auto* targetValve : valvesWithNonZeroRate)
                                {
                                    if ((targetValve == actorPositions[actorIndex]) || nextState.openValves.hasValve(targetValve))
                                        continue;
                                    assert(actorPositions[actorIndex]);
                                    State stateToAdd = nextState;
                                    //stateToAdd.actorDistsToTargetValves[actorIndex] = minDistanceBetweenValves[actorPositions[actorIndex]][targetValve] - (actorPositions[actorIndex] != startingValve ? +1 : 0);
                                    stateToAdd.actorDistsToTargetValves[actorIndex] = minDistanceBetweenValves[actorPositions[actorIndex]][targetValve] - 1;
                                    stateToAdd.actorTargetValves[actorIndex] = targetValve;
                                    actorNextStates[actorIndex].push_back(stateToAdd);
                                }
                            }
                            else if (nextState.actorTargetValves[actorIndex] != nullptr && nextState.actorDistsToTargetValves[actorIndex] >= 1)
                            {
                                State stateToAdd = nextState;
                                stateToAdd.actorDistsToTargetValves[actorIndex]--;
                                actorNextStates[actorIndex].push_back(stateToAdd);
                            }
#if 0
                            else if (nextState.actorTargetValves[actorIndex] != nullptr && nextState.actorDistsToTargetValves[actorIndex] == 1)
                            {
                                for (const auto* targetValve : valvesWithNonZeroRate)
                                {
                                    if ((targetValve == actorPositions[actorIndex]) || nextState.openValves.hasValve(targetValve))
                                        continue;

                                    nextState.actorDistsToTargetValves[actorIndex] = minDistanceBetweenValves[actorPositions[actorIndex]][targetValve];
                                    nextState.actorTargetValves[actorIndex] = targetValve;
                                    actorNextStates[actorIndex].push_back(stateToAdd);
                                }
                            }
#endif
                            else
                            {
                                std::cout << "Uh-oh! actorIndex: " << actorIndex << " actorOpensValve: " << actorOpensValve[actorIndex] << " nextState.actorTargetValves[actorIndex]: " << nextState.actorTargetValves[actorIndex] << " nextState.actorDistsToTargetValves[actorIndex]: " << nextState.actorDistsToTargetValves[actorIndex] << std::endl;
                                assert(false);
                            }
                            //assert(!actorNextStates[actorIndex].empty());
                        }
                        //std::cout << "# next states for actor 1: " << actorNextStates[0].size() << std::endl;
                        //std::cout << "# next states for actor 2: " << actorNextStates[1].size() << std::endl;
                        if (actorNextStates[0].empty() || actorNextStates[1].empty())
                        {
                            //assert(nextState.openValves.totalFlowRate() != maxPossibleFlowRate);
                            // Presumably, we have no more valves that we can open?
                            bool noMoreValves = true;
                            for (const auto* valve : valvesWithNonZeroRate)
                            {
                                if (!nextState.openValves.hasValve(valve))
                                {
                                    noMoreValves = false;
                                    std::cout << " valve remains: " << valve->label << std::endl;
                                }
                            }
                            if (!noMoreValves)
                            {
                                std::cout << "Ran out of options, but not valves, exploring state: targetValve[0]: " << origState.actorTargetValves[0] << " targetValve[1]: " << origState.actorTargetValves[1] << " actorDistsToTargetValves[0]: " << origState.actorDistsToTargetValves[0] << " actorDistsToTargetValves[1]: " << origState.actorDistsToTargetValves[1] << " flow rate: " << nextState.openValves.totalFlowRate() << std::endl; 
                                //assert(false);
                            }
                            State dummyState = initialState;
                            dummyState.openValves = nextState.openValves;
                            handleNewState(dummyState, time + 1);
                            //const int64_t pressureAtEnd = pressureReleased + (timeLimit - time) * nextState.openValves.totalFlowRate();
                            //auto& currentHighestPressureForNewState = highestPressureForStateAtMinute[timeLimit][nextState];
                            //if (currentHighestPressureForNewState < pressureAtEnd)
                            //{
                            //    currentHighestPressureForNewState = pressureAtEnd;
                            //    //valveSets.insert(newState.openValves);
                            //}


                            continue;
                        }
                        for (const auto& actor1NextState : actorNextStates[0])
                        {
                            for (const auto& actor2NextState : actorNextStates[1])
                            {
                                State stateToAdd = actor1NextState;
                                stateToAdd.actorTargetValves[1] = actor2NextState.actorTargetValves[1];
                                stateToAdd.actorDistsToTargetValves[1] = actor2NextState.actorDistsToTargetValves[1];
                                handleNewState(stateToAdd, time + 1);
                            }
                        }
                    }
                }
            }
        }

        // End of this minute.
        for (const auto& [state, pressureReleased] : toExplore)
        {
            const int64_t pressureReleasedAtEndOfMinute = pressureReleased + state.openValves.totalFlowRate();
            //std::cout << "totalFlowRate: " << state.openValves.totalFlowRate() << std::endl;
            highestPressureReleased = max(pressureReleasedAtEndOfMinute, highestPressureReleased);
        }
        time++;
        toExplore.clear();
    }
    std::cout << "answer: " << highestPressureReleased << std::endl;
}
