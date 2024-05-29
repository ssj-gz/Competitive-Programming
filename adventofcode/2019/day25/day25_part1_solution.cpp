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
    std::cout << "Telling IntCode: >" << inputLine << "<" << std::endl;
    vector<int64_t> input;
    for (const char letter : inputLine)
        input.push_back(static_cast<int64_t>(letter));
    input.push_back(10);
    intCodeComputer.addInputs(input);
    const auto status = intCodeComputer.run();
    //assert(status != IntCodeComputer::Terminated);

};

vector<string> readAsciiOutput(IntCodeComputer& intCodeComputer)
{
    intCodeComputer.run();
    const auto output = intCodeComputer.takeOutput();
    //assert(!output.empty());
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
        //cout << static_cast<char>(letter);
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
    const std::string indent(depth, ' ');

    auto printLines = [&indent](const vector<string>& lines)
    {
        for (const auto& line : lines) cout << indent << line << std::endl;
    };


    const std::string directionFollowed = (directionsFollowed.empty() ? "" : directionsFollowed.back());
    std::cout << indent << "Explore: depth " << depth << " directionFollowed:" << directionFollowed << std::endl;
    const auto roomDescription = readAsciiOutput(intCodeComputer);
    std::cout << indent << "roomDescription: " << std::endl;
    printLines(roomDescription);
    const RoomInfo roomInfo = parseRoomInfo(roomDescription);

    if (roomsSeen.contains(roomInfo.roomName))
    {
        std::cout << indent << "Already been to " << roomInfo.roomName << "; backtracking.  directionFollowed: " << directionFollowed << std::endl;
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
            continue;
        inputAsciiToIntCode(intCodeComputer, "take " + item);
        printLines(readAsciiOutput(intCodeComputer));
        inputAsciiToIntCode(intCodeComputer, "inv");
        printLines(readAsciiOutput(intCodeComputer));

    }

    for (const auto& direction : roomInfo.directions)
    {
        if (!directionFollowed.empty() && direction == oppositeDirection(directionFollowed))
            continue;
        std::cout << indent << "Heading: >" << direction << "<" << std::endl;
        inputAsciiToIntCode(intCodeComputer, direction);
        vector<string> newDirectionFollowed = directionsFollowed;
        newDirectionFollowed.push_back(direction);
        explore(intCodeComputer, roomsSeen, newDirectionFollowed, depth + 1, directionsToRoom);
    }

    std::cout << indent << "nothing more to do here; backtracking" << std::endl;
    if (depth != 0)
    {
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
    ifstream programIn("day25_input.txt", std::ios::in);
    while (true)
    {
        programIn >> programInput;
        program.push_back(programInput);
        assert(programIn);
        char comma;
        programIn >> comma;
        if (!programIn)
            break;
        assert(comma == ',');
    }

    IntCodeComputer intCodeComputer(program);
    const auto status = intCodeComputer.run();
    assert(status == IntCodeComputer::WaitingForInput);

    //const auto lines = readAsciiOutput();
    //printLines(lines);

#if 0
    string command;
    while (std::getline(cin, command))
    {
        inputAsciiToIntCode(command);
        const auto lines = readAsciiOutput();
        std::cout << "IntCode response: " << std::endl;
        printLines(lines);
        std::cout << "End IntCode response" << std::endl;

        RoomInfo roomInfo = parseRoomInfo(lines);

        assert(!roomInfo.roomName.empty());
        std::cout << "roomName: " << roomInfo.roomName << std::endl;
        std::cout << "directions: " << std::endl;
        for (const auto& direction : roomInfo.directions)
        {
            std::cout << " > " << direction << std::endl;
        }
        std::cout << "items: " << std::endl;
        for (const auto& item : roomInfo.items)
        {
            std::cout << " > " << item << std::endl;
        }

    }
#endif
    set<string> roomsSeen;
    map<string, vector<string>> directionsToRoom;
    vector<string> directionsFollowed;
    explore(intCodeComputer, roomsSeen, directionsFollowed, 0, directionsToRoom);

    std::cout << "Explored " << roomsSeen.size() << " rooms: " << std::endl;
    for (const auto& roomName : roomsSeen)
    {
        std::cout << " >" << roomName << "<, reached as follows:" << std::endl;
        for (const auto& direction : directionsToRoom[roomName])
            std::cout << " " << direction << std::endl; 

    }

    std::cout << "Navigating back to Pressure Sensitive Floor" << std::endl;
    const auto toPressureSensitiveFloor = directionsToRoom["Pressure-Sensitive Floor"];
    vector<string> toCheckpoint(toPressureSensitiveFloor.begin(), std::prev(toPressureSensitiveFloor.end()));
    for (const auto& direction : toCheckpoint)
    {
        inputAsciiToIntCode(intCodeComputer, direction);
        printLines(readAsciiOutput(intCodeComputer));
    }

    set<string> allItems;
    inputAsciiToIntCode(intCodeComputer, "inv");
    const auto& inventoryLines = readAsciiOutput(intCodeComputer);
    for (const auto& line : inventoryLines)
    {
        if (line.starts_with("- "))
            allItems.insert(line.substr(2));
    }
    std::cout << "Items collected: " << std::endl;
    for (const auto& item : allItems) std::cout << ">" << item << "<" << std::endl;
    for (const auto& item : allItems) 
    {
        inputAsciiToIntCode(intCodeComputer, "drop " + item);
        printLines(readAsciiOutput(intCodeComputer));
    }

    vector<set<string>> itemsCombinations;
    set<string> currentCombination;
    generateItemCombinations(allItems, allItems.cbegin(), currentCombination, itemsCombinations);
    for (const auto& combination : itemsCombinations)
    {
        std::cout << "combination: " << std::endl;
        for (const auto item : combination) std::cout << " " << item << std::endl;
        std::cout << std::endl;

        // Gather this combination.
        for (const auto item : combination)
        {
            inputAsciiToIntCode(intCodeComputer, "take " + item);
            printLines(readAsciiOutput(intCodeComputer));
        }

        inputAsciiToIntCode(intCodeComputer, toPressureSensitiveFloor.back());
        printLines(readAsciiOutput(intCodeComputer));
        if (intCodeComputer.isTerminated())
        {
            std::cout << "Terminated!" << std::endl;
            break;
        }

        // ... and drop it, ready for the next combination.
        for (const auto item : combination)
        {
            inputAsciiToIntCode(intCodeComputer, "drop " + item);
            printLines(readAsciiOutput(intCodeComputer));
        }
    }


}
