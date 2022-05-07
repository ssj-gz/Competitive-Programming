#include <iostream>

using namespace std;

int main()
{
    int64_t moduleMass;
    int64_t totalfuelNeeded = 0;
    while (cin >> moduleMass)
    {
        const int64_t moduleFuelNeeded = (moduleMass / 3) - 2;
        cout << "Initial moduleFuelNeeded: " << moduleFuelNeeded << endl;
        int64_t fuelNeededForModuleFuel = 0;
        int64_t fuelThatNeedsFuel = moduleFuelNeeded;
        while (true)
        {
            fuelThatNeedsFuel = (fuelThatNeedsFuel / 3) - 2;
            if (fuelThatNeedsFuel > 0)
            {
                fuelNeededForModuleFuel += fuelThatNeedsFuel;
            }
            else
            {
                break;
            }

        }
        cout << "fuelNeededForModuleFuel: " << fuelNeededForModuleFuel << endl;
        cout << "total moduleFuelNeeded: " << (moduleFuelNeeded + fuelNeededForModuleFuel) << endl;
        totalfuelNeeded += moduleFuelNeeded + fuelNeededForModuleFuel;
    }
    cout << "totalfuelNeeded: " << totalfuelNeeded << endl;
}
