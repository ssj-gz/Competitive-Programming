// Simon St James (ssjgz) - 2019-04-05
#include <iostream>
#include <vector>

using namespace std;

int64_t modulus = 1'000'000'007ULL;

int64_t findNumWaysToClimbStairs(int numStairs, vector<int64_t>& numWaysToClimbStairsLookup)
{
    if (numStairs < 0)
        return 0;
    if (numStairs == 0)
        return 1;

    if (numWaysToClimbStairsLookup[numStairs] != -1)
        return numWaysToClimbStairsLookup[numStairs];


    int64_t numWays = 0;
    if (numStairs >= 1)
    {
        numWays += findNumWaysToClimbStairs(numStairs - 1, numWaysToClimbStairsLookup);
        numWays %= ::modulus;
    }
    if (numStairs >= 2)
    {
        numWays += findNumWaysToClimbStairs(numStairs - 2, numWaysToClimbStairsLookup);
        numWays %= ::modulus;
    }
    if (numStairs >= 3)
    {
        numWays += findNumWaysToClimbStairs(numStairs - 3, numWaysToClimbStairsLookup);
        numWays %= ::modulus;
    }

    numWaysToClimbStairsLookup[numStairs] = numWays;

    return numWays;
}

int main()
{
    int numTestcases;
    cin >> numTestcases;

    for (int t = 0; t < numTestcases; t++)
    {
        int numStairs;
        cin >> numStairs;

        vector<int64_t> numWaysToClimbStairsLookup(numStairs + 1, -1);

        cout << findNumWaysToClimbStairs(numStairs, numWaysToClimbStairsLookup) << endl;
    }
}
