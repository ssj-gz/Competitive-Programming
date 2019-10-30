// Simon St James (ssjgz) - 2019-10-30
// 
// Solution to: https://www.codechef.com/problems/HILLS
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

int findMaxReachableHillPos(const int numHills, const int maxJumpUpwards, const int maxJumpDownNoParachute, const vector<int>& hillHeights)
{
    auto hillPos = 0;
    auto haveParachute = true;

    for (hillPos = 0; hillPos + 1 < numHills; hillPos++)
    {
        if (hillHeights[hillPos + 1] - hillHeights[hillPos] > maxJumpUpwards)
            break;
        if (hillHeights[hillPos] - hillHeights[hillPos + 1] > maxJumpDownNoParachute)
        {
            if (haveParachute)
                haveParachute = false;
            else
                break;
        }
    }
    
    return hillPos + 1;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (auto t = 0; t < T; t++)
    {
        const auto numHills = read<int>();
        const auto maxJumpUpwards = read<int>();
        const auto maxJumpDownNoParachute = read<int>();

        vector<int> hillHeights(numHills);
        for (auto& height : hillHeights)
        {
            cin >> height;
        }

        cout << findMaxReachableHillPos(numHills, maxJumpUpwards, maxJumpDownNoParachute, hillHeights) << endl;
    }

    assert(cin);
}
