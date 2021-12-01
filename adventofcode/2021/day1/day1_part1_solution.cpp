#include <iostream>

using namespace std;

int main()
{
    bool havePreviousDepth = false;
    int64_t previousDepth = -1;
    int64_t depth = 0;
    int numIncreasedDepths = 0;
    while (cin >> depth)
    {
        if (havePreviousDepth && depth > previousDepth)
            numIncreasedDepths++;

        previousDepth = depth;
        havePreviousDepth = true;
    }
    cout << numIncreasedDepths << endl;
}
