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
    
    struct State
    {
        const Valve* currentPos = nullptr;
        ValveSet openValves;
        // Pressure released is deliberately omitted; instead, it's 
        // stored externally, in highestPressureForStateAtMinute.
        auto operator<=>(const State& other) const = default;
    };

    State initialState = { startingValve };

    constexpr int timeLimit = 30;
    vector<map<State, int>> highestPressureForStateAtMinute(timeLimit + 1);
    highestPressureForStateAtMinute[1] = { { initialState, 0} };
    int time = 1;
    int64_t highestPressureReleased = 0;
    while (time <= timeLimit)
    {
        const auto& toExplore = highestPressureForStateAtMinute[time];
        for (const auto& [state, pressureReleased] : toExplore)
        {
            auto handleNewState = [&highestPressureForStateAtMinute, &parentState = state, pressureReleased, parentStateTime = time](State& newState, const int newStateTime)
            {
                // Assumes newState.pressureReleased has not yet been updated.
                assert(parentStateTime < newStateTime);
                const int64_t pressureReleasedAtNewState = pressureReleased + (newStateTime - parentStateTime) * parentState.openValves.totalFlowRate();
                auto& currentHighestPressureForNewState = highestPressureForStateAtMinute[newStateTime][newState];
                if (currentHighestPressureForNewState < pressureReleasedAtNewState)
                {
                    currentHighestPressureForNewState = pressureReleasedAtNewState;
                }
            };
            if (time != timeLimit)
            {
                if ((state.currentPos->flowRate != 0) && (!state.openValves.hasValve(state.currentPos)))
                {
                    // Spend this turn opening the valve.
                    State newState = state;
                    newState.openValves.addValve(state.currentPos);
                    handleNewState(newState, time + 1);
                }

                // Move.
                for (const auto* neigbourValve : state.currentPos->neighbours)
                {
                    State newState = state;
                    newState.currentPos = neigbourValve;
                    handleNewState(newState, time + 1);
                }
            }
        }

        // End of this minute.
        for (const auto& [state, pressureReleased] : toExplore)
        {
            const int64_t pressureReleasedAtEndOfMinute = pressureReleased + state.openValves.totalFlowRate();
            highestPressureReleased = max(pressureReleasedAtEndOfMinute, highestPressureReleased);
        }
        time++;
    }
    std::cout << "answer: " << highestPressureReleased << std::endl;
}
