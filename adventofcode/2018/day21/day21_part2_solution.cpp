#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
#include <set>
#include <regex>

#include <cassert>

using namespace std;

string asBin(int64_t x)
{
    string ret;
    while (ret.size() < 32)
    {
        if (x & 1)
            ret.insert(ret.begin(), '1');
        else
            ret.insert(ret.begin(), '0');
        x >>= 1;
    }
    return ret;
}

int main()
{
    int instructionPointerRegister = -1;
    {
        string instrPointerRegisterLine;
        getline(cin, instrPointerRegisterLine);
        std::smatch instrPointerRegisterMatch;
        const bool matchSuccessful = std::regex_match(instrPointerRegisterLine, instrPointerRegisterMatch, std::regex(R"(^#ip\s*(\d+)$)"));
        assert(matchSuccessful);
        instructionPointerRegister = std::stoi(std::string(instrPointerRegisterMatch[1]));
    }
    std::cout << "instructionPointerRegister: " << instructionPointerRegister << std::endl;

    enum OpType
    {
        // Addition.
        addr,
        addi,
        // Multiplication.
        mulr,
        muli,
        // Bitwise "and".
        banr,
        bani,
        // Bitwise "or".
        borr,
        bori,
        // Assignment.
        setr,
        seti,
        // Greater-than testing.
        gtir,
        gtri,
        gtrr,
        // Equality testing.
        eqir,
        eqri,
        eqrr
    };
    map<string, OpType> opTypeForName;
    opTypeForName["addr"] = addr;
    opTypeForName["addi"] = addi;
    opTypeForName["mulr"] = mulr;
    opTypeForName["muli"] = muli;
    opTypeForName["banr"] = banr;
    opTypeForName["bani"] = bani;
    opTypeForName["borr"] = borr;
    opTypeForName["bori"] = bori;
    opTypeForName["setr"] = setr;
    opTypeForName["seti"] = seti;
    opTypeForName["gtir"] = gtir;
    opTypeForName["gtri"] = gtri;
    opTypeForName["gtrr"] = gtrr;
    opTypeForName["eqir"] = eqir;
    opTypeForName["eqri"] = eqri;
    opTypeForName["eqrr"] = eqrr;

    vector<vector<int>> instructions;
    string instructionLine;
    while (getline(cin, instructionLine))
    {
        std::smatch instructionMatch;
        const bool matchSuccessful = std::regex_match(instructionLine, instructionMatch, std::regex(R"(^(\w+)\s+(\d+)\s+(\d+)\s+(\d+)\s*$)"));
        assert(matchSuccessful);
        vector<int> instruction(4);
        const std::string instructionName = instructionMatch[1];
        assert(opTypeForName.contains(instructionName));
        const auto opType = opTypeForName[instructionName];
        instruction[0] = opType;
        instruction[1] = std::stoi(instructionMatch[2]);
        instruction[2] = std::stoi(instructionMatch[3]);
        instruction[3] = std::stoi(instructionMatch[4]);

        instructions.push_back(instruction);
    }

    auto applyInstruction = [&instructionPointerRegister]( const vector<int>& instruction, const vector<int64_t>& inputRegistersOrig, int& instructionPointer)
    {
        vector<int64_t> inputRegisters = inputRegistersOrig;
        inputRegisters[instructionPointerRegister] = instructionPointer;
        const OpType opType = static_cast<OpType>(instruction[0]);
        const int A = instruction[1];
        const int B = instruction[2];
        const int C = instruction[3];

        vector<int64_t> outputRegisters = inputRegisters;
        switch (opType)
        {
            case addr:
                outputRegisters[C] = inputRegisters[A] + inputRegisters[B];
                break;
            case addi:
                outputRegisters[C] = inputRegisters[A] + B;
                break;
            case mulr:
                outputRegisters[C] = inputRegisters[A] * inputRegisters[B];
                break;
            case muli:
                outputRegisters[C] = inputRegisters[A] * B;
                break;
            case banr:
                outputRegisters[C] = inputRegisters[A] & inputRegisters[B];
                break;
            case bani:
                outputRegisters[C] = inputRegisters[A] & B;
                break;
            case borr:
                outputRegisters[C] = inputRegisters[A] | inputRegisters[B];
                break;
            case bori:
                outputRegisters[C] = inputRegisters[A] | B;
                break;
            case setr:
                outputRegisters[C] = inputRegisters[A];
                break;
            case seti:
                outputRegisters[C] = A;
                break;
            case gtir:
                outputRegisters[C] = (1 ? A > inputRegisters[B] : 0);
                break;
            case gtri:
                outputRegisters[C] = (1 ? inputRegisters[A] > B : 0);
                break;
            case gtrr:
                outputRegisters[C] = (1 ? inputRegisters[A] > inputRegisters[B] : 0);
                break;
            case eqir:
                outputRegisters[C] = (1 ? A == inputRegisters[B] : 0);
                break;
            case eqri:
                outputRegisters[C] = (1 ? inputRegisters[A] == B : 0);
                break;
            case eqrr:
                outputRegisters[C] = (1 ? inputRegisters[A] == inputRegisters[B] : 0);
                break;
            default:
                assert(false);
        };
        instructionPointer = outputRegisters[instructionPointerRegister];
        instructionPointer++;
        return outputRegisters;
    };

    std::cout << asBin(5) << std::endl;
    std::cout << asBin(13) << std::endl;
    std::cout << asBin(32) << std::endl;
    std::cout << asBin(256) << std::endl;

    set<int64_t> seenComparisonValues;
    int instructionPointer = 0;
    vector<int64_t> registers(6);
    registers[0] = 12464363; // This makes the program terminate as late as possible.  Set to 0 instead if you want to see this value being derived.
    int numIterations = 0;
    int oldReg3 = -1;
    while (true)
    {
        if (instructionPointer < 0 || instructionPointer >= static_cast<int>(instructions.size()))
            break;
#if 0
        std::cout << "Running instruction#: " << instructionPointer << std::endl;
        std::cout << "Registers: " << std::endl;
        for (const auto& r : registers)
        {
            std::cout << asBin(r) << " ";
        }
        std::cout << std::endl;
#endif
        //const int oldInstructionPointer = instructionPointer;
        //if (instructionPointer == 28)
            //oldReg3 = registers[3];
        const auto& toExecute = instructions[instructionPointer];
        registers = applyInstruction(toExecute, registers, instructionPointer);
        if (instructionPointer == 28)
        {
            // This is the comparison of register 3 with register 0 that
            // terminate the program if they are equal.
            //
            // Look for the first repeated value of register 3 at this point
            // in the program; we want to set register 0 to the *previous*
            // value of register 3 at this point in the program.
            std::cout << "comparison: reg3: " << registers[3] << std::endl;
            if (seenComparisonValues.contains(registers[3]))
            {
                std::cout << "Repetition! You probably want to set register 0 to " << oldReg3 << std::endl;
                break;
            }
            seenComparisonValues.insert(registers[3]);
            oldReg3 = registers[3];
        }

#if 0
        if (instructionPointer == 26)
        {
            // Verify that the block of code from instruction #17-#27 (inc) just
            // left-shift register 1 by 8.
            assert(registers[4] == (registers[1] >> 8));
        }
#endif
        if (instructionPointer == 17)
        {
            // Shortcut.
            registers[1] >>= 8;
            instructionPointer = 8;
        }
        numIterations++;
    }

    // The program, crudely converted to pseudo-c++.
    /*
    do
    {
        (1) = (3) | 65536;
        (3) = 14906355;
        do
        {
            (4) = (1) & 255;
            (3) = ((3) + (4)) & (16777215);
            (3) *= 65899;
            (3) = (3) & (16777215);
            if ((1) >= 256)
            {
                // Lines 17-26, inclusive; equivalent to
                // (1) >>= 8.
                (4) = 0;
                do
                {
                    (2) = (4) + 1;
                    (2) << 8;
                    if (2) > (1){
                        (1) = (4);
                        break;
                    }
                    (4)++;
                } while (true)
            }
            else
            {
                if ((3) == (0))
                {
                    // Final comparison - we want to choose (0)
                    // so this triggers, but as late as possible.
                    exit;
                }
                else
                {
                    break;
                }
            }
        } while (true)
    } while (true)
    */

    /*
    
    do
    {
        (1) = (3) | 65536;
        (3) = 14906355;
        do
        {
            (4) = (1) & 255;
            (3) = ((3) + (4)) & (16777215);
            (3) *= 65899;
            (3) = (3) & (16777215);
            if ((1) >= 256)
            {
                (1) >> 8;
            }
            else
            {
                if ((3) == (0))
                {
                    exit;
                }
                else
                {
                    break;
                }
            }
        } while (true)
    } while (true)
    */

    return 0;
}
