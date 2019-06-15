// Simon St James (ssjgz)  2019-15-06
#include <iostream>
#include <limits>

using namespace std;

int main()
{
    // Again, trivially easy - code is hopefully self-explanatory :)
    constexpr auto width = 6;
    constexpr auto height = 6;
    int a[height][width] = {};

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            cin >> a[i][j];
        }
    }

    constexpr auto hourGlassWidth = 3;
    constexpr auto hourGlassHeight = 3;
    int maxHourGlassSum = std::numeric_limits<int>::min();
    for (int startRow = 0; startRow + hourGlassHeight <= height; startRow++)
    {
        for (int startCol = 0; startCol + hourGlassWidth <= width; startCol++)
        {
            int hourGlassSum = 0;
            for (int j = startCol; j < startCol + hourGlassWidth; j++)
            {
                hourGlassSum += a[startRow][j];
                hourGlassSum += a[startRow + hourGlassHeight - 1][j];
            }
            hourGlassSum += a[startRow + 1][startCol + 1];

            maxHourGlassSum = std::max(maxHourGlassSum, hourGlassSum);
        }
    }

    cout << maxHourGlassSum << endl;
}
