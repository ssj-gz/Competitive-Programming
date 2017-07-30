#include <iostream>
#include <vector>

using namespace std;

int main()
{
    int numRows, numCols;
    cin >> numRows >> numCols; 

    vector<string> land(numRows);
    for (int i = 0; i < numRows; i++)
    {
        cin >> land[i];
    }

    // Grrr ... there's an obvious O((n * m) ^ 2) solution to this but, based on the sizes of m and n
    // and the fact that this is a Dynamic Programming problem, I spent *ages* trying to
    // use clever DP techniques to get better asymptotic bounds.  Came up with quite a few heuristics,
    // but never a proper algorithm, which, given the high completion rate, rather alarmed me - I 
    // thought I must be incredibly stupid :)
    // So I hacked together a simple solution with an obvious optimisation (debatably using DP - I'd
    // say "not") so I could look at the testcases and see if there was some pattern I could exploit, and imagine
    // my surprise when it passed all testcases in a fraction of the allotted time!
    // What a waste of time and effort that was :/
    // Anyway, all pretty self explanatory: for each of the n * m land elements, find the 
    // biggest fence with that land element as its top left corner, with a few 
    // obvious lookup tables and shortcuts.
    vector<vector<int>> numClearToRight(numRows, vector<int>(numCols));
    vector<vector<int>> numClearDownwards(numRows, vector<int>(numCols));

    const char clear = '.';

    for (int row = numRows - 1; row >= 0; row--)
    {
        for (int col = numCols - 1; col >= 0; col--)
        {
            if (land[row][col] == clear)
            {
                if (col + 1 < numCols)
                {
                    numClearToRight[row][col] = numClearToRight[row][col + 1] + 1;
                }
                else
                {
                    numClearToRight[row][col] = 0;
                }
                if (row + 1 < numRows)
                {
                    numClearDownwards[row][col] = numClearDownwards[row + 1][col] + 1;
                }
                else
                {
                    numClearDownwards[row][col] = 0;
                }
            }
            else
            {
                numClearToRight[row][col] = -1;
                numClearDownwards[row][col] = -1;
            }
        }
    }

    auto fencePerimeter = [](int width, int height) { return 2 * width + 2 * height - 4; };
    int largestFencePerimeter = 0;
    for (int row = 0; row < numRows; row++)
    {
        for (int col = 0; col < numCols; col++)
        {
            const int maxRight = numClearToRight[row][col] + col;
            const int maxDown = numClearDownwards[row][col] + row;
            const int maxWidth = maxRight - col + 1;
            const int maxHeight = maxDown - row + 1;
            const int maxPossibleFencePerimeter = fencePerimeter(maxWidth, maxHeight);
            if (maxPossibleFencePerimeter < largestFencePerimeter)
                continue;

            for (int bottomRow = maxDown; bottomRow > row; bottomRow--)
            {
                for (int rightCol = maxRight; rightCol > col; rightCol--)
                {
                    const int width = rightCol - col + 1;
                    const int height = bottomRow - row + 1;
                    const bool canBuildFence = (numClearToRight[bottomRow][col] >= width - 1) &&
                        (numClearDownwards[row][rightCol] >= height - 1);
                    if (canBuildFence)
                    {
                        const int perimeter = fencePerimeter(width, height);
                        largestFencePerimeter = max(largestFencePerimeter, perimeter);
                    }
                }
            }
        }
    }
    if (largestFencePerimeter > 0)
        cout << largestFencePerimeter << endl;
    else
        cout << "impossible" << endl;
}


