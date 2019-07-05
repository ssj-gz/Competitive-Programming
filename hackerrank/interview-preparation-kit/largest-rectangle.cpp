#include <iostream>
#include <vector>
#include <limits>

#include <sys/time.h>

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
}

