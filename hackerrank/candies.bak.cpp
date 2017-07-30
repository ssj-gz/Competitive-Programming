// Simon St James (ssjgz) 2017-07-19
//#define SUBMISSION
#ifndef SUBMISSION
#undef NDEBUG
#endif
#include <iostream>
#include <vector>
#include <limits>
#include <cassert>

using namespace std;

int64_t bruteForce(const vector<int>& ratings)
{
    vector<int> candies(ratings.size());
    candies[0] = 1;
    int previousRating = ratings[0];
    for (int i = 1; i < ratings.size(); i++)
    {
        const int rating = ratings[i];
        if (rating <= previousRating)
        {
            candies[i] = 1;
        }
        else
        {
            candies[i] = candies[i - 1] + 1;
        }

        for (int j = i; j >= 1; j--)
        {
            if (ratings[j - 1] > ratings[j] && candies[j - 1] <= candies[j])
            {
                assert(candies[j] - candies[j - 1] <= 1);
                candies[j - 1]++;
            }
        }

        previousRating = rating;
    }

    int64_t totalCandies = 0;
    for (const auto& candy : candies)
    {
        totalCandies += candy;
        cout << candy << " ";
    }
    cout << endl;
    return totalCandies;
}

int main()
{
    int N;
    cin >> N;

    int64_t totalCandies = 0;
    int previousRating = std::numeric_limits<int>::min();
    bool isOnIncreasingLadder = false;
    bool isOnDescendingLadder = false;
    int descendingLadderLength = 1;
    int lastCandyOnIncreasingLadder = -1;
    int descendingLadderBeginIndex = -1;
    int descendingLadderBeginRating = -1;
    int descendingLadderBeforeBeginRating = -1;
    int descendingLadderBeforeBeginCandies = -1;
    bool haveIncorporatedPriorToDescendingLadder = false;
#define BRUTEFORCE
#ifdef BRUTEFORCE
    vector<int> ratings(N);
#endif
    int lastTotalCandies = 0;
    int lastCandiesGiven = 0;
    for (int i = 0; i < N; i++)
    {
        cout << "i: " << i << endl;
        int rating;
        cin >> rating;
#ifdef BRUTEFORCE
        ratings[i] = rating;
#endif
        if (rating <= previousRating)
        {
            cout << " not greater!" << endl;
            //lastCandyOnIncreasingLadder = 1;
            isOnIncreasingLadder = false;
            if (isOnDescendingLadder)
            {
                descendingLadderLength++;
                //cout << " descendingLadderBeforeBeginCandies: " << descendingLadderBeforeBeginCandies
                if (descendingLadderLength >= descendingLadderBeforeBeginCandies)
                {
                    cout << "descendingLadderBeforeBeginRating: " << descendingLadderBeforeBeginRating << " descendingLadderBeginRating: " << descendingLadderBeginRating << endl;
                    if (descendingLadderBeforeBeginRating > descendingLadderBeginRating && !haveIncorporatedPriorToDescendingLadder)
                    {
                        cout << "increasing blah" << endl;
                        descendingLadderLength++;
                        haveIncorporatedPriorToDescendingLadder = true;
                    }
                }
            }
            else
            {
                isOnDescendingLadder = true;
                descendingLadderLength = 1;
                descendingLadderBeginRating = rating;
                descendingLadderBeforeBeginRating = previousRating;
                descendingLadderBeforeBeginCandies = lastCandiesGiven;
                haveIncorporatedPriorToDescendingLadder = false;
            }
            totalCandies += descendingLadderLength;
            
        }
        else if (rating > previousRating)
        {
            cout << " greater!" << endl;
            isOnDescendingLadder = false;
#if 1
            if (!isOnIncreasingLadder)
            {
                cout << " not on increasing ladder" << endl;
                lastCandyOnIncreasingLadder = (i == 0 ? 1 : 2);
                isOnIncreasingLadder = true;
            }
            else
            {
                cout << " was on increasing ladder" << endl;
                //totalCandies += lastCandiesGiven;
                lastCandyOnIncreasingLadder++;
            }
            totalCandies += lastCandyOnIncreasingLadder;
#endif
            //totalCandies += lastCandiesGiven + 1;
        }
        lastCandiesGiven = (totalCandies - lastTotalCandies);
        cout << "i: " << i << " totalCandies: " << totalCandies << " given this time: " << lastCandiesGiven << endl;
        previousRating = rating;
        lastTotalCandies = totalCandies;

    }
    assert(cin);

    cout << "ratings.size() " << ratings.size() << endl;

#ifdef BRUTEFORCE
    const int64_t bruteForceResult = bruteForce(ratings);
    cout << "bruteForceResult: " << bruteForceResult << endl;
    cout << "optimised result: ";
#endif
    cout << totalCandies << endl;
#ifdef BRUTEFORCE
    assert(totalCandies == bruteForceResult);
#endif
}

