#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

int main()
{
    string enhancementAlgorithm;
    cin >> enhancementAlgorithm;
    assert(enhancementAlgorithm.size() == 512);
    cout << "enhancementAlgorithm: " << enhancementAlgorithm << endl;

    vector<string> grid;
    string row;
    while(cin >> row)
        grid.push_back(row);

    auto printGrid = [](const vector<string>& grid)
    {
        for (const auto& row : grid)
            cout << row << endl;
    };
    cout << "grid: " << endl;
    printGrid(grid);

    auto surroundedWithStarBoundary = [](const vector<string>& grid)
    {
        const int width = static_cast<int>(grid.front().size());
        const int height = static_cast<int>(grid.size());
        vector<string> expanded = grid;
        expanded.insert(expanded.begin(), string(width, '*'));
        expanded.insert(expanded.end(), string(width, '*'));
        for (auto& row : expanded)
        {
            row = '*' + row + '*';
        }

        return expanded;
    };

    int numIterations = 0;
    grid = surroundedWithStarBoundary(grid);
    bool infiniteBlocksToggle = false;
    if (enhancementAlgorithm[0] == '#')
    {
        assert(enhancementAlgorithm.back() == '.');
        infiniteBlocksToggle = true;
    }
    while (numIterations < 50)
    {
        grid = surroundedWithStarBoundary(grid);
        cout << "Begin iteration: " << endl;
        printGrid(grid);
        const int width = static_cast<int>(grid.front().size());
        const int height = static_cast<int>(grid.size());

        vector<string> nextGrid(height, string(width, '*'));

        for (int row = 1; row < height - 1; row++)
        {
            for (int col = 1; col < width - 1; col++)
            {
                string binaryString;
                for (int neighbourRow = row - 1; neighbourRow <= row + 1; neighbourRow++)
                {
                    for (int neighbourCol = col - 1; neighbourCol <= col + 1; neighbourCol++)
                    {
                        switch (grid[neighbourRow][neighbourCol])
                        {
                            case '#':
                                binaryString += '1';
                                break;
                            case '.':
                                binaryString += '0';
                                break;
                            case '*':
                                // The '*' char represents the boundary between the portion of the grid
                                // we're interested in, and "the rest of it" i.e. the infinite expanse 
                                // of grid in all directions.
                                // If infiniteBlocksToggle is true, the "rest of it" will be all '.''s 
                                // on even iterations (starting from 0) and all '#'s on odd iterations.
                                // If infiniteBlocksToggle is false, the "rest of it" is always all '.'s.
                                if (infiniteBlocksToggle)
                                    binaryString += (numIterations % 2 == 0) ? '0' : '1';
                                else
                                    binaryString += '0';
                                break;
                        }
                    }
                }
                const int asDecimal = stoi(binaryString, nullptr, 2);
                assert(asDecimal >=0 && asDecimal < 512);
                nextGrid[row][col] = enhancementAlgorithm[asDecimal];
            }
        }

        cout << "After iteration: " << endl;
        printGrid(nextGrid);
        grid = nextGrid;
        numIterations++;
    }
    int64_t numOnPixels = 0;
    for (const auto& row : grid)
    {
        numOnPixels += count(row.begin(), row.end(), '#');
    }
    cout << "numOnPixels: " << numOnPixels << endl;


}
