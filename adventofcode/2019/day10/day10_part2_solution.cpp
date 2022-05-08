#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numbers>
#include <cassert>

using namespace std;

int gcd(int a, int b)
{
    while (b != 0)
    {
        const int t = b;
        b = (a % b);
        a = t;
    }
    return a;
}

vector<string> getVisible(const int startRowIndex, const int startColIndex, const vector<string>& map)
{
    const int height = static_cast<int>(map.size());
    const int width = static_cast<int>(map[0].size());
    vector<vector<bool>> isBlocked(height, vector<bool>(width, false));

    for (int destRowIndex = 0; destRowIndex < height; destRowIndex++)
    {
        for (int destColIndex = 0; destColIndex < width; destColIndex++)
        {
            if (map[destRowIndex][destColIndex] != '#')
                continue;
            if (startRowIndex == destRowIndex && startColIndex == destColIndex)
                continue;

            int rowIndex = destRowIndex;
            int colIndex = destColIndex;

            int dRow = destRowIndex - startRowIndex;
            int dCol = destColIndex - startColIndex;
            // The problem statement is Vague as hell, but it looks like we should divide dRow and dCol
            // by their highest common factor so that they are co-prime.
            // *Really* badly explained problem - not impressed!
            const int reductionFactor = gcd(abs(dRow), abs(dCol));
            dRow /= reductionFactor;
            dCol /= reductionFactor;

            do
            {
                rowIndex += dRow;
                colIndex += dCol;

                if (rowIndex < 0 || rowIndex >= height)
                    break;
                if (colIndex < 0 || colIndex >= width)
                    break;
                isBlocked[rowIndex][colIndex] = true;

            } while (true);
        }
    }

    vector<string> visible(height, string(width, '.'));
    visible[startRowIndex][startColIndex] = 'X';
    for (int destRowIndex = 0; destRowIndex < height; destRowIndex++)
    {
        for (int destColIndex = 0; destColIndex < width; destColIndex++)
        {
            if (destRowIndex == startRowIndex && destColIndex == startColIndex)
                continue;
            if (map[destRowIndex][destColIndex] != '#' || isBlocked[destRowIndex][destColIndex])
                continue;
            visible[destRowIndex][destColIndex] = '#';
        }
    }

    return visible;
}

int main()
{
    int width = -1;
    string mapLine;
    vector<string> map;
    while (getline(cin, mapLine))
    {
        assert(width == -1 || static_cast<int>(mapLine.size()) == width);
        width = static_cast<int>(mapLine.size());
        map.push_back(mapLine);
    }
    const int height = static_cast<int>(map.size());

    int bestNumAsteroids = -1;
    int bestStationRow = -1;
    int bestStationCol = -1;
    for (int startRowIndex = 0; startRowIndex < height; startRowIndex++)
    {
        for (int startColIndex = 0; startColIndex < width; startColIndex++)
        {
            if (map[startRowIndex][startColIndex] != '#')
                continue;


            const auto& visible = getVisible(startRowIndex, startColIndex, map);
            int numAsteroidsSeen = 0;
            for (const auto& visibleMapLine : visible)
            {
                numAsteroidsSeen += static_cast<int>(count(visibleMapLine.begin(), visibleMapLine.end(), '#'));
            }
            if (numAsteroidsSeen > bestNumAsteroids)
            {
                bestNumAsteroids = numAsteroidsSeen;

                bestStationRow = startRowIndex;
                bestStationCol = startColIndex;
            }
        }
    }
    assert(bestStationRow != -1 && bestStationCol != -1);
    cout << "bestStationRow: " << bestStationRow << " bestStationCol: " << bestStationCol << endl;

    int numAsteroidsDestroyed = 0;
        struct Coord
        {
            int rowIndex = -1;
            int colIndex = -1;
        };
    Coord twoHundredthAsteroidDestroyed;
    while (true)
    {
        const auto currentMap = getVisible(bestStationRow, bestStationCol, map);
        vector<Coord> asteroidsToDestroy;
        for (int rowIndex = 0; rowIndex < height; rowIndex++)
        {
            for (int colIndex = 0; colIndex < width; colIndex++)
            {
                if (currentMap[rowIndex][colIndex] == '#')
                    asteroidsToDestroy.push_back({rowIndex, colIndex});
            }
        }

        if (asteroidsToDestroy.empty())
            break;

        auto getBearingDegrees = [=](const Coord& coord)
        {
            double bearing = 180.0 + (atan2(-(coord.colIndex - bestStationCol), coord.rowIndex - bestStationRow) * 180.0 / std::numbers::pi);
            while (bearing >= 360.0)
                bearing -= 360.0;
            return bearing;

        };
        // Sort by bearing i.e. in clockwise order, starting from the one vertically about the Station (if any).
        sort(asteroidsToDestroy.begin(), asteroidsToDestroy.end(), [&](const auto& coord1, const auto& coord2)
                {
                return getBearingDegrees(coord1) < getBearingDegrees(coord2);
                });

        for (const auto& coord : asteroidsToDestroy)
        {
            numAsteroidsDestroyed++;
            map[coord.rowIndex][coord.colIndex] = '.';
            cout << numAsteroidsDestroyed << "th asteroid to be destroyed is " << coord.colIndex << "," << coord.rowIndex << endl;
            if (numAsteroidsDestroyed == 200)
                twoHundredthAsteroidDestroyed = coord;
        }

    }


    cout << "200th asteroid to be destroyed is " << twoHundredthAsteroidDestroyed.colIndex << "," << twoHundredthAsteroidDestroyed.rowIndex << endl;
    cout << 100 * twoHundredthAsteroidDestroyed.colIndex + twoHundredthAsteroidDestroyed.rowIndex << endl; 

}
