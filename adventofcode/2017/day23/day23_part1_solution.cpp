#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <limits>

#include <cassert>

using namespace std;

struct Instruction
{
    enum Type { Set, Sub, Mul, Jump, Unknown };
    Type type = Unknown;
    char argReg[2] = { '\0', '\0'};
    int64_t argVal[2] = { std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::min()};
    int64_t getArgValue(int argNum, const map<char, int64_t>& registerValue) const
    {
        if (argReg[argNum] != '\0')
        {
            const auto regAndValIter = registerValue.find(argReg[argNum]);
            if (regAndValIter == registerValue.end())
                return 0;
            else
            {
                return regAndValIter->second;
            }
        }
        else
        {
            assert(argVal[argNum] != std::numeric_limits<int64_t>::min());
            return argVal[argNum];
        }
    };
    char getArgRegister(int argNum) const
    {
        assert(argReg[argNum] >= 'a' && argReg[argNum] <= 'z');
        return argReg[argNum];
    }
};

int main()
{
    vector<Instruction> instructions;
    string instructionLine;
    while (getline(cin, instructionLine))
    {
        Instruction instruction;
        istringstream instructionStream(instructionLine);
        string instructionType;
        instructionStream >> instructionType;
        auto readRegOrValArg = [&instructionStream, &instruction](int argNum)
        {
            char peekChar = '\0';
            instructionStream >> peekChar;
            if (peekChar >= 'a' && peekChar <= 'z')
            {
                instruction.argReg[argNum] = peekChar;
            }
            else
            {
                instructionStream.putback(peekChar);
                instructionStream >> instruction.argVal[argNum];
            }
        };
        if (instructionType == "set")
        {
            instruction.type = Instruction::Set;
            instructionStream >> instruction.argReg[0];
            readRegOrValArg(1);
        }
        else if (instructionType == "sub")
        {
            instruction.type = Instruction::Sub;
            instructionStream >> instruction.argReg[0];
            readRegOrValArg(1);
        }
        else if (instructionType == "mul")
        {
            instruction.type = Instruction::Mul;
            instructionStream >> instruction.argReg[0];
            readRegOrValArg(1);
        }
        else if (instructionType == "jnz")
        {
            instruction.type = Instruction::Jump;
            readRegOrValArg(0);
            readRegOrValArg(1);
        }
        else
        {
            assert(false);
        }
        assert(instructionStream);
        instructions.push_back(instruction);
    }

    map<char, int64_t> registerValue;
    int64_t instructionPointer = 0;
    int64_t numMults = 0;
    while(true)
    {
        if (instructionPointer < 0 || instructionPointer >= static_cast<int64_t>(instructions.size()))
            break;
        const auto& instruction = instructions[instructionPointer];
        switch (instruction.type)
        {
            case Instruction::Set:
                registerValue[instruction.getArgRegister(0)] = instruction.getArgValue(1, registerValue);
                break;
            case Instruction::Sub:
                registerValue[instruction.getArgRegister(0)] -= instruction.getArgValue(1, registerValue);
                break;
            case Instruction::Mul:
                registerValue[instruction.getArgRegister(0)] *= instruction.getArgValue(1, registerValue);
                numMults++;
                break;
            case Instruction::Jump:
                if (instruction.getArgValue(0, registerValue) != 0)
                {
                    instructionPointer += instruction.getArgValue(1, registerValue);
                    instructionPointer--; // Counteract the increment that always happens at the end of the loop.
                }
                break;
            default:
                break;
        }
        instructionPointer++;
    }
    std::cout << "numMults: " << numMults << std::endl;


    return 0;
}
