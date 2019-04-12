#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#endif
#include <iostream>
#include <vector>
#include <cassert>
#include <iomanip>
#include <limits>
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
    //cout << "simulate run" << endl;
    const std::pair<int, int> possibleDirections[] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
    const int width = cellTypes.size();
    const int height = cellTypes[0].size();
    while (true)
    {
        //cout << "Frog at (" << frogStartX << "," << frogStartY << ")" << endl;
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
            //cout << " can move in direction: " << direction.first << " " << direction.second << endl;
        }

        if (numDirectionsCanMoveIn == 0)
        {
            // Stuck.
            //cout << "Stuck!" << endl;
            return false;
        }
        const int directionIndex = rand() % numDirectionsCanMoveIn;
        //cout << " chose: " << directionsCanMoveIn[directionIndex].first << " " << directionsCanMoveIn[directionIndex].second << endl;
        int newFrogX = frogStartX + directionsCanMoveIn[directionIndex].first;
        int newFrogY = frogStartY + directionsCanMoveIn[directionIndex].second;
        //cout << " moved to " << newFrogX << "," << newFrogY << endl;

        switch(cellTypes[newFrogX][newFrogY])
        {
            case CellType::Free:
                // Do nothing.
                break;
            case CellType::Wall:
                assert(false);
                break;
            case CellType::Mine:
                //cout << " Boom!" << endl;
                return false;
            case CellType::Exit:
                //cout << " Free!" << endl;
                return true;
            case CellType::Tunnel:
                const int afterTunnelFrogX = tunnelDestinations[newFrogX][newFrogY].first;
                const int afterTunnelFrogY = tunnelDestinations[newFrogX][newFrogY].second;
                //cout << " slid down tunnel to: " << afterTunnelFrogX << "," << afterTunnelFrogY << endl;
                newFrogX = afterTunnelFrogX;
                newFrogY = afterTunnelFrogY;
                assert(newFrogX != -1 && newFrogY != -1);
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
    for (int sim = 0; sim < 100'000'000; sim++)
    {
        if (simulateRun(cellTypes, tunnelDestinations, frogStartX, frogStartY))
            numEscapes++;
        numSims++;
    }

    return static_cast<double>(numEscapes) / numSims;
}

double resultOptimized(const vector<vector<CellType>>& cellTypes, const vector<vector<std::pair<int,int>>>& tunnelDestinations, const int frogStartX, const int frogStartY)
{
    const std::pair<int, int> possibleDirections[] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
    const int width = cellTypes.size();
    const int height = cellTypes[0].size();
    vector<vector<long double>> successProbabilities(width, vector<long double>(height, 0.0));
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            if (cellTypes[x][y] == CellType::Exit)
            {
                successProbabilities[x][y] = 1.0;
            }
        }
    }
    vector<vector<long double>> nextSuccessProbabilities(width, vector<long double>(height, 0.0));

    for (int sim = 0; sim < 1'000'000; sim++)
    {
#if 0
        if ( (sim % 100) == 0)
        {
            cout << "probabilities:" << endl;
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    cout << successProbabilities[x][y] << " ";
                }
                cout << endl;
            }
        }
#endif
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                nextSuccessProbabilities[x][y] = 0.0;
                if (cellTypes[x][y] != CellType::Free && cellTypes[x][y] != CellType::Tunnel)
                {
                    nextSuccessProbabilities[x][y] = successProbabilities[x][y];
                    continue;
                } 
                std::pair<int, int> directionsCanMoveIn[4];
                int numDirectionsCanMoveIn = 0;
                for (const auto direction : possibleDirections)
                {
                    const int newFrogX = x + direction.first;
                    const int newFrogY = y + direction.second;

                    if (newFrogX < 0 || newFrogX >= width)
                        continue;
                    if (newFrogY < 0 || newFrogY >= height)
                        continue;
                    if (cellTypes[newFrogX][newFrogY] == CellType::Wall)
                        continue;

                    directionsCanMoveIn[numDirectionsCanMoveIn] = direction;
                    numDirectionsCanMoveIn++;
                    //cout << " can move in direction: " << direction.first << " " << direction.second << endl;
                }

                for(int directionIndex = 0; directionIndex < numDirectionsCanMoveIn; directionIndex++)
                {
                    int newFrogX = x + directionsCanMoveIn[directionIndex].first;
                    int newFrogY = y + directionsCanMoveIn[directionIndex].second;

                    if (cellTypes[newFrogX][newFrogY] == CellType::Tunnel)
                    {
                        const int afterTunnelFrogX = tunnelDestinations[newFrogX][newFrogY].first;
                        const int afterTunnelFrogY = tunnelDestinations[newFrogX][newFrogY].second;
                        newFrogX = afterTunnelFrogX;
                        newFrogY = afterTunnelFrogY;
                    }
                    assert(newFrogX != -1 && newFrogY != -1);
                    nextSuccessProbabilities[x][y] += successProbabilities[newFrogX][newFrogY] / numDirectionsCanMoveIn;
                }
            }

        }
        std::swap(successProbabilities, nextSuccessProbabilities);
        //successProbabilities = nextSuccessProbabilities;
    }
    return successProbabilities[frogStartX][frogStartY];
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
                    //cout << "frogStartX: " << frogStartX << " frogStartY: " << frogStartY << endl;
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
        assert(beginX >= 0 && beginY >= 0 && endX >= 0 && endY >= 0);
        //cout << "Tunnel: (" << beginX << "," << beginY << ") - (" << endX << "," << endY << ")" << endl;

        cellTypes[beginX][beginY] = CellType::Tunnel;
        cellTypes[endX][endY] = CellType::Tunnel;

        tunnelDestinations[beginX][beginY] = {endX, endY};
        tunnelDestinations[endX][endY] = {beginX, beginY};
    }

#ifdef BRUTE_FORCE
    for (int frogStartX = 0; frogStartX < width; frogStartX++)
    {
        for (int frogStartY = 0; frogStartY < height; frogStartY++)
        {
            cout << "frogStartX: " << frogStartX << " frogStartY: " << frogStartY << endl;
            const double simulatedResult = simulate(cellTypes, tunnelDestinations, frogStartX, frogStartY);
            cout << " simulated: " << simulatedResult << endl;
            const double calculatedResult = resultOptimized(cellTypes, tunnelDestinations, frogStartX, frogStartY);
            cout << " calculated: " << calculatedResult << endl;
            cout << " diff: "  << std::setprecision(std::numeric_limits<long double>::digits10 + 1) << abs(calculatedResult - simulatedResult) << std::endl;

        }
    }
#else
    const double calculatedResult = resultOptimized(cellTypes, tunnelDestinations, frogStartX, frogStartY);
    cout << std::setprecision(std::numeric_limits<long double>::digits10 + 1) << calculatedResult << endl;
#endif
}
