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
    vector<string> grid;
    string row;
    while(cin >> row)
        grid.push_back(row);

    cout << "enhancementAlgorithm: " << enhancementAlgorithm << endl;
    auto printGrid = [](const vector<string>& grid)
    {
        for (const auto& row : grid)
            cout << row << endl;
    };
    cout << "grid: " << endl;
    printGrid(grid);

    auto gridNeedsExpanding = [](const vector<string>& grid)
    {
        const int width = static_cast<int>(grid.front().size());
        const int height = static_cast<int>(grid.size());
        //for (int col : {0, 1, width - 1, width - 2})
        for (int col : {0, width - 1})
        {
            for (int row = 0; row < height; row++)
            {
                if (grid[row][col] == '#')
                    return true;
            }
        }
        //for (int row : {0, 1, height - 1, height - 2})
        for (int row : {0, height - 1})
        {
            for (int col = 0; col < width; col++)
            {
                if (grid[row][col] == '#')
                    return true;
            }
        }
        return false;
    };
    auto expanded = [](const vector<string>& grid)
    {
        const int width = static_cast<int>(grid.front().size());
        const int height = static_cast<int>(grid.size());
#if 0
        vector<string> expanded = grid;
        expanded.insert(expanded.begin(), height / 2, string(width, '.'));
        expanded.insert(expanded.end(), height / 2,string(width, '.'));
        for (auto& row : expanded)
        {
            row = string(width / 2, '.') + row + string(width / 2, '.');
        }
#elif 0
        vector<string> expanded = grid;
        expanded.insert(expanded.begin(), string(width, '.'));
        expanded.insert(expanded.end(), string(width, '.'));
        for (auto& row : expanded)
        {
            row = '.' + row + '.';
        }
#else
        vector<string> expanded = grid;
        expanded.insert(expanded.begin(), 20, string(width, '.'));
        expanded.insert(expanded.end(), 20,string(width, '.'));
        for (auto& row : expanded)
        {
            row = string(20, '.') + row + string(20, '.');
        }
#endif

        return expanded;
    };
    auto surroundedWithStars = [](const vector<string>& grid)
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
    grid = surroundedWithStars(grid);
    bool offBlocksToggle = false;
    if (enhancementAlgorithm[0] == '#')
    {
        assert(enhancementAlgorithm.back() == '.');
        offBlocksToggle = true;
    }
    while (numIterations < 2)
    {
#if 0
        while (gridNeedsExpanding(grid))
        {
            cout << "needs expanding!" << endl;
            grid = expanded(grid);
            //assert(!gridNeedsExpanding(grid));
            cout << "Expanded: " << endl;
            printGrid(grid);
        }
#endif
        grid = surroundedWithStars(grid);
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
                                if (offBlocksToggle)
                                    binaryString += (numIterations % 2 == 0) ? '0' : '1';
                                else
                                    binaryString += '0';
                                break;
                        }
                    }
                }
                //cout << "binaryString: " << binaryString << endl;
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
