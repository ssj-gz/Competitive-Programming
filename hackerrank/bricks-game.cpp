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
        int64_t sumOfBricksTaken = 0;
        for (int i = 1; i <= numBricksToTake; i++)
        {
            sumOfBricksTaken += brickValues[numBricks - i];
        }
        //cout << "numBricks: " << numBricks << " numBricksToTake: " << numBricksToTake << " sumOfBricksTaken: " << sumOfBricksTaken << endl;
        const auto otherPlayersMaxScoreWithRemainingBricks = findMaxScoreForNumBricks(numBricks - numBricksToTake, brickValues, sumOfFirstNBrickValues, maxScoreForNumBricksLookup);
        const auto maxScoreForTakingThisNumBricks = sumOfBricksTaken + (sumOfFirstNBrickValues[numBricks - numBricksToTake] - otherPlayersMaxScoreWithRemainingBricks);

        maxScore = max(maxScore, maxScoreForTakingThisNumBricks);
    } 

    maxScoreForNumBricksLookup[numBricks] = maxScore;

    return maxScore;
}

int main()
{
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
