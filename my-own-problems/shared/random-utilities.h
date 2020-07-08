#ifndef RANDOM_UTILITIES_H
#define RANDOM_UTILITIES_H

#include <numeric>
#include <set>
#include <map>
#include <algorithm>
#include <avl-tree.h>

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
 * @return a vector of \a numValues values, each of which is at least \a minValue, and
 *         where the sum of all values is precisely \a targetSum
 *
 * Takes O(numValues + targetSum) steps and O(1) memory.
 */
inline std::vector<int64_t> chooseRandomValuesWithSum2(const int64_t numValues, const int64_t targetSum, const int64_t minValue)
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
                           || (rnd.next(numBars + numStars) >= numStars);
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
    assert(static_cast<int64_t>(values.size()) == numValues);
    assert(std::accumulate(values.begin(), values.end(), 0) == targetSum);
    assert(*min_element(values.begin(), values.end()) >= minValue);

    return values;
}

std::vector<int64_t> chooseKRandomIndicesFrom(int numToChoose, int64_t numToChooseFrom);

/**
 * @return a vector of \a numValues values, each of which is at least \a minValue, and
 *         where the sum of all values is precisely \a targetSum
 *
 * Takes O(numValues x log(numValues)) steps and O(numValues) memory i.e. irrespective of targetSum
 */
inline std::vector<int64_t> chooseRandomValuesWithSum3(const int64_t numValues, const int64_t targetSum, const int64_t minValue)
{
    assert(numValues > 0 && "Can't make a sum with no values!");
    const auto adjustedTargetSum = targetSum - numValues * minValue  
        + (numValues - 1); // The distance between the numValues - 1 chosen indices will be at least 1, so we'll remove 1 each time; this compensates for that.
    auto indices  = chooseKRandomIndicesFrom(numValues - 1, adjustedTargetSum);
    indices.insert(indices.begin(), -1); // Sentinel (left)
    indices.push_back(adjustedTargetSum); // Sentinel (right)
    assert(is_sorted(indices.begin(), indices.end()));

    std::vector<int64_t> chosenValues;
    for (size_t i = 1; i < indices.size(); i++)
    {
        assert(indices[i] - indices[i - 1] >= 0);
        chosenValues.push_back(indices[i] - indices[i - 1] + minValue 
                - 1); // Since (indices[i] - indices[i - 1] >= 1), we must subtract 1 else all chosen numbers will always be >= minValue + 1!
    }

    assert(static_cast<int64_t>(chosenValues.size()) == numValues);
    assert(std::accumulate(chosenValues.begin(), chosenValues.end(), static_cast<int64_t>(0)) == targetSum);
    assert(*min_element(chosenValues.begin(), chosenValues.end()) >= minValue);

    return chosenValues;
}

/**
 * @return a vector of \a numToChoose values randomly-chosen from \a toChooseFrom.
 * Deprecated.  Use chooseKRandomFromNG instead - it's faster, but will likely
 * result in difference choices, so it's not a "drop-in" replacement.
 */
template<typename ValueType>
std::vector<ValueType> chooseKRandomFrom(const unsigned int numToChoose, const std::vector<ValueType>& toChooseFrom)
{
    std::cout << "chooseKRandomFrom: numToChoose: " << numToChoose << " toChooseFrom: " << toChooseFrom.size() << std::endl;
    assert(numToChoose <= toChooseFrom.size());
    std::vector<ValueType> toChooseFromScrambled = toChooseFrom;
    shuffle(toChooseFromScrambled.begin(), toChooseFromScrambled.end());

    return std::vector<ValueType>(toChooseFromScrambled.begin(), toChooseFromScrambled.begin() + numToChoose);
}

/**
 * Returns a choice of \a numToChoose distinct indices, where each index
 * is in the range 0 <= index < numToChoose.
 *
 * The choice is represented as a strictly-increasing vector of indices.
 *
 * Each possible choice of \a numToChoose distinct indices from \a numToChooseFrom
 * has the same probability of being returned.
 *
 * The time complexity is O(N log N), where N = \a numToChoose i.e. it is independent of
 * the value of \a numToChooseFrom.  The space complexity is O(N).
 */
std::vector<int64_t> chooseKRandomIndicesFrom(int numToChoose, int64_t numToChooseFrom)
{
    assert(numToChoose <= numToChooseFrom);
    assert(numToChoose >= 0);
    Internal::AVLTree removedIndices;

    // Create the sequence of indices to choose - here, we assume that we actually remove
    // the chosen index after choosing it.
    std::vector<int64_t> chosenIndexInRemaining;
    {
        int64_t numRemaining = numToChooseFrom;
        for (int i = 0; i < numToChoose; i++)
        {
            chosenIndexInRemaining.push_back(rnd.next(numRemaining));
            numRemaining--;
        }
    }

    // Simulate the removal of the indices in chosenIndexInRemaining and remap each chosen index
    // to account for the removal of prior indices.
    // The result - "chosenIndices" - is a strictly increasing list of indices i, each in the range
    // 0 <= i < numToChooseFrom.
    std::vector<int64_t> chosenIndices;
    for (const auto nthOfRemainingToChoose : chosenIndexInRemaining)
    {
        // Be optimistic and give remappedIndex the smallest possible value:
        // we'll correct our optimism as we go along :)
        int64_t remappedIndex = nthOfRemainingToChoose;
        auto currentNode = removedIndices.root();
        int64_t numRemovedUpToCurrentNodeIndexOffset = 0;
        while (currentNode)
        {
            const int64_t indexOfCurrentNode = currentNode->value;
            const int numDescendantsLeftSubChild = (currentNode->leftChild ? currentNode->leftChild->numDescendants : 0);
            const int64_t numRemovedUpToCurrentNodeIndex = numRemovedUpToCurrentNodeIndexOffset + numDescendantsLeftSubChild;
            const int64_t numFreeUpToCurrentNodeIndex = indexOfCurrentNode - numRemovedUpToCurrentNodeIndex;
            if (numFreeUpToCurrentNodeIndex >= nthOfRemainingToChoose + 1)
            {
                // We've overshot; the required remappedIndex is to the left of indexOfCurrentNode; "recurse"
                // into left child.
                currentNode = currentNode->leftChild;
            }
            else
            {
                // Again, be optimistic about remappedIndex - we'll correct it as we go along.
                remappedIndex = std::max(remappedIndex, indexOfCurrentNode + (nthOfRemainingToChoose - numFreeUpToCurrentNodeIndex) + 1);
                // Required index is to the right of here; "recurse" into the right child.
                // In doing this, we're "forgetting" all the Removed indices to the left of
                // currentNode - record them in numRemovedUpToCurrentNodeIndexOffset.
                numRemovedUpToCurrentNodeIndexOffset += 1 + // currentNode is Removed ...
                                                        numDescendantsLeftSubChild; // ... as are all the indices in currentNode->leftChild.
                currentNode = currentNode->rightChild;
            }

        }
        // We've successfully found the index in the original array; now mark it as Removed.
        assert(0 <= remappedIndex && remappedIndex < numToChooseFrom);
        removedIndices.insertValue(remappedIndex);
        chosenIndices.push_back(remappedIndex);
    }

    std::sort(chosenIndices.begin(), chosenIndices.end());
    assert(static_cast<int>(chosenIndices.size()) == numToChoose);
    assert(static_cast<int>(std::set<int>(chosenIndices.begin(), chosenIndices.end()).size()) == numToChoose);

    return chosenIndices;
}

/**
 * @return a vector of \a numToChoose values randomly-chosen from \a toChooseFrom.
 * The chosen values will be "distinct" in that the indices chosen from toChooseFrom are
 * distinct i.e. the chosen values will definitely be distinct if all items in toChooseFrom are
 * distinct, but may not be distinct otherwise.
 */
template<typename ValueType>
std::vector<ValueType> chooseKRandomFromNG(const unsigned int numToChoose, const std::vector<ValueType>& toChooseFrom)
{
    std::vector<ValueType> chosenValues;
    chosenValues.reserve(numToChoose);
    for (const int64_t index : chooseKRandomIndicesFrom(numToChoose, toChooseFrom.size()))
    {
        chosenValues.push_back(toChooseFrom[index]);
    }
    assert(chosenValues.size() == numToChoose);

    return chosenValues;
}

template <typename T>
std::vector<T> chooseWithWeighting(const std::map<T, double>& valueWeight, const int numValuesToChoose)
{
    struct WeightAndValue
    {
        double weight = 0.0;
        T value;
    };
    std::vector<WeightAndValue> cumulativeWeightsAndValue;
    double totalWeight = 0.0;
    for (const auto& [value, weight] : valueWeight)
    {
        cumulativeWeightsAndValue.push_back({totalWeight, value});
        totalWeight += weight;
    }
    assert(totalWeight == 100.0);
    sort(cumulativeWeightsAndValue.begin(), cumulativeWeightsAndValue.end(), 
         [](const auto& lhs, const auto& rhs)
         {
             return lhs.weight > rhs.weight;
         });

    std::vector<T> chosenValues;
    const int numAvailableValues = cumulativeWeightsAndValue.size();
    for (int i = 0; i < numValuesToChoose; i++)
    {
        const double randPercent = rnd.next(0.0, 100.0);
        bool found = false;
        for (int chosenIndex = 0; chosenIndex < numAvailableValues; chosenIndex++)
        {
            if (randPercent >= cumulativeWeightsAndValue[chosenIndex].weight)
            {
                chosenValues.push_back(cumulativeWeightsAndValue[chosenIndex].value);
                found = true;
                break;
            }
        }
        assert(found);
    }
    assert(static_cast<int>(chosenValues.size()) == numValuesToChoose);
    return chosenValues;
}

/**
 * Deprecated - use WeightedChooser2 for new tests
 */
template <typename T>
class WeightedChooser
{
    public:
        WeightedChooser(const std::map<T, double>& valueWeight)
        {
            m_totalWeight = 0.0;
            for (const auto& [value, weight] : valueWeight)
            {
                m_totalWeight += weight;
                m_cumulativeWeightsAndValue.push_back({m_totalWeight, value});
            }
        }
        T nextValue()
        {
            const auto randCumulativeWeight = rnd.next(0.0, m_totalWeight);
            auto firstGEIter = std::lower_bound(m_cumulativeWeightsAndValue.begin(), m_cumulativeWeightsAndValue.end(), randCumulativeWeight, [](const auto& cumulativeWeightAndValue, const auto randCumulativeWeight)
                    {
                        return cumulativeWeightAndValue.weight < randCumulativeWeight;
                    });
            assert(firstGEIter != m_cumulativeWeightsAndValue.end());
            assert(firstGEIter->weight >= randCumulativeWeight);
            return firstGEIter->value;
        }
    private:
        struct WeightAndValue
        {
            double weight = 0.0;
            T value;
        };
        double m_totalWeight = 0.0;
        std::vector<WeightAndValue> m_cumulativeWeightsAndValue;
};

template <typename T>
class WeightedChooser2
{
    public:
        WeightedChooser2(const std::map<T, double>& valueWeight)
        {
            m_totalWeight = 0.0;
            for (const auto& [value, weight] : valueWeight)
            {
                if (weight == 0) 
                {
                    // If we values with 0 probability weighting, I *think* there's a chance that they
                    // will still end up being chosen; instead, excluse them from consideration entirely.
                    continue;
                }
                m_totalWeight += weight;
                m_cumulativeWeightsAndValue.push_back({m_totalWeight, value});
            }
        }
        T nextValue()
        {
            const auto randCumulativeWeight = rnd.next(0.0, m_totalWeight);
            auto firstGEIter = std::lower_bound(m_cumulativeWeightsAndValue.begin(), m_cumulativeWeightsAndValue.end(), randCumulativeWeight, [](const auto& cumulativeWeightAndValue, const auto randCumulativeWeight)
                    {
                        return cumulativeWeightAndValue.weight < randCumulativeWeight;
                    });
            assert(firstGEIter != m_cumulativeWeightsAndValue.end());
            assert(firstGEIter->weight >= randCumulativeWeight);
            return firstGEIter->value;
        }
    private:
        struct WeightAndValue
        {
            double weight = 0.0;
            T value;
        };
        double m_totalWeight = 0.0;
        std::vector<WeightAndValue> m_cumulativeWeightsAndValue;
};



#endif

