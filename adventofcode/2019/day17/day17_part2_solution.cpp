#include "../shared/intcodecomputer.h"

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <regex>
#include <limits>

using namespace std;

struct Coord
{
    int x = -1;
    int y = -1;
    auto operator<=>(const Coord&) const = default;
};

vector<string> computeMapAndGetPrompt(IntCodeComputer& intCodeComputer)
{
    const auto status = intCodeComputer.run();
    assert(status == IntCodeComputer::WaitingForInput);

    const auto output = intCodeComputer.takeOutput();

    vector<string> map;
    string currentLine;
    for (const auto ascii : output)
    {
        if (ascii == 10)
        {
            map.push_back(currentLine);
            currentLine.clear();
        }
        else 
            currentLine.push_back(static_cast<char>(ascii));
    }
    while(map.back().empty())
    {
        map.pop_back();
    }


    return map;
}

struct Command
{
    enum Cmd { Left, Right, Forward } cmd;
    int amount = 1;
    auto operator<=>(const Command& other) const = default;
};

struct RobotState
{
    Coord coord;
    enum Direction { Up = 0, Right = 1, Down = 2, Left = 3 };
    Direction direction = Up;
    RobotState& applyCommand(const Command& command)
    {
        switch (command.cmd)
        {
            case Command::Forward:
                {
                    switch (direction)
                    {
                        case RobotState::Direction::Up:
                            coord.y -= command.amount;
                            break;
                        case RobotState::Direction::Down:
                            coord.y += command.amount;
                            break;
                        case RobotState::Direction::Left:
                            coord.x -= command.amount;
                            break;
                        case RobotState::Direction::Right:
                            coord.x += command.amount;
                            break;
                        default:
                            assert(false);
                    }
                }
                break;
            case Command::Left:
                direction = static_cast<Direction>((static_cast<int>(direction) + 4 - 1) % 4);
                break;
            case Command::Right:
                direction = static_cast<Direction>((static_cast<int>(direction) + 1) % 4);
                break;
            default:
                assert(false);
        }

        return *this;
    }
    auto operator<=>(const RobotState&) const = default;
};

constexpr int maxFunctionCalls = 10;

string toString(const vector<Command>& commandList)
{
    if (commandList.empty())
        return "";
    string asStr;
    for (const auto& command : commandList)
    {
        switch (command.cmd)
        {
            case Command::Left:
                asStr += "L";
                break;
            case Command::Right:
                asStr += "R";
                break;
            case Command::Forward:
                asStr += to_string(command.amount);
                break;
        }
        asStr += ",";
    }
    asStr.pop_back();
    return asStr;
}


void buildValidCommandList(RobotState& state, vector<Command>& commandsFollowed, int& commandsFollowedStringLen, vector<vector<Command>>& destCommandList, const vector<string>& worldMap)
{
    if (commandsFollowedStringLen > 20)
        return;
    if (worldMap[state.coord.y][state.coord.x] != '#')
        return;

    if (!commandsFollowed.empty())
    {
        destCommandList.push_back(commandsFollowed);
    }

    const int leadingCommaLen = (commandsFollowed.empty() ? 0 : 1);

    if (commandsFollowed.empty() || (commandsFollowed.back().cmd == Command::Forward || (commandsFollowed.back().cmd == Command::Left ))  )
    {
        if (commandsFollowed.size() < 2 || (commandsFollowed.back().cmd != Command::Left || commandsFollowed[commandsFollowed.size() - 2].cmd != Command::Left))
        {
            // Left.
            commandsFollowed.push_back(Command{Command::Left, 1});
            const int increaseInStrLen = leadingCommaLen + 1;
            commandsFollowedStringLen += increaseInStrLen;
            RobotState newState = RobotState(state).applyCommand(commandsFollowed.back());

            buildValidCommandList(newState, commandsFollowed, commandsFollowedStringLen, destCommandList, worldMap);

            commandsFollowedStringLen -= increaseInStrLen;
            commandsFollowed.pop_back();
        }
    }


    // Right.
    if (commandsFollowed.empty() || (commandsFollowed.back().cmd == Command::Forward))
    {
        commandsFollowed.push_back(Command{Command::Right, 1});
        const int increaseInStrLen = leadingCommaLen + 1;
        commandsFollowedStringLen += increaseInStrLen;
        RobotState newState = RobotState(state).applyCommand(commandsFollowed.back());

        buildValidCommandList(newState, commandsFollowed, commandsFollowedStringLen, destCommandList, worldMap);

        commandsFollowedStringLen -= increaseInStrLen;
        commandsFollowed.pop_back();
    }

    if (commandsFollowed.empty() || (commandsFollowed.back().cmd != Command::Forward))
    {
        // Forward.
        int amount = 1;
        while (true)
        {
            assert(amount < 100);

            const Command forwardCommand{Command::Forward, amount};
            RobotState newState = RobotState(state).applyCommand(forwardCommand);
            if (newState.coord.y < 0 || newState.coord.y >= worldMap.size())
                break;
            if (newState.coord.x < 0 || newState.coord.x >= worldMap.front().size())
                break;
            if (worldMap[newState.coord.y][newState.coord.x] != '#')
                break;

            commandsFollowed.push_back(forwardCommand);
            const int increaseInStrLen = leadingCommaLen + (amount <= 9 ? 1 : 2);
            commandsFollowedStringLen += increaseInStrLen;

            buildValidCommandList(newState, commandsFollowed, commandsFollowedStringLen, destCommandList, worldMap);

            commandsFollowedStringLen -= increaseInStrLen;
            commandsFollowed.pop_back();

            amount++;
            
        }
    }
}

set<Coord> cellsCovered(const RobotState& startingState, const vector<Command>& commands)
{
    RobotState state = startingState;
    set<Coord> cellsCovered = {state.coord};
    for (const auto& command : commands)
    {
        if (command.cmd == Command::Forward)
        {
            for (int i = 0; i < command.amount; i++)
            {
                state.applyCommand(Command{Command::Forward, 1});
                cellsCovered.insert(state.coord);
            }
        }
        else
        {
            state.applyCommand(command);
            cellsCovered.insert(state.coord);
        }
    }
    return cellsCovered;
}
struct Outcome
{
    set<Coord> cellsCovered;
    RobotState endingState;
    auto operator<=>(const Outcome&) const = default;
};

Outcome outcomeForCommands(const vector<Command>& commands)
{
    RobotState state({0,0}, static_cast<RobotState::Direction>(0));

    Outcome outcome;
    outcome.cellsCovered = cellsCovered(state, commands);

    outcome.endingState = state;
    for (const auto& command : commands)
    {
        outcome.endingState.applyCommand(command);
    }

    return outcome;
}

map<RobotState, vector<vector<Command>>> buildValidCommandListForAllStates(const vector<string>& worldMap)
{
    map<RobotState, vector<vector<Command>>> stateToCommandListsMap;
    const int height = static_cast<int>(worldMap.size());
    const int width = static_cast<int>(worldMap[0].size());
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            if (worldMap[y][x] == '#')
            {
                RobotState startingState;
                startingState.coord = {x, y};
                for (int direction = 0; direction < 4; direction++)
                {
                    startingState.direction = static_cast<RobotState::Direction>(direction);
                    vector<Command> commandsFollowed;
                    int commandsFollowedStringLen = 0;
                    vector<vector<Command>> destCommandList;
                    buildValidCommandList(startingState, commandsFollowed, commandsFollowedStringLen, destCommandList, worldMap);

                    stateToCommandListsMap[startingState] = destCommandList;
                }
            }
        }
    }
    return stateToCommandListsMap;
}

class Function
{
    public:
        Function(const vector<Command>& commandList, int numCellsCovered)
            : m_commandList{commandList},
              m_numCellsCovered{numCellsCovered}
        {
            assert(m_numCellsCovered > 0);
            static int maxNumCellsCovered = 0;
            if (numCellsCovered > maxNumCellsCovered)
            {
                maxNumCellsCovered = numCellsCovered;
            }
        }
        void incStatesRunnableFrom()
        {
            m_numStatesRunnableFrom++;
        }
        int64_t score() const
        {
            assert(m_numStatesRunnableFrom != 0);
            assert(m_numCellsCovered != 0);
            return m_numCellsCovered;// * std::min(maxFunctionCalls, m_numStatesRunnableFrom);
        }
        vector<Command> commandList() const
        {
            return m_commandList;
        }

        int numStatesRunnableFrom() const
        {
            return m_numStatesRunnableFrom;
        }

        int numCellsCovered() const
        {
            return m_numCellsCovered;
        }
        Function() = default;
    private:
        vector<Command> m_commandList;
        int m_numStatesRunnableFrom = 0;
        int m_numCellsCovered = 0;
};

vector<Function*> winner;

void findFunctionsToCoverAllCells(const RobotState state, map<Coord, int>& numTimesVisitedCell, set<Function*>& functionsUsed, vector<Function*>& functionsCalled, const int numCellsRemaining, map<RobotState, set<Function*>>& isFunctionRunnableFromStateLookup,  map<RobotState, vector<Function*>>& orderedFunctionsRunnableFromState)
{
    if (!winner.empty())
        return;
    if (numCellsRemaining == 0)
    {
        std::cout << "Hallelujah!" << std::endl;
        winner = functionsCalled;
    }

    static int minNumCellsRemaining = std::numeric_limits<int>::max();
    if (numCellsRemaining < minNumCellsRemaining)
    {
        minNumCellsRemaining = numCellsRemaining;
    }
    if (functionsCalled.size() >= maxFunctionCalls)
        return;
    // First, try and call a function again.
    if (!functionsUsed.empty())
    {
        for (auto* function : functionsUsed)
        {
            if (isFunctionRunnableFromStateLookup[state].contains(function))
            {
                const auto cellsCovered = ::cellsCovered(state, function->commandList());
                RobotState newState = state;
                for (const auto& command : function->commandList())
                    newState.applyCommand(command);
                int newNumCellsRemaining = numCellsRemaining;
                for (const auto& visitedCell : cellsCovered)
                {
                    if (numTimesVisitedCell[visitedCell] == 0)
                    {
                        newNumCellsRemaining--;
                    }
                    numTimesVisitedCell[visitedCell]++;
                }
                // Recurse.
                functionsCalled.push_back(function);
                findFunctionsToCoverAllCells(newState, numTimesVisitedCell, functionsUsed, functionsCalled, newNumCellsRemaining, isFunctionRunnableFromStateLookup, orderedFunctionsRunnableFromState);
                functionsCalled.pop_back();;
                for (const auto& visitedCell : cellsCovered)
                {
                    numTimesVisitedCell[visitedCell]--;
                    assert(numTimesVisitedCell[visitedCell] >= 0);
                }
            }

        }
    }
    // Now try a new function.
    if (functionsUsed.size() < 3)
    {
        for (auto* function : orderedFunctionsRunnableFromState[state])
        {
            if (functionsUsed.contains(function))
                continue;

            const auto cellsCovered = ::cellsCovered(state, function->commandList());
            RobotState newState = state;
            for (const auto& command : function->commandList())
                newState.applyCommand(command);

            int newNumCellsRemaining = numCellsRemaining;
            for (const auto& visitedCell : cellsCovered)
            {
                if (numTimesVisitedCell[visitedCell] == 0)
                {
                    newNumCellsRemaining--;
                }
                numTimesVisitedCell[visitedCell]++;
            }
            // Recurse.
            functionsCalled.push_back(function);
            functionsUsed.insert(function);
            findFunctionsToCoverAllCells(newState, numTimesVisitedCell, functionsUsed, functionsCalled, newNumCellsRemaining, isFunctionRunnableFromStateLookup, orderedFunctionsRunnableFromState);
            functionsUsed.erase(function);
            functionsCalled.pop_back();;
            for (const auto& visitedCell : cellsCovered)
            {
                numTimesVisitedCell[visitedCell]--;
            }
        }
    }
}


int main()
{
    vector<int64_t> program;
    int64_t programInput;
    while (true)
    {
        cin >> programInput;
        program.push_back(programInput);
        assert(cin);
        char comma;
        cin >> comma;
        if (!cin)
            break;
        assert(comma == ',');
    }

    assert(program[0] == 1);
    program[0] = 2;
    IntCodeComputer intCodeComputer(program);
    const auto mapAndPrompt = computeMapAndGetPrompt(intCodeComputer);
    assert(!mapAndPrompt.empty());
    vector<string> worldMap{mapAndPrompt.begin(), mapAndPrompt.begin() + mapAndPrompt.size() - 1};
    while(worldMap.back().empty())
    {
        worldMap.pop_back();
    }
    const int height = static_cast<int>(worldMap.size());
    const int width = static_cast<int>(worldMap[0].size());
    cout << "width: " << width << " height: " << height << endl;

    Coord robotCoord;
    for (int row = 0; row < height; row++)
    {
        assert(static_cast<int>(worldMap[row].size()) == width);
        for (int col = 0; col < width; col++)
        {
            if (worldMap[row][col] == '^')
            {
                robotCoord = {col, row};
                worldMap[row][col] = '#';
            }
            cout << worldMap[row][col];
        }
        cout << endl;
    }
    assert(robotCoord.x != -1 && robotCoord.y != -1);
    std::cout << "Robot starts at " << robotCoord.x << "," << robotCoord.y << std::endl;
    int numCells = 0;
    for (const auto& row : worldMap)
        numCells += std::count(row.begin(), row.end(), '#');
    std::cout << "numCells: " << numCells << std::endl;

    const string mainPrompt = mapAndPrompt.back();


    auto stateToCommandListsMap = buildValidCommandListForAllStates(worldMap);
    map<vector<Command>, Function> functionForCommandList;
    map<Outcome, vector<Command>> smallestCommandListForOutcome;
    map<RobotState, vector<Function*>> functionsRunnableFromState;
    map<RobotState, set<Function*>> isFunctionRunnableFromStateLookup;
    vector<vector<Command>> allCommandLists;
    std::cout << "Building functionsRunnableFromState" << std::endl;
    for (const auto& [robotState, commandListForState] : stateToCommandListsMap)
    {
        allCommandLists.insert(allCommandLists.end(), commandListForState.begin(), commandListForState.end());
    }
    sort(allCommandLists.begin(), allCommandLists.end(), [](const auto& lhsCommandList, const auto& rhsCommandList) 
            {
            if (lhsCommandList.size() != rhsCommandList.size())
            return lhsCommandList.size() < rhsCommandList.size();
            return lhsCommandList < rhsCommandList;
            });
    std::cout << "allCommandLists.size(): " << allCommandLists.size() << std::endl;
    int numShapes = 0;
    for (const auto& commandList : allCommandLists)
    {
        const auto outcome = outcomeForCommands(commandList);
        if (!smallestCommandListForOutcome.contains(outcome))
        {
            smallestCommandListForOutcome[outcome] = commandList;
        }
    }
    for (auto& [robotState, reducedCommandList] : stateToCommandListsMap)
    {
        for (auto& commands : reducedCommandList)
        {
            commands = smallestCommandListForOutcome[outcomeForCommands(commands)];
            if (!functionForCommandList.contains(commands))
            {
                functionForCommandList[commands] = Function(commands, cellsCovered(robotState, commands).size());
            }
            functionsRunnableFromState[robotState].push_back(&functionForCommandList[commands]);
            functionForCommandList[commands].incStatesRunnableFrom();

        }
    }
    std::cout << "Finished building functionsRunnableFromState" << std::endl;
    std::cout << "functionForCommandList.size(): " << functionForCommandList.size() << std::endl;
    std::cout << "smallestCommandListForOutcome.size(): " << smallestCommandListForOutcome.size() << std::endl;

    for (auto& [robotState, runnableFunctions] : functionsRunnableFromState)
    {
        sort(runnableFunctions.begin(), runnableFunctions.end());
        runnableFunctions.erase(std::unique(runnableFunctions.begin(), runnableFunctions.end()), runnableFunctions.end());
        sort(runnableFunctions.begin(), runnableFunctions.end(), [](const auto* lhsFunction, const auto* rhsFunction)
                {
                return lhsFunction->score() > rhsFunction->score();
                });
        for (auto* function : runnableFunctions)
            isFunctionRunnableFromStateLookup[robotState].insert(function);
    }

    vector<const Function*> allFunctions;
    for (const auto& [unused, function] : functionForCommandList)
    {
        allFunctions.push_back(&function);
    }
    sort(allFunctions.begin(), allFunctions.end(), [](const auto* lhsFunction, const auto* rhsFunction)
            {
            return lhsFunction->score() > rhsFunction->score();
            });

    map<Coord, int> numTimesVisitedCell;
    set<Function*> functionsUsed;
    vector<Function*> functionsCalled;
    std::cout << "Running findFunctionsToCoverAllCells" << std::endl;
    findFunctionsToCoverAllCells(RobotState{robotCoord, RobotState::Direction::Up}, numTimesVisitedCell, functionsUsed, functionsCalled, numCells, isFunctionRunnableFromStateLookup,  functionsRunnableFromState);

    if (winner.empty())
    {
        std::cout << "Failed!" << std::endl;
        return EXIT_FAILURE;
    }

    auto printWorldMap = [&]()
    {
        std::cout << "Map: " << std::endl;
        for (const auto& row : worldMap)
        {
            std::cout << row << std::endl;
        }
    };

    printWorldMap();

    char letter = 'A';
    map<Function*, char> functionName;
    for (auto* function : winner)
    {
        if (!functionName.contains(function))
        {
            functionName[function] = letter;
            letter++;
        }
    }

    RobotState state = { robotCoord, RobotState::Direction::Up };
    for (auto* function : winner)
    {
        const char name = functionName[function];
        const auto cellsCovered = ::cellsCovered(state, function->commandList());
        for (const auto cell : cellsCovered)
        {
            worldMap[cell.y][cell.x] = name;
        }
        for (const auto command : function->commandList())
        {
            state.applyCommand(command);
        }

        printWorldMap();
    }

    std::cout << "winner.size(): " << winner.size() << std::endl;
    std::cout << "Function calls: " << std::endl;
    std::string functionCallString;
    for (auto* function : winner)
    {
        const char name = functionName[function];
        functionCallString += name;
        functionCallString += ",";
    }
    assert(!functionCallString.empty());
    functionCallString.pop_back();
    std::cout << functionCallString << std::endl;
    std::cout << "Functions: " << std::endl;
    for (const auto& [function, name] : functionName)
    {
        std::cout << name << ":" << std::endl;
        std::cout << toString(function->commandList()) << std::endl;
    }

    //std::cout << "Waking IntCode" << std::endl;
    //assert(program[0] == 1);
    //program[0] = 2;

    std::cout << "Output from IntCode:" << std::endl;
    cout << "mainPrompt: " << mainPrompt << endl;
    assert(mainPrompt == "Main:");

    for (const auto letter : functionCallString)
    {
        intCodeComputer.addInput(letter);
    }
    intCodeComputer.addInput(10);


    std::cout << "Provided function call list to IntCodeComputer."<< std::endl;
    for (int i = 1; i <= 3; i++)
    {
        const auto status = intCodeComputer.run();
        assert(status == IntCodeComputer::WaitingForInput);
        const auto output = intCodeComputer.takeOutput();
        std::string outputStr;
        for (const auto ascii : output)
        {
            if (ascii != 10)
                outputStr += static_cast<char>(ascii);
        }
        std::cout << "IntCode says: >>>" << outputStr << "<<<" << std::endl;
        const std::regex functionNamePromptRegex("^Function (.):$");
        std::smatch fnNamePromptMatch;
        const bool matched = std::regex_match(outputStr, fnNamePromptMatch, functionNamePromptRegex );
        assert(matched);
        std::cout << "Asked for function: " << fnNamePromptMatch[1] << std::endl;
        for (const auto& [function, name] : functionName)
        {
            if (name == fnNamePromptMatch[1])
            {
                std::cout << "Providing definition of fn " << name << " to IntCode" << std::endl;
                for (const auto letter : toString(function->commandList()))
                {
                    intCodeComputer.addInput(letter);
                }
                intCodeComputer.addInput(10);
            }
        }
    }
    {
        const auto status = intCodeComputer.run();
        assert(status == IntCodeComputer::WaitingForInput);
        const auto output = intCodeComputer.takeOutput();
        std::string outputStr;
        for (const auto ascii : output)
        {
            if (ascii != 10)
                outputStr += static_cast<char>(ascii);
        }
        std::cout << "IntCode: >>>" << outputStr << "<<<" << std::endl;
    }
    {
        std::cout << "Saying 'no' to Continuous Video Feed request" << std::endl;
        // No video feed, thanks!
        intCodeComputer.addInput('n');
        intCodeComputer.addInput(10);
        const auto status = intCodeComputer.run();
        //assert(status == IntCodeComputer::WaitingForInput);
        auto output = intCodeComputer.takeOutput();
        const auto finalValue = output.back();
        output.pop_back();
        std::string outputStr;
        for (const auto ascii : output)
        {
            if (ascii != 10)
                outputStr += static_cast<char>(ascii);
            else
                outputStr += "\n";
        }
        std::cout << "IntCode: >>>" << outputStr << "<<<" << std::endl;
        std::cout << "finalValue: " << finalValue << std::endl;
    }


#if 0
    int largestNumThings = 0;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            if (worldMap[y][x] == '#')
            {
                for (int direction = 0; direction < 4; direction++)
                {
                    map<Coord, int> visitCount;
                    map<string, vector<Command>> movementFunctions;
                    vector<Command> pendingMovementFunctionDef;
                    vector<string> functionsExecuted;
                    numThings = 0;
                    blah(0, worldMap, visitCount, movementFunctions, pendingMovementFunctionDef, "A", functionsExecuted, {x, y}, direction);
                    cout << "x: " << x << " y: " << y << " dir: " << direction << " numThings: " << numThings << endl;
                    largestNumThings = max(largestNumThings, numThings);
                }
            }
        }
    }
    cout << "largestNumThings: " << largestNumThings << endl;
#endif

}

