#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <limits>
#include <cassert>

using namespace std;

vector<int> evaluateProgram(const vector<int>& originalProgram, const vector<int>& originalInputs)
{
    auto program = originalProgram;
    int position = 0;
    bool terminateProgram = false;

    deque<int> inputs(originalInputs.begin(), originalInputs.end());

    vector<int> outputs;

    while (!terminateProgram)
    {
        const int instruction = program[position++];
        const int opCode = instruction % 100;
        const int parameterModes = instruction / 100;

        auto nextParamValue = [&program, &position, firstParamPosition = position, parameterModes]()
        {
            cout << "nextParamValue! position: " << position << endl;
            const int paramIndex = position - firstParamPosition;
            cout << "paramIndex: " << paramIndex << endl;
            int parameterMode = parameterModes;
            for (int i = 0; i < paramIndex; i++)
                parameterMode /= 10;
            parameterMode %= 10;

            assert(parameterMode == 0 || parameterMode == 1);
            cout << "parameterMode: " << parameterMode << endl;

            if (parameterMode == 0)
            {
                const auto returnVal = program[program[position++]];
                cout << "Returning " << returnVal << endl;
                return returnVal;
            }
            else
            {
                const auto returnVal = program[position++];
                cout << "Returning " << returnVal << endl;
                return returnVal;
            }

        };

        switch(opCode)
        {
            case 1:
                {
                    cout << "Sum!" << endl;
                    const int sumand1 = nextParamValue();
                    const int sumand2 = nextParamValue();
                    const int destPos = program[position++];
                    cout << " sumand1: " << sumand1 << " sumand2: " << sumand2 << " destPos: " << destPos << endl;
                    program[destPos] = sumand1 + sumand2;
                    break;
                }
            case 2:
                {
                    cout << "Multiplication!" << endl;
                    const int multiplicand1 = nextParamValue();
                    const int multiplicand2 = nextParamValue();
                    const int destPos = program[position++];
                    cout << " multiplicand1: " << multiplicand1 << " multiplicand2: " << multiplicand2 << " destPos: " << destPos << endl;
                    program[destPos] = multiplicand1 * multiplicand2;
                    break;
                }
            case 3:
                {
                    cout << "Input!" << endl;
                    assert(!inputs.empty());
                    const int input = inputs.front();
                    inputs.pop_front();
                    const int destPos = program[position++];
                    cout << " input: " << input << " destPos: " << destPos << endl;
                    program[destPos] = input;
                    break;
                }
            case 4:
                {
                    cout << "Output!" << endl;
                    const int output = nextParamValue();
                    cout << " outputting: " << output << endl;
                    outputs.push_back(output);
                    break;
                }
            case 5:
                {
                    cout << "Jump-if-true!" << endl;
                    const int toCheck = nextParamValue();
                    const int positionIfNonZero = nextParamValue();
                    cout << " toCheck: " << toCheck << " positionIfNonZero: " << positionIfNonZero << endl;
                    if (toCheck != 0)
                        position = positionIfNonZero;
                    break;
                }
            case 6:
                {
                    cout << "Jump-if-false!" << endl;
                    const int toCheck = nextParamValue();
                    const int positionIfZero = nextParamValue();
                    cout << " toCheck: " << toCheck << " positionIfZero: " << positionIfZero << endl;
                    if (toCheck == 0)
                        position = positionIfZero;
                    break;
                }
            case 7:
                {
                    cout << "less-than!" << endl;
                    const int param1 = nextParamValue();
                    const int param2 = nextParamValue();
                    const int destPos = program[position++];
                    cout << " param1: " << param1 << " param2: " << param2 << " destPos: " << destPos << endl;
                    if (param1  < param2)
                        program[destPos] = 1;
                    else
                        program[destPos] = 0;
                    break;
                }
            case 8:
                {
                    cout << "equals!" << endl;
                    const int param1 = nextParamValue();
                    const int param2 = nextParamValue();
                    const int destPos = program[position++];
                    cout << " param1: " << param1 << " param2: " << param2 << " destPos: " << destPos << endl;
                    if (param1 == param2)
                        program[destPos] = 1;
                    else
                        program[destPos] = 0;
                    break;
                }
            case 99:
                cout << "Terminate!" << endl;
                terminateProgram = true;
                break;
            default:
                cout << "Unknown opCode: " << opCode << endl;
                assert(false);
        }
    }
    return outputs;
}

int main()
{
    vector<int> program;
    int input;
    while (true)
    {
        cin >> input;
        program.push_back(input);
        assert(cin);
        char comma;
        cin >> comma;
        if (!cin)
            break;
        assert(comma == ',');
    }

    vector<int> phaseSettingPermutation = {0,1,2,3,4};
    const auto originalPhaseSettingPermutation = phaseSettingPermutation;

    int largestThrust = std::numeric_limits<int>::min();
    do
    {
        int ampInput = 0;
        for (int phase : phaseSettingPermutation)
        {
            vector<int> input;
            input.push_back(phase);
            input.push_back(ampInput);
            const auto output = evaluateProgram(program, input);
            assert(output.size() == 1);
            ampInput = output.front();
        }
        const int thrust = ampInput;
        if (thrust > largestThrust)
        {
            largestThrust = max(largestThrust, thrust);
            cout << "New largest thrust: " << largestThrust << endl;
            cout << "From phaseSettingPermutation: " << endl;
            for (const auto x : phaseSettingPermutation)
                cout << x << " ";
            cout << endl;
        }

        std::next_permutation(phaseSettingPermutation.begin(), phaseSettingPermutation.end());
    } while (phaseSettingPermutation != originalPhaseSettingPermutation);

    cout << "largestThrust: " << largestThrust << endl;
}
