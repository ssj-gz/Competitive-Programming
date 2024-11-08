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

    Coord santaCoord = {0, 0};
    Coord roboCoord = {0, 0};
    set<Coord> coordsReceivingGift = { santaCoord };

    bool isSantasTurn = true;
    for (const auto directionChar : directions)
    {
        auto& currentCoord = (isSantasTurn ? santaCoord : roboCoord);
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
        isSantasTurn = !isSantasTurn;
    }

    std::cout << "#coordsReceivingGift: " << coordsReceivingGift.size() << std::endl;
    return 0;
}
