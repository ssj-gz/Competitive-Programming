#include <iostream>
#include <vector>
#include <limits>
#include <queue>
#include <deque>

using namespace std;

int main()
{
    string riskRowString;
    vector<vector<int>> riskGridTile;
    while (cin >> riskRowString)
    {
        vector<int> riskRow;
        for (const auto digit : riskRowString)
            riskRow.push_back(digit - '0');
        riskGridTile.push_back(riskRow);
    }

    const int tileWidth = static_cast<int>(riskGridTile.front().size());
    const int tileHeight = static_cast<int>(riskGridTile.size());

    const int tilesAcross = 5;
    const int tilesDown = 5;
    const int width = tileWidth * tilesAcross;
    const int height = tileWidth * tilesDown;

    vector<vector<int>> riskGrid(height, vector<int>(width));

    for (int tileRow = 0; tileRow < tilesDown; tileRow++)
    {
        for (int tileCol = 0; tileCol < tilesAcross; tileCol++)
        {
            const int riskIncrease = tileRow + tileCol;
            for (int rowInTile = 0; rowInTile < tileWidth; rowInTile++)
            {
                for (int colInTile = 0; colInTile < tileHeight; colInTile++)
                {
                    int cellRisk = riskGridTile[rowInTile][colInTile] + riskIncrease;
                    while (cellRisk > 9)
                        cellRisk -= 9;
                    riskGrid[tileRow * tileHeight + rowInTile][tileCol * tileWidth + colInTile] = cellRisk;
                }
            }
        }
    }

    cout << "Grid: " << endl;
    for (const auto& row: riskGrid)
    {
        for (const auto risk : row)
        {
            cout << static_cast<char>('0' + risk);
        }
        cout << endl;
    }


    struct CoordInfo
    {
        int row = -1;
        int col = -1;
        int shortestTo = std::numeric_limits<int>::max();
    };

    CoordInfo start{0, 0, 0};

    vector<vector<int>> shortestPathLenTo(height, vector<int>(width, std::numeric_limits<int>::max()));

    deque<CoordInfo> toExplore ( {start} );

    while (!toExplore.empty())
    {
        const auto coordInfo = toExplore.front();
        toExplore.pop_front();

        if (shortestPathLenTo[coordInfo.row][coordInfo.col] > coordInfo.shortestTo)
        {
            shortestPathLenTo[coordInfo.row][coordInfo.col] = coordInfo.shortestTo;
            const pair<int, int> dxy[] = {
                {-1, 0},
                {0, -1},
                {1, 0},
                {0, 1}
            };
            for (const auto [dx, dy] : dxy)
            {
                const int neighbourRow = coordInfo.row - dy;
                const int neighbourCol = coordInfo.col - dx;
                if (neighbourRow >= 0 && neighbourRow < height && neighbourCol >= 0 && neighbourCol < width)
                {
                    toExplore.push_back({neighbourRow, neighbourCol, coordInfo.shortestTo + riskGrid[neighbourRow][neighbourCol]});
                }
            }
        }
    }

    cout << shortestPathLenTo.back().back() << endl;

}
