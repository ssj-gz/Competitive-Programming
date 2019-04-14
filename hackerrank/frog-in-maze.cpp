// Simon St James - 2019-04-12 12:08
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <cassert>
#include <iomanip>
#include <limits>
#include <chrono>

using namespace std;

enum class CellType
{
    Free,
    Mine,
    Exit,
    Wall,
    Tunnel
};

double calculateResult(const vector<vector<CellType>>& cellTypes, const vector<vector<std::pair<int,int>>>& tunnelDestinations, const int frogStartX, const int frogStartY)
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

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
        if ( (sim % 100) == 0)
        {
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

            // Bail out after approx 1.8 seconds.
            if (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() >= 1800)
            {
                break;
            }
        }

        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                // nextSuccessProbabilities is the probability (for each cell) of reaching
                // the Exit with up to (# simulations + 1) moves.
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
                    // Simple probability.
                    nextSuccessProbabilities[x][y] += successProbabilities[newFrogX][newFrogY] / numDirectionsCanMoveIn;
                }
            }

        }
        // Faster than re-allocating a new nextSuccessProbabilities!
        std::swap(successProbabilities, nextSuccessProbabilities);
    }
    return successProbabilities[frogStartX][frogStartY];
}

int main()
{
    // Easy one - use some elementary probability theory to get a simple recurrence relation
    // that tells us the probability of a given cell reaching an Exit in up to N
    // moves, and iterate for a large number of N.
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
        // Make coords 0-relative.
        const int beginY = readInt() - 1;
        const int beginX = readInt() - 1;
        const int endY = readInt() - 1;
        const int endX = readInt() - 1;
        assert(beginX >= 0 && beginY >= 0 && endX >= 0 && endY >= 0);

        cellTypes[beginX][beginY] = CellType::Tunnel;
        cellTypes[endX][endY] = CellType::Tunnel;

        tunnelDestinations[beginX][beginY] = {endX, endY};
        tunnelDestinations[endX][endY] = {beginX, beginY};
    }

    const double calculatedResult = calculateResult(cellTypes, tunnelDestinations, frogStartX, frogStartY);
    cout << std::setprecision(std::numeric_limits<long double>::digits10 + 1) << calculatedResult << endl;
}
