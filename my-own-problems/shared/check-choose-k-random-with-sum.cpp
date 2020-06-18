#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cassert>
#include <numeric>

#include "testlib.h"
#include "random-utilities.h"

#include <sys/time.h>

using namespace std;

#if 0
vector<int> chooseKWithSumExperimental(int numToChoose, int targetSum)
{
    string blah(targetSum, ' ');
    for (int i = 0; i < numToChoose - 1; i++)
    {
        const auto indexInRemaining = rand() % (targetSum - i);
        //cout << "i: " << i << " indexInRemaining: " << indexInRemaining << endl;
        //cout << "blah: >" << blah << "<" << endl;
        int numEmptySpacesSeen = 0;
        bool added = false;
        for (int j = 0; j < blah.size(); j++)
        {
            if (blah[j] == ' ')
            {
                if (numEmptySpacesSeen == indexInRemaining)
                {
                    blah[j] = 'X';
                    added = true;
                    break;
                }
                numEmptySpacesSeen++;
            }
        }
        assert(added);
    }


    vector<int> prelimChoices;
    prelimChoices.push_back(0);
    prelimChoices.push_back(targetSum);
    for (int i = 0; i < blah.size(); i++)
    {
        if (blah[i] == 'X')
            prelimChoices.push_back(i);
    }
    //cout << "blah: " << blah << endl;

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
#endif

namespace std
{
    template<>
    struct hash<vector<int64_t>>
    {
        size_t operator()(const vector<int64_t>& toHash) const
        {
            uint64_t hash = 0;
            for (const auto x : toHash)
            {
                hash = combineHashes(hash, x);
            }
            return hash;
        }
        uint64_t combineHashes(uint64_t hash1, const uint64_t hash2) const
        {
            // Largely arbitrary "hash combine" function, taken from
            //
            //   https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
            //
            hash1 ^= hash2 + 0x9e3779b9 + (hash1<<6) + (hash1>>2);
            return hash1;
        }

    };
};


int main()
{
    struct timeval time;
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

    const auto numToChoose = 6;

    unordered_map<vector<int64_t>, int> numOfChoice;
    numOfChoice.reserve(10'000'000);
    //map<vector<int>, int> numOfChoice;
    for (int i = 0; i < 100'000'000; i++)
    {
        //const auto choice = chooseKWithSumExperimental(numToChoose, 30, 0);
        //const auto choice = chooseRandomValuesWithSum(numToChoose, 100, 4);
        //const auto choice = chooseRandomValuesWithSum2(numToChoose, 30, 0);
        const auto choice = chooseRandomValuesWithSum3(numToChoose, 100, 7);
#if 0
        for (const auto x : choice)
        {
            cout << x << " ";
        }
        cout << endl;
#endif

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
            cout << "largestPercentageErrorVsAverage: " << largestPercentageErrorVsAverage << " num chosen: " << (i + 1) << endl;
            //for (const auto& [choice, numTimesChosen] : numOfChoice)
            //{
                //cout << numTimesChosen << endl;
            //}
        }
    }

}

