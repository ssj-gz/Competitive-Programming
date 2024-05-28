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

    inputAsciiToIntCode("NOT A T"); // 1.
    inputAsciiToIntCode("OR T J");  // 2.
    inputAsciiToIntCode("NOT B T"); // 3.
    inputAsciiToIntCode("OR T J");  // 4.
    inputAsciiToIntCode("NOT C T"); // 5.
    inputAsciiToIntCode("OR T J");  // 6.
    inputAsciiToIntCode("AND D J"); // 7. Initial decision as to whether we will jump or not!
    inputAsciiToIntCode("NOT J T"); // 8. T will be false if initial decision to jump is true, otherwise false.
    // Don't jump if E and H are *both* empty; deal with this situation:
    // ..@..............
    // #####.#.##..#.###
    //    ABCDEFGHIJ
    // 
    // where we initiate a jump at the "@" symbol, landing on D, but then
    // end up in a no-win situation.
    inputAsciiToIntCode("OR E T");  // 9.
    inputAsciiToIntCode("OR H T");  // 10.
    inputAsciiToIntCode("AND T J"); // 11. If we initially decided *not* to jump at 7., J will still be false.
                                    // If we initially decided *to* jump at 7., then T would be set to false 
                                    // at 8, and J would be true at the beginning of line 11.  The lines 9. and 10. 
                                    // ensure that T will only be true at line 11. if E & H are not *both* empty,
                                    // and so we abort the jump if E & H are both empty.
    inputAsciiToIntCode("RUN");

    const auto statusAfterInstructions = intCodeComputer.run();
    readAndPrintAsciiOutput();
}


