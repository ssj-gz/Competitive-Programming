#include <iostream>
#include <sstream>
#include <cassert>

using namespace std;

int main()
{
    int64_t x = 1;
    int currentCycleNum = 1;
    bool isMidwayThroughAdd = false;
    int64_t numberToAdd = 0;
    int64_t totalSignalStrength = 0;
    while (cin || isMidwayThroughAdd)
    {
        const bool wasMidwayThroughAdd = isMidwayThroughAdd;
        if (isMidwayThroughAdd)
        {
            isMidwayThroughAdd = false;
        }
        else
        {
            string instructionLine;
            getline(cin, instructionLine);
            if (!instructionLine.empty())
            {
                istringstream instructionLineStream(instructionLine);
                string command;
                instructionLineStream >> command;
                assert(instructionLineStream);
                cout << "command: " << command << endl;
                if (command == "noop")
                {
                    // Do nothing.
                }
                else if (command == "addx")
                {
                    isMidwayThroughAdd = true;
                    instructionLineStream >> numberToAdd;
                    cout << "Need to add " << numberToAdd << std::endl;
                    assert(instructionLineStream);
                }
                else
                {
                    assert(false);
                }
            }
        }

        
        // About to finish cycle.
        if (((currentCycleNum - 20) % 40) == 0)
        {
            const int64_t signalStrength = x * currentCycleNum;
            totalSignalStrength += signalStrength;
            cout << "Adding " << signalStrength << " to totalSignalStrength" << endl;
        }
        // Finished cycle.
        if (wasMidwayThroughAdd)
        {
            x += numberToAdd;
        }
        std::cout << "At end of cycle " << currentCycleNum << " x: " << x << " wasMidwayThroughAdd: " << wasMidwayThroughAdd << " isMidwayThroughAdd: " << isMidwayThroughAdd << endl;
        // Begin next cycle.
        currentCycleNum++;
    }

    std::cout << "totalSignalStrength: " <<totalSignalStrength << std::endl;

}
