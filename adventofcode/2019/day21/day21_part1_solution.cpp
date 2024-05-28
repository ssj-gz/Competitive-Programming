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

    IntCodeComputer intCodeComputer(program);
    const auto status = intCodeComputer.run();
    assert(status == IntCodeComputer::WaitingForInput);

    auto inputAsciiToIntCode = [&intCodeComputer](const std::string& inputLine)
    {
        vector<int64_t> input;
        for (const char letter : inputLine)
            input.push_back(static_cast<int64_t>(letter));
        input.push_back(10);
        intCodeComputer.addInputs(input);

    };

    auto readAndPrintAsciiOutput = [&intCodeComputer]()
    {
        const auto output = intCodeComputer.takeOutput();
        assert(!output.empty());
        for (const auto letter : output)
        {
            if (letter > 255)
            {
                cout << letter;
            }
            else
            {
                cout << static_cast<char>(letter);
            }
        }
    };

    readAndPrintAsciiOutput();

    inputAsciiToIntCode("NOT A T");
    inputAsciiToIntCode("OR T J");
    inputAsciiToIntCode("NOT B T");
    inputAsciiToIntCode("OR T J");
    inputAsciiToIntCode("NOT C T");
    inputAsciiToIntCode("OR T J");
    inputAsciiToIntCode("AND D J");
    inputAsciiToIntCode("WALK");

    const auto statusAfterInstructions = intCodeComputer.run();
    readAndPrintAsciiOutput();
}


