#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

void findLongestPath(const int x, const int y, const int numStepsTaken, const vector<vector<char>>& map, vector<vector<bool>>& visited, int& longestPathLen)
{
    if (visited[x][y])
        return;
    assert(map[x][y] != '#');

    if (numStepsTaken > longestPathLen)
    {
        longestPathLen = numStepsTaken;
        std::cout << "Found new longestPathLen:" << longestPathLen << std::endl;
    }
    visited[x][y] = true;

    if (map[x][y] == '.')
    {
        for (const auto& [dx, dy] : std::initializer_list<std::pair<int, int>>{ {-1, 0},
                {+1, 0},
                {0, -1},
                {0, +1}
                })
        {
            const int newX = x + dx;
            const int newY = y + dy;
            if (map[newX][newY] != '#')
            {
                findLongestPath(newX, newY, numStepsTaken + 1, map, visited, longestPathLen);
            }
        }
    }
    else
    {
        int newX = x;
        int newY = y;
        switch (map[x][y])
        {
            case '>':
                newX++;
                break;
            case 'v':
                newY++;
                break;
            case '<':
                newX--;
                break;
            case '^':
                newY--;
                break;
        }
        if (map[newX][newY] != '#')
        {
            findLongestPath(newX, newY, numStepsTaken + 1, map, visited, longestPathLen);
        }
    }

    visited[x][y] = false;
}

int main()
{
    vector<string> rawMap;
    string rawMapLine;
    while (getline(cin, rawMapLine))
    {
        rawMap.push_back(rawMapLine);
    }

    const int width = rawMap.front().size();
    const int height = rawMap.size();

    vector<vector<char>> map(width, vector<char>(height, ' '));
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            map[x][y] = rawMap[y][x];
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
    vector<vector<bool>> visited(width, vector<bool>(height, false));
    int longestPathLen = -1;
    const int startX = 1;
    const int startY = 1;
    findLongestPath(startX, startY, 0, map, visited, longestPathLen);

    return 0;
}
