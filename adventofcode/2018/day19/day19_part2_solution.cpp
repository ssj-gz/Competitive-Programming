#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
#include <regex>

#include <cassert>

using namespace std;

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

    // From reverse-engineering the source input, the program creates a
    // number at register 5 (that, once fully constructed, never changes again), then makes 
    // registers 1 & 3 iterate over all values 1...reg[5] independently,
    // adding the value of register 3 to register 0 whenever register[1] * register[3] == register[5] i.e.
    // whenever register[1] and register[3] are both factors of register[5].
    //
    // Thus, register 0 contains the sum of the factors of register[5].
    int64_t numberToFactorise = -1;

    // Run the code just far enough to get the value of register[5] (numberToFactorise).
    int instructionPointer = 0;
    vector<int64_t> registers(6);
    registers[0] = 1;
    int numIterations = 0;
    while (true)
    {
        if (instructionPointer < 0 || instructionPointer >= instructions.size())
            break;
        std::cout << "Running instruction#: " << instructionPointer << std::endl;
        std::cout << "Registers: " << std::endl;
        for (const auto& r : registers)
        {
            std::cout << r << " ";
        }
        std::cout << std::endl;
        const int oldInstructionPointer = instructionPointer;
        const auto& toExecute = instructions[instructionPointer];
        if (oldInstructionPointer == 1)
        {
            // If we're here, then the number to factorise is stored in register 5.
            numberToFactorise = registers[5];
            std::cout << "numberToFactorise: " << numberToFactorise << std::endl;
            break;
        }
        registers = applyInstruction(toExecute, registers, instructionPointer);
        numIterations++;
    }
    assert(numberToFactorise != -1);
    int64_t result = 0;
    for (int factor = 1; factor * factor < numberToFactorise; factor ++)
    {
        if (numberToFactorise % factor == 0)
        {
            std::cout << "Adding factor: " << factor << std::endl;
            result += factor;
            std::cout << "Adding factor: " << numberToFactorise / factor << std::endl;
            result += (numberToFactorise / factor);
        }
    }
    // numberToFactorise itself is a factor that we won't have found in the loop above; 
    // add it now.
    std::cout << "result: " << result << std::endl;

    return 0;
}
