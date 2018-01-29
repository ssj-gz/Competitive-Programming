// Simon St James (ssjgz) - 2018-01-29
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int64_t findMaxScoreForNumBricks(int numBricks, const vector<int>& brickValues, const vector<int64_t>& sumOfFirstNBrickValues, vector<int64_t>& maxScoreForNumBricksLookup)
{
    if (maxScoreForNumBricksLookup[numBricks] != -1)
        return maxScoreForNumBricksLookup[numBricks];

    int64_t maxScore = 0;
    for (int numBricksToTake = 1; numBricksToTake <= 3 && numBricksToTake <= numBricks; numBricksToTake++)
    {
        const auto sumOfBricksTaken = sumOfFirstNBrickValues[numBricks] - sumOfFirstNBrickValues[numBricks - numBricksToTake];
        const auto otherPlayersMaxScoreWithRemainingBricks = findMaxScoreForNumBricks(numBricks - numBricksToTake, brickValues, sumOfFirstNBrickValues, maxScoreForNumBricksLookup);
        const auto sumOfRemainingBricks = sumOfFirstNBrickValues[numBricks - numBricksToTake];
        const auto maxScoreForTakingThisNumBricks = sumOfBricksTaken + (sumOfRemainingBricks - otherPlayersMaxScoreWithRemainingBricks);

        maxScore = max(maxScore, maxScoreForTakingThisNumBricks);
    } 

    maxScoreForNumBricksLookup[numBricks] = maxScore;

    return maxScore;
}

int main()
{
    // Nice and easy: let maxScoreForNumBricks(k) be the maximum score that can be achieved for the current player if only the first k
    // bricks remain.  Assume the current player takes x bricks (so his current score is the sum of the values of these x bricks); then 
    // the other player will get the max score for the remaining k - x bricks.  Since this is a zero-sum game, continuing the game to the end,
    // the current player will get the remaining points from the k - x bricks i.e. they get an additional [sum of values of remaining (k - x)
    // bricks - maxScoreForNumBricks(k - x)] i.e. the if the player takes x bricks, then their total score is
    //
    //   sum of values of x bricks taken + sum of values of remaining (k - x) bricks - maxScoreForNumBricks(k - x)
    //
    // His maximum possible score, then, is the maximum of this value over x = 1, 2, 3.
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int N;
        cin >> N;

        vector<int> brickValues(N);
        vector<int64_t> sumOfFirstNBrickValues;
        sumOfFirstNBrickValues.push_back(0);

        int64_t sumOfBrickValues = 0;
        for (int i = 0; i < N; i++)
        {
            cin >> brickValues[i];
        }

        // I prefer the bricks to be listed from bottom to top, thankyouverymuch :)
        reverse(brickValues.begin(), brickValues.end());

        for (int i = 0; i < N; i++)
        {
            sumOfBrickValues += brickValues[i];
            sumOfFirstNBrickValues.push_back(sumOfBrickValues);
        }

        vector<int64_t> maxScoreForNumBricksLookup(N + 1, -1);
        const auto maxScore = findMaxScoreForNumBricks(N, brickValues, sumOfFirstNBrickValues, maxScoreForNumBricksLookup);
        cout << maxScore << endl; 
    }
}
