#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.
#include <iostream>
#include <vector>
#include <map>
#include <sstream>

#include <cassert>

using namespace std;

int main()
{
    map<char, int64_t> registerValue;

    struct Instruction
    {
        enum Type { Sound, Set, Add, Mul, Mod, Recover, Jump };
        Type type;
        char argReg[2] = {};
        int64_t argVal[2] = {-1, -1};
        int64_t getArgValue(int argNum, const map<char, int64_t>& registerValue) const
        {
            if (argReg[argNum] != '\0')
            {
                const auto regAndValIter = registerValue.find(argReg[argNum]);
                if (regAndValIter == registerValue.end())
                    return 0;
                else
                    return regAndValIter->second;
            }
            else
                return argVal[argNum];
        };
    };
    vector<Instruction> instructions;
    string instructionLine;
    while (getline(cin, instructionLine))
    {
        std::cout << "instructionLine: " << instructionLine << std::endl;
        Instruction instruction;
        istringstream instructionStream(instructionLine);
        string instructionType;
        instructionStream >> instructionType;
        auto readRegOrValArg = [&instructionStream, &instruction](int argNum)
        {
            char peekChar = '\0';
            instructionStream >> peekChar;
            if (peekChar >= 'a' && peekChar <= 'z')
                instruction.argReg[argNum] = peekChar;
            else
            {
                instructionStream.putback(peekChar);
                instructionStream >> instruction.argVal[argNum];
            }
        };
        if (instructionType == "snd")
        {
            instruction.type = Instruction::Sound;
            readRegOrValArg(0);
        }
        else if (instructionType == "set")
        {
            instruction.type = Instruction::Set;
            instructionStream >> instruction.argReg[0];
            readRegOrValArg(1);
        }
        else if (instructionType == "add")
        {
            instruction.type = Instruction::Add;
            instructionStream >> instruction.argReg[0];
            readRegOrValArg(1);
        }
        else if (instructionType == "mul")
        {
            instruction.type = Instruction::Mul;
            instructionStream >> instruction.argReg[0];
            readRegOrValArg(1);
        }
        else if (instructionType == "mod")
        {
            instruction.type = Instruction::Mod;
            instructionStream >> instruction.argReg[0];
            readRegOrValArg(1);
        }
        else if (instructionType == "rcv")
        {
            instruction.type = Instruction::Recover;
            instructionStream >> instruction.argReg[0];
        }
        else if (instructionType == "jgz")
        {
            instruction.type = Instruction::Jump;
            instructionStream >> instruction.argReg[0];
            readRegOrValArg(1);
        }
        else
        {
            assert(false);
        }
        assert(instructionStream);
        instructions.push_back(instruction);
    }

    int instructionPointer = 0;
    int64_t lastPlayedFrequency = -1;
    bool halt = false;
    while (!halt)
    {
        const auto& instruction = instructions[instructionPointer];
        std::cout << "executing instruction at " << instructionPointer << std::endl;
        switch (instruction.type)
        {
            case Instruction::Sound:
                lastPlayedFrequency = instruction.getArgValue(0, registerValue);
                std::cout << "Playing sound: " << lastPlayedFrequency << std::endl;
                break;
            case Instruction::Set:
                registerValue[instruction.argReg[0]] = instruction.getArgValue(1, registerValue);
                break;
            case Instruction::Add:
                registerValue[instruction.argReg[0]] += instruction.getArgValue(1, registerValue);
                break;
            case Instruction::Mul:
                registerValue[instruction.argReg[0]] *= instruction.getArgValue(1, registerValue);
                break;
            case Instruction::Mod:
                registerValue[instruction.argReg[0]] %= instruction.getArgValue(1, registerValue);
                break;
            case Instruction::Recover:
                if (instruction.getArgValue(0, registerValue) != 0)
                {
                    std::cout << "Recover: " << lastPlayedFrequency << std::endl;
                    halt = true;
                }
                break;
            case Instruction::Jump:
                if (instruction.getArgValue(0, registerValue) != 0)
                {
                    instructionPointer += instruction.getArgValue(1, registerValue);
                    instructionPointer--; // Counteract the increment that always happens at the end of the loop.
                }
                break;
        }
        instructionPointer++;
        std::cout << " new values of registers: " << std::endl;
        for (const auto& [regName, value] : registerValue)
        {
            std::cout << "  " << regName << ": " << value << std::endl; 
        }
    }
    std::cout << "result: " << lastPlayedFrequency << std::endl;

    return 0;
}
