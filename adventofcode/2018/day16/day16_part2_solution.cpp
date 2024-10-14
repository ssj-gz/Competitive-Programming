#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
#include <set>

#include <cassert>

using namespace std;

int main()
{
    string line;
    auto extractFourNumbers = [](const string& line)
    {
        string strippedLine = line;
        strippedLine.erase(std::remove_if(strippedLine.begin(), strippedLine.end(), [](const auto character)
                    {
                        return character != ' ' && !(character >= '0' && character <= '9');
                    }), strippedLine.end());
        std::cout << "line: " << line << std::endl;
        std::cout << "strippedLine: " << strippedLine << std::endl;
        istringstream numbersIn(strippedLine);
        vector<int> fourNumbers(4);
        for (int i = 0; i < 4; i++)
        {
            numbersIn >> fourNumbers[i];
        }
        assert(numbersIn);
        return fourNumbers;

    };
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

    auto applyInstruction = []( const vector<int>& instruction, const vector<int>& inputRegisters)
    {
        const OpType opType = static_cast<OpType>(instruction[0]);
        const int A = instruction[1];
        const int B = instruction[2];
        const int C = instruction[3];

        vector<int> outputRegisters = inputRegisters;
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
        return outputRegisters;
    };

    const OpType allops[] =
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
    struct InstructionSample
    {
        vector<int> instruction;
        vector<int> inputRegisters;
        vector<int> expectedRegistersAfterInstruction;
    };
    vector<InstructionSample> instructionSamples;
    while (getline(cin, line))
    {
        if(!line.starts_with("Before:"))
        {
            std::cout << "Don't recognise line: " << line << "; breaking" << std::endl;
            break;
        }
        InstructionSample instructionSample;
        instructionSample.inputRegisters = extractFourNumbers(line);

        getline(cin, line);
        instructionSample.instruction = extractFourNumbers(line);
        getline(cin, line);
        instructionSample.expectedRegistersAfterInstruction = extractFourNumbers(line);
        instructionSamples.push_back(instructionSample);

        getline(cin, line);
        assert(line.empty());
    }
    map<int, set<OpType>> possibilitiesForInstruction;
    for (const auto opCode : allops)
    {
        possibilitiesForInstruction[static_cast<int>(opCode)] = set<OpType>(std::begin(allops), std::end(allops));
    }
    // Eliminate possibilities of opCode that would give incorrect results.
    for (const auto& instructionSample : instructionSamples)
    {
        const int instructionOpCode = instructionSample.instruction[0];

        for (const auto tryOpType : allops)
        {
            auto tryInstruction = instructionSample.instruction;
            tryInstruction[0] = static_cast<int>(tryOpType);
            if (instructionSample.expectedRegistersAfterInstruction != applyInstruction(tryInstruction, instructionSample.inputRegisters))
            {
                possibilitiesForInstruction[instructionOpCode].erase(tryOpType);
            }
        }
    }
    bool deducedAll = false;
    while (!deducedAll)
    {
        std::cout << "Deduction round" << std::endl;
        for (const auto instructionOpCode : allops)
        {
            bool deducedOpTypeForCode = (possibilitiesForInstruction[instructionOpCode].size() == 1);
            if (deducedOpTypeForCode)
            {
                const auto opType = *possibilitiesForInstruction[instructionOpCode].begin();
                std::cout << " deduced opCode: " << instructionOpCode << " is opType: " << opType << std::endl;
                for (auto& [otherInstructionOpCode, possibilities] : possibilitiesForInstruction)
                {
                    if (otherInstructionOpCode == instructionOpCode)
                        continue;
                    std::cout << " removing " << opType << " as a possibilty for otherInstructionOpCode: " << otherInstructionOpCode << " #possibilities before:" << possibilities.size() << std::endl;
                    possibilities.erase(opType);
                    std::cout << " #possibilities after:" << possibilities.size() << std::endl;
                }
            }
            for (const auto& [instructionOpCode, possibleOpTypes] : possibilitiesForInstruction)
            {
                std::cout << "instructionOpCode: " << instructionOpCode << " possible optypes: (" << possibleOpTypes.size() << "):";
                for (const auto opType : possibleOpTypes)
                {
                    std::cout << opType << " ";
                }
                std::cout << std::endl;
            }
            deducedAll = true;
            for (auto& [instructionOpCode_unused, possibilities] : possibilitiesForInstruction)
            {
                if (possibilities.size() != 1)
                {
                    std::cout << "Still not deduced: " << instructionOpCode_unused << std::endl;
                    deducedAll = false;
                }
            }

            if (deducedAll)
                break;

        }
    }

    string programLine;
    vector<int> registers(4, 0);
    while (getline(cin, programLine))
    {
        if (programLine.empty())
            continue;
        auto instruction = extractFourNumbers(programLine);
        // Translate opCode.
        instruction[0] = *possibilitiesForInstruction[instruction[0]].begin();
        registers = applyInstruction(instruction, registers);
        std::cout << "Applied instruction:" << instruction[0] << " " << instruction[1] << " " << instruction[2] << " " << instruction[3] <<  std::endl;
        std::cout << "New registers      :" << registers[0] << " " << registers[1] << " " << registers[2] << " " << registers[3]  << std::endl;
    }
    std::cout << "register 0: " << registers[0] << std::endl;
    return 0;
}
