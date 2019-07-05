// Simon St James (ssjgz) - 2019-04-05
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int64_t findMinDaysToReachGoal(const vector<int>& machineTimeToProduce, int64_t goal)
{
    const auto longestDaysToProduceOneItem = *std::max_element(machineTimeToProduce.begin(), machineTimeToProduce.end());
    const auto maxDaysToProduceGoal = goal * longestDaysToProduceOneItem;

    int64_t topDay = maxDaysToProduceGoal;
    int64_t bottomDay = 0;

    auto numProducedAfterDays = [&machineTimeToProduce](int64_t daysElapsed)
    {
        int64_t numProduced = 0;
        for (const auto daysToProduce : machineTimeToProduce)
        {
            numProduced += daysElapsed / daysToProduce;
        }
        return numProduced;
    };
    // Binary chop.
    int answer = -1;
    while (bottomDay <= topDay)
    {
        int64_t middleDay = bottomDay + ((topDay - bottomDay) / 2);

        const auto numProducedByMiddleDay = numProducedAfterDays(middleDay);
        if (numProducedByMiddleDay >= goal)
        {
            // Store this as a candidate answer.
            // We want to find the *smallest* middleDay, though, so keep going,
            // searching "to the left" in case we find a smaller satisfactory middleDay.
            topDay = middleDay - 1;
            answer = middleDay;
        }
        else
        {
            bottomDay = middleDay + 1;
        }
    }
    return answer;
}

int main(int argc, char* argv[])
{
    // Very easy: we can find a maximum upper bound for the number of days
    // to reach our goal (maxDaysToProduceGoal); we can determine how many
    // items have been produced in O(numMachines) for a given day (numProducedAfterDays);
    // just do a binary chop to find the number of days needed to reach goal.
    //
    // Hopefully the code is self-explanatory :)
    int numMachines;
    cin >> numMachines;

    int64_t goal;
    cin >> goal;

    vector<int> machineTimeToProduce(numMachines);
    for (int i = 0; i < numMachines; i++)
    {
        cin >> machineTimeToProduce[i];
    }

    cout << findMinDaysToReachGoal(machineTimeToProduce, goal) << endl;
}
