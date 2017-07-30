#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

int minStepsForEquality(const vector<int>& numCandiesForColleagues)
{
    const int numColleagues = numCandiesForColleagues.size(); 
    auto numStepsToFormAmount = [](int amount){
        return (amount / 5) + ((amount % 5) / 2) + ((amount % 5) % 2);
    };

    int64_t numSteps = 0;

    int numCandiesGivenToRemainingColleagues = 0;
    for (int i = 1; i < numColleagues; i++)
    {
        const int numCandiesCurrentColleague = (numCandiesForColleagues[i] + numCandiesGivenToRemainingColleagues);
        const int amountToRaiseUpPreviousColleague = numCandiesCurrentColleague - numCandiesForColleagues[i - 1];
        const int64_t numStepsToRaiseUpPreviousColleague = (amountToRaiseUpPreviousColleague / 5) + ((amountToRaiseUpPreviousColleague % 5) / 2) + 
            ((amountToRaiseUpPreviousColleague % 5) % 2);
        numCandiesGivenToRemainingColleagues = amountToRaiseUpPreviousColleague;

        numSteps += numStepsToRaiseUpPreviousColleague;
    }
    return numSteps;
}

int main() {
    int T;
    cin >> T;
    for (int t = 0; t < T; t++)
    {
        int numColleagues; 
        cin >> numColleagues;
        vector<int> numCandiesForColleagues(numColleagues);
        for (int i = 0; i < numColleagues; i++)
        {
            cin >> numCandiesForColleagues[i];
        }
        sort(numCandiesForColleagues.begin(), numCandiesForColleagues.end());

        int minSteps = minStepsForEquality(numCandiesForColleagues);
        for (int i = 1; i < numColleagues; i++)
        {
            numCandiesForColleagues[i]++;
        }
#if 1
        // Added a total of 1 candy: this can be done in 1 step.
        minSteps = min(minSteps, minStepsForEquality(numCandiesForColleagues) + 1);
        for (int i = 1; i < numColleagues; i++)
        {
            numCandiesForColleagues[i]++;
        }
        // Added a total of 2 candies: this can be done in 1 step.
        minSteps = min(minSteps, minStepsForEquality(numCandiesForColleagues) + 1);
        for (int i = 1; i < numColleagues; i++)
        {
            numCandiesForColleagues[i]++;
        }
        // Added a total of 3 candies: this can be done in 2 steps.
        minSteps = min(minSteps, minStepsForEquality(numCandiesForColleagues) + 2);
        for (int i = 1; i < numColleagues; i++)
        {
            numCandiesForColleagues[i]++;
        }
        // Added a total of 4 candies: this can be done in 2 steps.
        minSteps = min(minSteps, minStepsForEquality(numCandiesForColleagues) + 2);
#endif


        cout << minSteps << endl;
    }
    return 0;
}
