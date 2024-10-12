#include <iostream>
#include <vector>
#include <limits>

#include <cassert>

using namespace std;

int main()
{
    int serialNumber;
    cin >> serialNumber;
    assert(cin);
    std::cout << "serialNumber: " << serialNumber << std::endl;

    constexpr int gridWidth = 300;
    constexpr int gridHeight = 300;
    vector<vector<int>> fuelCellPowerLevels(gridWidth, vector<int>(gridHeight, 0));
    for (int x = 0; x < gridWidth; x++)
    {
        for (int y = 0; y < gridHeight; y++)
        {
            const int64_t rackId = (x + 1) + 10;
            int64_t powerLevel = rackId * (y + 1);
            powerLevel += serialNumber;
            powerLevel = powerLevel * rackId;
            powerLevel = (powerLevel / 100) % 10;
            powerLevel -= 5;
            fuelCellPowerLevels[x][y] = powerLevel;
            std::cout << "fuelCellPowerLevels: " << (x + 1) << ", " << (y + 1) << ": " << fuelCellPowerLevels[x][y] << std::endl;
        }
    }


    constexpr int subgridSize = 3;
    int bestStartX = -1;
    int bestStartY = -1;
    int64_t highestSubgridPower = std::numeric_limits<int64_t>::min();
    for (int startX = 0; startX < gridWidth - subgridSize; startX++)
    {
        for (int startY = 0; startY < gridHeight - subgridSize; startY++)
        {
            int64_t totalPower = 0;
            for (int x = startX; x < startX + subgridSize; x++)
            {
                for (int y = startY; y < startY + subgridSize; y++)
                {
                    totalPower += fuelCellPowerLevels[x][y];
                }
            }
            if (totalPower > highestSubgridPower)
            {
                highestSubgridPower = totalPower;
                bestStartX = startX;
                bestStartY = startY;
            }
        }
    }
    std::cout << "highestSubgridPower: " << highestSubgridPower << " bestStartX: " << bestStartX << " bestStartY: " << bestStartY << std::endl;
    std::cout << "Result: " << (bestStartX + 1) << "," << (bestStartY + 1) << std::endl;


    return 0;
}
