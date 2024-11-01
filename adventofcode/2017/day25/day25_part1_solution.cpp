#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <regex>

#include <cassert>

using namespace std;

int main()
{
    string currentBlock;
    vector<string> blocks;
    do
    {
        string line;
        getline(cin, line);
        currentBlock += line;
        if (!cin || line.empty())
        {
            blocks.push_back(currentBlock);
            currentBlock.clear();
        }
    } while (cin);

    std::regex preambleRegex(R"(^Begin in state ([A-Z])\.Perform a diagnostic checksum after (\d+) steps\.$)");
    std::smatch preambleMatch;
    const auto preamble = blocks.front();
    std::cout << "preamble: >" << preamble << "<" << std::endl;
    const bool preambleMatchSuccessful = std::regex_match(preamble, preambleMatch, preambleRegex);
    assert(preambleMatchSuccessful);
    const auto startingStateName = std::string(preambleMatch[1]).front();
    const auto diagnosticStepNum = std::stoll(preambleMatch[2]);
    std::cout << "Starting at state: " << startingStateName << " perform diagnostic at: " << diagnosticStepNum << std::endl;
    blocks.erase(blocks.begin());

    std::regex instructionRegex(R"(^In state ([A-Z]): If the current value is 0: - Write the value ([01]). - Move one slot to the (left|right). - Continue with state ([A-Z]). If the current value is 1: - Write the value ([01]). - Move one slot to the (left|right). - Continue with state ([A-Z]).$)");

    enum Direction { Left, Right, Unknown };
    struct Instruction
    {
        char appliesToStateName = '\0';

        int valueToWriteIf0 = -1;
        Direction dirToMoveIf0 = Unknown;
        char nextStateNameIf0 = '\0';

        int valueToWriteIf1 = -1;
        Direction dirToMoveIf1 = Unknown;
        char nextStateNameIf1 = '\0';
    };

    vector<Instruction> instructions;
    for (auto& instructionStr : blocks)
    {
        std::cout << "Instruction: " << std::endl;
        // Remove duplicate spaces.
        instructionStr.erase(std::unique(instructionStr.begin(), instructionStr.end(), [](const auto& character, const auto& nextCharacter) { return character == ' ' && nextCharacter == ' ';}), instructionStr.end());
        std::cout << instructionStr << std::endl;

        std::smatch instructionMatch;
        const bool matchSuccessful = std::regex_match(instructionStr, instructionMatch, instructionRegex);
        assert(matchSuccessful);

        Instruction instruction;
        instruction.appliesToStateName = std::string(instructionMatch[1]).front();

        instruction.valueToWriteIf0 = stoi(instructionMatch[2]);
        instruction.dirToMoveIf0 = (instructionMatch[3] == "left" ? Left : Right);
        instruction.nextStateNameIf0 = std::string(instructionMatch[4]).front();

        instruction.valueToWriteIf1 = stoi(instructionMatch[5]);
        instruction.dirToMoveIf1 = (instructionMatch[6] == "left" ? Left : Right);
        instruction.nextStateNameIf1 = std::string(instructionMatch[7]).front();

        instructions.push_back(instruction);
    }

    map<int, int> tapeContents;
    int cursorPos = 0;
    char stateName = startingStateName;
    int stepsTaken = 0;
    while (true)
    {
        assert(std::count_if(instructions.begin(), instructions.end(), [&stateName](const auto& instruction) { return instruction.appliesToStateName == stateName;}) == 1);
        auto instructionIter = std::find_if(instructions.begin(), instructions.end(), [&stateName](const auto& instruction) { return instruction.appliesToStateName == stateName;});
        assert(instructionIter != instructions.end());
        const auto& instruction = *instructionIter;

        if (tapeContents[cursorPos] == 0)
        {
            tapeContents[cursorPos] = instruction.valueToWriteIf0;
            cursorPos += (instruction.dirToMoveIf0 == Left ? -1 : +1);
            stateName = instruction.nextStateNameIf0;
        }
        else
        {
            tapeContents[cursorPos] = instruction.valueToWriteIf1;
            cursorPos += (instruction.dirToMoveIf1 == Left ? -1 : +1);
            stateName = instruction.nextStateNameIf1;
        }

        stepsTaken++;
        if (stepsTaken == diagnosticStepNum)
        {
            int64_t checkSum = 0;
            for (const auto [tapePos, value] : tapeContents)
            {
                assert(value == 0 || value == 1);
                checkSum += value;
            }
            std::cout << "checkSum: " << checkSum << std::endl;
            break;
        }
    }



    return 0;
}
