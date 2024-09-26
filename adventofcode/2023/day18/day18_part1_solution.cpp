#include <iostream>
#include <vector>
#include <regex>
#include <map>
#include <set>
#include <limits>

#include <cassert>

#define BRUTE_FORCE

using namespace std;

struct Coord
{
    int64_t x = -1;
    int64_t y = -1;
    auto operator<=>(const Coord& other) const = default;
};

struct DigSection
{
    enum Direction { Up, Left, Down, Right };
    Direction direction = Up;
    int64_t numToDig = -1; 
    string colourRGB;
};

#ifdef BRUTE_FORCE
int64_t lagoonSizeBruteForce(const vector<DigSection>& digPlan)
{
    std::map<Coord, string> colourAtCoord;
    int64_t x = 0;
    int64_t y = 0;

    for (const auto& digSection : digPlan)
    {
        for (int64_t i = 0; i < digSection.numToDig; i++)
        {
            switch (digSection.direction)
            {
                case DigSection::Up:
                    y--;
                    break;
                case DigSection::Right:
                    x++;
                    break;
                case DigSection::Down:
                    y++;
                    break;
                case DigSection::Left:
                    x--;
                    break;
            }
            colourAtCoord[{x, y}] = digSection.colourRGB;
        }
    }

    int64_t minX = std::numeric_limits<int64_t>::max();
    int64_t minY = std::numeric_limits<int64_t>::max();
    int64_t maxX = std::numeric_limits<int64_t>::min();
    int64_t maxY = std::numeric_limits<int64_t>::min();
    for (const auto& [coord, colour] : colourAtCoord)
    {
        minX = std::min(minX, coord.x);
        minY = std::min(minY, coord.y);
        maxX = std::max(maxX, coord.x);
        maxY = std::max(maxY, coord.y);
    }
    const int64_t width = maxX - minX + 3;
    const int64_t height = maxY - minY + 3;
    assert(width > 0);
    assert(height > 0);
    vector<vector<char>> fillMap(width, vector<char>(height, '.'));
    for (const auto& [coord, colour] : colourAtCoord)
    {
        const int64_t x = coord.x - minX + 1;
        const int64_t y = coord.y - minY + 1;
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        fillMap[x][y] = '#';
    }
    for (int64_t y = 0; y < height; y++)
    {
        for (int64_t x = 0; x < width; x++)
        {
            cout << fillMap[x][y];
        }
        cout << endl;
    }

    vector<Coord> toExplore = { {0, 0} };
    set<Coord> seen = { {0, 0} };

    int64_t numOutsideCells = 0;
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
                const int64_t newX = coord.x + dx;
                const int64_t newY = coord.y + dy;
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
    for (int64_t y = 0; y < height; y++)
    {
        for (int64_t x = 0; x < width; x++)
        {
            cout << fillMap[x][y];
        }
        cout << endl;
    }
    std::cout << "numOutsideCells: " << numOutsideCells << std::endl;
    return width * height - numOutsideCells;
}
#endif

int main()
{
    std::regex digPlanRegex(R"(^([URDL])\s*(\d+)\s*\((.*)\)\s*$)");
    string digPlanLine;

    vector<DigSection> digPlan;
    while (getline(cin, digPlanLine))
    {
        std::smatch digPlanMatch;

        const bool matchSuccessful = std::regex_match(digPlanLine, digPlanMatch, digPlanRegex);
        assert(matchSuccessful);

        const auto dirChar = std::string(digPlanMatch[1])[0];
        const int64_t numToDig = std::stoll(digPlanMatch[2]);
        const string colourRGB = digPlanMatch[3];

        std::cout << "dirChar: " << dirChar << " numToDig: " << numToDig << " colourRGB: " << colourRGB << std::endl;

        DigSection digSection;
        digSection.numToDig = numToDig;
        digSection.colourRGB;
        switch (dirChar)
        {
            case 'U':
                digSection.direction = DigSection::Up;
                break;
            case 'R':
                digSection.direction = DigSection::Right;
                break;
            case 'D':
                digSection.direction = DigSection::Down;
                break;
            case 'L':
                digSection.direction = DigSection::Left;
                break;
        }
        digPlan.push_back(digSection);
    }

    const int64_t resultBruteForce = lagoonSizeBruteForce(digPlan);
    std::cout << "resultBruteForce: " << resultBruteForce << std::endl;

}
