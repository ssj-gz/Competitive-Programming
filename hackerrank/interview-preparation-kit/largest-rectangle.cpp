#include <iostream>
#include <vector>
#include <limits>

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

int main()
{
    int n;
    cin >> n;

    vector<int> heights(n);

    for (int i = 0; i < n; i++)
    {
        cin >> heights[i];
    }

    const auto solutionBruteForce = solveBruteForce(heights);
    cout << "solutionBruteForce: " << solutionBruteForce << endl;
}

