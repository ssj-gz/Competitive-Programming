#ifndef RANDOM_UTILITIES_H
#define RANDOM_UTILITIES_H

#include <numeric>
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
    AVLTree removedIndices;

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


#endif

