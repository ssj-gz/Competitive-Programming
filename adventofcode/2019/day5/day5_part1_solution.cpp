#include <iostream>
#include <vector>
#include <deque>
#include <cassert>

using namespace std;

vector<int> evaluateProgram(const vector<int>& originalProgram)
{
    auto program = originalProgram;
    int position = 0;
    bool terminateProgram = false;

    deque<int> inputs;
    inputs.push_back(1);

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

    const auto outputs = evaluateProgram(program);
    cout << "Output: " << endl;
    for (const auto x : outputs)
        cout << x << " ";
    cout << endl;
}
