#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

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
    int numWithThreeOrMorePossibleOpTypes = 0;
    while (getline(cin, line))
    {
        if(!line.starts_with("Before:"))
        {
            std::cout << "Don't recognise line: " << line << "; breaking" << std::endl;
            break;
        }
        const auto inputRegisters = extractFourNumbers(line);

        getline(cin, line);
        auto instruction = extractFourNumbers(line);
        getline(cin, line);
        const auto registersAfterInstruction = extractFourNumbers(line);

        int numPossibleOpTypes = 0;
        for (const auto opType : allops)
        {
            instruction[0] = static_cast<int>(opType);
            if (registersAfterInstruction == applyInstruction(instruction, inputRegisters))
            {
                std::cout << "  Could be opType: " << static_cast<int>(opType) << std::endl;
                numPossibleOpTypes++;
            }
        }
        std::cout << " numPossibleOpTypes: " << numPossibleOpTypes << std::endl;
        if (numPossibleOpTypes >= 3)
            numWithThreeOrMorePossibleOpTypes++;
        getline(cin, line);
        assert(line.empty());

    }
    std::cout << "numWithThreeOrMorePossibleOpTypes: " << numWithThreeOrMorePossibleOpTypes << std::endl;
    return 0;
}
