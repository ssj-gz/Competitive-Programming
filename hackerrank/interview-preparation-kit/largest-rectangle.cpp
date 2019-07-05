#include <iostream>
#include <vector>
#include <stack>
#include <limits>
#include <algorithm>

#include <sys/time.h>
#include <cassert>

using namespace std;

int64_t solveBruteForce(const vector<int>& heights)
{
    int64_t largestRectangle = 0;
    const int n = heights.size();

    for (int i = 0; i < n; i++)
    {
        for (int j = i; j < n; j++)
        {
            int minHeight = std::numeric_limits<int>::max();
            for (int k = i; k <= j; k++)
            {
                if (heights[k] < minHeight)
                {
                    minHeight = heights[k];
                }
            }
            const int64_t rangeSize = (j - i + 1);
            const int64_t rectangleSize = rangeSize * minHeight;
            largestRectangle = max(largestRectangle, rectangleSize);
        }
    }

    return largestRectangle;
}

vector<int> findIndexOfNextLowerThan(const vector<int>& heights)
{
    std::stack<int> buildingHeightIndices;
    const int n = heights.size();

    vector<int> indexOfNextLowerThan(n, -1);

    for (int index = 0; index < n; index++)
    {
        cout << " findIndexOfNextLowerThan: index: " << index << " buildingHeightIndices.size(): " << buildingHeightIndices.size() << endl;
        while (!buildingHeightIndices.empty() && heights[buildingHeightIndices.top()] > heights[index])
        {
            indexOfNextLowerThan[buildingHeightIndices.top()] = index;
            cout << " Did an assignment to " << buildingHeightIndices.top() << " of " << index  << endl;
            buildingHeightIndices.pop();
        }
        buildingHeightIndices.push(index);
    }

    return indexOfNextLowerThan;
}

int64_t solveOptimised(const vector<int>& heights)
{
    int64_t largestRectangle = 0;
    const int n = heights.size();

    const auto indexOfNextLowerThan = findIndexOfNextLowerThan(heights);

    const vector<int> reversedHeights(heights.rbegin(), heights.rend());
    auto indexOfPrevLowerThan = findIndexOfNextLowerThan(reversedHeights);
    // Reverse order indexOfPrevLowerThan, and also "reverse" each index.
    for (auto& index : indexOfPrevLowerThan)
    {
        if (index != -1)
            index = (n - 1) - index;
    }
    reverse(indexOfPrevLowerThan.begin(), indexOfPrevLowerThan.end());

    for (int index = 0; index < n; index++)
    {
        cout << "index: " << index << " height: " << heights[index] << " indexOfNextLowerThan: " << indexOfNextLowerThan[index] << " indexOfPrevLowerThan: " << indexOfPrevLowerThan[index] << endl;
        assert(indexOfNextLowerThan[index] == -1 || indexOfNextLowerThan[index] > index);
        assert(indexOfPrevLowerThan[index] == -1 || indexOfPrevLowerThan[index] < index);
        const int64_t distanceToLeftWhereWeAreMin = (indexOfPrevLowerThan[index] == -1 ? index : index - indexOfPrevLowerThan[index]);
        const int64_t distanceToRightWhereWeAreMin = (indexOfNextLowerThan[index] == -1 ? n - 1 : indexOfNextLowerThan[index] - index);
        const int64_t largestRectangleWhereWeAreMin = (distanceToLeftWhereWeAreMin + 1 + distanceToRightWhereWeAreMin) * heights[index];

        largestRectangle = max(largestRectangle, largestRectangleWhereWeAreMin);
    }

    return largestRectangle;
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int n = (rand() % 100) + 1;
        const int maxHeight = rand() % 10000 + 1;

        cout << n << endl;

        for (int i = 0; i < n; i++)
        {
            cout << ((rand() % maxHeight) + 1) << " ";
        }

        return 0;
    }
    int n;
    cin >> n;

    vector<int> heights(n);

    for (int i = 0; i < n; i++)
    {
        cin >> heights[i];
    }

    const auto solutionBruteForce = solveBruteForce(heights);
    cout << "solutionBruteForce: " << solutionBruteForce << endl;
    const auto solutionOptimised = solveOptimised(heights);
    cout << "solutionOptimised: " << solutionOptimised << endl;
}

