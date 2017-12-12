// Simon St James (ssjgz) - 2017-12-06
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int main()
{
    // Easy 70(!) points - just note that this is basically Nim in disguise, and that
    // a winning first move is just one that makes the Nim sum of the number of chocolates
    // in each container zero.  Then note that, for each container, we can find the 
    // nim sum of all other containers except this one very easily, then compute the number of chocolates
    // that would need to be in this container for the nim sum to be zero, and make sure
    // that that number is less than the original number of chocolates in this container i.e.
    // that we could make this container have the required number of chocolates in one move.
    int numContainers;
    cin >> numContainers;

    auto nimSum = 0;
    vector<int> numChocsInContainer(numContainers);
    for (auto containerIndex = 0; containerIndex < numContainers; containerIndex++)
    {
        cin >> numChocsInContainer[containerIndex];
        nimSum ^= numChocsInContainer[containerIndex];
    }

    const auto originalNimSum = nimSum;
    auto numWinningFirstMoves = 0;
    for (auto containerIndex = 0; containerIndex < numContainers; containerIndex++)
    {
        const auto nimSumWithoutContainer = originalNimSum ^ numChocsInContainer[containerIndex];
        const auto numInThisContainerToMakeNimSumZero = nimSumWithoutContainer;
        assert((nimSumWithoutContainer ^ numInThisContainerToMakeNimSumZero) == 0);

        if (numInThisContainerToMakeNimSumZero < numChocsInContainer[containerIndex])
        {
            numWinningFirstMoves++;
        }
    }
    cout << numWinningFirstMoves << endl;
}
