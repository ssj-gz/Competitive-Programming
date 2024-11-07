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
    enum Type { Copy, Increment, Decrement, Jump, Out, Unknown };
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
        else if (instructionType == "out")
        {
            instruction.type = Instruction::Out;
            readRegOrValArg(0);
        }
        else
        {
            assert(false);
        }
        assert(instructionStream);
        instructions.push_back(instruction);
    }

    // Here is the reverse-engineered program, with comments.
    // TL;DR - given the original input a, set d = a + 362*7,
    // then continuously printout the binary representation
    // of d (LSB first).
    //
    //  d = a
    //  c = 7
    //  do 
    //  {  # This loop sets d to a + 362 * 7 (== a + 2534).
    //      b = 362
    //      do
    //      {
    //          d++
    //          b--
    //      }
    //      while (b != 0)
    //      c--
    //  }
    //  while(c != 0)
    //  
    //  do
    //  {
    //     do
    //     {
    //         # Main loop - continuously prints out the binary representation of d, in LSB order i.e. 
    //         # d = 13 would print out 1011 1011 1011 (without spaces) over and over again.
    //         a = d 
    //         do
    //         {
    //             # This loop sets a to a/2 (integer arithmetic) and stores "2 minus remainder" in c.
    //             b = a
    //             a = 0
    //             outerloop:
    //             do
    //             {
    //                 # "Divide" by by 2 and store remainder.
    //                 c = 2
    //                 do
    //                 {
    //                     if b == 0: 
    //                         break outerloop
    //                     b--
    //                     c--
    //                 } while(c != 0)
    //                 a++
    //             } while (true);
    //             b = 2
    //             do
    //             {
    //                 # This loop sets b = 2 - c i.e. if a (at the top of the "This loop sets a to a/2 ..." loop)
    //                 # divided by 2 has remainder 0 (so c == 2), b has value 0; else (i.e. c == 1) has value 1
    //                 # i.e. sets b to the aforementioned remainder.
    //                 if c == 0: 
    //                     break
    //                 b--
    //                 c--
    //             } while (true)
    //             # Output b i.e. the remainder of the value of a (at the top of the "This loop sets a to a/2 ..." loop) 
    //             # after dividing by 2.
    //             out b 
    //         } while (a != 0)
    //     } while (true)
    // } while (true)
    // 

    // Find first a such that d = a + 362 * 7, when expressed as binary (LSB first)
    // is of the form "01 repeated".
    int a = 0;
    while (true)
    {
        string asBinaryLSB;
        int d = a + 2534;
        const int orig_d = d;
        while (d > 0)
        {
            if ((d & 1) == 0)
                asBinaryLSB += '0';
            else
                asBinaryLSB += '1';
            d>>=1;
        }
        std::cout << "a: " << a << " d: " << orig_d << " asBinaryLSB: " << asBinaryLSB << std::endl;
        if (asBinaryLSB.size() % 2 == 0)
        {
            bool is01Repeating = true;
            for (string::size_type pos = 0; pos < asBinaryLSB.size(); pos += 2)
            {
                if (asBinaryLSB[pos] != '0' || asBinaryLSB[pos + 1] != '1')
                {
                    is01Repeating = false;
                    break;
                }

            }
            if (is01Repeating)
                break;
        }
        a++;
    }
    std::cout << "result: " << a << std::endl;

    map<char, int64_t> registerValue;
    registerValue['a'] = a;
    int64_t instructionPointer = 0;
    while(true)
    {
        if (instructionPointer < 0 || instructionPointer >= static_cast<int64_t>(instructions.size()))
            break;
        const auto& instruction = instructions[instructionPointer];
#if 1
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
            case Instruction::Out:
                std::cout << "OUT:" << instruction.getArgValue(0, registerValue) << std::endl;
                break;
            default:
                break;
        }
#if 1
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
