// Simon St James (ssjgz) - 2019-12-23
// 
// Solution to: https://www.codechef.com/ZCOPRAC/problems/ZCO14001
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

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
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
        }

        return 0;
    }
    
    const int numStacks = read<int>();
    const int maxHeight = read<int>();

    vector<int> numBoxesInStack(numStacks);
    for (auto& numBoxes : numBoxesInStack)
        numBoxes = read<int>();

    bool finished = false;
    int cranePos = 0;
    bool carryingBox = false;
    while (!finished)
    {
        const int command = read<int>();

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
                // Pick up.
                if (!carryingBox && numBoxesInStack[cranePos] > 0)
                {
                    carryingBox = true;
                    numBoxesInStack[cranePos]--;
                }
                break;
            case 4:
                // Drop.
                if (carryingBox && numBoxesInStack[cranePos] < maxHeight)
                {
                    carryingBox = false;
                    numBoxesInStack[cranePos]++;
                }
                break;
            case 0:
                finished = true;
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
