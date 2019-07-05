// Simon St James (ssjgz) - 2019-04-05
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>
#include <sys/time.h>

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

        const auto numProducedByMiddleDay = numProducedAfterDays(middleDay);
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
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int n = (rand() % 1000) + 1;
        const int goal = rand() % 1000000;

        cout << n << " " << goal << endl;

        for (int i = 0; i < n; i++)
        {
            cout << ((rand() % 100) + 1) << " ";
        }
        cout << endl;

        return 0;
    }
    int numMachines;
    cin >> numMachines;

    int64_t goal;
    cin >> goal;

    vector<int> machineTimeToProduce(numMachines);
    for (int i = 0; i < numMachines; i++)
    {
        cin >> machineTimeToProduce[i];
    }

#ifdef BRUTE_FORCE
    const auto solutionBruteForce = solveBruteForce(machineTimeToProduce, goal);
    cout << "solutionBruteForce: " << solutionBruteForce << endl;
    const auto solutionOptimised = solveOptimised(machineTimeToProduce, goal);
    cout << "solutionOptimised: " << solutionOptimised << endl;
    assert(solutionOptimised == solutionBruteForce);
#else
    const auto solutionOptimised = solveOptimised(machineTimeToProduce, goal);
    cout << solutionOptimised << endl;
#endif
}
