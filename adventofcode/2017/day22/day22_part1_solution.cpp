#include <iostream>
#include <set>
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

    set<Coord> infectedCoords;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            if (rawMap[y][x] == '#')
                infectedCoords.insert({x, y});

        }
    }
    assert(width % 2 == 1);
    assert(height % 2 == 1);
    int carrierX = width / 2;
    int carrierY = height / 2;
    enum Direction { Up, Right, Down, Left };
    Direction carrierDirection = Up;
    std::cout << "carrierX: " << carrierX << " carrierY: " << carrierY << std::endl;

    auto print = [&carrierX, &carrierY, &infectedCoords]()
    {
        int minX = std::numeric_limits<int>::max();
        int maxX = std::numeric_limits<int>::min();
        int minY = std::numeric_limits<int>::max();
        int maxY = std::numeric_limits<int>::min();
        for (const auto coord : infectedCoords)
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
                std::cout << (x == carrierX && y == carrierY ? '[' : ' ');
                std::cout << (infectedCoords.contains({x, y}) ? '#' : '.');
                std::cout << (x == carrierX && y == carrierY ? ']' : ' ');
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
    for (int burst = 1; burst <= 10000; burst++)
    {
        bool createdInfection = false;
        if (infectedCoords.contains({carrierX, carrierY}))
        {
            carrierDirection = static_cast<Direction>((static_cast<int>(carrierDirection) + 1) % 4);
            infectedCoords.erase({carrierX, carrierY});
        }
        else
        {
            carrierDirection = static_cast<Direction>((static_cast<int>(carrierDirection) + 3) % 4);
            infectedCoords.insert({carrierX, carrierY});
            createdInfection = true;
        }
        if (createdInfection)
            numBurstsThatCreatedInfection++;
        auto [dx, dy] = dxdy(carrierDirection);
        carrierX += dx;
        carrierY += dy;
    }
    std::cout << "numBurstsThatCreatedInfection: " << numBurstsThatCreatedInfection << std::endl;


    return 0;
}
