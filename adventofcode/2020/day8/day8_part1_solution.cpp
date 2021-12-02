#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int main()
{
    struct Instruction
    {
        string opCode;
        int64_t argument;
        bool alreadyExecuted = false;
    };
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

    int instructionIndex = 0;
    int64_t accumulatorValue = 0;
    while (true)
    {
        cout << "instructionIndex: " << instructionIndex << endl;
        cout << "opCode: " << instructions[instructionIndex].opCode << " argument: " << instructions[instructionIndex].argument << " alreadyExecuted: " << instructions[instructionIndex].alreadyExecuted << endl;
        assert(0 <= instructionIndex && instructionIndex < instructions.size());
        if (instructions[instructionIndex].alreadyExecuted)
            break;
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
    cout << accumulatorValue << endl;
}
