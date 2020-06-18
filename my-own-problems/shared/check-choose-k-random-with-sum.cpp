#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <cassert>
#include <numeric>

#include <sys/time.h>

using namespace std;

vector<int> chooseKWithSumExperimental(int numToChoose, int targetSum)
{
    vector<int> prelimChoices;
    prelimChoices.push_back(0);
    prelimChoices.push_back(targetSum);

    for (int i = 0; i < numToChoose - 1; i++)
    {
        prelimChoices.push_back(rand() % (targetSum + 1));
    }
    sort(prelimChoices.begin(), prelimChoices.end());

    vector<int> choices;
    for (int i = 1; i < prelimChoices.size(); i++)
    {
        choices.push_back(prelimChoices[i] - prelimChoices[i - 1]);
    }
    assert(choices.size() == numToChoose);
    assert(accumulate(choices.begin(), choices.end(), 0) == targetSum);

    return choices;
}

inline std::vector<int> chooseRandomValuesWithSum(const int numValues, const int targetSum, const int minValue = 0)
{
    // See e.g. https://en.wikipedia.org/wiki/Stars_and_bars_(combinatorics).
    const int numBars = numValues;
    const int numStars = targetSum - minValue * numValues;
    std::string starsAndBars;
    for (int i = 0; i < numStars; i++)
    {
        starsAndBars.push_back('*');
    }
    for (int i = 0; i < numBars - 1; i++)
    {
        starsAndBars.push_back('-');
    }
    std::random_shuffle(starsAndBars.begin(), starsAndBars.end());
    // Sentinel.
    starsAndBars.push_back('-');

    std::vector<int> values;
    int numStarsInRun = 0;
    for (const auto character : starsAndBars)
    {
        if (character == '-')
        {
            values.push_back(minValue + numStarsInRun);
            numStarsInRun = 0;
        }
        else
        {
            numStarsInRun++;
        }
    }
    assert(values.size() == numValues);
    assert(std::accumulate(values.begin(), values.end(), 0) == targetSum);
    assert(*min_element(values.begin(), values.end()) >= minValue);

    return values;
}

inline std::vector<int> chooseRandomValuesWithSum2(const int64_t numValues, const int64_t targetSum, const int64_t minValue = 0)
{
    // See e.g. https://en.wikipedia.org/wiki/Stars_and_bars_(combinatorics).
    // This simulates construction of the stars and bars list and so is memory-efficient.
    int64_t numBars = numValues - 1; // The "-1" is part of a mechanism to add a "fake" sentinel bar at the "end" of the simulated list.
    int64_t numStars = targetSum - minValue * numValues;

    std::vector<int64_t> values;
    int64_t numStarsInRun = 0;
    int numIterations = 0;
    while (static_cast<int64_t>(values.size()) < numValues)
    {
        if (numIterations % 1'000'000 == 0)
        {
            // Give a quick progress update, as this function can be very slow.
            //std::cout << "numBars: " << numBars << " numStars: " << numStars << std::endl;
        }
        const bool isBar = (numStars == 0) // This clause is the other part of the mechanism to add a "fake" sentinel bar at the "end" of the simulated list.
                           || ((rand() % (numBars + numStars)) >= numStars);
        if (isBar)
        {
            values.push_back(minValue + numStarsInRun);
            numStarsInRun = 0;
            numBars--;
        }
        else
        {
            numStarsInRun++;
            numStars--;
        }
        numIterations++;
    }
    assert(values.size() == numValues);
    assert(std::accumulate(values.begin(), values.end(), 0) == targetSum);
    assert(*min_element(values.begin(), values.end()) >= minValue);

    return vector<int>(values.begin(), values.end());
}

int main()
{
    struct timeval time;
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

    const auto numToChoose = 5;

    map<vector<int>, int> numOfChoice;
    for (int i = 0; i < 100'000'000; i++)
    {
        const auto choice = chooseKWithSumExperimental(numToChoose, 10);
        //const auto choice = chooseRandomValuesWithSum(numToChoose, 10);
        //const auto choice = chooseRandomValuesWithSum2(numToChoose, 10);
        for (const auto x : choice)
        {
            cout << x << " ";
        }
        cout << endl;

        numOfChoice[choice]++;

        if ((i % 1'000) == 0)
        {
            const auto totalNumChosen = i + 1;
            const auto numPossibleChoices = numOfChoice.size();
            const auto averageTimesChosen = static_cast<double>(totalNumChosen) / numPossibleChoices;
            double largestPercentageErrorVsAverage = 0.0;
            for (const auto& [choice, numTimesChosen] : numOfChoice)
            {
                const auto percentageErrorVsAverage = 100.0 - min<double>(averageTimesChosen, numTimesChosen) / max<double>(averageTimesChosen, numTimesChosen) * 100.0;
                largestPercentageErrorVsAverage = max(largestPercentageErrorVsAverage, percentageErrorVsAverage);
            }
            // With a uniform way of choosing numToChoose numbers with a fixed sum, largestPercentageErrorVsAverage should
            // trend down to 0.
            cout << "largestPercentageErrorVsAverage: " << largestPercentageErrorVsAverage << endl;
            //for (const auto& [choice, numTimesChosen] : numOfChoice)
            //{
                //cout << numTimesChosen << endl;
            //}
        }
    }

}

