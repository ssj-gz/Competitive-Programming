#include <iostream>
#include <map>
#include <vector>
#include <limits>

#include <cassert>

using namespace std;

int main()
{
    vector<string> rawMap;
    string rawMapLine;
    while (std::getline(cin, rawMapLine))
    {
        rawMap.push_back(rawMapLine);
    }
    const int width = rawMap.front().size();
    const int height = rawMap.size();

    struct Coord
    {
        int x = -1;
        int y = -1;
        auto operator<=>(const Coord& other) const = default;
    };

    enum NodeState { Clean, Weakened, Infected, Flagged };
    map<Coord, NodeState> stateOfCoord;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            if (rawMap[y][x] == '#')
                stateOfCoord[{x, y}] = Infected;

        }
    }
    assert(width % 2 == 1);
    assert(height % 2 == 1);
    Coord carrierCoord = { width / 2, height / 2};
    enum Direction { Up, Right, Down, Left };
    Direction carrierDirection = Up;

    auto print = [&carrierCoord, &stateOfCoord]()
    {
        int minX = std::numeric_limits<int>::max();
        int maxX = std::numeric_limits<int>::min();
        int minY = std::numeric_limits<int>::max();
        int maxY = std::numeric_limits<int>::min();
        for (const auto [coord, nodeState] : stateOfCoord)
        {
            minX = std::min(minX, coord.x);
            maxX = std::max(maxX, coord.x);
            minY = std::min(minY, coord.y);
            maxY = std::max(maxY, coord.y);
        }
        const int width = maxX - minX + 1;
        const int height = maxY - minY + 1;
        for (int y = minY; y <= maxY; y++)
        {
            for (int x = minX; x <= maxX; x++)
            {
                std::cout << (x == carrierCoord.x && y == carrierCoord.y ? '[' : ' ');
                switch (stateOfCoord[{x, y}])
                {
                    case Clean:
                        std::cout << '.';
                        break;
                    case Weakened:
                        std::cout << 'W';
                        break;
                    case Infected:
                        std::cout << '#';
                        break;
                    case Flagged:
                        std::cout << 'F';
                        break;
                }
                std::cout << (x == carrierCoord.x && y == carrierCoord.y ? ']' : ' ');
            }
            std::cout << std::endl;
        }
    };

    auto dxdy = [](const Direction direction)
    {
        int dx = 0;
        int dy = 0;
        switch (direction)
        {
            case Up:
                dy = -1;
                break;
            case Right:
                dx = 1;
                break;
            case Down:
                dy = 1;
                break;
            case Left:
                dx = -1;
                break;
        }
        return std::pair {dx, dy};
    };

    print();
    int numBurstsThatCreatedInfection = 0;
    for (int burst = 1; burst <= 10'000'000; burst++)
    {
        bool createdInfection = false;
        switch (stateOfCoord[carrierCoord])
        {
            case Clean:
                carrierDirection = static_cast<Direction>((static_cast<int>(carrierDirection) + 3) % 4);
                stateOfCoord[carrierCoord] = Weakened;
                break;
            case Weakened:
                stateOfCoord[carrierCoord] = Infected;
                createdInfection = true;
                break;
            case Infected:
                carrierDirection = static_cast<Direction>((static_cast<int>(carrierDirection) + 1) % 4);
                stateOfCoord[carrierCoord] = Flagged;
                break;
            case Flagged:
                carrierDirection = static_cast<Direction>((static_cast<int>(carrierDirection) + 2) % 4);
                stateOfCoord[carrierCoord] = Clean;
                break;
        }
        if (createdInfection)
            numBurstsThatCreatedInfection++;

        auto [dx, dy] = dxdy(carrierDirection);
        carrierCoord.x += dx;
        carrierCoord.y += dy;
    }
    std::cout << "numBurstsThatCreatedInfection: " << numBurstsThatCreatedInfection << std::endl;


    return 0;
}
