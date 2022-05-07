#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

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

    program[1] = 12;
    program[2] = 2;

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
    cout << "Value at position 0: " << program[0] << endl;
}
