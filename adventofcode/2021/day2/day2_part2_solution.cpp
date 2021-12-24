#include <iostream>
#include <cassert>

using namespace std;

int main()
{
    int64_t horizOffset = 0;
    int64_t depthOffset = 0;
    int64_t aim = 0;
    while (true)
    {
        string direction;
        cin >> direction;
        int numUnits;
        cin >> numUnits;
        if (!cin)
            break;
        if (direction == "forward")
        {
            horizOffset += numUnits;
            depthOffset += numUnits * aim;
        }
        else if (direction == "down")
            aim += numUnits;
        else if (direction == "up")
            aim -= numUnits;
        else
            assert(false);
    }
    cout << (horizOffset * depthOffset) << endl;
}
