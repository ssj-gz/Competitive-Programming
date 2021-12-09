#include <iostream>
#include <vector>

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
    int64_t risk = 0;

    for (int lowPointX = 0; lowPointX < width; lowPointX++)
    {
        for (int lowPointY = 0; lowPointY < height; lowPointY++)
        {
            const std::pair<int, int> directions[] = { {-1,0}, {0,-1}, {1,0}, {0, 1}};
            bool isLowPoint = true;
            for (const auto [dx, dy] : directions)
            {
                int neighbourX = lowPointX + dx;
                int neighbourY = lowPointY + dy;
                if (neighbourX < 0 || neighbourX >= width || neighbourY < 0 || neighbourY >= height)
                    continue;

                if (heightMap[neighbourX][neighbourY] <= heightMap[lowPointX][lowPointY])
                {
                    isLowPoint = false;
                    break;
                }
            }
            if (isLowPoint)
            {
                cout << "Low point: (" << lowPointX << ", " << lowPointY << ")" << endl;
                risk += (heightMap[lowPointX][lowPointY] - '0') + 1;
            }
        }
    }
    cout << risk << endl;

}
