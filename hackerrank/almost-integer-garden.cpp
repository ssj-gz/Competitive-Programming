#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <cmath>
#include <cstdio>
#include <vector>
#include <map>
#include <iostream>
#include <set>
#include <algorithm>
#include <memory>
#include <iomanip>
#include <chrono>
#include <cassert>
using namespace std;

const int numStonesInBatch = 5;
// Decimal values (with integer part removed) are multiplied by this
// and treated as int64_t to help with rounding issues when adding.
// The decimal equivalent of two such int64_ts differ by less that 10^-12
// places if their int64_t representations differ by less than 97.
// Taking an int64_t modulo decimalToIntMultiplier is the same as
// removing the integer portion from the corresponding decimal.
const int64_t decimalToIntMultiplier = 100'000'000'000'000ULL;

struct DistanceBatchSum
{
    DistanceBatchSum(int64_t sum)
        : sum{sum}
    {
    }
    int64_t sum;
    char choices[numStonesInBatch];
};

bool isSquare(int n)
{
    const int nearestIntegerToSqrt = static_cast<int>(sqrt(n) + 0.5);
    return (nearestIntegerToSqrt * nearestIntegerToSqrt == n);
}

bool operator<(const DistanceBatchSum& lhs, const DistanceBatchSum& rhs)
{
    return lhs.sum < rhs.sum;
}

vector<DistanceBatchSum> generateBatchDistanceSums(const vector<int64_t>& stoneDistances)
{
    int64_t currentBatchSum = 0;
    const int numChoices = numStonesInBatch;
    const int numToChooseFrom = stoneDistances.size();
    vector<DistanceBatchSum> batchDistanceSums;
    vector<int> choicePointer(numChoices, 0);
    for (int i = 0; i < numChoices; i++)
    {
        currentBatchSum += stoneDistances[choicePointer[i]];
    }
    batchDistanceSums.reserve(15'000'000UL);
    while (true)
    {
        int choiceIndex = 0;
        while (choiceIndex < numChoices && choicePointer[choiceIndex] == numToChooseFrom - 1)
        {
            choiceIndex++;
        }
        if (choiceIndex == numChoices)
            break;
        currentBatchSum -= stoneDistances[choicePointer[choiceIndex]];
        choicePointer[choiceIndex]++;
        currentBatchSum += stoneDistances[choicePointer[choiceIndex]];
        for (int i = 0; i < choiceIndex; i++)
        {
            currentBatchSum -= stoneDistances[choicePointer[i]];
            choicePointer[i] = choicePointer[choiceIndex];
            currentBatchSum += stoneDistances[choicePointer[i]];
        }
        const int64_t batchSumWithIntegerPartRemoved = currentBatchSum % decimalToIntMultiplier;
        DistanceBatchSum distanceBatchSum = DistanceBatchSum(batchSumWithIntegerPartRemoved);
        distanceBatchSum.choices[0] = choicePointer[0];
        distanceBatchSum.choices[1] = choicePointer[1];
        distanceBatchSum.choices[2] = choicePointer[2];
        distanceBatchSum.choices[3] = choicePointer[3];
        distanceBatchSum.choices[4] = choicePointer[4];
        batchDistanceSums.push_back(distanceBatchSum);
    }
    sort(batchDistanceSums.begin(), batchDistanceSums.end(), [](const DistanceBatchSum& lhs, const DistanceBatchSum& rhs)
            {
            return lhs.sum < rhs.sum;
            });
    return batchDistanceSums;
}

double decimalPortionOfDistance(int x, int y)
{
    double dummyIntPart;
    const double distance = sqrt(x * x + y * y);
    const double distanceDecimal = modf(distance, &dummyIntPart);
    return distanceDecimal;
}

vector<std::pair<int, int>> findFullStonePlacement(int initialStonex, int initialStoneY)
{
    vector<int64_t> stoneDistances;
    map<int64_t, vector<pair<int, int>>> stonesWithDistance;
    for (int x = -12; x <= 12; x++)
    {
        for (int y = -12; y <= 12; y++)
        {
            if (isSquare(x * x + y * y))
                continue;
            const int64_t stoneDistance = decimalPortionOfDistance(x, y) * decimalToIntMultiplier;
            stoneDistances.push_back(stoneDistance);
            stonesWithDistance[stoneDistance].push_back({x, y});
        }
    }
    sort(stoneDistances.begin(), stoneDistances.end());
    stoneDistances.erase(unique(stoneDistances.begin(), stoneDistances.end()), stoneDistances.end());
    reverse(stoneDistances.begin(), stoneDistances.end()); // Dunno why, but this makes us converge on solutions slightly faster XD

    const vector<DistanceBatchSum> batchDistanceSums = generateBatchDistanceSums(stoneDistances);
    const int64_t stoneDistance = decimalPortionOfDistance(initialStonex, initialStoneY) * decimalToIntMultiplier;
    for (const int64_t secondStoneDistance : stoneDistances)
    {
        const int64_t firstTwoStonesSum = stoneDistance + secondStoneDistance;
        for (int addOne = 0; addOne <= 1; addOne++)
        {
            // As mentioned, with the format the distances are expressed in, the decimal equivalents of the distances differ by less that 10^-12
            // when the distances differ by 97 or less.
            const int64_t remainderStonesTargetSumUpper = decimalToIntMultiplier - firstTwoStonesSum + 97 + addOne * decimalToIntMultiplier;
            const int64_t remainderStonesTargetSumLower = decimalToIntMultiplier - firstTwoStonesSum - 97 + addOne * decimalToIntMultiplier;

            auto lowPointer = batchDistanceSums.begin();
            auto lastHighPointer = upper_bound(batchDistanceSums.begin(), batchDistanceSums.end(), remainderStonesTargetSumUpper - lowPointer->sum);
            if (lastHighPointer == batchDistanceSums.end())
                lastHighPointer--;
            while (lowPointer <= batchDistanceSums.begin() + batchDistanceSums.size() / 2)
            {
                auto highPointer = lastHighPointer;
                if (highPointer < lowPointer || highPointer == batchDistanceSums.begin())
                {
                    // Can't find any solutions from here.
                    break;
                }
                // Maintain beginning of loop invariant: highPointer is the first value such that the total batch sum exceeds our target range.
                const int64_t highPointerUpperCutoff = remainderStonesTargetSumUpper - lowPointer->sum; // Cache this as a micro-optimisation.
                if (highPointer >= lowPointer && (highPointer - 1)->sum >= highPointerUpperCutoff)
                {
                    highPointer--;
                    while (highPointer != batchDistanceSums.begin() && ((highPointer - 1)->sum) >= highPointerUpperCutoff)
                    {
                        highPointer--;
                    }
                }
                lastHighPointer = highPointer; // Make a note of this - it will be a good performance-saving hint for the next round.
                // Decrease high pointer until total batch sum is less than our target range (or we hit our target).
                const int64_t highPointerLowerCutoff = remainderStonesTargetSumLower - lowPointer->sum; // Cache this as a micro-optimisation.
                while (highPointer >= lowPointer && (highPointer->sum)  >= highPointerLowerCutoff)
                {
                    const int64_t totalSum = (lowPointer->sum + highPointer->sum);
                    if (totalSum >= remainderStonesTargetSumLower && totalSum <= remainderStonesTargetSumUpper)
                    {

                        // Make sure we haven't used the same stone twice (i.e. - the same distance more times than there are stones with that distance!)
                        bool isValidPlacement = true;
                        map<int64_t, vector<pair<int, int>>> stonesWithDistanceCopy(stonesWithDistance);
                        vector<pair<int, int>> stonePlacement;
                        stonesWithDistanceCopy[stoneDistance].pop_back(); // Imagine we added the first stone - the output format requires that we not actually output it!
                        stonePlacement.push_back(stonesWithDistanceCopy[secondStoneDistance].back());
                        stonesWithDistanceCopy[secondStoneDistance].pop_back();
                        int64_t stoneDistancesFromBothBatches[2 * numStonesInBatch];
                        int i = 0;
                        for (const auto stoneDistanceIndex : lowPointer->choices)
                        {
                            stoneDistancesFromBothBatches[i] = stoneDistances[stoneDistanceIndex];
                            i++;
                        }
                        for (const auto stoneDistanceIndex : highPointer->choices)
                        {
                            stoneDistancesFromBothBatches[i] = stoneDistances[stoneDistanceIndex];
                            i++;
                        }
                        for (const auto stoneDistance : stoneDistancesFromBothBatches)
                        {
                            if (stonesWithDistanceCopy[stoneDistance].empty())
                            {
                                isValidPlacement = false;
                                break;
                            }
                            stonePlacement.push_back(stonesWithDistanceCopy[stoneDistance].back());
                            stonesWithDistanceCopy[stoneDistance].pop_back();
                        }

                        if (isValidPlacement)
                        {
                            return stonePlacement;
                        }
                    }
                    highPointer--;
                }
                lowPointer++;
            }
        }
    }
    assert(false && "Should always be able  to find a valid placement!");
    return vector<std::pair<int, int>>();
}

int main() {
    // Interesting one :) Firstly, as a general strategy note: -12 <= x <= 12 and -12 <= y <= 12 gives (even
    // if we are naive) a very small amount of possible testcases (625).  By ignoring symmetries and reflections,
    // we can reduce this still further: in fact, given x & y in this range, the number of *distinct* stone
    // distances (see stoneDistances array, above), comes out as just 68.  This means that this problem is amenable
    // to a "lookup table" approach, where we can pre-compute a lookup table of solutions for  each stone distance,
    // and then answer the test cases essentially instantaneously just by looking up the solution.
    // Needless to say, I consider this method cheating, and won't be doing that - although I *did* compute the 
    // lookup table as an intermediate step towards getting a proper solution, so it's worth describing how it all works! 
    // A naive approach to building this lookup table would probably take only a few days to run, based on my calculations:
    // we essentially want to be able to choose - with repetitions - 12 stones from 68 stone distances, and then
    // prune down any "invalid" choices (where we pick more of a given stone distance that there are stones with that
    // distance).  A little thought shows that we can break this down a bit further - choosing 12 stones from 68
    // is the same as choosing 6 stones, *twice*.  If we built up a table of all sums we could get from choosing 6 stones,
    // we'd probably be able to combine them in some way to generate all sums we could get with 12.
    // In fact, we could do the following: for each sum generated, cut off the integer part i.e. if a sum is 12.3456,
    // reduce it to .3456 - since we only care about getting close to any integer, the integer part is of no use.
    // Then sort the list of sums, again  minus their integer parts.  We can get a sum of 12 differing
    // from its nearest integer by less than 10^-12 if and only if we can find a pair from our choice of 6 table
    // whose sum differs from 1 by less than 10^-12 e.g. imagine we can generate 17.456 using 6 choices, and we can generate
    // 12.544 using 6 choices: stripping off the integer parts gives .456 and .544, and their sum : "1", differs from 1
    // by less than 10^-12 (obviously!) and we see that indeed 17.456 + 12.544 = 30 - a sum that can be built from 12 choices -
    // differs from its nearest integer by less than 0.  We'd then need to verify that our combined choice is still "valid" according
    // to the criteria described earlier, so when generating our list of sums from 6 choices, we'd have to store the
    // list of 6 choices we used to get each sum so that we can check validity.
    // Finding such a pair that sums close to 1 is easy and fast if we use the two-pointer technique!
    // So: how quickly could we generate a lookup table this way? On my machine, it took about 30 seconds with somewhere around
    // 3-4GB RAM usage - far too much for a hackerrank solution!
    // What about answering the question for just the given stone, as is the case in this problem? Well, a choice of 12, incorporating
    // this given stone, consists of choosing one other stone, and then *two lots of 5 choices*.  Can we use the "6-choice" approach
    // to try and solve this? Yes, we can!
    // So we're given one stone (== stone distance).  We can then choose the second stone distance from any of the stoneDistances.
    // Sum these two distances, subtract the result from the next highest integer and call the result x (note that 0 <= x <= 1): 
    // we'll have a choice of 12 if we can find two pairs of sums from the 5-choice lookup tables that sum to either:
    //   x; or
    //   x + 1
    // and if the choice of 12 is valid (easily checked).
    // For example, assume the given stone distance is 12.348 and the second stone distance is 11.765.  Their sum is 24.113.  Subtract
    // from the next highest integer(25) and we have 0.887.  If the 5-choice sums included e.g. 0.622 and 0.265, then these sum
    // to 0.887, and the full sum would give a sum that was an almost integer.  However,  0.974 and 0.913 would also give one, 
    // but these sum to 1.887 - this is why we check against both x and x + 1.  We can't generate x + 2, or higher, so don't need to
    // check those.
    // And that's about it: generateBatchDistanceSums() generates a sorted (by sum) list of DistanceBatchSums, each of which
    // is a sum, with the integer part removed, that can be formed by picking 5 stones, and a list of the 5 choices we made.
    // findFullStonePlacement then takes our given stone, goes through all possibilities of second stone, and then sees if it can
    // find a pair of 5-choices (using the two-pointer technique) that when added to the distances of the first and second stone
    // is will give us an almost-integer.
    // There's a few twists along the way, though - double's are used only in the initial sqrt call, after which they are transformed
    // to a kind of fixed-point representation using int64_t - this may or may not have speed advantages, but I mainly did it for
    // accuracy when adding!

#if 1
    int x, y;
    cin >> x >> y;
    const auto stonePlacement = findFullStonePlacement(x, y);

    for (const auto& stone : stonePlacement)
    {
        cout << stone.first << " " << stone.second << endl;
    }
#else
    int numDistinctDistances = 0;
    std::set<int> distanceSquareds;
    for (int x = 0; x <= 12; x++)
    {
        for (int y = x; y <= 12; y++)
        {
            const auto distanceSquared = x * x + y * y;
            if ( isSquare(distanceSquared) )
                continue;
            if ( distanceSquareds.find(distanceSquared) != distanceSquareds.end())
                continue;
            distanceSquareds.insert(distanceSquared);
            numDistinctDistances++;
            cout << "x: " << x << " y: " << y << " distance squared: " << distanceSquared << endl;
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            const auto stonePlacement = findFullStonePlacement(x, y);
            std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
            std::cout << "Time taken (seconds) = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0 <<std::endl;
            long double sum = sqrt(x * x + y * y);
            for (const auto& stone : stonePlacement)
            {
                sum += sqrt(stone.first * stone.first + stone.second * stone.second);
            }
            std::cout << std::setprecision(std::numeric_limits<long double>::digits10 + 1) << "sum: " << sum << std::endl;
            //std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono
        }
    }
    std::cout << "num distinct distances  = " << numDistinctDistances << std::endl;
#endif
}
