#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>
#include <sys/time.h>

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

int64_t solveOptimised(const vector<int>& heightsOriginal)
{
    int64_t numPaths = 0;
    vector<int> heights(heightsOriginal);
    heights.push_back(*max_element(heightsOriginal.begin(), heightsOriginal.end()) + 1);

    vector<int> heightStack;

    auto nChoose2 = [](int n)
    {
        return (static_cast<int64_t>(n) * (n - 1)) / 2;
    };

    for (const auto height : heights)
    {
        cout << "height: " << height << " current stack: " << endl;
        for (const auto x : heightStack)
        {
            cout << x << " ";
        }
        cout << endl;
        int lastHeightPopped = -1;
        int numOfSameHeightPopped = 1;
        while (!heightStack.empty() && height > heightStack.back())
        {
            cout << "numOfSameHeightPopped: " << numOfSameHeightPopped << endl;
            const int heightToPop = heightStack.back();
            if (heightToPop == lastHeightPopped)
            {
                numOfSameHeightPopped++;
            }
            else
            {
                cout << "Flushing" << endl;
                numPaths += nChoose2(numOfSameHeightPopped) * 2;
                numOfSameHeightPopped = 1;
            }

            lastHeightPopped = heightToPop;

            heightStack.pop_back();
        }
        numPaths += nChoose2(numOfSameHeightPopped) * 2;
        heightStack.push_back(height);
    }

    assert(heightStack.size() == 1);


    return numPaths;
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int n = rand() % 100 + 1;
        cout << n << endl;

        const int maxHeight = rand() % 1000 + 1;

        for (int i = 0; i < n; i++)
        {
            cout << ((rand() % maxHeight) + 1) << " ";
        }
        cout << endl;
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
    assert(solutionOptimised == solutionBruteForce);

}

