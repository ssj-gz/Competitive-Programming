// Simon St James (ssjgz) - 2017-11-15 07:22:00
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int main()
{
    int H, W;
    cin >> H >> W;

    vector<vector<int>> A(W, vector<int>(H, -1));

    for (int y = 0; y < H; y++)
    {
        for (int x = 0; x < W; x++)
        {
            cin >> A[x][y];
            assert(A[x][y] >= 1);
        }
    }

    int64_t totalCost = 0;
    for (int y = 0; y < H; y++)
    {
        for (int x = 0; x < W; x++)
        {
            totalCost += 2; // Top and bottom surface of this cell.

            // The "sides" of each square that is stacked on this cell.
            for (int depth = 1; depth <= A[x][y]; depth++)
            {
                int exposedSides = 4;
                if (x > 0 && A[x - 1][y] >= depth)
                {
                    exposedSides--;
                }
                if (x < W - 1 && A[x + 1][y] >= depth)
                {
                    exposedSides--;
                }
                if (y > 0 && A[x][y - 1] >= depth)
                {
                    exposedSides--;
                }
                if (y < H - 1 && A[x][y + 1] >= depth)
                {
                    exposedSides--;
                }
                totalCost += exposedSides;
            }
        }
    }
    cout << totalCost << endl;
}

