// Simon St James (ssjgz) - 2019-07-10
#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

#include <cassert>

using namespace std;

int64_t findNumValidPaths(const vector<int>& heightsOriginal)
{
    int64_t numPaths = 0;
    vector<int> heights(heightsOriginal);
    // A bit of a hack, but append a dummy element to the end of "heights"
    // that is larger than all other heights - this causes the stack to
    // be flushed automatically, so we don't have to duplicate code
    // flushing the stack manually.
    heights.push_back(*max_element(heightsOriginal.begin(), heightsOriginal.end()) + 1);

    stack<int> heightStack;

    for (const auto height : heights)
    {
        int prevHeightPopped = -1;
        int numOfSameHeightPoppedInARow = 1;
        while (!heightStack.empty() && height > heightStack.top())
        {
            const int heightToPop = heightStack.top();
            if (heightToPop == prevHeightPopped)
            {
                // Cumulatively summing numOfSameHeightPoppedInARow ( * 2, to include
                // the right-to-left "mirror" of each left-to-right path) is an online
                // alternative to waiting to get the full length r of the run of the same
                // heights in a row and then doing r choose 2.
                numPaths += numOfSameHeightPoppedInARow * 2;

                numOfSameHeightPoppedInARow++;
            }
            else
            {
                numOfSameHeightPoppedInARow = 1;
            }

            prevHeightPopped = heightToPop;

            heightStack.pop();
        }
        heightStack.push(height);
    }

    assert(heightStack.size() == 1); // The "dummy" largest element we appended to the end of heights.

    return numPaths;
}

int main(int argc, char* argv[])
{
    // Pretty easy one, and similar to e.g. "Max Min Riddle"; "Largest Rectangle"; etc.
    //
    // We maintain a stack of heights encountered from left-to-right, and ensure it is non-decreasing:
    // we do this by adding each new height h to the stack *after* popping all heights that
    // are less than h from the stack.  Imagine if instead of just adding the heights, we 
    // added pairs of the height plus the index of this height in heights.  It's hopefully clear
    // that indices i and j, i < j, would both be on the stack at once if and only if there is a path
    // between them that does not encounter any skyscraper taller than height[i].
    //
    // Further, it's hopefully clear that if the same height is present more than once in the stack,
    // then these heights will be adjacent i.e. in a run of the same height.  Further further,
    // each pair of the indices i < j with h[i] == h[j] will be on the stack if and only if
    // there is a *valid* path between them.  If there are r of these in a row, then that will
    // give rise to r choose 2 paths going from left to right, and (r choose 2) * 2 paths in total
    // (there is a bijection between paths going left to right and paths going from right to left).
    //
    // So: we simply maintain the stack as described above, and count the number of runs of the same
    // height that end up in the stack, and update numPaths as described above.  The whole process
    // is O(n).
    int n;
    cin >> n;

    vector<int> heights(n);

    for (int i = 0; i < n; i++)
    {
        cin >> heights[i];
    }

    cout << findNumValidPaths(heights) << endl;
}

