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
    registerValue['a'] = 1;
    int64_t instructionPointer = 0;
    int64_t numMults = 0;
    while(true)
    {
        if (instructionPointer < 0 || instructionPointer >= static_cast<int64_t>(instructions.size()))
            break;
        const auto& instruction = instructions[instructionPointer];
        std::cout << "executing instruction at " << instructionPointer << std::endl;
        std::cout << "  current register values: " << std::endl;
        for (const auto& [regName, value] : registerValue)
        {
            std::cout << "  " << regName << ": " << value;
        }
        std::cout << std::endl;
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
        std::cout << "  register values after instruction: " << std::endl;
        for (const auto& [regName, value] : registerValue)
        {
            std::cout << "  " << regName << ": " << value;
        }
        std::cout << std::endl;
#if 1
        // Shortcut: count the non-primes between lowerBound ('b') and upperBound ('c'), step 17,
        // manually.
        if (instructionPointer == 8)
        {
            const auto lowerBound = registerValue['b'];
            const auto upperBound = registerValue['c'];
            std::cout << "lowerBound: " << lowerBound << " upperBoundL " << upperBound << std::endl;
            const int step = 17;
            assert((upperBound - lowerBound) % step == 0);
            int numComposites = 0;
            for (int x = lowerBound; x <= upperBound; x += step)
            {
                bool isPrime = true;
                int factor = 2;
                while (factor * factor <= x)
                {
                    if ((x % factor) == 0)
                    {
                        isPrime = false;
                        std::cout << "x: " << x << " is divisible by: " << factor << std::endl;
                        break;
                    }
                    factor++;
                }
                std::cout << "x: " << x << " prime? " << isPrime << std::endl;
                if (!isPrime)
                    numComposites++;
            }
            std::cout << "numComposites: " << numComposites << std::endl;
            return 0;
        }
#endif
        instructionPointer++;
    }
    std::cout << "numMults: " << numMults << std::endl;
    std::cout << "reg h: " << registerValue['h'] << std::endl;

    // Reverse-engineering notes:
    // # Basic overview: for all numbers x (step 17) between b (lowerBound) and c (upperBound) inclusive,
    // # increment h if x is non-prime.
    // # The code executed is the same whether a is 0 or not, but in the case where it is not, the
    // # lowerBound and upperBound are both larger and have many more numbers in between them.
    // b = 93
    // c = b
    // if a != 0:
    //   b = b * 100 + 100000
    //   c = b + 17000
    // # This point corresponds to instruction #8; when we're here, b (lowerBound) and c (upperBound) have been set,
    // # and the main loop (that counts non-primes) begins.
    // do
    //     f = 1
    //     d = 2
    //     do
    //         e = 2
    //         do
    //             if d * e == b
    //                 f = 0 # b is composite.
    //             e++
    //         while (e != b)
    //         d++
    //     while (d != b)
    //     if f == 0:
    //         h++
    //     if b != c:
    //         b = b + 17
    //     else:
    //         halt
    // while (true)
    return 0;
}
