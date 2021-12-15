#include <iostream>
#include <vector>
#include <limits>
#include <queue>

using namespace std;

int main()
{
    string riskRowString;
    vector<vector<int>> riskGrid;
    while (cin >> riskRowString)
    {
        vector<int> riskRow;
        for (const auto digit : riskRowString)
            riskRow.push_back(digit - '0');
        riskGrid.push_back(riskRow);
    }

    const int width = static_cast<int>(riskGrid.front().size());
    const int height = static_cast<int>(riskGrid.size());
    cout << "width: " << width << " height: " << height << endl;

    struct Coord
    {
        int row = -1;
        int col = -1;
    };
    struct CoordInfo
    {
        Coord coord;
        int shortestTo = std::numeric_limits<int>::max();
    };

    Coord start{0, 0};
    Coord end{height - 1, width - 1};

    vector<vector<int>> shortestPathLenTo(height, vector<int>(width, std::numeric_limits<int>::max()));

    queue<CoordInfo> toExplore ( {{start, 0}} );

    while (!toExplore.empty())
    {
        const auto coordInfo = toExplore.front();
        toExplore.pop();

        if (shortestPathLenTo[coordInfo.coord.row][coordInfo.coord.col] > coordInfo.shortestTo)
        {
            shortestPathLenTo[coordInfo.coord.row][coordInfo.coord.col] = coordInfo.shortestTo;
            cout << "Found new shortest path to: (" << coordInfo.coord.row << ", " << coordInfo.coord.col << "): " << coordInfo.shortestTo << endl;
            const pair<int, int> dxy[] = {
                {-1, 0},
                {0, -1},
                {1, 0},
                {0, 1}
            };
            for (const auto [dx, dy] : dxy)
            {
                const int neighbourRow = coordInfo.coord.row - dy;
                const int neighbourCol = coordInfo.coord.col - dx;
                if (neighbourRow >= 0 && neighbourRow < height && neighbourCol >= 0 && neighbourCol < width)
                {
                    toExplore.push({{neighbourRow, neighbourCol}, coordInfo.shortestTo + riskGrid[neighbourRow][neighbourCol]});
                }
            }
        }
    }

    cout << shortestPathLenTo[end.row][end.col] << endl;

}
