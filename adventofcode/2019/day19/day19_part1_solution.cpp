#include "../shared/intcodecomputer.h"

#include <iostream>
#include <vector>

using namespace std;

int main()
{
    vector<int64_t> program;
    int64_t programInput;
    while (true)
    {
        cin >> programInput;
        program.push_back(programInput);
        assert(cin);
        char comma;
        cin >> comma;
        if (!cin)
            break;
        assert(comma == ',');
    }

    int numPointsAffected = 0;
    for (int x = 0; x <= 49; x++)
    {
        for (int y = 0; y <= 49; y++)
        {
            IntCodeComputer intCodeComputer(program);
            const auto status = intCodeComputer.run();
            assert(status == IntCodeComputer::WaitingForInput);

            intCodeComputer.addInput(x);
            intCodeComputer.addInput(y);

            const auto statusAfterInput = intCodeComputer.run();
            assert(statusAfterInput == IntCodeComputer::Terminated);

            const auto output = intCodeComputer.takeOutput();
            std::cout << "output size: " << output.size() << std::endl;
            assert(output.size() == 1);
            std::cout << "x: " << x << " y: " << y << " output: " << output[0] << std::endl;
            if (output[0] == 1)
                numPointsAffected++;
        }
    }
    std::cout << "numPointsAffected: " << numPointsAffected << std::endl;
}


