#include <iostream>
#include <vector>
#include <set>
#include <limits>
#include <algorithm>
#include <cassert>


using namespace std;

struct Pos
{
    int x = -1;
    int y = -1;
    auto operator<=>(const Pos& other) const = default;
};

int main()
{
    set<Pos> elfPositions;
    int y = 0;
    string rawMapLine;
    while (getline(cin, rawMapLine))
    {
        for (int x = 0; x < rawMapLine.size(); x++)
        {
            if (rawMapLine[x] == '#')
                elfPositions.insert({x, y});
        }
        y++;
    }

    char directionOrder[] = { 'N', 'S', 'W', 'E' };

    auto proposedPositionFor = [&elfPositions, &directionOrder](int x, int y)
    {
        auto hasElf = [&elfPositions](int x, int y)
        {
            return elfPositions.contains({x, y});
        };

        bool hasNeigbour = false;
        for (int neighbourX = x - 1; neighbourX <= x + 1; neighbourX++)
        {
            for (int neighbourY = y - 1; neighbourY <= y + 1; neighbourY++)
            {
                if (neighbourX == x && neighbourY == y)
                    continue;
                if (hasElf(neighbourX, neighbourY))
                    hasNeigbour = true;
            }
        }
        if (!hasNeigbour)
            return Pos{x, y};
        for (const auto direction : directionOrder)
        {
            //std::cout << " direction: " << direction << std::endl;
            switch (direction)
            {
                case 'N':
                    if (!hasElf(x - 1, y - 1) && !hasElf(x, y - 1) && !hasElf(x + 1, y - 1)  )
                    {
                        return Pos{x, y - 1};
                    }
                    break;
                case 'S':
                    if (!hasElf(x - 1, y + 1) && !hasElf(x, y + 1) && !hasElf(x + 1, y + 1)  )
                    {
                        return Pos{x, y + 1};
                    }
                    break;
                case 'W':
                    if (!hasElf(x - 1, y - 1) && !hasElf(x - 1, y) && !hasElf(x - 1, y + 1))
                    {
                        return Pos{x - 1, y};
                    }
                    break;
                case 'E':
                    if (!hasElf(x + 1, y - 1) && !hasElf(x + 1, y) && !hasElf(x + 1, y + 1))
                    {
                        return Pos{x + 1, y};
                    }
                    break;
            };
        }
        return Pos{x, y};
    };

    auto printMap = [&elfPositions]()
    {
        int minX = std::numeric_limits<int>::max();
        int minY = std::numeric_limits<int>::max();
        int maxX = std::numeric_limits<int>::min();
        int maxY = std::numeric_limits<int>::min();
        for (const auto& elfPos : elfPositions)
        {
            minX = std::min(minX, elfPos.x);
            minY = std::min(minY, elfPos.y);
            maxX = std::max(maxX, elfPos.x);
            maxY = std::max(maxY, elfPos.y);
        }
        const int width = maxX - minX + 1;
        const int height = maxY - minY + 1;
        vector<vector<char>> map(width, vector<char>(height, '.'));
        for (const auto& elfPos : elfPositions)
        {
            map[elfPos.x - minX][elfPos.y - minY] = '#';
        }
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                cout << map[x][y];
            }
            std::cout << endl;
        }
    };

    cout << "Initial state: " << endl;
    printMap();

    int round = 1;
    while (true)
    {
        std::cout << "Round: " << round << std::endl;
#if 0
        std::cout << "Round: "<< round << " direction order: " << std::endl;
        for (const auto dir : directionOrder)
            std::cout << dir;
        std::cout << std::endl;
#endif

        set<Pos> proposedPositions;
        // Build up bannedNewPositions.
        set<Pos> bannedNewPositions;
        for (const auto& elfPos : elfPositions)
        {
            //std::cout << "Forming bannedNewPositions - Elf : " << elfPos.x << ", " << elfPos.y << std::endl;
            const Pos proposedNewPosition = proposedPositionFor(elfPos.x, elfPos.y);
            assert(!elfPositions.contains(proposedNewPosition) || proposedNewPosition == elfPos);
            if (proposedPositions.contains(proposedNewPosition))
                bannedNewPositions.insert(proposedNewPosition);
            proposedPositions.insert(proposedNewPosition);
        }
        // Compute next elf positions.
        set<Pos> nextElfPositions;

        for (const auto& elfPos : elfPositions)
        {
            //std::cout << "Computing nextElfPositions - Elf : " << elfPos.x << ", " << elfPos.y << std::endl;
            const Pos proposedNewPosition = proposedPositionFor(elfPos.x, elfPos.y);
            if (!bannedNewPositions.contains(proposedNewPosition))
            {
                assert(!nextElfPositions.contains(proposedNewPosition));
                nextElfPositions.insert(proposedNewPosition);
            }
            else
            {
                // Stay where we are.
                assert(!nextElfPositions.contains({elfPos.x, elfPos.y}));
                nextElfPositions.insert({elfPos.x, elfPos.y});
            }
        }
        assert(elfPositions.size() == nextElfPositions.size());
        if (elfPositions == nextElfPositions)
        {
            break;
        }
        elfPositions = nextElfPositions;
        std::rotate(std::begin(directionOrder), std::next(std::begin(directionOrder)), std::end(directionOrder)); 
        //std::cout << "At end of round " << round << ": " << endl;
        //printMap();

        round++;


    }
    std::cout << round << std::endl;

}
