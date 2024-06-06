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
    
    struct State
    {
        Valve* currentPos = nullptr;
        ValveSet openValves;
        int64_t pressureReleased = 0;
        auto operator<=>(const State& other) const = default;
    };

    State initialState = { startingValve };

    vector<State> toExplore = { initialState };
    std::set<State> seenStates = { initialState };
    int time = 1;
    int64_t highestPressureReleased = 0;
    while (time <= 30)
    {
        vector<State> nextToExplore;
        std::cout << "time: " << time << " # toExplore: " << toExplore.size() << std::endl;
        for (const auto& state : toExplore)
        {
            auto handleNewState = [&seenStates, &nextToExplore, &highestPressureReleased](State& newState, const State& parentState)
            {
                // Assumes newState.pressureReleased has not yet been updated.
                newState.pressureReleased += parentState.openValves.totalFlowRate();
                if (newState.pressureReleased > highestPressureReleased)
                {
                    highestPressureReleased = newState.pressureReleased;
                }
                if (!seenStates.contains(newState))
                {
                    nextToExplore.push_back(newState);
                    seenStates.insert(newState);
                }
            };
            if ((state.currentPos->flowRate != 0) && (!state.openValves.hasValve(state.currentPos)))
            {
                // Spend this turn opening the valve.
                State newState = state;
                newState.openValves.addValve(state.currentPos);
                handleNewState(newState, state);
            }

            // Move.
            for (auto* neighbourValve : state.currentPos->neighbours)
            {
                State newState = state;
                newState.currentPos = neighbourValve;
                handleNewState(newState, state);
            }
        }

        toExplore = nextToExplore;
        time++;
        std::cout << "highestPressureReleased after time " << time << " : " << highestPressureReleased << std::endl;
    }
}
