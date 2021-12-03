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
    int direction = 1;
    while (cin >> instruction)
    {
        const char type = instruction.front();
        const int64_t value = stol(instruction.substr(1));

        cout << "type: " << type << " value: " << value << endl;

        switch (type)
        {
            case 'N':
                posY -= value;
                break;
            case 'E':
                posX += value;
                break;
            case 'S':
                posY += value;
                break;
            case 'W':
                posX -= value;
                break;
            case 'L':
                direction = direction - value / 90;
                break;
            case 'R':
                direction = direction + value / 90;
                break;
            case 'F':
                while (direction < 0)
                    direction += 4;
                direction = direction % 4;
                posX += value * directions[direction].dx;
                posY += value * directions[direction].dy;
                break;
            default:
                assert(false);

        }
        cout << "posX: " << posX << " posY: " << posY << endl;
    }
    cout << (abs(posX) + abs(posY)) << endl;

}
