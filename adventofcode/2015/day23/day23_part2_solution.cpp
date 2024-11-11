#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>

#include <cassert>

using namespace std;

int main()
{
    struct Instruction
    {
        enum Type { Halve, Triple, Increment, JumpOffset, JumpIfEven, JumpIfOne, Unknown};
        Type type = Unknown;

        char regName = '\0';
        
        int offset = 0;
    };
    string instructionLine;
    vector<Instruction> instructions;
    while (getline(cin, instructionLine))
    {
        std::replace(instructionLine.begin(), instructionLine.end(), ',', ' ');
        Instruction instruction;
        istringstream instructionStream(instructionLine);
        string instructionName;
        instructionStream >> instructionName;

        if (instructionName == "hlf")
        {
            instruction.type = Instruction::Halve;
            instructionStream >> instruction.regName;
        }
        else if (instructionName == "tpl")
        {
            instruction.type = Instruction::Triple;
            instructionStream >> instruction.regName;
        }
        else if (instructionName == "inc")
        {
            instruction.type = Instruction::Increment;
            instructionStream >> instruction.regName;
        }
        else if (instructionName == "jmp")
        {
            instruction.type = Instruction::JumpOffset;
            instructionStream >> instruction.offset;
        }
        else if (instructionName == "jie")
        {
            instruction.type = Instruction::JumpIfEven;
            instructionStream >> instruction.regName;
            instructionStream >> instruction.offset;
        }
        else if (instructionName == "jio")
        {
            instruction.type = Instruction::JumpIfOne;
            instructionStream >> instruction.regName;
            instructionStream >> instruction.offset;
        }
        else
            assert(false);
        assert(instructionStream);
        assert(instruction.type != Instruction::Unknown);

        instructions.push_back(instruction);
    }

    int instructionPointer = 0;
    map<char, int> registerValues = { {'a', 1}};
    while (true)
    {
        if (instructionPointer < 0 || instructionPointer >= static_cast<int>(instructions.size()))
        {
            break;
        }
        std::cout << "About to execute instruction # " << instructionPointer << std::endl;
        const auto& instruction = instructions[instructionPointer];
        switch (instruction.type)
        {
            case Instruction::Halve:
                assert(instruction.regName != '\0');
                registerValues[instruction.regName] /= 2;
                instructionPointer++;
                break;
            case Instruction::Triple:
                assert(instruction.regName != '\0');
                registerValues[instruction.regName] *= 3;
                instructionPointer++;
                break;
            case Instruction::Increment:
                assert(instruction.regName != '\0');
                registerValues[instruction.regName]++;
                instructionPointer++;
                break;
            case Instruction::JumpOffset:
                instructionPointer += instruction.offset;
                break;
            case Instruction::JumpIfEven:
                assert(instruction.regName != '\0');
                if (registerValues[instruction.regName] % 2 == 0)
                    instructionPointer += instruction.offset;
                else 
                    instructionPointer++;
                break;
            case Instruction::JumpIfOne:
                assert(instruction.regName != '\0');
                if (registerValues[instruction.regName] == 1)
                    instructionPointer += instruction.offset;
                else 
                    instructionPointer++;
                break;
            default:
                assert(false);
        }
        std::cout << " reg values after execution: " << std::endl;
        for (const auto& [regName, value] : registerValues)
        {
            std::cout << "  regName: " << regName << " has value: " << value << std::endl;
        }
    }
    for (const auto& [regName, value] : registerValues)
    {
        std::cout << "regName: " << regName << " has value: " << value << std::endl;
    }
    return 0;
}
