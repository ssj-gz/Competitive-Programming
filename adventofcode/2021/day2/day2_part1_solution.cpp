#include <iostream>
#include <cassert>

using namespace std;

int main()
{
    int64_t horizOffset = 0;
    int64_t depthOffset = 0;
    while (true)
    {
        string direction;
        cin >> direction;
        int numUnits;
        cin >> numUnits;
        if (!cin)
            break;
        if (direction == "forward")
            horizOffset += numUnits;
        else if (direction == "down")
            depthOffset += numUnits;
        else if (direction == "up")
            depthOffset -= numUnits;
        else
            assert(false);
    }
    cout << (horizOffset * depthOffset) << endl;
}
