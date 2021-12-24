#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <algorithm>
#include <cassert>

using namespace std;

int main()
{
    struct Arg
    {
        bool isLiteral = false;
        int varIndex = -1; // 0, 1, 2, 3.
        int64_t literalValue;
    };
    struct Instruction
    {
        enum Type { inp, add, mul, div, mod, eql } type;
        Arg arg1;
        Arg arg2;
    };

    string token;
    vector<Instruction> instructions;
    while (cin >> token)
    {
        auto readArg = []()
        {
            static string varNames = "wxyz";
            string argToken;
            cin >> argToken;
            assert(cin);
            if (varNames.find(argToken) != string::npos)
            {
                assert(argToken.size() == 1);
                return Arg{false, argToken[0] - 'w', -1};
            }
            else
            {
                return Arg{true, -1, stoll(argToken)};
            }
        };
        auto createInstruction = [&readArg](Instruction::Type instructionType, int numArgs)
        {
            assert(numArgs >= 1 && numArgs <= 2);
            Instruction instruction;
            instruction.type = instructionType;
            instruction.arg1 = readArg();
            if (numArgs == 2)
                instruction.arg2 = readArg();
            return instruction;
        };;
        if (token == "inp")
        {
            instructions.push_back(createInstruction(Instruction::Type::inp, 1));
        }
        else if (token == "add")
        {
            instructions.push_back(createInstruction(Instruction::Type::add, 2));
        }
        else if (token == "mul")
        {
            instructions.push_back(createInstruction(Instruction::Type::mul, 2));
        }
        else if (token == "div")
        {
            instructions.push_back(createInstruction(Instruction::Type::div, 2));
        }
        else if (token == "mod")
        {
            instructions.push_back(createInstruction(Instruction::Type::mod, 2));
        }
        else if (token == "eql")
        {
            instructions.push_back(createInstruction(Instruction::Type::eql, 2));
        }
    }


    auto evaluateInstructions = [](const vector<Instruction>& instructions, const string& input, std::array<int64_t, 4> varValues = {}) -> int64_t
    {
        int inputIndex = 0;
        auto evaluateArg = [&varValues](const Arg& arg)
        {
            if (arg.isLiteral)
                return arg.literalValue;
            else
                return varValues[arg.varIndex];
        };
        for (const auto & instruction : instructions)
        {
            switch(instruction.type)
            {
                case Instruction::Type::inp:
                    {
                        //cout << "Reading input; current var values: " << varValues[0] << "," << varValues[1] << "," << varValues[2] << "," << varValues[3] << endl;
                        assert(inputIndex < static_cast<int>(instructions.size()));
                        varValues[instruction.arg1.varIndex] = input[inputIndex] - '0';
                        inputIndex++;
                        break;
                    }
                case Instruction::Type::add:
                    {
                        const int64_t result = evaluateArg(instruction.arg1) + evaluateArg(instruction.arg2);
                        varValues[instruction.arg1.varIndex] = result;
                        break;
                    }
                case Instruction::Type::mul:
                    {
                        const int64_t result = evaluateArg(instruction.arg1) * evaluateArg(instruction.arg2);
                        varValues[instruction.arg1.varIndex] = result;
                        break;
                    }
                case Instruction::Type::div:
                    {
                        if (evaluateArg(instruction.arg2) == 0)
                        {
                            cout << "Division by zero" << endl;
                            assert(false);
                            return -1;
                        }
                        const int64_t result = evaluateArg(instruction.arg1) / evaluateArg(instruction.arg2);
                        assert(evaluateArg(instruction.arg1) >= 0 && evaluateArg(instruction.arg2) > 0);
                        varValues[instruction.arg1.varIndex] = result;
                        break;
                    }
                case Instruction::Type::mod:
                    {
                        if (evaluateArg(instruction.arg2) == 0)
                        {
                            cout << "Modulo zero" << endl;
                            assert(false);
                            return -1;
                        }
                        const int64_t result = evaluateArg(instruction.arg1) % evaluateArg(instruction.arg2);
                        assert(evaluateArg(instruction.arg1) >= 0 && evaluateArg(instruction.arg2) > 0);
                        varValues[instruction.arg1.varIndex] = result;
                        break;
                    }
                case Instruction::Type::eql:
                    {
                        const int64_t result = (evaluateArg(instruction.arg1) == evaluateArg(instruction.arg2) ? 1 : 0);
                        varValues[instruction.arg1.varIndex] = result;
                        break;
                    }
            }
            //cout << "processed instruction; vars now: w: " << varValues[0] << " x: " << varValues[1] << " y: " << varValues[2] << " z: " << varValues[3] << endl;
        }
        assert(inputIndex == static_cast<int>(input.size()));
        const auto z = varValues[3];
        //cout << "final z: " << z << endl;
        return z;
    };

    vector<vector<Instruction>> inputBlocks;
    for (const auto& instruction : instructions)
    {
        if (instruction.type == Instruction::Type::inp)
        {
            inputBlocks.push_back({});
        }
        inputBlocks.back().push_back(instruction);
    }

#if 0
    const auto& lastBlock = inputBlocks.back();

    for (int64_t z = 0; z < 10'000; z++)
    {
        for (int input = 1; input <= 9; input++)
        {
            cout << "z: " << z << " input: " << input << endl;
            const bool isValid = evaluateInstructions(lastBlock, to_string(input), {0,0,0,z});
            cout << "isValid?" << isValid << endl;
        }
    }
#endif

    const int numFinalInputBlocks = 5;
    vector<Instruction> finalInputBlocks;
    const int numInputBlocks = inputBlocks.size();
    for (int i = numInputBlocks - numFinalInputBlocks; i < numInputBlocks; i++)
    {
        finalInputBlocks.insert(finalInputBlocks.end(), inputBlocks[i].begin(), inputBlocks[i].end());
    }
    assert(count_if(finalInputBlocks.begin(), finalInputBlocks.end(), [](const auto instruction) { return instruction.type == Instruction::Type::inp; }) == numFinalInputBlocks);
    int64_t cumulativeDivisionDenom = 1;
    for (const auto& instruction : finalInputBlocks)
    {
        if (instruction.type == Instruction::Type::div)
        {
            assert(instruction.arg2.isLiteral);
            cumulativeDivisionDenom *= instruction.arg2.literalValue;
        }
    }
    cout << "cumulativeDivisionDenom: "<< cumulativeDivisionDenom << endl;



    map<int64_t, string> highestValidDigitsForZ;


#if 1
    for (int64_t z = 0; z <= cumulativeDivisionDenom; z++)
    {
        cout << "z: " << z << "/" << cumulativeDivisionDenom << endl;
        string testInput = string(numFinalInputBlocks, '1');

        while (true)
        {
            int digitIndex = 0;
            while (digitIndex >= 0 && testInput[digitIndex] == '9')
            {
                testInput[digitIndex] = '1';
                digitIndex++;
            }
            if (digitIndex == static_cast<int>(testInput.size()))
                break;
            testInput[digitIndex]++;
            const int64_t finalZ = evaluateInstructions(finalInputBlocks, testInput, {0,0,0,z});
            //cout << "z: " << z << " testInput: " << testInput << " isValid?" << isValid << endl;
#if 1
            if (finalZ == 0)
            {
                cout << "woo: z: " << z << " final digits: " << testInput << endl;
                if (!highestValidDigitsForZ.contains(z) || highestValidDigitsForZ[z] < testInput)
                {
                    highestValidDigitsForZ[z] = testInput;
                }
            }
#endif
        }
    }
#endif

    const int numFrontInputBlocks = numInputBlocks - numFinalInputBlocks;
    //const int numFrontInputBlocks = 4;
    vector<Instruction> frontInputBlocks;
    for (int i = numInputBlocks - numFinalInputBlocks - numFrontInputBlocks; i < numInputBlocks - numFinalInputBlocks; i++)
    {
        frontInputBlocks.insert(frontInputBlocks.end(), inputBlocks[i].begin(), inputBlocks[i].end());
    }
    assert(count_if(frontInputBlocks.begin(), frontInputBlocks.end(), [](const auto instruction) { return instruction.type == Instruction::Type::inp; }) == numFrontInputBlocks);

    bool found = false;
    int64_t blee = 0;
    vector<string> optimised;
    {
        string testInput = string(numFrontInputBlocks, '9');
        while (true)
        {
            int digitIndex = static_cast<int>(testInput.size()) - 1;
            while (digitIndex >= 0 && testInput[digitIndex] == '1')
            {
                testInput[digitIndex] = '9';
                digitIndex--;
            }
            if (digitIndex < 0)
                break;
            testInput[digitIndex]--;
            //cout << "testInput: " << testInput << endl;
            const int64_t finalZ = evaluateInstructions(frontInputBlocks, testInput);
            //cout << "testInput: " << testInput << " finalZ: " << finalZ << endl;
            if (highestValidDigitsForZ.contains(finalZ))
            {
                const string full = testInput + highestValidDigitsForZ[finalZ];
                cout << "opt:" << full << endl;
                found = true;
                optimised.push_back(full);
                break;
            }
            blee++;
            if (blee % 1000 == 0)
                cout << "blee: " << blee << " testInput: " <<testInput << endl;
        }
    }
    assert(found);

#if 0
    // Brute-force.
    vector<Instruction> bruteInstructions;
    for (int i = numInputBlocks - numFinalInputBlocks - numFrontInputBlocks; i < numInputBlocks; i++)
    {
        bruteInstructions.insert(bruteInstructions.end(), inputBlocks[i].begin(), inputBlocks[i].end());
    }
    assert(count_if(bruteInstructions.begin(), bruteInstructions.end(), [](const auto instruction) { return instruction.type == Instruction::Type::inp; }) == numFrontInputBlocks + numFinalInputBlocks);
    vector<string> bruteforceFound;
    {
        string testInput = string(numFrontInputBlocks + numFinalInputBlocks, '9');
        while (true)
        {
            int digitIndex = 0;
            while (digitIndex >= 0 && testInput[digitIndex] == '1')
            {
                testInput[digitIndex] = '9';
                digitIndex++;
            }
            if (digitIndex == static_cast<int>(testInput.size()))
                break;
            testInput[digitIndex]--;
            //cout << "testInput: " << testInput << endl;
            const int64_t finalZ = evaluateInstructions(bruteInstructions, testInput);
            //cout << "testInput: " << testInput << " finalZ: " << finalZ << endl;
            if (finalZ == 0)
            {
                cout << "brute:" << testInput << endl;
                found = true;
                bruteforceFound.push_back(testInput);
            }
        }
    }

    sort(optimised.begin(), optimised.end());
    sort(bruteforceFound.begin(), bruteforceFound.end());
    assert(optimised == bruteforceFound);
#endif

}
