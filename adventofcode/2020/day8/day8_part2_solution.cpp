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

pair<bool, int> runInstructions(vector<Instruction> instructions)
{
    int instructionIndex = 0;
    int64_t accumulatorValue = 0;
    while (true)
    {
        assert(0 <= instructionIndex && instructionIndex <= instructions.size());
        if (instructionIndex == instructions.size())
        {
            return {true, accumulatorValue};
        }
        if (instructions[instructionIndex].alreadyExecuted)
        {
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
    int64_t finalAccumulatorValue = -1;
    for (auto& instruction : instructions)
    {
        const string originalOpCode = instruction.opCode;

        if (instruction.opCode == "jmp")
            instruction.opCode = "nop";
        else if (instruction.opCode == "nop")
            instruction.opCode = "jmp";

        if (const auto [terminatedNormally, accumulatorValue] = runInstructions(instructions); terminatedNormally)
        {
            assert(!foundSuccessfulChange);
            finalAccumulatorValue = accumulatorValue;
            foundSuccessfulChange = true;
        }


        instruction.opCode = originalOpCode;
    }
    assert(foundSuccessfulChange);
    cout << finalAccumulatorValue << endl;
}
