#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>

using namespace std;

struct Coord
{
    int x = -1;
    int y = -1;

    void moveNextTo(const Coord& other)
    {
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
            const int manhattanDistance = abs(other.x - x) + abs(other.y - y);
            if (other.x != x && other.y != y)
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
            x += sgn(other.x - x);
            y += sgn(other.y - y);
            assert(areTouching());
        }
    }

    auto operator<=>(const Coord& other) const = default;
};

int main()
{
    const int numKnots = 10;
    vector<Coord> knotCoords(numKnots, Coord{0, 0});

    std::set<Coord> tailVisitedCoords;

    auto recordTailPos = [&]()
    {
        tailVisitedCoords.insert(knotCoords.back());
    };

    recordTailPos();
    while (true)
    {
        auto printGrid = [&]()
        {
            vector<string> grid(5, string(6, '.'));
            grid[0][0] = 's';
            for (int knotIndex = numKnots - 1; knotIndex >= 0; knotIndex--)
            {
                const char knotChar = (knotIndex != 0 ? static_cast<char>(knotIndex + '0') : 'H');
                const auto knotCoord = knotCoords[knotIndex];
                grid[knotCoord.y][knotCoord.x] = knotChar;
            }
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
                    knotCoords.front().x++;
                    break;
                case 'L':
                    knotCoords.front().x--;
                    break;
                case 'U':
                    knotCoords.front().y++;
                    break;
                case 'D':
                    knotCoords.front().y--;
                    break;
                default:
                    assert(false);
            }

            for (int knotIndex = 1; knotIndex < numKnots; knotIndex++)
            {
                knotCoords[knotIndex].moveNextTo(knotCoords[knotIndex - 1]);
            }


            recordTailPos();

            //printGrid();

        }

    }

    std::cout << "# tail visited coords: " << tailVisitedCoords.size() << std::endl;
}
