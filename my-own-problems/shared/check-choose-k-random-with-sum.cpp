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

int64_t ncr(int64_t n, int64_t r)
{
    if (n < r)
        swap(r, n);
    if (r == 0)
        return 1;
    if (n == 0)
        return 0;
    if (r > n / 2)
        r = n - r;
    return ncr( n-1, r-1 ) + ncr( n-1, r );
}

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

#if 1
    const auto numToChoose = 5;
    const auto targetSum = 70;
    const auto minValue = 5;
#else
    const auto numToChoose = 7;
    const auto targetSum = 29;
    const auto minValue = 4;
#endif

    const auto numPossibleChoices = ncr(targetSum - minValue * (numToChoose - 0) + (numToChoose - 1), (numToChoose - 1));

    unordered_map<vector<int64_t>, int> numOfChoice;
    numOfChoice.reserve(100'000'000);
    //map<vector<int64_t>, int> numOfChoice;
    //map<vector<int>, int> numOfChoice;
    map<int, int> blee;
    for (int i = 0; i < 1'000'000'000; i++)
    {
        //const auto choice = chooseKWithSumExperimental(numToChoose, targetSum, minValue);
        //const auto choice = chooseRandomValuesWithSum(numToChoose, targetSum, minValue);
        //const auto choice = chooseRandomValuesWithSum2(numToChoose, targetSum, minValue);
        const auto choice = chooseRandomValuesWithSum3(numToChoose, targetSum, minValue);
#if 0
        for (const auto x : choice)
        {
            cout << x << " ";
        }
        cout << endl;
#endif

        const auto blah = numOfChoice[choice]++;
        if (blah != 0)
        {
            blee[blah]--;
            if (blee[blah] == 0)
                blee.erase(blee.find(blah));
        }
        blee[blah + 1]++;
#if 0
        cout << "blah: " << blah << endl;
        cout << "blee: " << endl;
        for (const auto x : blee)
        {
            cout << " " << x.first << "," << x.second << endl;
        }
#endif

        if ((i % 100'000) == 0)
        {
            cout << "i: " << i << endl;
#if 1
            const auto totalNumChosen = i + 1;
            //const auto numPossibleChoices = numOfChoice.size();
            //const auto numPossibleChoices = 15890700;
            const auto averageTimesChosen = static_cast<double>(totalNumChosen) / numPossibleChoices;
            double largestPercentageErrorVsAverage = 0.0;
#if 0
            for (const auto& [choice, numTimesChosen] : numOfChoice)
            {
                const auto percentageErrorVsAverage = 100.0 - min<double>(averageTimesChosen, numTimesChosen) / max<double>(averageTimesChosen, numTimesChosen) * 100.0;
                largestPercentageErrorVsAverage = max(largestPercentageErrorVsAverage, percentageErrorVsAverage);
            }
#endif
#if 1
            {
                const auto numTimesChosen = blee.begin()->first;
                const auto percentageErrorVsAverage = 100.0 - min<double>(averageTimesChosen, numTimesChosen) / max<double>(averageTimesChosen, numTimesChosen) * 100.0;
                largestPercentageErrorVsAverage = max(largestPercentageErrorVsAverage, percentageErrorVsAverage);
            }
            {
                const auto numTimesChosen = std::prev(blee.end())->first;
                const auto percentageErrorVsAverage = 100.0 - min<double>(averageTimesChosen, numTimesChosen) / max<double>(averageTimesChosen, numTimesChosen) * 100.0;
                largestPercentageErrorVsAverage = max(largestPercentageErrorVsAverage, percentageErrorVsAverage);
            }
#endif
            // With a uniform way of choosing numToChoose numbers with a fixed sum, largestPercentageErrorVsAverage should
            // trend down to 0.
            cout << "largestPercentageErrorVsAverage: " << largestPercentageErrorVsAverage << " averageTimesChosen:" << averageTimesChosen << " num chosen: " << (i + 1) << " numOfChoice.size(): " << numOfChoice.size() << " numPossibleChoices: " << numPossibleChoices << endl;
#endif
            //for (const auto& [choice, numTimesChosen] : numOfChoice)
            //{
                //cout << numTimesChosen << endl;
            //}
        }
    }

}

