#include <iostream>
#include <sstream>
#include <set>
#include <vector>

#include <cassert>

using namespace std;

struct Coord
{
    int x = -1;
    int y = -1;
    auto operator<=>(const Coord& other) const = default;
};

// Coordinate system:
//  * Starting hexagon is 0,0
//  * Given a hexagon at x,y:
//     * hexagon to the n is at x,y-1
//     * hexagon to the s is at x,y+1
//     * hexagon to the ne is x+1, y
//     * hexagon to the se is (therefore) x+1, y + 1
//     * hexagon to the sw is  x-1, y
//     * hexagon to the nw is (therefore) x-1, y-1
auto dxdy = [](const auto& dirString)
{
    int dx = 0;
    int dy = 0;
    if (dirString == "n")
    {
        dy = -1;
    }
    else if (dirString == "s")
    {
        dy = 1;
    }
    else if (dirString == "ne")
    {
        dx = 1;
    }
    else if (dirString == "se")
    {
        dx = 1;
        dy = 1;
    }
    else if (dirString == "nw")
    {
        dx = -1;
        dy = -1;
    }
    else if (dirString == "sw")
    {
        dx = -1;
    }
    else
    {
        assert(false);
    }
    return std::pair<int, int>(dx, dy);

};

#ifdef BRUTE_FORCE
int minDistanceBruteForce(int targetX, int targetY)
{
    int numSteps = 0;
    vector<Coord> toExplore = { {0, 0} };
    set<Coord> seen = { {0, 0}};
    while (!toExplore.empty())
    {
        vector<Coord> nextToExplore;
        for (const auto& coord : toExplore)
        {
            if (coord == Coord{targetX, targetY})
                return numSteps;
            for (const auto& dirString : { "n", "s", "ne", "nw", "se", "sw" })
            {
                const auto [dx, dy] = dxdy(dirString);
                Coord neighbourCoord = { coord.x + dx, coord.y + dy };
                if (!seen.contains(neighbourCoord))
                {
                    nextToExplore.push_back(neighbourCoord);
                    seen.insert(neighbourCoord);
                }
            }
        }

        toExplore = nextToExplore;
        numSteps++;
    }
    assert(false);
    return -1;
}
#endif

int main()
{
    string directions;
    std::getline(cin, directions);
    for (auto& character : directions)
    {
        if (character == ',')
            character = ' ';
    }


    istringstream dirStream(directions);
    string dir;
    int x = 0;
    int y = 0;
    while (dirStream >> dir)
    {
        const auto [dx, dy] = dxdy(dir);
        x += dx;
        y += dy;
    }
    std::cout << "Ended up at: " << x << ", " << y << std::endl;
    int minDistanceOpt = 0;
    auto sgn = [](int x)
    {
        if (x == 0)
            return 0;
        else if (x < 0)
            return -1;
        else
            return 1;
    };
    if (sgn(x) == sgn(y))
        minDistanceOpt = abs(abs(x) - abs(y)) + std::min(abs(x), abs(y));
    else
        minDistanceOpt = abs(x) + abs(y);
    std::cout << "minDistance: " << minDistanceOpt << std::endl;

#ifdef BRUTE_FORCE
    for (int x = -20; x <= 20; x++)
    {
        for (int y = -20; y <= 20; y++)
        {
            const int minDistanceBruteForce = ::minDistanceBruteForce(x, y);
            int minDistanceOpt = 0;
            if (sgn(x) == sgn(y))
                minDistanceOpt = abs(abs(x) - abs(y)) + std::min(abs(x), abs(y));
            else
                minDistanceOpt = abs(x) + abs(y);
            std::cout << "x: " << x << " y: " << y << " minDistanceBruteForce: " << minDistanceBruteForce << " minDistanceOpt: " << minDistanceOpt << std::endl;
            assert(minDistanceOpt == minDistanceBruteForce);

        }
    }
#endif

    return 0;
}
