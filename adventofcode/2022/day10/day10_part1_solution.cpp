#include "CycleSimulator.h"
#include <iostream>

using namespace std;

int main()
{
    CycleSimulator cycleSimulator(cin);

    int64_t totalSignalStrength = 0;

    while (!cycleSimulator.isFinished())
    {
        if (((cycleSimulator.currentCycleNum() - 20) % 40) == 0)
        {
            const int64_t signalStrength = cycleSimulator.registerValueDuringCurrentCycle() * cycleSimulator.currentCycleNum();
            totalSignalStrength += signalStrength;
        }

        cycleSimulator.computeNextCycle();
    }

    std::cout << "totalSignalStrength: " <<totalSignalStrength << std::endl;

}
