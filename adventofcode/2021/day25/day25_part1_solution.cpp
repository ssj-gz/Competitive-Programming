#include <iostream>
#include <vector>

using namespace std;

int main()
{
    vector<string> grid;
    string gridRow;
    while (cin >> gridRow)
        grid.push_back(gridRow);

    const int height = static_cast<int>(grid.size());
    const int width = static_cast<int>(grid.front().size());

    auto printGrid = [](const vector<string>& grid)
    {
        for (const auto& row : grid)
            cout << row << endl;
    };

    cout << "initial state: " << endl;
    cout << "width: " << width << " height: " << height << endl;
    printGrid(grid);


    int iterationNum = 0;
    while (true)
    {
        vector<string> nextGrid(height, string(width, '.'));
        // Fish moving rightwards.
        for (int rowIndex = 0; rowIndex < height; rowIndex++)
        {
            for (int colIndex = 0; colIndex < width; colIndex++)
            {
                if (grid[rowIndex][colIndex] == '>')
                {
                    if (grid[rowIndex][(colIndex + 1) % width] == '.')
                    {
                        nextGrid[rowIndex][(colIndex + 1) % width] = '>';
                    }
                    else
                    {
                        nextGrid[rowIndex][colIndex] = '>';
                    }
                }
            }
        }
        // Fish moving downwards.
        for (int rowIndex = 0; rowIndex < height; rowIndex++)
        {
            for (int colIndex = 0; colIndex < width; colIndex++)
            {
                if (grid[rowIndex][colIndex] == 'v')
                {
                    if (grid[(rowIndex + 1) % height][colIndex] != 'v' && nextGrid[(rowIndex + 1) % height][colIndex] == '.')
                    {
                        nextGrid[(rowIndex + 1) % height][colIndex] = 'v';
                    }
                    else
                    {
                        nextGrid[rowIndex][colIndex] = 'v';
                    }
                }
            }
        }
        iterationNum++;
        cout << "After iteration # " << iterationNum << endl;
        printGrid(nextGrid);
        if (grid == nextGrid)
            break;
        grid = nextGrid;
    }
    cout << "Finished after " << iterationNum << " iterations" << endl;
}
