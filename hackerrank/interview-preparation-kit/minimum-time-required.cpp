#include <iostream>
#include <vector>

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
}
