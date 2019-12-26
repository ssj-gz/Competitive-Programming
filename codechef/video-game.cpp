// Simon St James (ssjgz) - 2019-12-23
// 
// Solution to: https://www.codechef.com/ZCOPRAC/problems/ZCO14001
//
#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto numStacks = read<int>();
    const auto maxHeight = read<int>();

    vector<int> numBoxesInStack(numStacks);
    for (auto& numBoxes : numBoxesInStack)
        numBoxes = read<int>();

    auto isFinished = false;
    auto cranePos = 0;
    auto isCarryingBox = false;
    while (!isFinished)
    {
        const auto command = read<int>();

        switch (command)
        {
            case 1:
                // Move left.
                if (cranePos > 0)
                    cranePos--;
                break;
            case 2:
                // Move right.
                if (cranePos < numStacks - 1)
                    cranePos++;
                break;
            case 3:
                // Pick up, if we can.
                if (!isCarryingBox && numBoxesInStack[cranePos] > 0)
                {
                    isCarryingBox = true;
                    numBoxesInStack[cranePos]--;
                }
                break;
            case 4:
                // Drop, if we can.
                if (isCarryingBox && numBoxesInStack[cranePos] < maxHeight)
                {
                    isCarryingBox = false;
                    numBoxesInStack[cranePos]++;
                }
                break;
            case 0:
                isFinished = true;
                break;
        }

    }

    for (const auto numBoxes : numBoxesInStack)
    {
        cout << numBoxes << " ";
    }
    cout << endl;
    assert(cin);
}
