#define INTCODE_SILENT
#include "../shared/intcodecomputer.h"

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <fstream>
#include <cassert>

using namespace std;

struct RoomInfo
{
    string roomName;
    vector<string> directions;
    vector<string> items;
};

void inputAsciiToIntCode(IntCodeComputer& intCodeComputer, const std::string& inputLine)
{
    vector<int64_t> input;
    for (const char letter : inputLine)
        input.push_back(static_cast<int64_t>(letter));
    input.push_back(10);
    intCodeComputer.addInputs(input);
};

vector<string> readAsciiOutput(IntCodeComputer& intCodeComputer)
{
    intCodeComputer.run();
    const auto output = intCodeComputer.takeOutput();
    vector<string> lines;
    string currentLine;
    for (const auto letter : output)
    {
        if (letter == 10)
        {
            lines.push_back(currentLine);
            currentLine.clear();
        }
        else
        {
            currentLine.push_back(static_cast<char>(letter));
        }
    }
    lines.push_back(currentLine);

    return lines;
};

void printLines(const vector<string>& lines)
{
    for (const auto& line : lines) cout << line << std::endl;
}

RoomInfo parseRoomInfo(const vector<string>& roomDescription)
{
    string roomName;
    vector<string> directions;
    vector<string> items;
    bool isReadingDoorList = false;
    bool isReadingItemList = false;
    for (const auto& line : roomDescription)
    {
        if (line.starts_with("==") && line.ends_with("==") && roomName.empty())
        {
            assert(roomName.empty());
            roomName = line.substr(3, line.size() - 6);
        }
        else if (line == "Doors here lead:")
        {
            assert(!isReadingItemList);
            assert(!isReadingDoorList);
            isReadingDoorList = true;
        }
        else if (line == "Items here:")
        {
            assert(!isReadingItemList);
            assert(!isReadingDoorList);
            isReadingItemList = true;

        }
        else if (isReadingDoorList)
        {
            if (line.empty())
                isReadingDoorList = false;
            else
            {
                assert(line.starts_with("- "));
                directions.push_back(line.substr(2));

            }
        }
        else if (isReadingItemList)
        {
            if (line.empty())
                isReadingItemList = false;
            else
            {
                assert(line.starts_with("- "));
                items.push_back(line.substr(2));
            }
        }
    }

    return { roomName, directions, items };
}

string oppositeDirection(const std::string& direction)
{
    if (direction == "north")
    {
        return "south";
    }
    else if (direction == "east")
    {
        return "west";
    }
    else if (direction == "south")
    {
        return "north";
    }
    else if (direction == "west")
    {
        return "east";
    }
    assert(false);
    return "";
}

void explore(IntCodeComputer& intCodeComputer, set<string>& roomsSeen, const vector<string>& directionsFollowed, int depth, map<string, vector<string>>& directionsToRoom)
{
    const std::string directionFollowed = (directionsFollowed.empty() ? "" : directionsFollowed.back());
    const auto roomDescriptionText = readAsciiOutput(intCodeComputer);
    const RoomInfo roomInfo = parseRoomInfo(roomDescriptionText);

    if (roomsSeen.contains(roomInfo.roomName))
    {
        // Already explored this room; backtrack to previous room by following the opposite direction to the one that brought
        // us here.
        const string oppositeDir = oppositeDirection(directionFollowed);
        assert(std::find(roomInfo.directions.begin(), roomInfo.directions.end(), oppositeDir) != roomInfo.directions.end());
        inputAsciiToIntCode(intCodeComputer, oppositeDir);
        readAsciiOutput(intCodeComputer); // Swallow output.
        return;
    }

    roomsSeen.insert(roomInfo.roomName);
    directionsToRoom[roomInfo.roomName] = directionsFollowed;

    for (const auto& item : roomInfo.items)
    {
        if (item == "infinite loop" || item == "molten lava" || item == "photons" || item == "giant electromagnet" || item == "escape pod")
        {
            // Skip the "deadly" items that I found by trial and error, Groundhog-day style!
            continue;
        }
        inputAsciiToIntCode(intCodeComputer, "take " + item);
        readAsciiOutput(intCodeComputer); // Swallow output.

    }

    // Explore further, but don't go back the way we came until 
    // we've exhausted all other directions.
    for (const auto& direction : roomInfo.directions)
    {
        if (!directionFollowed.empty() && direction == oppositeDirection(directionFollowed))
            continue;
        inputAsciiToIntCode(intCodeComputer, direction);
        vector<string> newDirectionFollowed = directionsFollowed;
        newDirectionFollowed.push_back(direction);
        explore(intCodeComputer, roomsSeen, newDirectionFollowed, depth + 1, directionsToRoom);
    }

    if (depth != 0)
    {
        // We've explored this room (and the rooms it leads to); go back the way we came.
        inputAsciiToIntCode(intCodeComputer, oppositeDirection(directionFollowed));
        readAsciiOutput(intCodeComputer); // Swallow output.
    }
}

void generateItemCombinations(const set<string>& allItems, set<string>::const_iterator itemsIter, set<string>& currentCombination, vector<set<string>>& itemsCombinations)
{
    if (itemsIter == allItems.end())
    {
        itemsCombinations.push_back(currentCombination);
        return;
    }

    // Don't include this item.
    generateItemCombinations(allItems, std::next(itemsIter), currentCombination, itemsCombinations);
    // Do include this item.
    currentCombination.insert(*itemsIter);
    generateItemCombinations(allItems, std::next(itemsIter), currentCombination, itemsCombinations);
    currentCombination.erase(*itemsIter);
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

    IntCodeComputer intCodeComputer(program);
    const auto status = intCodeComputer.run();
    assert(status == IntCodeComputer::WaitingForInput);

    set<string> roomsSeen;
    map<string, vector<string>> directionsToRoom;
    vector<string> directionsFollowed;
    explore(intCodeComputer, roomsSeen, directionsFollowed, 0, directionsToRoom);

    // Navigate back to just outside the Pressure-Sensitive Floor room. 
    const auto toPressureSensitiveFloor = directionsToRoom["Pressure-Sensitive Floor"];
    vector<string> toCheckpoint(toPressureSensitiveFloor.begin(), std::prev(toPressureSensitiveFloor.end()));
    for (const auto& direction : toCheckpoint)
    {
        inputAsciiToIntCode(intCodeComputer, direction);
        readAsciiOutput(intCodeComputer); // Swallow output.
    }

    set<string> allItems;
    inputAsciiToIntCode(intCodeComputer, "inv");
    const auto& inventoryLines = readAsciiOutput(intCodeComputer);
    for (const auto& line : inventoryLines)
    {
        if (line.starts_with("- "))
            allItems.insert(line.substr(2));
    }
    // Drop everything, ready for the "trial and error" portion.
    for (const auto& item : allItems) 
    {
        inputAsciiToIntCode(intCodeComputer, "drop " + item);
        readAsciiOutput(intCodeComputer); // Swallow output.
    }

    vector<set<string>> itemsCombinations;
    set<string> currentCombination;
    generateItemCombinations(allItems, allItems.cbegin(), currentCombination, itemsCombinations);
    for (const auto& combination : itemsCombinations)
    {
        // Gather up this combination of items ...
        for (const auto item : combination)
        {
            inputAsciiToIntCode(intCodeComputer, "take " + item);
            readAsciiOutput(intCodeComputer); // Swallow output.
        }

        // ... enter the Pressure-Sensitive Floor room with this combination of
        // Items ...
        inputAsciiToIntCode(intCodeComputer, toPressureSensitiveFloor.back());
        printLines(readAsciiOutput(intCodeComputer)); // The solution will be in the final text printed
                                                      // prior to termination.
        if (intCodeComputer.isTerminated())
        {
            std::cout << "IntCodeComputer Terminated; we're done :)" << std::endl;
            break;
        }

        // ... and drop everything, ready for the next combination.
        for (const auto item : combination)
        {
            inputAsciiToIntCode(intCodeComputer, "drop " + item);
            readAsciiOutput(intCodeComputer); // Swallow output.
        }
    }
}
