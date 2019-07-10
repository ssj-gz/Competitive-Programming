#include <iostream>
#include <vector>

using namespace std;

int64_t solveBruteForce(const vector<int>& heights)
{
    int64_t numPaths = 0;
    const int n = heights.size();
    for (int start = 0; start < n; start++)
    {
        const int initialHeight = heights[start];
        for (int end = start + 1; end < n; end++)
        {
            if (heights[end] == initialHeight)
                numPaths += 2;
            else if (heights[end] > initialHeight)
                break;
        }
    }

    return numPaths;
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

