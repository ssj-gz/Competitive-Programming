#include <cmath>
#include <cstdio>
#include <vector>
#include <deque>
#include <array>
#include <iostream>
#include <algorithm>
#include <cassert>
using namespace std;


int main() {
    long n, d;
    cin >> n >> d;
    vector<int> expenditures(n);
    for (int i = 0; i < n; i++)
    {
        cin >> expenditures[i];
    }
    // Basic approach: since the range of expenditures is tiny (0 - 200),
    // use an incremental counting sort to keep the list of d last
    // expenditures "sorted", and find the median of these (currently
    // by brute force, which is good enough to pass all the test-cases :/).
    const int maxExpenditure = 200;
    array<int, maxExpenditure + 1> expenditureTally = {};
    // A std::list is, with the C++ Standard Library used by hackerrank, ungodly slow - a deque works nicely, though!
    deque<int> lastDExpenditures; 
    long numNotifications = 0;
    const bool dIsOdd = ((d % 2) == 1);
    for (auto expenditure : expenditures)
    {
        if (lastDExpenditures.size() >= d)
        {
            // Recompute median - TODO - this is an ugly, brute force solution:
            // a better way would be too incrementally decide whether to 
            // increase or decrease the median based on the most recently added
            // expenditure.
            int twiceMedian = -1; // I'd rather not deal with "0.5"'s and floating point.
            long numExpendituresSeen = 0;
            for (int i = 0; i < expenditureTally.size(); i++)
            {
                if (dIsOdd)
                {
                    if (numExpendituresSeen + expenditureTally[i] >= d / 2 + 1)
                    {
                        twiceMedian = 2 * i;
                        break;
                    }
                }
                else
                {
                    if (expenditureTally[i] > 0 && twiceMedian != -1)
                    {
                        // Pick up the split median.
                        twiceMedian += i;
                        break;
                    }
                    if (numExpendituresSeen + expenditureTally[i] == d / 2)
                    {
                        // Split median.  The remainder of twiceMedian will be found in a later
                        // iteration.
                        twiceMedian = i;
                    } 
                    else if (numExpendituresSeen + expenditureTally[i] >= d / 2 + 1)
                    {
                        // No need to split.
                        twiceMedian = 2 * i;
                        break;
                    }
                }

                numExpendituresSeen += expenditureTally[i];
            }
            if (expenditure >= twiceMedian)
            {
                numNotifications++;
            }
            // Pop the oldest value.
            expenditureTally[lastDExpenditures.front()]--;
            assert(expenditureTally[lastDExpenditures.front()] >= 0);
            lastDExpenditures.pop_front();            
        }
        // Incorporate latest value.
        lastDExpenditures.push_back(expenditure);
        expenditureTally[expenditure]++;
    }
    cout << numNotifications << endl;
    return 0;
}


