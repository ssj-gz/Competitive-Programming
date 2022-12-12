#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>
#include <cassert>

using namespace std;

int main()
{
    struct Coord
    {
        int row = -1;
        int col = -1;
        bool operator==(const Coord& other) const
        {
            return (row == other.row && col == other.col);
        }
    };
         
    vector<string> grid;
    string gridRow;
    while (std::getline(cin, gridRow))
    {
        grid.push_back(gridRow);
    }
    const int numRows = static_cast<int>(grid.size());
    const int numCols = static_cast<int>(grid[0].size());

    Coord endCoord;

    vector<Coord> startCoords;
    for (int row = 0; row < numRows; row++)
    {
        for (int col = 0; col < numCols; col++)
        {
            if (grid[row][col] == 'S')
            {
                grid[row][col] = 'a';
            }
            else if (grid[row][col] == 'E')
            {
                endCoord = Coord{row, col};
                grid[row][col] = 'z';
            }
            if (grid[row][col] == 'a')
                startCoords.push_back({row, col});
        }
    }

    int minStepsFromAnyStartCoord = std::numeric_limits<int>::max();
    for (const auto startCoord : startCoords)
    {
        vector<vector<int>> minStepsToReach(numRows, vector<int>(numCols, std::numeric_limits<int>::max()));

        int numSteps = 0;
        vector<Coord> toExplore = {startCoord};
        bool reachedEnd = false;
        while (!toExplore.empty() && !reachedEnd)
        {
            //std::cout << "numSteps: " << numSteps << " #toExplore: " << toExplore.size() << std::endl;
            vector<Coord> toExploreNext;
            for (const auto& coord : toExplore)
            {
                if (coord == endCoord)
                    reachedEnd = true;

                const int directions[][2] = { {-1, 0},
                    {+1, 0},
                    { 0, -1},
                    { 0, +1} };
                for (const auto [dRow, dCol] : directions)
                {
                    const int neighbourCol = coord.col + dCol;
                    const int neighbourRow = coord.row + dRow;

                    if (neighbourCol < 0 || neighbourCol >= numCols || neighbourRow < 0 || neighbourRow >=  numRows )
                        continue;

                    if (static_cast<int>(grid[neighbourRow][neighbourCol]) - static_cast<int>(grid[coord.row][coord.col]) <= 1)
                    {
                        if (minStepsToReach[neighbourRow][neighbourCol] > numSteps + 1)
                        {
                            minStepsToReach[neighbourRow][neighbourCol] = min(minStepsToReach[neighbourRow][neighbourCol], numSteps + 1);
                            toExploreNext.push_back({neighbourRow, neighbourCol});
                        }
                    }
                }
            }
            toExplore = toExploreNext;
            numSteps++;
        }
#if 0
        for (int row = 0; row < numRows; row++)
        {
            for (int col = 0; col < numCols; col++)
            {
                std::cout << setw(3) << minStepsToReach[row][col];

            }
            std::cout << endl;
        }
#endif
        std::cout << minStepsToReach[endCoord.row][endCoord.col] << std::endl;
        minStepsFromAnyStartCoord = min(minStepsFromAnyStartCoord, minStepsToReach[endCoord.row][endCoord.col]);
    }
    std:: cout << "minStepsFromAnyStartCoord: " << minStepsFromAnyStartCoord << std::endl;
}


