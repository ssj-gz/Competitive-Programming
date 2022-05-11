#include "../shared/intcodecomputer.h"

#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <iterator>
#include <algorithm>
#include <limits>
#include <cassert>

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
    intCodeComputer.addInput(2);
    const auto status = intCodeComputer.run();
    assert(status == IntCodeComputer::Terminated);
    cout << "output:" << endl;
    for (const auto x : intCodeComputer.takeOutput())
    {
        cout << x << " ";
    }
    cout << endl;
}
