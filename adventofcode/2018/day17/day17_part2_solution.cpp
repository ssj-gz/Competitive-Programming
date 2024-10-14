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
    int numRetained = 0;
    auto markVisited = [&numVisited, &contents, &areaContents, minY, maxY](const Coord& coord)
    {
        if (contents(coord) == '.' && coord.y >= minY && coord.y <= maxY)
            numVisited++;
        if (contents(coord) == '.')
            areaContents[coord] = '|';
    };
    auto markRetained = [&numRetained, &contents, &areaContents, minY, maxY](const Coord& coord)
    {
        if (contents(coord) != '~' && coord.y >= minY && coord.y <= maxY)
        {
            numRetained++;
        }
        if (contents(coord) != '~')
            areaContents[coord] = '~';
    };
    while (true)
    {
        vector<WaterDroplet> toExplore = { {{500, 0}, Down} };
        std::cout << "new drop" << std::endl;
        const int oldNumVisited = numVisited;
        while (!toExplore.empty())
        {
            set<WaterDroplet> nextToExplore;
            for (auto& droplet : toExplore)
            {
                markVisited(droplet.coord);
                std::cout << "Beginning to move droplet at: " << droplet.coord << " direction: " << droplet.direction << std::endl;
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

                    std::cout << " trying to move to coord: " << newCoord << " contents: " << contents(newCoord) << std::endl;
                    if (newCoord.y > maxY)
                    {
                        std::cout << "Fell off the bottom" << std::endl;
                        break;
                    }

                    if (contents(newCoord) == '~' || contents(newCoord) == '#')
                    {
                        if (droplet.direction == Down)
                        {
                            std::cout << " hit ground" << std::endl;
                            bool isFreeToLeft = true;
                            {
                                Coord toLeft = droplet.coord;
                                while (true)
                                {
                                    if (contents({toLeft.x, toLeft.y + 1}) == '.' || contents({toLeft.x, toLeft.y + 1}) == '|')
                                        break;
                                    if (contents(toLeft) == '.' || contents(toLeft) == '|')
                                    {
                                        markVisited(toLeft);
                                        toLeft.x--;
                                    }
                                    else
                                    {
                                        isFreeToLeft = false;
                                        break;
                                    }
                                }
                            }
                            bool isFreeToRight = true;
                            {
                                Coord toRight = droplet.coord;
                                while (true)
                                {
                                    if (contents({toRight.x, toRight.y + 1}) == '.' || contents({toRight.x, toRight.y + 1}) == '|')
                                        break;
                                    if (contents(toRight) == '.' || contents(toRight) == '|')
                                    {
                                        markVisited(toRight);
                                        toRight.x++;
                                    }
                                    else
                                    {
                                        isFreeToRight = false;
                                        break;
                                    }
                                }
                            }
                            std::cout << " isFreeToLeft? " << isFreeToLeft << " isFreeToRight? " << isFreeToRight << std::endl;
                            if (!isFreeToRight && !isFreeToLeft)
                            {
                                // Can't escape to the sides; mark to left and right with ~.
                                // (these tiles have already been markVisited).
                                {
                                    Coord toLeft = droplet.coord;
                                    while (true)
                                    {
                                        if (contents(toLeft) == '.' || contents(toLeft) == '|')
                                        {
                                            markRetained(toLeft);
                                            toLeft.x--;
                                        }
                                        else
                                        {
                                            break;
                                        }
                                    }
                                }
                                {
                                    Coord toRight = droplet.coord;
                                    toRight.x++;
                                    while (true)
                                    {
                                        if (contents(toRight) == '.' || contents(toRight) == '|')
                                        {
                                            markRetained(toRight);
                                            toRight.x++;
                                        }
                                        else
                                        {
                                            break;
                                        }
                                    }
                                }
                                break;
                            }
                            else if (isFreeToLeft && isFreeToRight)
                            {
                                droplet.direction = Left;
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
                            //assert(false);
                            //areaContents[droplet.coord] = '~';
                            std::cout << "Stopped at " << droplet.coord << std::endl;
                            break;
                        }
                    }
                    else
                    {
                        droplet.coord = newCoord;
                        std::cout <<  " moved to " << droplet.coord << std::endl;
                        markVisited(newCoord);
                        const auto underfoot = contents({droplet.coord.x, droplet.coord.y + 1});
                        if (underfoot == '.' || underfoot == '|')
                            droplet.direction = Down;
                    }
                }

            }
            toExplore.assign(nextToExplore.begin(), nextToExplore.end());
        }
        std::cout << "numVisited: " << numVisited << std::endl;
        if (oldNumVisited == numVisited)
        {
            break;
        }
    }
    std::cout << "numVisited: " << numVisited << std::endl;
    std::cout << "numRetained: "<< numRetained << std::endl;
    return 0;
}
