#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>

using namespace std;

int main()
{
    int headX = 0;
    int headY = 0;
    int tailX = headX;
    int tailY = headY;

    std::set<std::pair<int, int>> tailVisitedCoords;

    auto recordTailPos = [&]()
    {
        tailVisitedCoords.insert({tailX, tailY});
    };

    recordTailPos();
    while (true)
    {
        auto printGrid = [&]()
        {
            vector<string> grid(5, string(6, '.'));
            grid[0][0] = 's';
            grid[tailY][tailX] = 'T';
            grid[headY][headX] = 'H';
            std::reverse(grid.begin(), grid.end());
            std::cout << endl;
            for (const auto& gridRow : grid)
            {
                std::cout << gridRow << std::endl;
            }
        };

        char dirChar;
        cin >> dirChar;
        if (!cin)
            break;
        int numSteps;
        cin >> numSteps;

        std::cout << "Moving " << numSteps << " " << dirChar << std::endl;

        //printGrid();

        for (int stepNum = 0; stepNum < numSteps; stepNum++)
        {
            switch (dirChar)
            {
                case 'R':
                    headX++;
                    break;
                case 'L':
                    headX--;
                    break;
                case 'U':
                    headY++;
                    break;
                case 'D':
                    headY--;
                    break;
                default:
                    assert(false);
            }

            auto sgn = [](int x)
            {
                if (x < 0)
                    return -1;
                else if (x == 0)
                    return 0;
                else 
                    return +1;
            };

            auto areTouching = [&]()
            {
                const int manhattanDistance = abs(headX - tailX) + abs(headY - tailY);
                if (headX != tailX && headY != tailY)
                {
                    return manhattanDistance <= 2;
                }
                else
                    return manhattanDistance <= 1;
            };

            if (!areTouching())
            {
#if 0
                if (headX == tailX)
                {
                    tailY += sgn(headY - tailY);
                }
                else if (headY == tailY)
                {
                    tailX += sgn(headX - tailX);
                }
                else
                {
                    // Diagonal.
                }
#endif
                tailX += sgn(headX - tailX);
                tailY += sgn(headY - tailY);
                assert(areTouching());
            }

            recordTailPos();

            //printGrid();

        }

    }

    std::cout << "# tail visited coords: " << tailVisitedCoords.size() << std::endl;
}
