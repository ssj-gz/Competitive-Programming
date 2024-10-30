#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

int main()
{
    vector<int> instructions;
    int instruction = -1;
    while (cin >> instruction)
    {
        instructions.push_back(instruction);
    }
    int instructionPointer = 0;
    int numSteps = 0;
    while (instructionPointer >= 0 && instructionPointer < static_cast<int>(instructions.size()))
    {
        const int offset = instructions[instructionPointer];
        instructions[instructionPointer]++;
        instructionPointer += offset;
        numSteps++;
    }
    std::cout << "numSteps: " << numSteps << std::endl;
    return 0;
}
