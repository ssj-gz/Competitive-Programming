#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

struct Instruction
{
    string opCode;
    int64_t argument;
    bool alreadyExecuted = false;
};

pair<bool, int> terminatesCorrectly(vector<Instruction> instructions)
{
    cout << "Executing ... " << endl;
    int instructionIndex = 0;
    int64_t accumulatorValue = 0;
    while (true)
    {
        cout << " instructionIndex: " << instructionIndex << endl;
        assert(0 <= instructionIndex && instructionIndex <= instructions.size());
        if (instructionIndex == instructions.size())
        {
            cout << " hooray!" << endl;
            return {true, accumulatorValue};
        }
        cout << " opCode: " << instructions[instructionIndex].opCode << " argument: " << instructions[instructionIndex].argument << " alreadyExecuted: " << instructions[instructionIndex].alreadyExecuted << endl;
        if (instructions[instructionIndex].alreadyExecuted)
        {
            cout << " infinite loop" << endl;
            return {false, -1};
        }
        const auto opCode = instructions[instructionIndex].opCode;
        const auto argument = instructions[instructionIndex].argument;
        instructions[instructionIndex].alreadyExecuted = true; 

        if (opCode == "nop")
        {
            instructionIndex++;
        }
        else if (opCode == "acc")
        {
            accumulatorValue += argument;
            instructionIndex++;
        }
        else if (opCode == "jmp")
        {
            instructionIndex += argument;
        }
        else
        {
            assert(false);
        }
    }

}

int main()
{
    vector<Instruction> instructions;
    while (true)
    {
        string opCode;
        cin >> opCode;
        if (!cin)
            break;

        int64_t argument;
        cin >> argument;

        instructions.push_back({opCode, argument, false});
    }

    bool foundSuccessfulChange = false;
    int instructionIndex = 0;
    int64_t finalAccumulatorValue = -1;
    for (auto& instruction : instructions)
    {
        const string originalOpCode = instruction.opCode;

        if (instruction.opCode == "jmp")
            instruction.opCode = "nop";
        else if (instruction.opCode == "nop")
            instruction.opCode = "jmp";

        if (instruction.opCode != originalOpCode)
            cout << "Changing instruction at index " << instructionIndex << " from " << originalOpCode << " to " << instruction.opCode << endl;

        const auto executionResult = terminatesCorrectly(instructions);
        if (executionResult.first)
        {
            assert(!foundSuccessfulChange);
            finalAccumulatorValue = executionResult.second;
            foundSuccessfulChange = true;
        }


        instruction.opCode = originalOpCode;
        instructionIndex++;
    }
    assert(foundSuccessfulChange);
    cout << finalAccumulatorValue << endl;
}
