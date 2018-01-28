// Simon St James (ssjgz) - 2018-01-28.
#include <iostream>
#include <vector>
#include <limits>
#include <sys/time.h>

using namespace std;

struct Light
{
    int index = -1;
    int cost = -1;
};

void minCostBruteForceAux(vector<Light>& lights, vector<int>& switchesSoFar, const int numSwitchesPressed, const int64_t costSoFar, vector<bool>& isLightOn, const int k, int64_t& minCost)
{
    {
        bool allLightsOff = true;
        for (const auto lightState : isLightOn)
        {
            if (lightState)
            {
                allLightsOff = false;
                break;
            }
        }

        if (allLightsOff)
        {
            if (costSoFar < minCost)
            {
                cout << "Found new best cost: " << costSoFar << endl;
                cout << "Switches used: ";
                for (const auto x : switchesSoFar)
                {
                    cout << x << " ";
                }
                cout << endl;
                minCost = costSoFar;
            }
        }
    }

    for (int i = numSwitchesPressed; i < lights.size(); i++)
    {
        //cout << "numSwitchesPressed: " << numSwitchesPressed << " i: " << i << endl;
        //cout << "Switches: " << endl;
        for (const auto& x : lights)
        {
            //cout << "index: " << x.index << " cost: " << x.cost << " ";
        }
        //cout << endl;

        const auto nextCost = costSoFar + lights[i].cost;
        const auto switchIndexToTry = lights[i].index;
        //cout << "numSwitchesPressed: " << numSwitchesPressed << " trying switch " << switchIndexToTry << " (i = " << i << ")" << " which has cost " << lights[i].cost << endl;

        if (i != numSwitchesPressed)
        {
            swap(lights[numSwitchesPressed], lights[i]);
        }

        for (int lightIndex = switchIndexToTry - k; lightIndex <= switchIndexToTry + k; lightIndex++)
        {
            if (lightIndex >= 0 && lightIndex < isLightOn.size())
            {
                isLightOn[lightIndex] = !isLightOn[lightIndex];
            }
        }

        switchesSoFar.push_back(switchIndexToTry);
        minCostBruteForceAux(lights, switchesSoFar, numSwitchesPressed + 1, nextCost, isLightOn, k, minCost);
        switchesSoFar.pop_back();

        for (int lightIndex = switchIndexToTry - k; lightIndex <= switchIndexToTry + k; lightIndex++)
        {
            if (lightIndex >= 0 && lightIndex < isLightOn.size())
            {
                isLightOn[lightIndex] = !isLightOn[lightIndex];
            }
        }

        if (i != numSwitchesPressed)
        {
            swap(lights[numSwitchesPressed], lights[i]);
        }
    }
}

int64_t minCostBruteForce(const vector<int>& costs, int n, int k)
{
    auto costsCopy = costs;
    vector<bool> isLightOn(n, true);

    int64_t minCost = std::numeric_limits<int64_t>::max();

    vector<Light> lights;
    for (int i = 0; i < n; i++)
    {
        lights.push_back({i, costs[i]});
    }

    vector<int> switchesSoFar;
    minCostBruteForceAux(lights, switchesSoFar, 0, 0, isLightOn, k, minCost);

    return minCost;
}

int64_t minCost(const vector<int>& costs, int n, int k)
{
    const int maxLightsToggledBySwitch = 2 * k + 1;
    int64_t minCost = std::numeric_limits<int64_t>::max();
    for (int firstSwitchIndex = 0; firstSwitchIndex <= k; firstSwitchIndex++)
    {
        int64_t cost = 0;
        int lastSwitchIndex = -1;
        for (int i = firstSwitchIndex; i < costs.size(); i += maxLightsToggledBySwitch)
        {
            cost += costs[i];
            lastSwitchIndex = i;
        }
        if (lastSwitchIndex != -1 && lastSwitchIndex + k >= n - 1)
        {
            if (cost < minCost)
            {
                //cout << "Found new best: firstSwitchIndex " << firstSwitchIndex << " lastSwitchIndex: " << lastSwitchIndex << " cost: " << cost << endl;
            }
            minCost = min(minCost, cost);
        }
    }

    return minCost;
}

int main(int argc, char** argv)
{
//#define BRUTE_FORCE
    struct timeval time;
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

    if (argc == 2)
    {
        const int n = rand() % 12 + 1;
        const int k = rand() % n + 1;
        cout << n << " " << k << endl;

        for (int i = 0; i < n; i++)
        {
            cout << rand() % 30 << " ";
        }
        cout << endl;
        return 0;

    }
    int n, k;
    cin >> n >> k;

    vector<int> costs(n);
    for (int i = 0; i < n; i++)
    {
        cin >> costs[i];
    }

    const auto result = minCost(costs, n, k);
    cout << result << endl;
#ifdef BRUTE_FORCE
    const auto resultBruteForce = minCostBruteForce(costs, n, k);

    cout << "result: " << result << " resultBruteForce: " << resultBruteForce << endl;
    assert(result == resultBruteForce);
#endif


}
