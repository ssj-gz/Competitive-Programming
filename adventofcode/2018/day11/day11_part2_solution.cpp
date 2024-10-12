#include <iostream>
#include <vector>
#include <limits>

#include <cassert>

//#define BRUTE_FORCE

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


    int bestStartX = -1;
    int bestStartY = -1;
    int bestGridSize = -1;
    int64_t highestSubgridPower = std::numeric_limits<int64_t>::min();
    vector<vector<int64_t>> totalPowerLookup(gridWidth, vector<int64_t>(gridHeight, 0));
    for (int y = 0; y < gridHeight; y++)
    {
        int64_t totalPowerRow = 0;
        for (int x = 0; x < gridWidth; x++)
        {
            int64_t totalPower = 0;
            if (y > 0)
                totalPower += totalPowerLookup[x][y - 1];
            totalPowerRow += fuelCellPowerLevels[x][y];
            totalPower += totalPowerRow;
            totalPowerLookup[x][y] = totalPower;
        }
    }
    {
        for (int endY = 0; endY < gridHeight; endY++)
        {
            for (int endX = 0; endX < gridWidth; endX++)
            {
                int64_t dbgTotalPower = 0;
                for (int x = 0; x <= endX; x++)
                {
                    for (int y = 0; y <= endY; y++)
                    {
                        dbgTotalPower+= fuelCellPowerLevels[x][y];
                    }
                }
                std::cout << "endX: " << endX << " endY: " << endY << " dbgTotalPower: " << dbgTotalPower << " totalPowerLookup: " << totalPowerLookup[endX][endY] << std::endl;
                assert(dbgTotalPower == totalPowerLookup[endX][endY]);

            }
        }
    }


    for (int subgridSize = 1; subgridSize <= std::min(gridWidth, gridHeight); subgridSize++)
    {
        std::cout << "Trying  subgridSize: " << subgridSize << std::endl;
        for (int startX = 0; startX < gridWidth - subgridSize; startX++)
        {
            const int endX = startX + subgridSize - 1;
            if (endX >= gridWidth)
                continue;
            for (int startY = 0; startY < gridHeight - subgridSize; startY++)
            {
                const int endY = startY + subgridSize - 1;
                if (endY >= gridHeight)
                    continue;
                int64_t totalPower = totalPowerLookup[endX][endY];
                if (startX - 1 >= 0)
                {
                    totalPower -= totalPowerLookup[startX - 1][endY];
                }
                if (startY - 1 >= 0)
                {
                    totalPower -= totalPowerLookup[endX][startY - 1];
                }
                if ((startX - 1 >= 0) && (startY - 1 >= 0))
                {
                    totalPower += totalPowerLookup[startX - 1][startY - 1];
                }
#ifdef BRUTE_FORCE
                int64_t dbgTotalPower = 0;
                {
                    for (int x = startX; x <= endX; x++)
                    {
                        for (int y = startY; y <= endY; y++)
                        {
                            dbgTotalPower += fuelCellPowerLevels[x][y];
                        }
                    }
                }
                std::cout << "startX: " << startX << " startY: " << startY << " subgridSize: " << subgridSize << " dbgTotalPower: " << dbgTotalPower << " totalPower: " << totalPower << std::endl;
                assert(totalPower == dbgTotalPower);
#endif
                if (totalPower > highestSubgridPower)
                {
                    highestSubgridPower = totalPower;
                    bestStartX = startX;
                    bestStartY = startY;
                    bestGridSize = subgridSize;
                }
            }
        }
    }
    std::cout << "highestSubgridPower: " << highestSubgridPower << " bestStartX: " << bestStartX << " bestStartY: " << bestStartY << std::endl;
    std::cout << "Result: " << (bestStartX + 1) << "," << (bestStartY + 1) << "," << bestGridSize << std::endl;


    return 0;
}
