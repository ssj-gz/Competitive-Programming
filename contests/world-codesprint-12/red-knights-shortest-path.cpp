#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int main()
{
    int n;
    cin >> n;

    vector<vector<int>> distToDestination(n, vector<int>(n, -1));

    struct Move
    {
        int dx;
        int dy;
        const char* printable;
    };

    const Move movesInPreferredOrder[] = { 
        {-1, -2, "UL"},
        {1, -2, "UR"},
        {2, 0, "R"},
        {1, 2, "LR"},
        {-1, 2, "LL"},
        {-2, 0, "L"}
    };

    int startX, startY;
    cin >> startY >> startX;

    int destX,  destY;
    cin >> destY >> destX;

    //cout << "startX: " << startX << " startY: " << startY << endl;
    //cout << "destX: " << destX << " destY: " << destY << endl;

    struct Position
    {
        Position(int x, int y)
            : x{x}, y{y}
        {
        }
        int x;
        int y;
        bool operator==(const Position& other)
        {
            return x == other.x && y == other.y;
        }

    };

    int distanceFromDest = 0;

    vector<Position> positionsToExplore = {{destX, destY}};
    while (!positionsToExplore.empty())
    {
        vector<Position> nextPositionsToExplore;
        for (const Position& pos : positionsToExplore)
        {
            if (distToDestination[pos.x][pos.y] != -1)
                continue;

            distToDestination[pos.x][pos.y] = distanceFromDest;

            for (const auto& move : movesInPreferredOrder)
            {
                const Position newPos = { pos.x + move.dx, pos.y + move.dy };
                if (newPos.x >= 0 && newPos.x < n && newPos.y >= 0 && newPos.y < n)
                    nextPositionsToExplore.push_back(newPos);
            }

        }

        positionsToExplore = nextPositionsToExplore;
        distanceFromDest++;
    }

    if (distToDestination[startX][startY] == -1)
        cout << "Impossible" << endl;
    else
    {
        const int distFromStartToDest = distToDestination[startX][startY];
        cout << distFromStartToDest << endl;
        int distTravelled = 0;

        const Position destination = { destX, destY };
        Position position = { startX, startY };
        while (!(position == destination))
        {
            bool moveMade = false;
            for (const auto nextMove : movesInPreferredOrder)
            {
                const Position newPos = { position.x + nextMove.dx, position.y + nextMove.dy };
                if (newPos.x < 0 || newPos.x >= n || newPos.y < 0 || newPos.y >= n)
                    continue;

                if (distToDestination[newPos.x][newPos.y] == distFromStartToDest - (distTravelled + 1))
                {
                    moveMade = true;
                    position = newPos;
                    cout << nextMove.printable << " ";
                    break;
                }
            }
            assert(moveMade);
            distTravelled++;
        }
    }

}
