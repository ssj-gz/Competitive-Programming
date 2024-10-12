#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <optional>

#include <cassert>

using namespace std;

enum Direction { Up, Right, Down, Left };
Direction directions[] = { Up, Right, Down, Left };
enum Turns { TurnLeft, StraightAhead, TurnRight };
Turns turnTypes[] = { TurnLeft, StraightAhead, TurnRight };

struct Cart
{
    int cartId = -1;
    int x = -1;
    int y = -1;
    Direction direction;
    int turnTypeIndex = 0;

    void tick(const vector<vector<char>>& map)
    {
        assert(x >= 0 && y >= 0 && x < map.size() && y < map.front().size());
        char cellContent = map[x][y];
        switch(cellContent)
        {
            case '-':
                assert(direction == Left || direction == Right);
                break;
            case '|':
                assert(direction == Up || direction == Down);
                break;
            case '/':
                std::cout << " cart: " << cartId << " at a '/' " << x << ", " << y << std::endl;
                switch (direction)
                {
                    case Up:
                        direction = Right;
                        break;
                    case Right:
                        direction = Up;
                        break;
                    case Down:
                        direction = Left;
                        break;
                    case Left:
                        direction = Down;
                        break;
                };
                break;
            case '\\':
                switch (direction)
                {
                    case Up:
                        direction = Left;
                        break;
                    case Left:
                        direction = Up;
                        break;
                    case Down:
                        direction = Right;
                        break;
                    case Right:
                        direction = Down;
                        break;
                };
                break;
            case '+':
                {
                    const Turns turnType = turnTypes[turnTypeIndex++ % std::size(turnTypes)];
                    const int directionIndex = std::distance(std::begin(directions), std::find(std::begin(directions), std::end(directions), direction));

                    switch (turnType)
                    {
                        case TurnLeft:
                            direction = directions[(directionIndex + 3) % std::size(directions)];
                            break;
                        case TurnRight:
                            direction = directions[(directionIndex + 1) % std::size(directions)];
                            break;
                        case StraightAhead:
                            break;
                    }
                }
                break;
            default:
                {
                    std::cout << "Whoops: " << x << ", " << y << " : " << cellContent << std::endl;
                    assert(false);
                }
        }
        const auto [dx, dy] = dxdy(direction);
        x += dx;
        y += dy;
    }

    std::pair<int, int> dxdy(Direction direction)
    {
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
        return {dx, dy};
    }
};

int main()
{
    string rawMapLine;
    vector<string> rawMap;
    while (getline(cin, rawMapLine))
    {
        rawMap.push_back(rawMapLine);
    }

    const int width = rawMap.front().size();
    const int height = rawMap.size();

    vector<vector<char>> map(width, vector<char>(height, ' '));
    list<Cart> carts;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            char cellContent = rawMap[y][x];
            const int nextCartId = static_cast<int>(carts.size());
            switch (cellContent)
            {
                case '^':
                    carts.push_back({nextCartId, x, y, Up});
                    cellContent = '|';
                    break;
                case '>':
                    carts.push_back({nextCartId, x, y, Right});
                    cellContent = '-';
                    break;
                case 'v':
                    carts.push_back({nextCartId, x, y, Down});
                    cellContent = '|';
                    break;
                case '<':
                    carts.push_back({nextCartId, x, y, Left});
                    cellContent = '-';
                    break;
            }
            map[x][y] = cellContent;
        }
    }

    while (true)
    {
        carts.sort([](const auto& lhsCart, const auto& rhsCart)
                {
                    if (lhsCart.y != rhsCart.y)
                        return lhsCart.y < rhsCart.y;
                    return lhsCart.x < rhsCart.x;
                });
        std::optional<list<Cart>::iterator> otherCrashCartIter;
        for (auto cartIter = carts.begin(); cartIter != carts.end();)
        {
            auto& cart = *cartIter;
            std::cout << "Tick!" << std::endl;
            cart.tick(map);
            std::cout << "Cart " << cart.cartId << " moved to " << cart.x << ", " << cart.y << std::endl;
            bool collisionOccured = false;
            for (auto otherCartIter = carts.begin(); otherCartIter != carts.end();)
            {
                const auto& otherCart = *otherCartIter;
                if (&otherCart != &cart)
                {
                    if (otherCart.x == cart.x && otherCart.y == cart.y)
                    {
                        std::cout << "Collision at " << cart.x << "," << cart.y << std::endl;
                        collisionOccured = true;
                        otherCartIter = carts.erase(otherCartIter);
                    }
                    else
                    {
                        otherCartIter++;
                    }
                }
                else
                    otherCartIter++;
            }
            if (collisionOccured)
                cartIter = carts.erase(cartIter);
            else
                cartIter++;
        }
        if (carts.size() == 1)
        {
            std::cout << "Final cart at: " << carts.front().x << "," << carts.front().y << std::endl;
            break;
        }
    }
    return 0;
}
