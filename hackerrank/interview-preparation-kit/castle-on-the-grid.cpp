#include <iostream>
#include <vector>
#include <set>

using namespace std;

struct Coord
{
    int x = -1;
    int y = -1;
};

bool operator<(const Coord& lhs, const Coord& rhs)
{
    if (lhs.x != rhs.x)
        return lhs.x < rhs.x;
    return lhs.y < rhs.y;
}
bool operator==(const Coord& lhs, const Coord& rhs)
{
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

int findShortestPathToGoal(const vector<string>& grid, const Coord& startCoord, const Coord& goalCoord)
{
    auto canMoveTo = [&grid](const int x, const int y)
    {
        if (y < 0 || y >= grid.size())
            return false;
        if (x < 0 || x >= grid.front().size())
            return false;
        return (grid[y][x] == '.');
    };

    set<Coord> visitedCoords = { startCoord };
    vector<Coord> coordsToExplore = { startCoord };

    int numMoves = 0;
    bool found = false;
    while (!coordsToExplore.empty() && !found)
    {
        vector<Coord> nextCoordsToExplore;
        for (const auto& coord : coordsToExplore)
        {
            if (coord == goalCoord)
            {
                found = true;
                break;
            }

            const pair<int, int> directions[] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
            for (const auto direction : directions)
            {
                const int dx = direction.first;
                const int dy = direction.second;
                int x = coord.x;
                int y = coord.y;

                while (canMoveTo(x + dx, y + dy))
                {
                    x += dx;
                    y += dy;
                    const Coord coordAfterMove = { x, y };
                    if (visitedCoords.find(coordAfterMove) == visitedCoords.end())
                    {
                        visitedCoords.insert(coordAfterMove);
                        nextCoordsToExplore.push_back(coordAfterMove);
                    }
                }

            }
        }
        if (!found)
        {
            coordsToExplore = nextCoordsToExplore;
            numMoves++;
        }
    }

    return numMoves;
}

int main()
{
    int n;
    cin >> n;

    vector<string> grid(n);

    for (int i = 0; i < n; i++)
    {
        cin >> grid[i];
    }

    int startX, startY;
    cin >> startX >> startY;

    int goalX, goalY;
    cin >> goalX >> goalY;

    // *Sigh* - looks like the X/Y in the Problem Description 
    // are actually Row/Col (!)
    const Coord startCoord = { startY, startX };
    const Coord goalCoord = { goalY, goalX };


    cout << findShortestPathToGoal(grid, startCoord, goalCoord) << endl;
}

