#include <iostream>
#include <cassert>

using namespace std;

int main()
{
    string instruction;
    struct Direction 
    {
        int dx = 0;
        int dy = 0;
    };
    const Direction directions[] =
    {
        {0, -1},
        {1, 0},
        {0, +1},
        {-1, 0}
    };
    int64_t posX = 0;
    int64_t posY = 0;
    int64_t wayPointDx = +10;
    int64_t wayPointDy = -1;
    int direction = 1;
    while (cin >> instruction)
    {
        const char type = instruction.front();
        const int64_t value = stol(instruction.substr(1));

        cout << "type: " << type << " value: " << value << endl;

        switch (type)
        {
            case 'N':
                wayPointDy -= value;
                break;
            case 'E':
                wayPointDx += value;
                break;
            case 'S':
                wayPointDy += value;
                break;
            case 'W':
                wayPointDx -= value;
                break;
            case 'L':
                for (int i = 0; i < value / 90; i++)
                {
                    const int newWayPointDx = wayPointDy;
                    const int newWayPointDy = -wayPointDx;
                    wayPointDx = newWayPointDx;
                    wayPointDy = newWayPointDy;
                }
                break;
            case 'R':
                for (int i = 0; i < value / 90; i++)
                {
                    const int newWayPointDx = -wayPointDy;
                    const int newWayPointDy = wayPointDx;
                    wayPointDx = newWayPointDx;
                    wayPointDy = newWayPointDy;
                }

                break;
            case 'F':
                posX += wayPointDx * value;
                posY += wayPointDy * value;
                break;
            default:
                assert(false);

        }
        cout << "posX: " << posX << " posY: " << posY << endl;
        cout << "wayPointDx: " << wayPointDx << " wayPointDy: " << wayPointDy << endl;
    }
    cout << (abs(posX) + abs(posY)) << endl;

}
