#include "../shared/intcodecomputer.h"

#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <limits>
#include <cassert>

using namespace std;

int main()
{
    vector<int64_t> program;
    int64_t input;
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

    vector<int64_t> phaseSettingPermutation = {5,6,7,8,9};
    const auto originalPhaseSettingPermutation = phaseSettingPermutation;

    constexpr int numAmplifiers = 5;

    int64_t largestThrust = std::numeric_limits<int64_t>::min();
    do
    {
        cout << "Trying permutation" << endl;
        for (const auto x : phaseSettingPermutation)
            cout << x << " ";
        // Run all amplifiers to completion with current phaseSettingPermutation.
        IntCodeComputer intCodeComputers[] = { IntCodeComputer(program), IntCodeComputer(program), IntCodeComputer(program), IntCodeComputer(program), IntCodeComputer(program) };
        assert(phaseSettingPermutation.size() == std::size(intCodeComputers));
        for (int i = 0; i < numAmplifiers; i++)
        {
            vector<int64_t> phaseSettingInput;
            phaseSettingInput.push_back(phaseSettingPermutation[i]);
            intCodeComputers[i].addInputs(phaseSettingInput);
        }
        vector<int64_t> initialInput;
        initialInput.push_back(0);
        intCodeComputers[0].addInputs(initialInput);

        int64_t thrust = -1;
        int iterationNum = 1;
        bool allTerminated = true;
        do
        {
            allTerminated = true;
            cout << "iterationNum: " << iterationNum << endl;
            for (int i = 0; i < numAmplifiers; i++)
            {
                cout << "Executing amplifier #" << i << endl;
                const IntCodeComputer::ExitStatus status = intCodeComputers[i].run();
                if (status != IntCodeComputer::Terminated)
                    allTerminated = false;
                cout << " status: " << (status == IntCodeComputer::Terminated ? "Terminated" : "WaitingForInput") << endl;
                const auto output = intCodeComputers[i].output();
                if (i == numAmplifiers - 1 && output.size() == 1)
                {
                    thrust = output.front();
                }
                intCodeComputers[i].clearOutput();
                intCodeComputers[(i + 1) % numAmplifiers].addInputs(output);
            }
            iterationNum++;

        } while (!allTerminated);

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
