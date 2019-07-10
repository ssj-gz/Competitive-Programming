// Simon St James (ssgz) - 2019-07-10
#include <iostream>
#include <vector>
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

    vector<int> heightStack;

    auto nChoose2 = [](int n)
    {
        return (static_cast<int64_t>(n) * (n - 1)) / 2;
    };

    for (const auto height : heights)
    {
        int prevHeightPopped = -1;
        int numOfSameHeightPoppedInARow = 1;
        while (!heightStack.empty() && height > heightStack.back())
        {
            const int heightToPop = heightStack.back();
            if (heightToPop == prevHeightPopped)
            {
                numOfSameHeightPoppedInARow++;
            }
            else
            {
                numPaths += nChoose2(numOfSameHeightPoppedInARow) * 2;
                numOfSameHeightPoppedInARow = 1;
            }

            prevHeightPopped = heightToPop;

            heightStack.pop_back();
        }
        numPaths += nChoose2(numOfSameHeightPoppedInARow) * 2;
        heightStack.push_back(height);
    }

    assert(heightStack.size() == 1); // The "dummy" largest element we appended to the end of heights.

    return numPaths;
}

int main(int argc, char* argv[])
{
    int n;
    cin >> n;

    vector<int> heights(n);

    for (int i = 0; i < n; i++)
    {
        cin >> heights[i];
    }

    cout << findNumValidPaths(heights) << endl;
}

