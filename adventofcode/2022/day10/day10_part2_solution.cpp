#include "CycleSimulator.h"

#include <iostream>
#include <sstream>
#include <cassert>

using namespace std;

int main()
{

    string currentCrtRowPixels;
    int currentCrlCol = 0; // 0-relative.

    CycleSimulator cycleSimulator(cin);

    while (!cycleSimulator.isFinished())
    {
        if (abs(currentCrlCol - cycleSimulator.registerValueDuringCurrentCycle()) <= 1)
            currentCrtRowPixels += "#";
        else
            currentCrtRowPixels += ".";

        currentCrlCol++;
        if (currentCrlCol == 40)
        {
            currentCrlCol = 0;
            std::cout << "Row: " << currentCrtRowPixels << std::endl;
            currentCrtRowPixels.clear();
        }

        cycleSimulator.computeNextCycle();
    }
}
