#include <iostream>
#include <vector>
#include <regex>
#include <map>
#include <set>
#include <limits>

#include <cassert>

using namespace std;

struct Coord
{
    int x = -1;
    int y = -1;
    auto operator<=>(const Coord& other) const = default;
};

int main()
{
    std::regex digPlanRegex(R"(^([URDL])\s*(\d+)\s*\((.*)\)\s*$)");
    string digPlanLine;

    std::map<Coord, string> colourAtCoord;
    int x = 0;
    int y = 0;
    while (getline(cin, digPlanLine))
    {
        std::smatch digPlanMatch;

        const bool matchSuccessful = std::regex_match(digPlanLine, digPlanMatch, digPlanRegex);
        assert(matchSuccessful);

        const auto dirChar = std::string(digPlanMatch[1])[0];
        const int64_t numToDig = std::stoll(digPlanMatch[2]);
        const string colourRGB = digPlanMatch[3];

        std::cout << "dirChar: " << dirChar << " numToDig: " << numToDig << " colourRGB: " << colourRGB << std::endl;

        for (int i = 0; i < numToDig; i++)
        {
            switch (dirChar)
            {
                case 'U':
                    y--;
                    break;
                case 'R':
                    x++;
                    break;
                case 'D':
                    y++;
                    break;
                case 'L':
                    x--;
                    break;
            }
            colourAtCoord[{x, y}] = colourRGB;
        }
    }

    int minX = std::numeric_limits<int>::max();
    int minY = std::numeric_limits<int>::max();
    int maxX = std::numeric_limits<int>::min();
    int maxY = std::numeric_limits<int>::min();
    for (const auto& [coord, colour] : colourAtCoord)
    {
        minX = std::min(minX, coord.x);
        minY = std::min(minY, coord.y);
        maxX = std::max(maxX, coord.x);
        maxY = std::max(maxY, coord.y);
    }
    const int width = maxX - minX + 3;
    const int height = maxY - minY + 3;
    assert(width > 0);
    assert(height > 0);
    vector<vector<char>> fillMap(width, vector<char>(height, '.'));
    for (const auto& [coord, colour] : colourAtCoord)
    {
        const int x = coord.x - minX + 1;
        const int y = coord.y - minY + 1;
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        fillMap[x][y] = '#';
    }
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            cout << fillMap[x][y];
        }
        cout << endl;
    }

    vector<Coord> toExplore = { {0, 0} };
    set<Coord> seen = { {0, 0} };

    int numOutsideCells = 0;
    while (!toExplore.empty())
    {
        std::cout << "#toExplore: " << toExplore.size() << std::endl;
        vector<Coord> nextToExplore;

        for (const auto& coord : toExplore)
        {
            assert(fillMap[coord.x][coord.y] == '.');
            fillMap[coord.x][coord.y] = 'O';
            numOutsideCells++;
            for (const auto& [dx, dy] : std::initializer_list<std::pair<int, int>>{ {-1, 0},
                                         {+1, 0},
                                         {0, -1},
                                         {0, +1}
                                         })
            {
                const int newX = coord.x + dx;
                const int newY = coord.y + dy;
                if (newX < 0 || newX >= width || newY < 0 || newY >= height)
                    continue;
                if (fillMap[newX][newY] == '#')
                    continue;
                if (seen.contains({newX, newY}))
                    continue;
                nextToExplore.push_back({newX, newY});
                seen.insert({newX, newY});
            }

        }

        toExplore = nextToExplore;
    }
    std::cout << "After fill: " << std::endl;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            cout << fillMap[x][y];
        }
        cout << endl;
    }
    std::cout << "numOutsideCells: " << numOutsideCells << std::endl;
    const int64_t result = width * height - numOutsideCells; 
    std::cout << "result: " << result << std::endl;

}
