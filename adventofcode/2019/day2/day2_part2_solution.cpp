#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int evaluateProgram(const vector<int>& originalProgram)
{
    auto program = originalProgram;
    int position = 0;
    bool terminateProgram = false;
    while (!terminateProgram)
    {
        const int opCode = program[position++];
        switch(opCode)
        {
            case 1:
                {
                    const int sumand1 = program[program[position++]];
                    const int sumand2 = program[program[position++]];
                    const int destPos = program[position++];
                    cout << "sumand1: " << sumand1 << " sumand2: " << sumand2 << " destPos: " << destPos << endl;
                    program[destPos] = sumand1 + sumand2;
                    break;
                }
            case 2:
                {
                    const int multiplicand1 = program[program[position++]];
                    const int multiplicand2 = program[program[position++]];
                    const int destPos = program[position++];
                    cout << "multiplicand1: " << multiplicand1 << " multiplicand2: " << multiplicand2 << " destPos: " << destPos << endl;
                    program[destPos] = multiplicand1 * multiplicand2;
                    break;
                }
            case 99:
                terminateProgram = true;
                break;
            default:
                assert(false);
        }
    }
    return program[0];
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

    int requiredNoun = -1;
    int requiredVerb = -1;
    for (int noun = 0; noun <= 99; noun++)
    {
        for (int verb = 0; verb <= 99; verb++)
        {
            program[1] = noun;
            program[2] = verb;

            const int programResult = evaluateProgram(program);
            cout << "noun: " << noun << " verb: " << verb << " value at position 0: " << programResult << endl;
            if (programResult == 19690720)
            {
                requiredNoun = noun;
                requiredVerb = verb;
            }
        }
    }
    assert(requiredNoun != -1 && requiredVerb != -1);
    cout << "Woo: " << (100 * requiredNoun + requiredVerb)  << endl;
}
