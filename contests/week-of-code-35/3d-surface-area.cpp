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

    for (auto row = 0; row < H; row++)
    {
        for (auto col = 0; col < W; col++)
        {
            cin >> A[row][col];
            assert(A[row][col] >= 1);
        }
    }

    int64_t totalCost = 0;
    for (auto row = 0; row < H; row++)
    {
        for (auto col = 0; col < W; col++)
        {
            totalCost += 2; // Top and bottom surface of this cell.

            // The "sides" of each square that is stacked on this cell.
            int64_t exposedSideFaces = 0;
            const auto stackHeight = A[row][col];
            // Num exposed left faces of this stack.
            const auto stackToLeftHeight = (col > 0 ? A[row][col - 1] : 0);
            if (stackHeight > stackToLeftHeight)
                exposedSideFaces += stackHeight - stackToLeftHeight;
            // Right faces.
            const auto stackToRightHeight = (col < W - 1 ? A[row][col + 1] : 0);
            if (stackHeight > stackToRightHeight)
                exposedSideFaces += stackHeight - stackToRightHeight;
            // Back faces.
            const auto stackBehindHeight = (row > 0 ? A[row - 1][col] : 0);
            if (stackHeight > stackBehindHeight)
                exposedSideFaces += stackHeight - stackBehindHeight;
            // Front faces.
            const auto stackInFrontHeight = (row < H - 1 ? A[row + 1][col] : 0);
            if (stackHeight > stackInFrontHeight)
                exposedSideFaces += stackHeight - stackInFrontHeight;

            assert(exposedSideFaces >= 0);
            totalCost += exposedSideFaces;
        }
    }
    cout << totalCost << endl;
}

