#include "../shared/intcodecomputer.h"

#include <iostream>
#include <vector>
#include <deque>
#include <cassert>

using namespace std;

int main()
{
    vector<int64_t> program;
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

    IntCodeComputer intCodeComputer(program);
    intCodeComputer.addInput(1);
    const auto status = intCodeComputer.run();
    assert(status == IntCodeComputer::Terminated);
    const auto outputs = intCodeComputer.takeOutput();
    cout << "Output: " << endl;
    for (const auto x : outputs)
        cout << x << " ";
    cout << endl;
}
