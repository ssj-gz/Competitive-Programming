#include <iostream>
#include <vector>
#include <cassert>
#include <sys/time.h>


using namespace std;

enum class CellType
{
    Free,
    Mine,
    Exit,
    Wall,
    Tunnel
};

bool simulateRun(const vector<vector<CellType>>& cellTypes, const vector<vector<std::pair<int,int>>>& tunnelDestinations, int frogStartX, int frogStartY)
{
    const std::pair<int, int> possibleDirections[] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
    const int width = cellTypes.size();
    const int height = cellTypes[0].size();
    while (true)
    {
        std::pair<int, int> directionsCanMoveIn[4];
        int numDirectionsCanMoveIn = 0;
        for (const auto direction : possibleDirections)
        {
            const int newFrogX = frogStartX + direction.first;
            const int newFrogY = frogStartY + direction.second;

            if (newFrogX < 0 || newFrogX >= width)
                continue;
            if (newFrogY < 0 || newFrogY >= height)
                continue;
            if (cellTypes[newFrogX][newFrogY] == CellType::Wall)
                continue;

            directionsCanMoveIn[numDirectionsCanMoveIn] = direction;
            numDirectionsCanMoveIn++;
        }

        if (numDirectionsCanMoveIn == 0)
        {
            // Stuck.
            return false;
        }
        const int directionIndex = rand() % numDirectionsCanMoveIn;
        int newFrogX = frogStartX + directionsCanMoveIn[directionIndex].first;
        int newFrogY = frogStartY + directionsCanMoveIn[directionIndex].second;

        switch(cellTypes[newFrogX][newFrogY])
        {
            case CellType::Free:
                // Do nothing.
                break;
            case CellType::Wall:
                assert(false);
                break;
            case CellType::Mine:
                return false;
            case CellType::Exit:
                return true;
            case CellType::Tunnel:
                newFrogX = tunnelDestinations[frogStartX][frogStartY].first;
                newFrogY = tunnelDestinations[frogStartX][frogStartY].second;
                break;
        }

        frogStartX = newFrogX;
        frogStartY = newFrogY;
    }
}

double simulate(const vector<vector<CellType>>& cellTypes, const vector<vector<std::pair<int,int>>>& tunnelDestinations, int frogStartX, int frogStartY)
{
    int numSims = 0;
    int numEscapes = 0;
    for (int sim = 0; sim < 100'000; sim++)
    {
        if (simulateRun(cellTypes, tunnelDestinations, frogStartX, frogStartY))
            numEscapes++;
        numSims++;
    }

    return static_cast<double>(numEscapes) / numSims;
}

int main()
{
    struct timeval time;
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

    auto readInt = [](){int x; cin >> x; return x;};

    const int height = readInt();
    const int width = readInt();
    const int numTunnels = readInt();

    vector<vector<CellType>> cellTypes(width, vector<CellType>(height, CellType::Free));
    vector<vector<std::pair<int,int>>> tunnelDestinations(width, vector<std::pair<int, int>>(height, {-1, -1}));

    int frogStartX = -1;
    int frogStartY = -1;

    for (int y = 0; y < height; y++)
    {
        string rowDescription;
        cin >> rowDescription;
        assert(rowDescription.size() == width);

        for (int x = 0; x < width; x++)
        {
            CellType cellType = CellType::Free;
            switch (rowDescription[x])
            {
                case '#':
                    cellType = CellType::Wall;
                    break;
                case '*':
                    cellType = CellType::Mine;
                    break;
                case '%':
                    cellType = CellType::Exit;
                    break;
                case 'A':
                    frogStartX = x;
                    frogStartY = y;
                    break;
            };
            cellTypes[x][y] = cellType;
        }
    }

    for (int tunnelNum = 0; tunnelNum < numTunnels; tunnelNum++)
    {
        const int beginY = readInt() - 1;
        const int beginX = readInt() - 1;
        const int endY = readInt() - 1;
        const int endX = readInt() - 1;

        cellTypes[beginX][beginY] = CellType::Tunnel;
        cellTypes[endX][endY] = CellType::Tunnel;

        tunnelDestinations[beginX][beginY] = {endX, endY};
        tunnelDestinations[endX][endY] = {beginX, beginY};
    }

    const double result = simulate(cellTypes, tunnelDestinations, frogStartX, frogStartY);
    cout << result << endl;
}
