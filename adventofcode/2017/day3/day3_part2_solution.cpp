#include <iostream>
#include <map>

#include <cassert>

using namespace std;

int main()
{
    struct Coord
    {
        int x = -1;
        int y = -1;
        auto operator<=>(const Coord& other) const = default;
    };
    int64_t maxNumber = -1;
    cin >> maxNumber;

    int squareSideLen = 1;
    int numberXCoord = 0;
    int numberYCoord = 0;
    std::map<Coord, int64_t> cellContents;
    cellContents[{numberXCoord, numberYCoord}] = 1;
    while (true)
    {
        auto sgn = [](const int x)
        {
            if (x < 0)
                return -1;
            else if (x == 0)
                return 0;
            else 
                return 1;

        };

        const int outerSquareSideLen = squareSideLen + 2;
        const int squareBRX = numberXCoord;
        const int squareBRY = numberYCoord;

        for (const auto& [squareX, squareY] : std::initializer_list<std::pair<int, int>>{ 
                // Move right one.
                {squareBRX + 1, squareBRY},
                // Move up to top-right corner of outer square.
                {squareBRX + 1, squareBRY - outerSquareSideLen + 2},
                // Move left to top-left corner of outer square.
                {squareBRX + 1 - outerSquareSideLen + 1, squareBRY - outerSquareSideLen + 2},
                // Move down to bottom-left corner of outer square.
                {squareBRX + 1 - outerSquareSideLen + 1, squareBRY + 1},
                // Move right to bottom-right corner of outer square.
                {squareBRX + 1, squareBRY + 1},

                })
        {
            const int numStepsToMove = abs(squareX - numberXCoord) + abs(squareY - numberYCoord);
            const int dx = sgn(squareX - numberXCoord);
            const int dy = sgn(squareY - numberYCoord);
            for (int step = 0; step < numStepsToMove; step++)
            {
                numberXCoord += dx;
                numberYCoord += dy;
                int64_t newCellContent = 0;
                assert(!cellContents.contains({numberXCoord, numberYCoord}));
                for (int neighbourX = numberXCoord - 1; neighbourX <= numberXCoord + 1; neighbourX++)
                {
                    for (int neighbourY = numberYCoord - 1; neighbourY <= numberYCoord + 1; neighbourY++)
                    {
                        if (neighbourX == numberXCoord && neighbourY == numberYCoord)
                            continue;
                        if (cellContents.contains({neighbourX, neighbourY}))
                        {
                            newCellContent += cellContents[{neighbourX, neighbourY}];
                        }
                    }
                }
                std::cout << "Writing " << newCellContent << " to " << numberXCoord << ", " << numberYCoord << std::endl;
                if (newCellContent > maxNumber)
                {
                    std::cout << "Done: result: " << newCellContent << std::endl;
                    return 0;
                }
                cellContents[{numberXCoord, numberYCoord}] = newCellContent;
            }
        }
        std::cout << "numberXCoord: " << numberXCoord << " numberYCoord: " << numberYCoord << std::endl;
        squareSideLen += 2;
    }
    return 0;
}
