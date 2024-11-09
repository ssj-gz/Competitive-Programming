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
    enum Type { Copy, Increment, Decrement, Jump, Toggle, Unknown };
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
        else if (instructionType == "tgl")
        {
            instruction.type = Instruction::Toggle;
            readRegOrValArg(0);
        }
        else
        {
            assert(false);
        }
        assert(instructionStream);
        instructions.push_back(instruction);
    }

    map<char, int64_t> registerValue;
    registerValue['a'] = 12;
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
        // Shortcut.
        if (instructionPointer == 5 && instructions[7].type == Instruction::Jump && instructions[9].type == Instruction::Jump)
        {
            // The instructions from 5 - 9, inclusive, perform "a += c * d" in a very time-consuming manner.
            // Use a shortcut.
            std::cout << "Multiply? c: " << registerValue['c'] << " d: " << registerValue['d'] << std::endl;
            registerValue['a'] += registerValue['c'] * registerValue['d'];
            instructionPointer = 9; // Actually want to go to 10, which the instructionPointer++ does for us.
        }
        else
        {

            switch (instruction.type)
            {
                case Instruction::Copy:
                    if (instruction.argReg[1] != '\0')
                        registerValue[instruction.getArgRegister(1)] = instruction.getArgValue(0, registerValue);
                    break;
                case Instruction::Increment:
                    if (instruction.argReg[0] != '\0')
                        registerValue[instruction.getArgRegister(0)]++;
                    break;
                case Instruction::Decrement:
                    if (instruction.argReg[0] != '\0')
                        registerValue[instruction.getArgRegister(0)]--;
                    break;
                case Instruction::Jump:
                    if (instruction.getArgValue(0, registerValue) != 0)
                    {
                        instructionPointer += instruction.getArgValue(1, registerValue);
                        instructionPointer--; // Counteract the increment that always happens at the end of the loop.
                    }
                    break;
                case Instruction::Toggle:
                    {
                        const int indexToToggle = instructionPointer + instruction.getArgValue(0, registerValue);
                        std::cout << "toggling instruction at index: " << indexToToggle << std::endl;
                        if (indexToToggle >= 0 && indexToToggle < instructions.size())
                        {
                            auto& instructionToToggle = instructions[indexToToggle];
                            switch(instructionToToggle.type)
                            {

                                case Instruction::Copy: // Two-arg.
                                    instructionToToggle.type = Instruction::Jump;
                                    break;
                                case Instruction::Increment: // One-arg.
                                    instructionToToggle.type = Instruction::Decrement;
                                    break;
                                case Instruction::Decrement: // One-arg.
                                    instructionToToggle.type = Instruction::Increment;
                                    break;
                                case Instruction::Jump: // Two-arg.
                                    instructionToToggle.type = Instruction::Copy;
                                    break;
                                case Instruction::Toggle: // One-arg.
                                    instructionToToggle.type = Instruction::Increment;
                                    break;
                                default:
                                    assert(false);
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
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