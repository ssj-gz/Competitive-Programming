// Simon St James (ssjgz) - 2019-05-21
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

void solveBruteForce(const vector<int>& H, int mandragoraNum, int64_t currentHealth, int64_t currentExperience, int64_t& bestExperience, string& choicesSoFar)
{
    //cout << "mandragoraNum: " << mandragoraNum << " choicesSoFar: " << choicesSoFar << " currentHealth: " << currentHealth << " currentExperience: " << currentExperience << endl;
    if (mandragoraNum == H.size())
    {
        if (currentExperience >= bestExperience)
        {
            bestExperience = currentExperience;
            cout << "new best experience: " << bestExperience << " choicesSoFar: " << choicesSoFar << endl;
        }
        return;
    }

    choicesSoFar.push_back('e');
    solveBruteForce(H, mandragoraNum + 1, currentHealth + 1, currentExperience, bestExperience, choicesSoFar);
    choicesSoFar.pop_back();
    choicesSoFar.push_back('b');
    solveBruteForce(H, mandragoraNum + 1, currentHealth, currentExperience + currentHealth * H[mandragoraNum], bestExperience, choicesSoFar);
    choicesSoFar.pop_back();
}

int64_t solveBruteForce(const vector<int>& originalH)
{
    int64_t bestExperience = 0;
    auto hPermutation = originalH;
    do
    {
        cout << "Trying with permuted health: " << endl;
        for (const auto x : hPermutation)
        {
            cout << " " << x;
        }
        cout << endl;
        int64_t bestExperienceForPermutation = 0;
        string choicesSoFar;
        solveBruteForce(hPermutation, 0, 1, 0, bestExperienceForPermutation, choicesSoFar);

        bestExperience = max(bestExperience, bestExperienceForPermutation);
        cout << " best experience for this permutation: " << bestExperienceForPermutation << endl;
        next_permutation(hPermutation.begin(), hPermutation.end());

    }
    while (hPermutation != originalH);
    return bestExperience;
}

int64_t solveOptimised(const vector<int>& originalH)
{
    const int n = originalH.size();

    int64_t bestExperience = 0;
    auto sortedH = originalH;
    sort(sortedH.begin(), sortedH.end());

    vector<int64_t> sumOfRemainingHealths(n + 1);
    int64_t sumOfLastHealths = 0;
    for (const auto h : sortedH)
    {
        sumOfLastHealths += h;
    }
    for (int numRemaining = n; numRemaining >= 0; numRemaining--)
    {
        sumOfRemainingHealths[numRemaining] = sumOfLastHealths;
        if (n - numRemaining >= 0)
            sumOfLastHealths -= sortedH[n - numRemaining];
    }

    int64_t currentHealthIfOnlyEat = 1;
    const int64_t currentExperienceIfOnlyEat = 0;
    for (int i = 0; i < n; i++)
    {
        const int64_t currentExperience = currentHealthIfOnlyEat * sumOfRemainingHealths[n - i];
        bestExperience = max(bestExperience, currentExperience);
        currentHealthIfOnlyEat++;
    }
    return bestExperience;
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int n = rand() % 9 + 1;
        const int maxHealth = rand() % 100 + 1;

        cout << 1 << endl;
        cout << n << endl;
        for (int i = 0; i < n; i++)
        {
            cout << (rand() % maxHealth) + 1 << " ";
        }
        cout <<endl;
        return 0;
    }
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        vector<int> H(n);
        for (int i = 0; i < n; i++)
        {
            cin >> H[i];
        }

#ifdef BRUTE_FORCE
        const auto solutionBruteForce = solveBruteForce(H);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;

        const auto solutionOptimised = solveOptimised(H);
        cout << "solutionOptimised: " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#else
        cout << solveOptimised(H) << endl;
#endif
    }



}
