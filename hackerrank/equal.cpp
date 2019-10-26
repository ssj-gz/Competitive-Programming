// Simon St James (ssjgz) 2017-7-15 11:15
#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

int minStepsForEquality(const vector<int>& numCandiesForColleagues)
{
    const int numColleagues = numCandiesForColleagues.size();
    auto minNumStepsToFormAmount = [](int amount){
        return (amount / 5) + ((amount % 5) / 2) + ((amount % 5) % 2);
    };

    int64_t numSteps = 0;

    int numCandiesGivenToAllButPreviousColleague = 0;
    for (int i = 1; i < numColleagues; i++)
    {
        const int numCandiesCurrentColleague = numCandiesForColleagues[i];
        // Adding candies to all but the previous colleague will have increased the gap between current colleague and previous by
        // numCandiesGivenToAllButPreviousColleague.
        const int amountToRaiseUpPreviousColleague = numCandiesCurrentColleague + numCandiesGivenToAllButPreviousColleague - numCandiesForColleagues[i - 1];
        const int64_t numStepsToRaiseUpPreviousColleague = minNumStepsToFormAmount(amountToRaiseUpPreviousColleague);

        numSteps += numStepsToRaiseUpPreviousColleague;
        numCandiesGivenToAllButPreviousColleague = amountToRaiseUpPreviousColleague;
    }
    return numSteps;
}

int main() {
    // Quite a tough one for a mere 30 points, I thought!
    // Anyway, sort the colleagues in order of number of candies given.  Now, note that any sequence of
    // steps can be re-arranged in any order to give the same eventual result; thus, we can
    // assume that an optimal solution can be expressed as:
    //
    //  - List of steps where we give to everyone but colleague 0;
    //  - List of steps where we give to everyone but colleague 1;
    //  etc;
    //
    // Recall that we have sorted the colleagues in order of current candies, so colleague 0 initially has
    // less than or equal candies to colleague 1.  Also, the only way to affect the difference in candies
    // between colleague x and colleague x + 1 is to either add candies to everyone but x
    // (which increases the difference) or add candies to everyone but x + 1 (which decreases it).
    // So: referring to our re-arranged optimal list above, we see that the set of candies given to everyone
    // but x occur in one block, and after this block, *colleagues 0, 1, ... , x must all have equal
    // numbers of candies*.  So our re-arranged optimal set of steps looks like it brings up colleague 0
    // to colleague 1's level; then brings up colleague 1 to colleague 2's level; etc.
    // If we didn't add any candies to all but candidate 0, we see that an optimal set of steps must
    // raise  candidate 0 to candidate 1, so we must make (at least) the minimum number of steps
    // needed to form (numCandiesForColleagues[1] - numCandiesForColleagues[0]).  The minimum number
    // of steps to form an amount is easily computed; see minNumStepsToFormAmount.
    // After this, colleagues 0 and 1 have the same amount of candies.  We then need to bring candidate 1
    // up to candidate 2's level, but note that in adding candies to all but colleague 1 last round,
    // we've increased the original difference between colleague 2 and colleague 1 by the amount of
    // candies given to everyone but colleague 1 i.e. by (numCandiesForColleagues[1] - numCandiesForColleagues[0]).
    // Thus, to bring colleague 1 up to level of colleague 2, we need to make (at least) the min number
    // of steps to form the amount (numCandiesForColleagues[2] - numCandiesForColleagues[1] + numCandiesForColleagues[1]
    // - numCandiesForColleagues[0]).  The pattern for remaining colleagues should hopefully be clear.
    // It's hard to prove, but I can't see a more optimal strategy than this.  Note that the number of steps
    // is deterministic.
    // There is one last subtlety, however: adding some initial amount of candies to everyone but colleague 0
    // can actually make a pretty big difference to the eventual cost of levelling everyone up.
    // Adding 5 or more can be shown to have no effect, but adding 1, 2, 1&2, and 2&2 does make a difference.
    // Thus, we must find the minimum number of steps after adding each of the above, and take the total
    // minimum over these.  That's about it!
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

        // Min steps without adding any to colleague 0.
        int minSteps = minStepsForEquality(numCandiesForColleagues);

        auto addOneToAllButColleague0 = [&numCandiesForColleagues, numColleagues]()
        {
            for (int i = 1; i < numColleagues; i++)
            {
                numCandiesForColleagues[i]++;
            }
        };

        addOneToAllButColleague0();
        // Added a total of 1 candy to all but colleague 0: this can be done in 1 step.
        minSteps = min(minSteps, minStepsForEquality(numCandiesForColleagues) + 1);

        addOneToAllButColleague0();
        // Added a total of 2 candies: this can be done in 1 step.
        minSteps = min(minSteps, minStepsForEquality(numCandiesForColleagues) + 1);

        addOneToAllButColleague0();
        // Added a total of 3 candies: this can be done in 2 steps.
        minSteps = min(minSteps, minStepsForEquality(numCandiesForColleagues) + 2);

        addOneToAllButColleague0();
        // Added a total of 4 candies: this can be done in 2 steps.
        minSteps = min(minSteps, minStepsForEquality(numCandiesForColleagues) + 2);

        cout << minSteps << endl;
    }
    return 0;
}

