#include <iostream>
#include <vector>
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
    for (int startRowIndex = 0; startRowIndex < height; startRowIndex++)
    {
        for (int startColIndex = 0; startColIndex < width; startColIndex++)
        {
            if (map[startRowIndex][startColIndex] != '#')
                continue;

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
                    // Vague as hell, but it looks like we should divide dRow and dCol
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

            int numAsteroidsSeen = 0;
            vector<string> seen(height, string(width, '.'));
            seen[startRowIndex][startColIndex] = 'X';
            for (int destRowIndex = 0; destRowIndex < height; destRowIndex++)
            {
                for (int destColIndex = 0; destColIndex < width; destColIndex++)
                {
                    if (destRowIndex == startRowIndex && destColIndex == startColIndex)
                        continue;
                    if (map[destRowIndex][destColIndex] != '#' || isBlocked[destRowIndex][destColIndex])
                        continue;
                    numAsteroidsSeen++;
                    seen[destRowIndex][destColIndex] = '#';
                }
            }

            cout << "Starting from (" << startRowIndex << "," << startColIndex << ") can see " << numAsteroidsSeen << " asteroids" << endl;
            for (const auto& blah : seen)
            {
                cout << blah << endl;
            }
            bestNumAsteroids = max(bestNumAsteroids, numAsteroidsSeen);
        }
    }
    cout << "bestNumAsteroids: " << bestNumAsteroids << endl;

}
