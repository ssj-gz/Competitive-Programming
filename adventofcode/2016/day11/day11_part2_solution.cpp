#include <iostream>
#include <set>
#include <map>
#include <limits>

#include <regex>

#include <cassert>

using namespace std;

struct Item
{
    enum Type { Microchip, Generator };
    Type type;
    string material;
    auto operator<=>(const Item& other) const = default;
};

struct State
{
    int elevatorFloor = -1;
    static constexpr int numFloors = 4;
    set<Item> floorContent[numFloors] = {};

    void normalise()
    {
        normalisedRep.clear();
        for (int generatorFloor = 0; generatorFloor < numFloors; generatorFloor++)
        {
            for (const auto& generator : floorContent[generatorFloor])
            {
                if (generator.type == Item::Generator)
                {
                    for (int chipFloor = 0; chipFloor < numFloors; chipFloor++)
                    {
                        for (const auto& chip : floorContent[chipFloor])
                        {
                            if (chip.type == Item::Microchip && chip.material == generator.material)
                            {
                                normalisedRep.push_back({generatorFloor, chipFloor});
                            }
                        }

                    }
                }
            }
        }
        normalisedRep.push_back({-1, elevatorFloor});
        std::sort(normalisedRep.begin(), normalisedRep.end());
    }
    auto operator<=>(const State& other) const
    {
        assert(!normalisedRep.empty() && !other.normalisedRep.empty());
        return normalisedRep <=> other.normalisedRep;
    }
    vector<std::pair<int, int>> normalisedRep;
};


int main()
{
    string description;
    string descriptionLine;
    while (getline(cin, descriptionLine))
        description += descriptionLine;

    std::regex descriptionRegex(R"(^The first floor contains (.*)\.The second floor contains (.*)\.The third floor contains (.*)\.The fourth floor contains nothing relevant.$)");
    std::smatch descriptionMatch;
    const bool matchSuccessful = std::regex_match(description, descriptionMatch, descriptionRegex);
    assert(matchSuccessful);

    auto extractItemsFromDescription = [](std::string listOfItemsDescription)
    {
        const std::string_view andStr = "and";
        if (const auto andPos = listOfItemsDescription.find(andStr); andPos != std::string::npos)
        {
            listOfItemsDescription.replace(andPos, andStr.size(), "");
        };
        std::replace(listOfItemsDescription.begin(), listOfItemsDescription.end(), ',', ' ');
        static std::regex itemRegex(R"(^\s*((a (\w+) generator)|(a (\w+)-compatible microchip)).*$)");
        std::set<Item> items;
        while (true)
        {
            std::smatch itemMatch;
            const bool matchSuccessful = std::regex_match(listOfItemsDescription, itemMatch, itemRegex);
            if (!matchSuccessful)
            {
                listOfItemsDescription.erase(std::remove(listOfItemsDescription.begin(), listOfItemsDescription.end(), ' '), listOfItemsDescription.end());
                assert(listOfItemsDescription.empty());
                break;
            }
            else
            {
                if (!std::string(itemMatch[2]).empty())
                {
                    const string material = itemMatch[3];
                    items.insert({Item::Generator, material});

                    listOfItemsDescription.replace(itemMatch.position(2), itemMatch.length(2), "");
                }
                else
                {
                    const string material = itemMatch[5];
                    items.insert({Item::Microchip, material});
                    listOfItemsDescription.replace(itemMatch.position(4), itemMatch.length(4), "");
                }
            }
        }
        return items;
    };

    std::cout << "description: " << description << std::endl;
    State initialState;
    initialState.elevatorFloor = 0;
    initialState.floorContent[0] = extractItemsFromDescription(descriptionMatch[1]);
    initialState.floorContent[1] = extractItemsFromDescription(descriptionMatch[2]);
    initialState.floorContent[2] = extractItemsFromDescription(descriptionMatch[3]);
#if 1
    initialState.floorContent[0].insert({Item::Microchip, "elerium"});
    initialState.floorContent[0].insert({Item::Generator, "elerium"});
    initialState.floorContent[0].insert({Item::Microchip, "dilithium"});
    initialState.floorContent[0].insert({Item::Generator, "dilithium"});
#endif
    int numItems = 0;
    for (const auto& floor : initialState.floorContent)
    {
        numItems += floor.size();
    }
    std::cout << "numItems: " << numItems << std::endl;

    map<int, set<State>> statesAtStep;
    map<State, int> earliestStepForState;
    initialState.normalise();
    statesAtStep[0].insert(initialState);
    earliestStepForState[initialState] = 0;
    for (auto& [step, states] : statesAtStep)
    {
        std::cout << "step: " << step << " #states: " << states.size() << std::endl;
        while (!states.empty())
        {
            const auto state = *states.begin();
            states.erase(states.begin());
            if (static_cast<int>(state.floorContent[3].size()) == numItems)
            {
                std::cout << "Found the answer - takes " <<  step << " to move all objects to the top floor" << std::endl;
                return 0;
            }


            auto addSuccessorState = [&statesAtStep, &step, &earliestStepForState](const State& successorStateOrig, int successorStep)
            {
                State successorState = successorStateOrig;
                successorState.normalise();
                assert(successorStep >= step);
                if (earliestStepForState.contains(successorState))
                {
                    if (successorStep < earliestStepForState[successorState])
                    {
                        const auto previousEarliestStep = earliestStepForState[successorState];
                        statesAtStep[previousEarliestStep].erase(successorState);

                        earliestStepForState[successorState] = successorStep;
                        statesAtStep[earliestStepForState[successorState]].insert(successorState);
                    }
                }
                else
                {
                    earliestStepForState[successorState] = successorStep;
                    statesAtStep[earliestStepForState[successorState]].insert(successorState);
                }
            };

            auto causesIrradiation = [](const State& origState)
            {
                auto state = origState;
                const auto& allItemsAtFloor = state.floorContent[state.elevatorFloor];

                set<Item> unprotectedChips;
                for (const auto& item : allItemsAtFloor)
                {
                    if (item.type == Item::Microchip && !allItemsAtFloor.contains({Item::Generator, item.material}))
                        unprotectedChips.insert(item);
                }
                if (!unprotectedChips.empty() && std::find_if(allItemsAtFloor.begin(), allItemsAtFloor.end(), [](const auto& item) { return item.type == Item::Generator; }) != allItemsAtFloor.end())
                {
                    // Each of these chips will be irradiated by any of the generators in allItemsAtFloor.
                    return true;
                }
                return false;
            };

            for (int elevatorDirection : { -1, +1 })
            {
                const int floorToMoveTo = state.elevatorFloor + elevatorDirection;
                if (floorToMoveTo < 0 || floorToMoveTo >= State::numFloors)
                    continue;

                const auto& allItemsAtFloor = state.floorContent[state.elevatorFloor];

                // Move one item to floorToMoveTo.
                for (const auto& item : allItemsAtFloor)
                {
                    State successorState = state;
                    successorState.elevatorFloor = floorToMoveTo;
                    successorState.floorContent[state.elevatorFloor].erase(item);
                    successorState.floorContent[floorToMoveTo].insert(item);
                    if (!causesIrradiation(successorState))
                        addSuccessorState(successorState, step + 1);

                }
                // Move two distinct items to floorToMoveTo.
                for (auto item1Iter = allItemsAtFloor.begin(); item1Iter != allItemsAtFloor.end(); item1Iter++)
                {
                    for (auto item2Iter = std::next(item1Iter); item2Iter != allItemsAtFloor.end(); item2Iter++)
                    {
                        State successorState = state;
                        successorState.elevatorFloor = floorToMoveTo;
                        successorState.floorContent[state.elevatorFloor].erase(*item1Iter);
                        successorState.floorContent[floorToMoveTo].insert(*item1Iter);
                        successorState.floorContent[state.elevatorFloor].erase(*item2Iter);
                        successorState.floorContent[floorToMoveTo].insert(*item2Iter);
                        if (!causesIrradiation(successorState))
                            addSuccessorState(successorState, step + 1);
                    }
                }
            }
        }
    }

    return 0;
}
