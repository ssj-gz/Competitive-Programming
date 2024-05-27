#include "../shared/intcodecomputer.h"

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <limits>

using namespace std;

struct Coord
{
    int x = -1;
    int y = -1;
    auto operator<=>(const Coord&) const = default;
};

vector<string> computeMapAndGetPrompt(const vector<int64_t>& program)
{
    IntCodeComputer intCodeComputer(program);
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
int largestVisitCount = 0;
int numThings = 0;
void blah(int depth, const vector<string>& worldMap, map<Coord, int>& visitCount, map<string, vector<Command>>& movementFunctions, vector<Command>& pendingMovementFunctionDef, const string& pendingMovementFnName, vector<string>& functionsExecuted, const Coord& currentCoord, const int direction)
{
    if (pendingMovementFnName == "B")
    {
        numThings++;
        return;
    }
    const string indent = string(depth, ' ');
    const int height = static_cast<int>(worldMap.size());
    const int width = static_cast<int>(worldMap[0].size());

    auto isValidCoord = [&worldMap, width, height](const Coord& coord)
    {
        if (coord.x < 0 || coord.x >= width)
            return false;
        if (coord.y < 0 || coord.y >= height)
            return false;
        if (worldMap[coord.y][coord.x] == '.')
            return false;
        return true;
    };
    if (pendingMovementFunctionDef.size() > 10)
        return;

    if (!isValidCoord(currentCoord))
        return;

    auto coordInDirection = [](const Coord& coord, int direction)
    {
        Coord coordInDirection = coord;
        switch(direction)
        {
            case 0:
                // Up.
                coordInDirection.y--;
                break;
            case 1:
                // Right.
                coordInDirection.x++;
                break;
            case 2:
                // Down.
                coordInDirection.y++;
                break;
            case 3:
                // Left.
                coordInDirection.x--;
                break;
        };
        return coordInDirection;
    };
    const auto dbgVisitCount = visitCount;
#if 0
    for (const auto [coord, numTimesVisited] : visitCount)
    {
        if (numTimesVisited > 0)
            cout << indent << " visited " << coord.x << "," << coord.y << " " << numTimesVisited << " times" << endl;
    }
#endif

    visitCount[currentCoord]++;

#if 0
    cout << indent << "Blee: pendingMovementFnName: " << pendingMovementFnName << "#" << pendingMovementFunctionDef.size() << " dir: " << direction << " pos: " << currentCoord.x << "," << currentCoord.y << endl;
    cout << "Executed " << functionsExecuted.size() << " fns: ";
    for (const auto x : functionsExecuted)
    {
        cout << x << " ";
    }
    cout << endl;
#endif
    auto printCommands = [](const vector<Command>& commands)
    {
        cout << commands.size() << " commands: ";
        for (const auto& cmd : commands)
        {
            switch (cmd.cmd)
            {
                case Command::Left:
                    cout << "L";
                    break;
                case Command::Right:
                    cout << "R";
                    break;
                case Command::Forward:
                    cout << "F" << cmd.amount;
                    break;
                default:
                    assert(false);
            }
            cout << " ";
        }
    };
    auto coordsReachedByFollowingCmds = [&coordInDirection,&isValidCoord](const auto& startCoord, int direction, const vector<Command>& commands, Coord& endCoord, int& endDirection)
    {
        endCoord = startCoord;
        endDirection = direction;

        // "Execute" this function.
        vector<Coord> visitedCoords;
        for (const auto& command : commands)
        {
            switch(command.cmd)
            {
                case Command::Left:
                    endDirection = (endDirection + 4 - 1) % 4;
                    break;
                case Command::Right:
                    endDirection = (endDirection + 1) % 4;
                    break;
                case Command::Forward:
                    for (int i = 0; i < command.amount; i++)
                    {
                        endCoord = coordInDirection(endCoord, endDirection);
                        if (!isValidCoord(endCoord))
                        {
                            endCoord = {-1, -1};
                            endDirection = -1;
                            return vector<Coord>{};
                        }
                        visitedCoords.push_back(endCoord);
                    }
                    break;
            }
        }
        return visitedCoords;

    };
#if 1
#if 0
    int distinctVisited = 0;
    int needToVisit = 0;
    for (int row = 0; row < height; row++)
    {
        cout << indent;
        for (int col = 0; col < width; col++)
        {
            assert((visitCount[{col, row}] >= 0));
            if (row == currentCoord.y && col == currentCoord.x)
            {
                cout << "+";
            }
            else if (visitCount[{col, row}] > 0)
            {
                cout << "*";
                distinctVisited++;
            }
            else
            {
                cout << worldMap[row][col];
            }
            if (worldMap[row][col] == '#')
                needToVisit++;
        }
        cout << endl;
    }
    cout << "distinctVisited: " << distinctVisited << " needToVisit: " << needToVisit << endl;
    if (distinctVisited > largestVisitCount)
    {
        largestVisitCount = distinctVisited;
        cout << "New largestVisitCount: " << largestVisitCount << " (need " << needToVisit << ")" << endl;
    }
#endif
#endif

#if 0
    vector<std::pair<string, vector<Command>>> allFunctions;
    for (const auto& [fnName, fnDef] : movementFunctions)
    {
        allFunctions.push_back({fnName, fnDef});
    }
    if (!pendingMovementFnName.empty())
    {
        allFunctions.push_back({pendingMovementFnName, pendingMovementFunctionDef});
    }
    for (const auto& [fnName, fnDef] : allFunctions)
    {
        cout << indent << fnName << ": ";
        printCommands(fnDef);
    }

    if (movementFunctions.size() == 3)
    {
        vector<std::tuple<string, int, int>> fnNameAndMaxReach;
        for (const auto& [fnName, fnDef] : allFunctions)
        {
            cout << indent << fnName << ": ";
            printCommands(fnDef);
            int coordsReached = 0;
            int numCoordsCanExecuteFrom = 0;

            for (int row = 0; row < height; row++)
            {
                for (int col = 0; col < width; col++)
                {
                    Coord endCoord;
                    int endDirection;
                    bool canExecuteFromHere = false;
                    for (int startDirection = 0; startDirection < 4; startDirection++)
                    {
                        const auto blah = coordsReachedByFollowingCmds(Coord{col, row}, startDirection, fnDef, endCoord, endDirection); 
                        coordsReached = max(coordsReached, static_cast<int>(set<Coord>(blah.begin(), blah.end()).size()));
                        if (endCoord.x != -1)
                        {
                            numCoordsCanExecuteFrom++;
                        }

                    }

                }
            }
            cout << "coordsReached: " << coordsReached << " numCoordsCanExecuteFrom: " << numCoordsCanExecuteFrom;
            cout << endl;
            fnNameAndMaxReach.push_back({fnName, coordsReached, numCoordsCanExecuteFrom}); 
        }
        sort(fnNameAndMaxReach.begin(), fnNameAndMaxReach.end(), [](const auto& lhs, const auto& rhs)
                {
                return get<1>(lhs) > get<1>(rhs);
                });
        int maxCoordsReachable = 0;
        int numFunctionCallsRemaining = maxFunctionCalls - static_cast<int>(functionsExecuted.size()); 
        for (const auto& [fnName, coordsReached, numCoordsCanExecuteFrom] : fnNameAndMaxReach)
        {
            if (numFunctionCallsRemaining < 0)
                break;
            const int numTimesToCall = min(numFunctionCallsRemaining, numCoordsCanExecuteFrom);
            cout << indent << "numFunctionCallsRemaining: " << numFunctionCallsRemaining << " fnName: " << fnName << " numTimesToCall: " << numTimesToCall << endl;
            maxCoordsReachable += numTimesToCall * coordsReached;
            numFunctionCallsRemaining -= numTimesToCall;
        }
        cout << indent << "maxCoordsReachable: " << maxCoordsReachable << " and need: " << (needToVisit - distinctVisited) << endl;
        if (maxCoordsReachable < (needToVisit - distinctVisited))
        {
            cout << indent << "pruning" << endl;
            visitCount[currentCoord]--;
            return;
        }
    }

    auto totalAmountForward = [](const vector<Command>& commands)
    {
        int totalAmount = 0;
        for (const auto& cmd : commands)
        {
            if (cmd.cmd == Command::Forward)
                totalAmount += cmd.amount;
        }
        return totalAmount;
    };


    const bool pendingMovementFunctionHasForward = (totalAmountForward(pendingMovementFunctionDef) >= 1);
    int totalAmountForwardForAllFns = 0;
    for (const auto& [fnname, fndef] : movementFunctions)
    {
        totalAmountForwardForAllFns += totalAmountForward(fndef);
    }
    totalAmountForwardForAllFns += totalAmountForward(pendingMovementFunctionDef);
#endif
    if (!pendingMovementFnName.empty() && !pendingMovementFunctionDef.empty()/* && pendingMovementFunctionHasForward && (maxFunctionCalls * totalAmountForwardForAllFns >= needToVisit - distinctVisited)*/   )
    {
#if 0
        bool isWorkable = true;
        if (pendingMovementFnName == "C")
        {
            int maxTotalCoverage = 0;
            movementFunctions[pendingMovementFnName] = pendingMovementFunctionDef;
            for (const auto& [movementFnName, movementFunctionDef] : movementFunctions)
            {
                int maxCoordsReached = 0;
                int numCoordsCanExecuteFrom = 0;

                for (int row = 0; row < height; row++)
                {
                    for (int col = 0; col < width; col++)
                    {
                        Coord endCoord;
                        int endDirection;
                        bool canExecuteFromHere = false;
                        for (int startDirection = 0; startDirection < 4; startDirection++)
                        {
                            const auto blah = coordsReachedByFollowingCmds(Coord{col, row}, startDirection, movementFunctionDef, endCoord, endDirection); 
                            maxCoordsReached += static_cast<int>(blah.size());
                            if (!blah.empty())
                            {
                                canExecuteFromHere = true;
                            }

                        }
                        if (canExecuteFromHere)
                        {
                            numCoordsCanExecuteFrom++;
                        }

                    }
                }
                maxTotalCoverage += min(maxFunctionCalls, numCoordsCanExecuteFrom) * maxCoordsReached;
            }

            isWorkable = (maxTotalCoverage >= needToVisit - distinctVisited);

            if (!isWorkable)
            {
                movementFunctions.erase(pendingMovementFnName);
            }

        }
#endif
        if (true)
        {
            // Store this movement function definition.
            //cout << indent << "Storing " << pendingMovementFnName << " with " << pendingMovementFunctionDef.size() << " movements";
            movementFunctions[pendingMovementFnName] = pendingMovementFunctionDef;
            functionsExecuted.push_back(pendingMovementFnName);
            // Recurse.
            vector<Command> newPendingMovementFunctionDef;
            string newPendingMovementFnName;
            if (pendingMovementFnName[0] != 'C')
            {
                newPendingMovementFnName = pendingMovementFnName;
                newPendingMovementFnName[0] = static_cast<char>(newPendingMovementFnName[0] + 1);
            }
            blah(depth + 1, worldMap, visitCount, movementFunctions, newPendingMovementFunctionDef, newPendingMovementFnName, functionsExecuted, currentCoord, direction);
            // Pop this movement function definition.
            movementFunctions.erase(pendingMovementFnName);
            functionsExecuted.pop_back();
        }
    }
    if (!pendingMovementFnName.empty())
    {
        if (pendingMovementFunctionDef.size() <= 10)
        {
            if (pendingMovementFunctionDef.empty() || pendingMovementFunctionDef.back().cmd == Command::Forward)
            {
                // Turn Left (once, then twice) and recurse (then pop).
                //cout << indent << "Adding 'left' to " << pendingMovementFnName << endl;

                pendingMovementFunctionDef.push_back({Command::Left});
                blah(depth + 1, worldMap, visitCount, movementFunctions, pendingMovementFunctionDef, pendingMovementFnName, functionsExecuted, currentCoord, (direction + 4 - 1) % 4);

                pendingMovementFunctionDef.push_back({Command::Left});
                blah(depth + 1, worldMap, visitCount, movementFunctions, pendingMovementFunctionDef, pendingMovementFnName, functionsExecuted, currentCoord, (direction + 4 - 1) % 4);

                pendingMovementFunctionDef.pop_back();
                pendingMovementFunctionDef.pop_back();
            }

            if (pendingMovementFunctionDef.empty() || pendingMovementFunctionDef.back().cmd == Command::Forward)
            {
                // Turn Right and recurse (then pop).
                //cout << indent << "Adding 'right' to " << pendingMovementFnName << endl;

                pendingMovementFunctionDef.push_back({Command::Right});
                blah(depth + 1, worldMap, visitCount, movementFunctions, pendingMovementFunctionDef, pendingMovementFnName, functionsExecuted, currentCoord, (direction + 1) % 4);
                pendingMovementFunctionDef.pop_back();

#if 0
                pendingMovementFunctionDef.push_back({Command::Right});
                blah(depth + 1, worldMap, visitCount, movementFunctions, pendingMovementFunctionDef, pendingMovementFnName, functionsExecuted, currentCoord, (direction + 1) % 4);

                pendingMovementFunctionDef.pop_back();
                pendingMovementFunctionDef.pop_back();
#endif

            }

            if (pendingMovementFunctionDef.empty() || pendingMovementFunctionDef.back().cmd != Command::Forward)
            {
                // Move forward by varying amounts until we can't go any further, recursing (and popping) each time.
                // Optimisastion heuristic: consider largest amounts before smaller ones.
                Coord furthestForwardCoord = currentCoord;
                int largestAmountForward = 0;
                {
                    Coord nextCoord = currentCoord;
                    int amountForward = 0;
                    while (true)
                    {
                        nextCoord = coordInDirection(nextCoord, direction);
                        amountForward++;
                        if (!isValidCoord(nextCoord))
                        {
                            break;
                        }
                        furthestForwardCoord = nextCoord;
                        largestAmountForward = amountForward;
                        visitCount[furthestForwardCoord]++;
                    }
                }



                Coord nextCoord = furthestForwardCoord;
                int amountForward = largestAmountForward;
                while (nextCoord != currentCoord)
                {

                    pendingMovementFunctionDef.push_back({Command::Forward, amountForward});
                    //cout << indent << "Adding 'forward, " << amountForward << "' to " << pendingMovementFnName << endl;
                    blah(depth + 1, worldMap, visitCount, movementFunctions, pendingMovementFunctionDef, pendingMovementFnName, functionsExecuted, nextCoord, direction);
                    // Pop.
                    pendingMovementFunctionDef.pop_back();
                    visitCount[nextCoord]--;

                    nextCoord = coordInDirection(nextCoord, (direction + 2) % 4);
                    amountForward--;
                }

            }
        }
    }

    if (pendingMovementFunctionDef.empty() && !movementFunctions.empty() && functionsExecuted.size() <= 10)
    {
        // We haven't started defining a movement function; use one of the existing ones.
        for (const auto& [movementFunctionName, movementFunctionDefs] : movementFunctions)
        {
            Coord endCoord;
            int endDirection;
            const auto visitedCoords = coordsReachedByFollowingCmds(currentCoord, direction, movementFunctionDefs, endCoord, endDirection);
            if (endDirection != -1)
            {
                for (const auto& visited : visitedCoords)
                {
                    visitCount[visited]++;
                }
                // Recurse.
                cout << indent << "Executed function " << movementFunctionName << endl;
                functionsExecuted.push_back(movementFunctionName);
                blah(depth + 1, worldMap, visitCount, movementFunctions, pendingMovementFunctionDef, pendingMovementFnName, functionsExecuted, endCoord, endDirection);
                // Pop.
                functionsExecuted.pop_back();
                for (const auto& coord:  visitedCoords)
                {
                    visitCount[coord]--;
                }
            }
        }
    }
    visitCount[currentCoord]--;

    //cout << "visitCount: " << visitCount.size() << " dbgVisitCount: " << dbgVisitCount.size() << endl;
    //assert(visitCount == dbgVisitCount);
}

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
    //std::cout << "commandsFollowed: >" << toString(commandsFollowed) << "< size: " << toString(commandsFollowed).size() << " commandsFollowedStringLen: " << commandsFollowedStringLen << std::endl;
    assert(toString(commandsFollowed).size() == commandsFollowedStringLen);
    if (commandsFollowedStringLen > 20)
        return;
    if (worldMap[state.coord.y][state.coord.x] != '#')
        return;

    if (!commandsFollowed.empty())
    {
        std::cout << " adding command list: " << toString(commandsFollowed) << std::endl;
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

map<RobotState, vector<vector<Command>>> buildValidCommandListForAllStates(const vector<string>& worldMap)
{
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
                    std::cout << "Generating valid command lists for coord (" << x << "," << y << ") starting with direction: " << direction << std::endl;
                    startingState.direction = static_cast<RobotState::Direction>(direction);
                    vector<Command> commandsFollowed;
                    int commandsFollowedStringLen = 0;
                    vector<vector<Command>> destCommandList;
                    buildValidCommandList(startingState, commandsFollowed, commandsFollowedStringLen, destCommandList, worldMap);
                    std::cout << " destCommandList.size: " << destCommandList.size() << std::endl;

                    struct Outcome
                    {
                        set<Coord> cellsCovered;
                        RobotState endingState;
                        auto operator<=>(const Outcome&) const = default;
                    };
                    set<Outcome> commandListOutcomes;
                    sort(destCommandList.begin(), destCommandList.end(), [](const auto& lhsCommandList, const auto& rhsCommandList) {
                                return lhsCommandList.size() < rhsCommandList.size();
                            });
                    vector<vector<Command>> reducedCommandList;
                    for (const auto& commands : destCommandList)
                    {
                        RobotState state;
                        state.coord = {x, y};
                        state.direction = static_cast<RobotState::Direction>(direction);

                        set<Coord> cellsCovered = {state.coord};
                        for (const auto& command : commands)
                        {
                            state.applyCommand(command);
                            cellsCovered.insert(state.coord);
                        }
                        Outcome outcome;
                        outcome.cellsCovered = cellsCovered;
                        outcome.endingState = state;

                        if (!commandListOutcomes.contains(outcome)) {
                            reducedCommandList.push_back(commands);
                            commandListOutcomes.insert(outcome);
                        }


                    }
                    std::cout << " reducedCommandList.size: " << reducedCommandList.size() << std::endl;
                }
            }
        }
    }
    return map<RobotState, vector<vector<Command>>>();
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
    const auto mapAndPrompt = computeMapAndGetPrompt(program);
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

    const string mainPrompt = mapAndPrompt.back();
    assert(mainPrompt == "Main:");
    cout << "mainPrompt: " << mainPrompt << endl;

    buildValidCommandListForAllStates(worldMap);

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

