#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cassert>

using namespace std;


int main()
{
    string row;
    vector<string> heightMap;
    while (cin >> row)
    {
        heightMap.push_back(row);
    }
    const int width = heightMap.size();
    const int height = heightMap.front().size();
    auto neighbourPoints = [width, height](const int x, const int y) -> vector<pair<int, int>> 
    {
        vector<pair<int, int>> blah;
        const std::pair<int, int> directions[] = { {-1,0}, {0,-1}, {1,0}, {0, 1}};
        for (const auto [dx, dy] : directions)
        {
            int neighbourX = x + dx;
            int neighbourY = y + dy;
            if (neighbourX < 0 || neighbourX >= width || neighbourY < 0 || neighbourY >= height)
                continue;
            blah.push_back({neighbourX, neighbourY});
        }
        return blah;
    };

    vector<pair<int, int>> lowPoints;
    for (int lowPointX = 0; lowPointX < width; lowPointX++)
    {
        for (int lowPointY = 0; lowPointY < height; lowPointY++)
        {
            bool isLowPoint = true;
            for (const auto [neighbourX, neighbourY] : neighbourPoints(lowPointX, lowPointY))
            {
                if (heightMap[neighbourX][neighbourY] <= heightMap[lowPointX][lowPointY])
                {
                    isLowPoint = false;
                    break;
                }
            }
            if (isLowPoint)
            {
                cout << "Low point: (" << lowPointX << ", " << lowPointY << ")" << endl;
                lowPoints.push_back({lowPointX, lowPointY});
            }
        }
    }
    vector<vector<int>> lowPointFor(width, vector<int>(height, -1));
    int lowPointIndex = -1;
    for (const auto& [lowPointX, lowPointY] : lowPoints)
    {
        cout << "Exploring low point: (" << lowPointX << ", " << lowPointY << ")" << endl;
        lowPointIndex++;
        vector<pair<int, int>> toExplore = { {lowPointX, lowPointY } };
        while (!toExplore.empty())
        {
            vector<pair<int, int>> nextToExplore;
            for (const auto& point : toExplore)
            {
                cout << " point: (" << point.first << ", " << point.second << ")" << endl;
                assert(lowPointFor[point.first][point.second] == -1 || lowPointFor[point.first][point.second] == lowPointIndex);
                lowPointFor[point.first][point.second] = lowPointIndex;
                for (const auto& [neighbourX, neighbourY] : neighbourPoints(point.first, point.second))
                {
                    if (heightMap[neighbourX][neighbourY] != '9' && /*heightMap[neighbourX][neighbourY] == heightMap[point.first][point.second] + 1*/ heightMap[neighbourX][neighbourY] > heightMap[point.first][point.second])
                    {
                        cout << " adding " << neighbourX << ", " << neighbourY << " to exploration list" << endl;
                        nextToExplore.push_back({neighbourX, neighbourY});
                    }
                }
            }
            toExplore = nextToExplore;
        }
    }
    vector<int64_t> basinSizes(lowPoints.size(), 0);
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            cout << std::setw(2) << lowPointFor[x][y] << " ";
            if (lowPointFor[x][y] == -1)
            {
                if (heightMap[x][y] != '9')
                {
                    cout << endl << "Whoops: " << x << ", " << y << endl;
                    assert(false);
                }
            }
            else
            {
                basinSizes[lowPointFor[x][y]]++;
            }
        }
        cout << endl;
    }
    sort(basinSizes.begin(), basinSizes.end(), std::greater<>());
    cout << "basin sizes: " << endl;
    for (const auto size : basinSizes)
    {
        cout << size << endl;
    }
    cout << basinSizes[0] * basinSizes[1] * basinSizes[2] << endl;
}
