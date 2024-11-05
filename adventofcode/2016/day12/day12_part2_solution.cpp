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
    enum Type { Copy, Increment, Decrement, Jump, Unknown };
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
        if (instructionType == "cpy")
        {
            instruction.type = Instruction::Copy;
            readRegOrValArg(0);
            instructionStream >> instruction.argReg[1];
        }
        else if (instructionType == "inc")
        {
            instruction.type = Instruction::Increment;
            instructionStream >> instruction.argReg[0];
        }
        else if (instructionType == "dec")
        {
            instruction.type = Instruction::Decrement;
            instructionStream >> instruction.argReg[0];
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
    registerValue['c'] = 1;
    int64_t instructionPointer = 0;
    while(true)
    {
        if (instructionPointer < 0 || instructionPointer >= static_cast<int64_t>(instructions.size()))
            break;
        const auto& instruction = instructions[instructionPointer];
#if 0
        std::cout << "executing instruction at " << instructionPointer << std::endl;
        std::cout << "  current register values: " << std::endl;
        for (const auto& [regName, value] : registerValue)
        {
            std::cout << "  " << regName << ": " << value;
        }
        std::cout << std::endl;
#endif
        switch (instruction.type)
        {
            case Instruction::Copy:
                registerValue[instruction.getArgRegister(1)] = instruction.getArgValue(0, registerValue);
                break;
            case Instruction::Increment:
                registerValue[instruction.getArgRegister(0)]++;
                break;
            case Instruction::Decrement:
                registerValue[instruction.getArgRegister(0)]--;
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
#if 0
        std::cout << "  register values after instruction: " << std::endl;
        for (const auto& [regName, value] : registerValue)
        {
            std::cout << "  " << regName << ": " << value;
        }
        std::cout << std::endl;
#endif
        instructionPointer++;
    }
    std::cout << "final value of 'a': " << registerValue['a'] << std::endl;
    return 0;
}
