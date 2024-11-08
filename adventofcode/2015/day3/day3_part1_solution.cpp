#include <iostream>
#include <set>

#include <cassert>

using namespace std;

int main()
{
    string directions;
    std::getline(cin, directions);

    struct Coord
    {
        int x = -1;
        int y = -1;
        auto operator<=>(const Coord& other) const = default;
    };

    Coord currentCoord = {0, 0};
    set<Coord> coordsReceivingGift = { currentCoord };

    for (const auto directionChar : directions)
    {
        switch (directionChar)
        {
            case '^':
                currentCoord.y--;
                break;
            case '>':
                currentCoord.x++;
                break;
            case 'v':
                currentCoord.y++;
                break;
            case '<':
                currentCoord.x--;
                break;
            default:
                assert(false);
        }
        coordsReceivingGift.insert(currentCoord);
    }

    std::cout << "#coordsReceivingGift: " << coordsReceivingGift.size() << std::endl;
    return 0;
}
