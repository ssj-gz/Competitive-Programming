#include "../shared/intcodecomputer.h"

#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <iterator>
#include <algorithm>
#include <limits>
#include <cassert>

using namespace std;

int main()
{
    vector<int64_t> program;
    int64_t programInput;
    while (true)
    {
        cin >> programInput;
        program.push_back(programInput);
        assert(cin);
        char comma;
        cin >> comma;
        if (!cin)
            break;
        assert(comma == ',');
    }

    IntCodeComputer intCodeComputer(program);
    struct Coord
    {
        int x = -1;
        int y = -1;
        auto operator<=>(const Coord&) const = default;
    };
    Coord robotCoord{0, 0};
    enum Direction {Up, Right, Down, Left}; // Clockwise order.
    Direction robotDirection = Up;
    // 0 = black, 1 = white.
    map<Coord, int> coordColour;
    coordColour[robotCoord] = 1;
    while (true)
    {
        intCodeComputer.addInput(coordColour[robotCoord]);
        const auto status = intCodeComputer.run();
        if (status == IntCodeComputer::Terminated)
        {
            cout << "Done" << endl;
            break;
        }
        auto output = intCodeComputer.output();
        const int outputSize = static_cast<int>(output.size());
        intCodeComputer.clearOutput();
        cout << "outputSize: " << outputSize << endl;
        assert(outputSize == 2);

        {
            const int colourToPaint = static_cast<int>(output[0]);
            const int64_t directionToTurn = output[1];
            coordColour[robotCoord] = colourToPaint;
            cout << "colourToPaint: " << colourToPaint << " directionToTurn: " << directionToTurn << endl;

            if (directionToTurn == 0)
            {
                robotDirection = static_cast<Direction>((static_cast<int>(robotDirection) + 4 - 1) % 4);
            }
            else
            {
                robotDirection = static_cast<Direction>((static_cast<int>(robotDirection) + 1) % 4);
            }

            switch (robotDirection)
            {
                case Up:
                    cout << "Now facing Up" << endl;
                    robotCoord.y--;
                    break;
                case Down:
                    cout << "Now facing Down" << endl;
                    robotCoord.y++;
                    break;
                case Left:
                    cout << "Now facing Left" << endl;
                    robotCoord.x--;
                    break;
                case Right:
                    cout << "Now facing Right" << endl;
                    robotCoord.x++;
                    break;
            }
            cout << " new coord: " << robotCoord.x << "," << robotCoord.y << endl;
            //output.erase(output.begin());
            //output.erase(output.begin());

        }
#if 0
        else
        {
            assert(false);
            cout << "Got no output; not moving robot" << endl;
        }
#endif
    }
    cout << "Painted: " << coordColour.size() << " cells!" << endl;
    int minX = std::numeric_limits<int>::max();
    int minY = std::numeric_limits<int>::max();
    int maxX = std::numeric_limits<int>::min();
    int maxY = std::numeric_limits<int>::min();
    for (const auto& [coord, colour] : coordColour)
    {
        minX = min(minX, coord.x);
        minY = min(minY, coord.y);
        maxX = max(maxX, coord.x);
        maxY = max(maxY, coord.y);
    }
    const int width = maxX - minX + 1;
    const int height = maxY - minY + 1;
    vector<string> paintedGrid(height, string(width, ' '));
    for (const auto& [coord, colour] : coordColour)
    {
        const int row = coord.y - minY;
        const int col = coord.x - minX;
        paintedGrid[row][col] = (colour == 0 ? ' ' : 'X');
    }
    for (const auto& paintedRow : paintedGrid)
        cout << paintedRow << endl; 
}
