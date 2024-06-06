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

void updateMinDistances(const Valve* startValve, map<const Valve*, map<const Valve*, int>>& minDistanceBetweenValves)
{
    std::cout << "updateMinDistances starting with: " << startValve->label << std::endl;
    vector<const Valve*> toExplore = { startValve };
    set<const Valve*> seen = { startValve };
    int distance = 1;
    while (!toExplore.empty())
    {
        std::cout << "toExplore: " << toExplore.size() << std::endl;
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
#if 0
        string toString() const
        {
            string asStr;
            for (int valveIndex = 0; valveIndex < 63; valveIndex++)
            {
                if (hasValve())
                    asStr += key;
            }
            return asStr;
        }
#endif
    private:
        uint64_t m_valveBits = 0;
        int64_t m_totalFlowRate = 0;

        uint64_t bitForValve(const Valve* valve) const
        {
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
    while (std::getline(cin, valveInfoLine))
    {
        std::smatch valveInfoMatch;
        std::cout << "line: " << valveInfoLine << std::endl;
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
        std::cout << "label: " << valveInfo.label  << " flowRate: " << valveInfo.flowRate << " neighbourLabels: " << std::endl;
        for (const auto& label : valveInfo.neighbourLabels)
        {
            std::cout << " " << label << std::endl;
        }
        valves.push_back(valveInfo);

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
        std::cout << "valve with label: " << valve.label << " has neighbours: " << std::endl;
        for (const auto* neighbour : valve.neighbours)
        {
            std::cout << " " << neighbour->label << std::endl;
        }
    }
    assert(startingValve != nullptr);
    // Build minDistanceBetweenValves map.
    std::cout << "Building minDistanceBetweenValves" << std::endl;
    map<const Valve*, map<const Valve*, int>> minDistanceBetweenValves;
    for (const auto& valve : valves)
    {
        if (valve.flowRate != 0)
            updateMinDistances(&valve, minDistanceBetweenValves);
    }
    updateMinDistances(startingValve, minDistanceBetweenValves);
    std::cout << "Finished Building minDistanceBetweenValves" << std::endl;
    for (const auto [valve, minDistanceFromValve] : minDistanceBetweenValves)
    {
        for (const auto [otherValve, distance] : minDistanceFromValve)
        {
            std::cout << "Min distance between " << valve->label << " and " << otherValve->label << " is: " << distance << std::endl;
        }
    }
    
    struct State
    {
        const Valve* currentPos = nullptr;
        ValveSet openValves;
        int64_t pressureReleased = 0;
        auto operator<=>(const State& other) const = default;
    };

    State initialState = { startingValve };

    constexpr int timeLimit = 30;
    vector<set<State>> statesToExploreAtMinute(timeLimit + 1);
    int time = 1;
    statesToExploreAtMinute[time] = { initialState };
    std::set<State> seenStates = { initialState };
    int64_t highestPressureReleased = 0;
    while (time <= timeLimit)
    {
        const auto& toExplore = statesToExploreAtMinute[time];
        vector<State> nextToExplore;
        std::cout << "time: " << time << " # toExplore: " << toExplore.size() << std::endl;
        for (const auto& state : toExplore)
        {
            seenStates.insert(state);
            auto handleNewState = [&seenStates, &nextToExplore, &statesToExploreAtMinute](State& newState, const int newStateTime, const State& parentState, const int parentStateTime)
            {
                // Assumes newState.pressureReleased has not yet been updated.
                assert(parentStateTime < newStateTime);
                //assert(newStateTime <= timeLimit);
                newState.pressureReleased += (newStateTime - parentStateTime) * parentState.openValves.totalFlowRate();
                if (!seenStates.contains(newState))
                {
                    statesToExploreAtMinute[newStateTime].insert(newState);
                }
            };
            if (time != timeLimit)
            {
                if ((state.currentPos->flowRate != 0) && (!state.openValves.hasValve(state.currentPos)))
                {
                    // Spend this turn opening the valve.
                    State newState = state;
                    newState.openValves.addValve(state.currentPos);
                    handleNewState(newState, time + 1, state, time);
                }

                // Move.
                for (const auto [otherValve, distance] : minDistanceBetweenValves[state.currentPos])
                {
                    State newState = state;
                    if (time + distance <= timeLimit)
                    {
                        newState.currentPos = otherValve;
                        handleNewState(newState, time + distance, state, time);
                    }
                    else
                    {
                        // Can't reach this other valve in time; just stand here until the clock
                        // runs out.
                        if (time != timeLimit)
                            handleNewState(newState, timeLimit, state, time);
                    }
                }
            }
        }

        // End of this minute.
        for (const auto& state : toExplore)
        {
            const int64_t pressureReleasedAtEndOfMinute = state.pressureReleased + state.openValves.totalFlowRate();
            highestPressureReleased = max(pressureReleasedAtEndOfMinute, highestPressureReleased);
        }
        std::cout << "highestPressureReleased after time " << time << " : " << highestPressureReleased << std::endl;
        time++;
        //toExplore = nextToExplore;
    }
}
