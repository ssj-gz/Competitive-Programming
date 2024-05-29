#include <iostream>
#include <vector>
#include <set>
#include <cstdint>

using namespace std;

int main()
{
    vector<string> area;
    string row;
    while (std::getline(cin, row))
    {
        area.push_back(row);
    }

    auto printArea = [](const auto& area)
    {
        for (const auto& row : area)
        {
            std::cout << row << std::endl;
        }
    };

    const int areaWidth = area.front().size();
    const int areaHeight = area.size();
    printArea(area);

    set<vector<string>> seenAreas = { area };

    while (true)
    {
        vector<string> newArea(area);
        for (int row = 0; row < areaHeight; row++)
        {
            for (int col = 0; col < areaWidth; col++)
            {
                int numAdjacentBugs = 0;
                const int directions[][2] = { {-1, 0}, { 1, 0}, { 0, -1}, {0, 1} };
                for (const auto dxdy : directions )
                {
                    const int adjacentRow = row + dxdy[0];
                    const int adjacentCol = col + dxdy[1];

                    if (adjacentRow < 0 || adjacentRow >= areaHeight)
                        continue;
                    if (adjacentCol < 0 || adjacentCol >= areaWidth)
                        continue;
                    const auto cellContent = area[adjacentRow][adjacentCol];
                    if (cellContent == '#')
                        numAdjacentBugs++;
                }
                if (area[row][col] == '#')
                {
                    if (numAdjacentBugs != 1)
                        newArea[row][col] = '.';
                }
                else
                {
                    if ((numAdjacentBugs == 1) || (numAdjacentBugs == 2))
                        newArea[row][col] = '#';
                }
            }
        }
        area = newArea;

        std::cout << "After iteration: " << std::endl;
        printArea(area);
        if (seenAreas.contains(area))
        {
            std::cout << "is duplicate" << std::endl;
            uint64_t biodiversityRating = 0;
            uint64_t powerOf2 = 1;

            for (int row = 0; row < areaHeight; row++)
            {
                for (int col = 0; col < areaWidth; col++)
                {
                    if (area[row][col] == '#')
                        biodiversityRating += powerOf2;
                    powerOf2 *= 2;
                }
            }
            std::cout << "biodiversityRating: " << biodiversityRating << std::endl;
            break;
        }
        seenAreas.insert(area);
    }
}
