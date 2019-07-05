#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

int64_t solveBruteForce(const vector<int>& machineTimeToProduce, int64_t goal)
{
    int64_t daysElapsed = 0;
    int64_t numMade = 0;
    while (numMade < goal)
    {
        daysElapsed++;
        for (const auto daysToProduce : machineTimeToProduce)
        {
            if ((daysElapsed % daysToProduce) == 0)
            {
                numMade++;
            }
        }
    }
    return daysElapsed;
}
int64_t solveOptimised(const vector<int>& machineTimeToProduce, int64_t goal)
{
    const auto longestDaysToProduceOneItem = *std::max_element(machineTimeToProduce.begin(), machineTimeToProduce.end());
    const auto maxDaysToProduceGoal = goal * longestDaysToProduceOneItem;

    int64_t topDay = maxDaysToProduceGoal;
    int64_t bottomDay = 0;

    int64_t middleDay = -1;

    auto numProducedAfterDays = [&machineTimeToProduce](int64_t daysElapsed)
    {
        int64_t numProduced = 0;
        for (const auto daysToProduce : machineTimeToProduce)
        {
            numProduced += daysElapsed / daysToProduce;
        }
        return numProduced;
    };
    while (topDay - bottomDay > 1)
    {
        middleDay = bottomDay + (topDay - bottomDay) / 2;
        cout << "topDay: " << topDay << " middleDay: " << middleDay << " bottomDay: " << bottomDay << endl;

        const auto numProducedByMiddleDay = numProducedAfterDays(middleDay);
        cout << " numProducedByMiddleDay: " << numProducedByMiddleDay << endl;
        if (numProducedByMiddleDay < goal)
        {
            bottomDay = middleDay;
        }
        else
        {
            topDay = middleDay;
        }
    }
    if (numProducedAfterDays(bottomDay) >= goal)
    {
        return bottomDay;
    }
    if (numProducedAfterDays(topDay) >= goal)
    {
        return topDay;
    }
    assert(false);
    return middleDay;
}

int main(int argc, char* argv[])
{
    int numMachines;
    cin >> numMachines;

    int64_t goal;
    cin >> goal;

    vector<int> machineTimeToProduce(numMachines);
    for (int i = 0; i < numMachines; i++)
    {
        cin >> machineTimeToProduce[i];
    }

    const auto solutionBruteForce = solveBruteForce(machineTimeToProduce, goal);
    cout << "solutionBruteForce: " << solutionBruteForce << endl;
    const auto solutionOptimised = solveOptimised(machineTimeToProduce, goal);
    cout << "solutionOptimised: " << solutionOptimised << endl;
    assert(solutionOptimised == solutionBruteForce);
}
