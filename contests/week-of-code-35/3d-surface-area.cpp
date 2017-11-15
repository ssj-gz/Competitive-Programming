// Simon St James (ssjgz) - 2017-11-15 07:22:00
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int main()
{
    int H, W;
    cin >> H >> W;

    vector<vector<int>> A(H, vector<int>(W, -1));

    for (int row = 0; row < H; row++)
    {
        for (int col = 0; col < W; col++)
        {
            cin >> A[row][col];
            assert(A[row][col] >= 1);
        }
    }

    int64_t totalCost = 0;
    for (int row = 0; row < H; row++)
    {
        for (int col = 0; col < W; col++)
        {
            totalCost += 2; // Top and bottom surface of this cell.

            // The "sides" of each square that is stacked on this cell.
            for (int depth = 1; depth <= A[row][col]; depth++)
            {
                int exposedSides = 4;
                if (col > 0 && A[row][col - 1] >= depth)
                {
                    exposedSides--;
                }
                if (col < W - 1 && A[row][col + 1] >= depth)
                {
                    exposedSides--;
                }
                if (row > 0 && A[row - 1][col] >= depth)
                {
                    exposedSides--;
                }
                if (row < H - 1 && A[row + 1][col] >= depth)
                {
                    exposedSides--;
                }
                totalCost += exposedSides;
            }
        }
    }
    cout << totalCost << endl;
}

