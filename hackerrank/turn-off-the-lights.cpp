// Simon St James (ssjgz) - 2018-01-28.
#include <iostream>
#include <vector>
#include <limits>

using namespace std;

int64_t minCost(const vector<int>& costs, int n, int k)
{
    const int maxLightsToggledBySwitch = 2 * k + 1;
    int64_t minCost = std::numeric_limits<int64_t>::max();
    // The firstSwitchIndex must be at or before index k, otherwise some lights at the beginning
    // of the array won't be turned off.
    for (int firstSwitchIndex = 0; firstSwitchIndex <= k; firstSwitchIndex++)
    {
        int64_t cost = 0;
        int lastSwitchIndex = -1;
        for (int lightIndex = firstSwitchIndex; lightIndex < costs.size(); lightIndex += maxLightsToggledBySwitch)
        {
            cost += costs[lightIndex];
            lastSwitchIndex = lightIndex;
        }
        // Must have lastSwitchIndex within k of the last light, otherwise some lights at the end
        // won't be turned off.
        if (lastSwitchIndex != -1 && lastSwitchIndex + k >= n - 1)
        {
            minCost = min(minCost, cost);
        }
    }

    return minCost;
}

int main(int argc, char** argv)
{
    // Very easy one: firstly, note that the order in which we flick the switches is unimportant, since
    // any permutation of them will give exactly the same set of lights turned off (proof left as an exercise
    // for the reader: just show that, given any list of switches, swapping any two adjacent ones makes no
    // difference to which lights are turned off, then note that any permutation is can be expressed as a sequence
    // of transpositions of adjacent elements).  Thus, we can assume that the switches are switched in strictly
    // increasing order (obviously, there is never any benefit to switching the same switch more than once - it
    // costs more, and is a no-op!).
    //
    // Secondly, note that  TODO
    int n, k;
    cin >> n >> k;

    vector<int> costs(n);
    for (int i = 0; i < n; i++)
    {
        cin >> costs[i];
    }

    const auto result = minCost(costs, n, k);
    cout << result << endl;
}
