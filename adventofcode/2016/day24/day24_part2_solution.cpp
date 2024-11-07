#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <limits>

#include <cassert>

using namespace std;

int main()
{
    vector<string> rawMap;
    string mapRow;
    while (getline(cin, mapRow))
        rawMap.push_back(mapRow);

    const int width = rawMap.front().size();
    const int height = rawMap.size();

    struct Coord
    {
        int x = -1;
        int y = -1;
    };

    vector<vector<char>> map(width, vector<char>(height, ' '));

    constexpr auto numDigits = 10;
    std::vector<Coord> posOfDigit(numDigits);
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            map[x][y] = rawMap[y][x];
            if (map[x][y] >= '0' && map[x][y] <= '9')
            {
                posOfDigit[map[x][y] - '0'] = {x, y};
                map[x][y] = '.';
            }
        }
    }

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            std::cout << map[x][y];
        }
        std::cout << std::endl;
    }

    vector<vector<int>> distBetweenDigits(numDigits, vector<int>(numDigits, -1));
    for (int digit = 0; digit < numDigits; digit++)
    {
        const auto digitCoord = posOfDigit[digit];
        std::cout << "digit: " << digit << " pos: " << digitCoord.x << ", " << digitCoord.y << std::endl;
        if (digitCoord.x == -1 && digitCoord.y == -1)
            continue;

        vector<vector<int>> shortestDistTo(width, vector<int>(height, -1));
        shortestDistTo[digitCoord.x][digitCoord.y] = 0;

        vector<Coord> toExplore = {digitCoord};
        int numSteps = 0;
        while (!toExplore.empty())
        {
            vector<Coord> nextToExplore;
            for (const auto& coord : toExplore)
            {

                for (const auto& [dx, dy] : std::initializer_list<std::pair<int, int>>{ {-1, 0},
                        {+1, 0},
                        {0, -1},
                        {0, +1}
                        })
                {
                    const int neighbourX = coord.x + dx;
                    const int neighbourY = coord.y + dy;
                    if (neighbourX < 0 || neighbourX >= width || neighbourY < 0 || neighbourY >= height)
                        continue;
                    if (map[neighbourX][neighbourY] == '#')
                        continue;
                    if (shortestDistTo[neighbourX][neighbourY] == -1)
                    {
                        shortestDistTo[neighbourX][neighbourY] = numSteps + 1;
                        nextToExplore.push_back({neighbourX, neighbourY});
                    }
                }
            }

            toExplore = nextToExplore;
            numSteps++;
        }

        for (int otherDigit = 0; otherDigit < numDigits; otherDigit++)
        {
            const auto otherDigitCoord = posOfDigit[otherDigit];
            if (otherDigitCoord.x == -1 && otherDigitCoord.y == -1)
                continue;
            std::cout << "dist between digit: " << digit << " and digit " << otherDigit << " is " << shortestDistTo[otherDigitCoord.x][otherDigitCoord.y] << std::endl;
            distBetweenDigits[digit][otherDigit] = shortestDistTo[otherDigitCoord.x][otherDigitCoord.y];
        }
    }

    std::vector<int> nonStartDigits;
    for (int digit = 1; digit < numDigits; digit++)
    {
        const auto digitCoord = posOfDigit[digit];
        if (digitCoord.x == -1 && digitCoord.y == -1)
            continue;
        nonStartDigits.push_back(digit);
    }
    const auto initialNonStartDigits = nonStartDigits;
    int minDistance = std::numeric_limits<int>::max();
    do
    {
        std::cout << "digit order: " << std::endl;
        for (const auto x : nonStartDigits) cout << " " << x;
        std::cout << std::endl;
        int prevDigit = 0;
        int distance = 0;
        for (const auto& digit : nonStartDigits)
        {
            distance += distBetweenDigits[prevDigit][digit];
            prevDigit = digit;
        }
        distance += distBetweenDigits[nonStartDigits.back()][0];
        std::cout << "distance: " << distance << std::endl;
        if (distance < minDistance)
        {
            minDistance = distance;
            std::cout << "new minDistance: " << minDistance << std::endl;
        } 
        std::next_permutation(nonStartDigits.begin(), nonStartDigits.end());
    } while (nonStartDigits != initialNonStartDigits);
    std::cout << "minDistance: " << minDistance << std::endl;

    return 0;
}
