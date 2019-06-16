#include <iostream>
#include <limits>

using namespace std;

void solveBruteForce(int64_t numWorkers, int64_t numMachines, int64_t costOfNewMachineOrWorker, int64_t numCandies, int64_t numCandiesToProduce, int numPassesSoFar, int64_t& bestNumPasses)
{
    if (numCandies >= numCandiesToProduce)
    {
        if(numPassesSoFar <= bestNumPasses)
        {
            cout << "Found new best: " << numPassesSoFar << endl;
            bestNumPasses = numPassesSoFar;
        }
        return;
    }
    if (numPassesSoFar > bestNumPasses)
    {
        return;
    }

    numCandies += numMachines * numWorkers;

    for (int numWorkersToBuy = 0; numWorkersToBuy * costOfNewMachineOrWorker <= numCandies; numWorkersToBuy++)
    {
        const auto numCandiesAfterWorkers = numCandies -(numWorkersToBuy * costOfNewMachineOrWorker);
        for (int numMachinesToBuy = 0; numMachinesToBuy * costOfNewMachineOrWorker <= numCandiesAfterWorkers; numMachinesToBuy++)
        {
            solveBruteForce(numWorkers + numWorkersToBuy, numMachines + numMachinesToBuy, costOfNewMachineOrWorker, numCandiesAfterWorkers - numMachinesToBuy * costOfNewMachineOrWorker, numCandiesToProduce, numPassesSoFar + 1, bestNumPasses);
        }
    }
}

int64_t solveBruteForce(int64_t m, int64_t w, int64_t p, int64_t n)
{
    int64_t bestNumPasses = std::numeric_limits<int>::max();
    solveBruteForce(w, m, p, 0, n, 0, bestNumPasses);
    return bestNumPasses;
}

int main(int argc, char* argv[])
{
    int64_t m, w, p, n;
    cin >> m >> w >> p >> n;

    const auto solutionBruteForce = solveBruteForce(m, w, p, n);
    cout << "solutionBruteForce: " << solutionBruteForce << endl;

}

