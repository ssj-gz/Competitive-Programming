#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

int main()
{
    string rowString;
    vector<vector<int>> octopusGrid;
    while (cin >> rowString)
    {
        vector<int> row;
        for (const auto rowChar : rowString)
        {
            row.push_back(rowChar - '0');
        }
        octopusGrid.push_back(row);
    }

    auto printGrid = [&octopusGrid]()
    {
        for (const auto& row : octopusGrid)
        {
            for (const auto& energy : row)
            {
                cout << setw(3) << energy;
            }
            cout << endl;
        }


    };

    printGrid();

    int numIterations = 0;
    const int height = static_cast<int>(octopusGrid.size());
    const int width = static_cast<int>(octopusGrid.front().size());
    int64_t totalFlashes = 0;
    while (true)
    {
        int flashesThisIteration = 0;
        vector<vector<bool>> hasFlashed(height, vector<bool>(width, false));
        for (auto& row : octopusGrid)
        {
            for (auto& energy : row)
            {
                energy++;
            }
        }
        while (true)
        {
            bool newOctopusFlashed = false;
            for (int rowIndex = 0; rowIndex < height; rowIndex++)
            {
                for (int colIndex = 0; colIndex < width; colIndex++)
                {
                    if (octopusGrid[rowIndex][colIndex] > 9 && !hasFlashed[rowIndex][colIndex])
                    {
                        newOctopusFlashed = true;
                        hasFlashed[rowIndex][colIndex] = true;
                        totalFlashes++;
                        flashesThisIteration++;

                        for (int neighbourRow = rowIndex - 1; neighbourRow <= rowIndex + 1; neighbourRow++)
                        {
                            for (int neighbourCol = colIndex - 1; neighbourCol <= colIndex + 1; neighbourCol++)
                            {
                                if (neighbourRow == rowIndex && neighbourCol == colIndex)
                                    continue;
                                if (neighbourCol < 0 || neighbourCol >= height)
                                    continue;
                                if (neighbourRow < 0 || neighbourRow >= width)
                                    continue;
                                octopusGrid[neighbourRow][neighbourCol]++;
                            }
                        }
                    }
                }
            }
            if (!newOctopusFlashed)
                break;
        }
        for (auto& row : octopusGrid)
        {
            for (auto& energy : row)
            {
                if (energy > 9)
                    energy = 0;
            }
        }

        numIterations++;
        cout << "After " << numIterations << " iterations: " << endl;
        printGrid();
        if (flashesThisIteration == width * height)
            break;

    }
    cout << numIterations << endl;

}
