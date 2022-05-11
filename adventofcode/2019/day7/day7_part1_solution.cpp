#include "../shared/intcodecomputer.h"

#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <limits>
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

    vector<int> phaseSettingPermutation = {0,1,2,3,4};
    const auto originalPhaseSettingPermutation = phaseSettingPermutation;

    int largestThrust = std::numeric_limits<int>::min();
    do
    {
        int ampInput = 0;
        for (int phase : phaseSettingPermutation)
        {
            vector<int64_t> input;
            input.push_back(phase);
            input.push_back(ampInput);
            IntCodeComputer intCodeComputer(program);
            intCodeComputer.addInputs(input);
            const auto status = intCodeComputer.run();
            assert(status == IntCodeComputer::Terminated);
            const auto output = intCodeComputer.takeOutput();
            assert(output.size() == 1);
            ampInput = output.front();
        }
        const int thrust = ampInput;
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
