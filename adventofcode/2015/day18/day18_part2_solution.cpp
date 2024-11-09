#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

int main()
{
    vector<string> rawGrid;
    string rawGridRow;
    while (std::getline(cin, rawGridRow))
    {
        rawGrid.push_back(rawGridRow);
    }

    const int width = static_cast<int>(rawGrid.front().size());
    const int height = static_cast<int>(rawGrid.size());

    vector<vector<bool>> isLightOn(width, vector<bool>(height, false));
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            isLightOn[x][y] = (rawGrid[y][x] == '#');
        }
    }

    auto printLights = [&isLightOn, width, height]()
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                std::cout << (isLightOn[x][y] ? '#' : '.');
            }
            std::cout << std::endl;
        }
    };
    auto forceCornersOn = [&isLightOn, width, height]()
    {
        isLightOn[0][0] = true;
        isLightOn[0][height - 1] = true;
        isLightOn[width - 1][0] = true;
        isLightOn[width - 1][height - 1] = true;
    };

    forceCornersOn();

    printLights();


    for (int step = 1; step <= 100; step++)
    {
        vector<vector<bool>> nextIsLightOn(width, vector<bool>(height, false));
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                int numNeighbourLightsOn = 0;
                for (int dx = -1; dx <= 1; dx++)
                {
                    for (int dy = -1; dy <= 1; dy++)
                    {
                        if (dx == 0 && dy == 0)
                            continue;
                        const int neighbourX = x + dx;
                        const int neighbourY = y + dy;
                        if (neighbourX < 0 || neighbourX >= width || neighbourY < 0 || neighbourY >= height)
                            continue;
                        if (isLightOn[neighbourX][neighbourY])
                            numNeighbourLightsOn++;
                    }
                }

                if (isLightOn[x][y])
                    nextIsLightOn[x][y] = (numNeighbourLightsOn == 2 || numNeighbourLightsOn == 3);
                else
                    nextIsLightOn[x][y] = (numNeighbourLightsOn == 3);
            }
        }
        isLightOn = nextIsLightOn;
        forceCornersOn();


        std::cout << "After: " << step << " steps: " << std::endl;
        printLights();
    }

    int numOnLights = 0;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            if (isLightOn[x][y])
                numOnLights++;
        }
    }
    std::cout << "numOnLights: " << numOnLights << std::endl;
    return 0;
}
