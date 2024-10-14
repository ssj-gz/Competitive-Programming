#include <iostream>
#include <regex>
#include <limits>
#include <set>

#include <cassert>

using namespace std;

struct Coord
{
    int x = -1;
    int y = -1;
    auto operator<=>(const Coord& other) const = default;
};

ostream& operator<<(ostream& os, const Coord& coord)
{
    os << "(" << coord.x << ", " << coord.y << ")";
    return os;
}

enum Direction { Down, Left, Right };

ostream& operator<<(ostream& os, const Direction& direction)
{
    switch (direction)
    {
        case Down:
            os << "Down";
            break;
        case Left:
            os << "Left";
            break;
        case Right:
            os << "Right";
            break;
    };
    return os;
}


int main()
{
    std::regex scanRegex(R"(^([xy])=(\d+),\s*([xy])=(\d+)\.\.(\d+)$)");
    string scanLine;
    map<Coord, char> areaContents;
    while(getline(cin, scanLine))
    {
        std::smatch scanMatch;
        const bool matchSuccessful = std::regex_match(scanLine, scanMatch, scanRegex);
        assert(matchSuccessful);
        if (std::string(scanMatch[1]) == "x")
        {
            assert(std::string(scanMatch[3]) == "y");
            const int x = std::stoi(scanMatch[2]);
            const int topY = std::stoi(scanMatch[4]);
            const int bottomY = std::stoi(scanMatch[5]);
            assert(topY <= bottomY);
            for (int y = topY; y <= bottomY; y++)
            {
                areaContents[{x, y}] = '#';
            }
        }
        else if (std::string(scanMatch[1]) == "y")
        {
            assert(std::string(scanMatch[3]) == "x");
            const int y = std::stoi(scanMatch[2]);
            const int topX = std::stoi(scanMatch[4]);
            const int bottomX = std::stoi(scanMatch[5]);
            assert(topX <= bottomX);
            for (int x = topX; x <= bottomX; x++)
            {
                areaContents[{x, y}] = '#';
            }
        }
        else
        {
            assert(false);
        }
    }

    int minY = std::numeric_limits<int>::max();
    int maxY = std::numeric_limits<int>::min();
    for (const auto& [coord, contents] : areaContents)
    {
        minY = std::min(minY, coord.y);
        maxY = std::max(maxY, coord.y);
    }

    auto contents = [&areaContents](const Coord& coord)
    {
        if (!areaContents.contains(coord))
            return '.';
        else
            return areaContents[coord];
    };

    auto canMoveTo = [&contents](const Coord& coord)
    {
        return contents(coord) == '.' || contents(coord) == '|';
    };

    auto printArea = [&areaContents, &contents, minY, maxY]()
    {
        return;
        int minX = std::numeric_limits<int>::max();
        int maxX = std::numeric_limits<int>::min();
        for (const auto& [coord, contents] : areaContents)
        {
            minX = std::min(minX, coord.x);
            maxX = std::max(maxX, coord.x);
        }
        for (int y = minY; y <= maxY; y++)
        {
            for (int x = minX; x <= maxX; x++)
            {
                std::cout << contents({x,y});
                        
            }
            std::cout << std::endl;
        }
    };

    printArea();

    struct WaterDroplet
    {
        Coord coord;
        Direction direction;
        auto operator<=>(const WaterDroplet& other) const = default;
    };

    int numVisited = 0;
    auto markVisited = [&numVisited, &contents, &areaContents, minY, maxY](const Coord& coord)
    {
        if (contents(coord) == '.')
        {
            if (coord.y >= minY && coord.y <= maxY)
                numVisited++;
            areaContents[coord] = '|';
        }
    };
    int numRetained = 0;
    auto markRetained = [&numRetained, &contents, &areaContents, minY, maxY](const Coord& coord)
    {
        if (contents(coord) != '~' )
        {
            if (coord.y >= minY && coord.y <= maxY)
                numRetained++;
            areaContents[coord] = '~';
        }
    };
    while (true)
    {
        vector<WaterDroplet> toExplore = { {{500, 0}, Down} };
        const int oldNumVisited = numVisited;
        while (!toExplore.empty())
        {
            set<WaterDroplet> nextToExplore;
            for (auto& droplet : toExplore)
            {
                markVisited(droplet.coord);
                printArea();
                while (true)
                {
                    Coord newCoord = droplet.coord;
                    switch (droplet.direction)
                    {
                        case Down:
                            newCoord.y++;
                            break;
                        case Left:
                            newCoord.x--;
                            break;
                        case Right:
                            newCoord.x++;
                            break;
                    }

                    if (newCoord.y > maxY)
                    {
                        // Fell off the bottom, into the abyss; ditch
                        // this droplet.
                        break;
                    }

                    if (!canMoveTo(newCoord))
                    {
                        if (droplet.direction == Down)
                        {
                            // Hit the ground - can we escape to the sides, or must we fill
                            // from left to right?
                            auto checkXDirection = [&canMoveTo, &dropletCoord = droplet.coord, &markVisited](const int dx)
                            {
                                Coord coordToCheck = dropletCoord;
                                while (true)
                                {
                                    if (canMoveTo({coordToCheck.x, coordToCheck.y + 1}))
                                        return coordToCheck;
                                    if (canMoveTo(coordToCheck))
                                    {
                                        markVisited(coordToCheck);
                                        coordToCheck.x += dx;
                                    }
                                    else
                                    {
                                        return coordToCheck;
                                    }
                                }
                            };
                            const auto leftwardImportantCoord = checkXDirection(-1);
                            const auto rightwardImportantCoord = checkXDirection(+1);
                            const bool isFreeToLeft = canMoveTo(leftwardImportantCoord);
                            const bool isFreeToRight = canMoveTo(rightwardImportantCoord);
                            if (!isFreeToRight && !isFreeToLeft)
                            {
                                // Can't escape to the sides; mark to left and right with ~.
                                // (these tiles have already been markVisited).
                                assert(leftwardImportantCoord.y == droplet.coord.y && rightwardImportantCoord.y == droplet.coord.y);
                                for (int x = leftwardImportantCoord.x + 1; x <= rightwardImportantCoord.x - 1; x++)
                                {
                                    markRetained({x, droplet.coord.y});
                                }

                                break;
                            }
                            else if (isFreeToLeft && isFreeToRight)
                            {
                                droplet.direction = Left;
                                // Fork to the Right.
                                nextToExplore.insert({droplet.coord, Right});
                            }
                            else if (isFreeToLeft)
                            {
                                droplet.direction = Left;
                            }
                            else if (isFreeToRight)
                            {
                                droplet.direction = Right;
                            }
                            else
                            {
                                assert(false);
                            }
                        }
                        else
                        {
                            // Unusual situation where we could initially move horizontally
                            // or vertically, but then our escape point overflowed before
                            // we could move, and we got stuck.  Just ditch this
                            // droplet.
                            break;
                        }
                    }
                    else
                    {
                        droplet.coord = newCoord;
                        markVisited(newCoord);
                        // Start falling, if we can.
                        if (canMoveTo({droplet.coord.x, droplet.coord.y + 1}))
                            droplet.direction = Down;
                    }
                }

            }
            toExplore.assign(nextToExplore.begin(), nextToExplore.end());
        }
        if (oldNumVisited == numVisited)
        {
            // No progress; abort the simulation.
            break;
        }
    }
    std::cout << "numVisited: " << numVisited << " numRetained: " << numRetained << std::endl;
    return 0;
}
