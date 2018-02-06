// Simon St James (ssjgz) - 2018-02-06 07:19
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int numUnlocks(const vector<bool>& isLockedOriginal, bool minimiseNumDoors)
{
    vector<bool> isLocked(isLockedOriginal);

    int numUnlocked = 0;

    const int numDoors = isLocked.size();
    auto handleDoor = [&numUnlocked, &isLocked, numDoors](int doorIndex)
    {
        if (isLocked[doorIndex])
        {
            isLocked[doorIndex] = false;
            numUnlocked++;
            if (doorIndex + 1 < numDoors)
            {
                isLocked[doorIndex + 1] = false;
            }
        }
    };

    if (minimiseNumDoors)
    {
        for (int i = 0; i < numDoors; i++)
        {
            handleDoor(i);
        }
    }
    else
    {
        for (int i = numDoors - 1; i >= 0; i--)
        {
            handleDoor(i);
        }
    }
    return numUnlocked;
}

int main()
{
    int n;
    cin >> n;

    vector<bool> isLocked(n);

    for (int i = 0; i < n; i++)
    {
        bool locked;
        cin >> locked;
        isLocked[i] = locked;
    }

    const auto minUnlocks = numUnlocks(isLocked, true);

    const auto maxUnlocks = numUnlocks(isLocked, false);

    cout << minUnlocks << " " << maxUnlocks << endl;
}
