// Simon St James (ssjgz) - 2018-02-06 07:19
#include <iostream>
#include <vector>
#include <algorithm>
#include <sys/time.h>

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

void numUnlocksBruteForceAux(const vector<bool>& isLocked, int numSoFar, int& minimum, int& maximum)
{
    bool unlocksMade = false;
    for (int i = 0; i < isLocked.size(); i++)
    {
        if (isLocked[i])
        {
            unlocksMade = true;
            vector<bool> nextIsLocked(isLocked);
            nextIsLocked[i] = false;
            if (i + 1 < isLocked.size())
            {
                nextIsLocked[i + 1] = false;
            }
            numUnlocksBruteForceAux(nextIsLocked, numSoFar + 1, minimum, maximum);
        }
    }
    if (!unlocksMade)
    {
        minimum = min(minimum, numSoFar);
        maximum = max(maximum, numSoFar);
    }
}

int numUnlocksBruteForce(const vector<bool>& isLocked, bool minimiseNumDoors)
{
    int minimum = std::numeric_limits<int>::max();
    int maximum = std::numeric_limits<int>::min();
    numUnlocksBruteForceAux(isLocked, 0, minimum, maximum);

    return (minimiseNumDoors ? minimum : maximum);
}

int main(int argc, char** argv)
{
    struct timeval time;
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

    if (argc == 2)
    {
        const int n = rand() % 10 + 1;
        cout << n << endl;
        for (int i = 0; i < n; i++)
        {
            cout << (rand() % 2) << " ";
        }
        return 0;
    }
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
#define BRUTE_FORCE
#ifdef BRUTE_FORCE
    const auto minUnlocksBruteForce = numUnlocksBruteForce(isLocked, true);
    const auto maxUnlocksBruteForce = numUnlocksBruteForce(isLocked, false);
    assert(minUnlocksBruteForce == minUnlocks);
    assert(maxUnlocksBruteForce == maxUnlocks);
    cout << "minUnlocks: " << minUnlocks << " minUnlocksBruteForce: " << minUnlocksBruteForce << endl;
    cout << "maxUnlocks: " << maxUnlocks << " maxUnlocksBruteForce: " << maxUnlocksBruteForce << endl;
#endif
}
