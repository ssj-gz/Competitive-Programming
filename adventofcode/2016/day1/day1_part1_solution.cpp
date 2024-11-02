#include <iostream>
#include <algorithm>
#include <sstream>

#include <cassert>

using namespace std;

int main()
{
    string directionsList;
    std::getline(cin, directionsList);
    std::replace(directionsList.begin(), directionsList.end(), ',', ' ');
    istringstream directionsStream(directionsList);

    enum Direction { Up, Right, Down, Left};
    Direction direction = Up;
    int posX = 0;
    int posY = 0;

    string directionDescr;
    while (directionsStream >> directionDescr)
    {
        if (directionDescr.front() == 'L')
        {
            direction = static_cast<Direction>((static_cast<int>(direction) + 3) % 4);
        } 
        else if (directionDescr.front() == 'R')
        {
            direction = static_cast<Direction>((static_cast<int>(direction) + 1) % 4);
        }
        else
            assert(false);

        const int numSteps = std::stoi(directionDescr.substr(1));
        int dx = 0;
        int dy = 0;
        switch (direction)
        {
            case Up:
                dy = -1;
                break;
            case Right:
                dx = +1;
                break;
            case Down:
                dy = +1;
                break;
            case Left:
                dx = -1;
                break;
        }
        posX += numSteps * dx;
        posY += numSteps * dy;
    }
    std::cout << "distance: " << (abs(posX) + abs(posY)) << std::endl;

    return 0;
}
