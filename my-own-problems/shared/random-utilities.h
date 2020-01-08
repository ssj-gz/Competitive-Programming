#ifndef RANDOM_UTILITIES_H
#define RANDOM_UTILITIES_H

#include <numeric>

/**
 * @return a vector of \a numValues values, each of which is at least \a minValue, and
           where the sum of all values is precisely \a targetSum
 */
inline std::vector<int> chooseRandomValuesWithSum(const unsigned int numValues, const int targetSum, const int minValue)
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
    shuffle(starsAndBars.begin(), starsAndBars.end());
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

/**
 * @return a vector of \a numToChoose values randomly-chosen from \a toChooseFrom.
 */
template<typename ValueType>
std::vector<ValueType> chooseKRandomFrom(const unsigned int numToChoose, const std::vector<ValueType>& toChooseFrom)
{
    assert(numToChoose <= toChooseFrom.size());
    std::vector<ValueType> toChooseFromScrambled = toChooseFrom;
    shuffle(toChooseFromScrambled.begin(), toChooseFromScrambled.end());

    return std::vector<ValueType>(toChooseFromScrambled.begin(), toChooseFromScrambled.begin() + numToChoose);
}

#endif

