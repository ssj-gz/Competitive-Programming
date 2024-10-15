#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

int main()
{
    string areaLine;
    vector<string> areaRaw;
    while (getline(cin, areaLine))
    {
        areaRaw.push_back(areaLine);
    }
    const int width = areaRaw.front().size();
    const int height = areaRaw.size();

    vector<vector<char>> area(width, vector<char>(height, ' '));
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            area[x][y] = areaRaw[y][x];
        }
    }

    for (int minute = 1; minute <= 10; minute++)
    {
        std::cout << "Beginning minute # " << minute << std::endl;
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                cout << area[x][y];
            }
            std::cout << std::endl;
        }

        auto areaNext = area;
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                int numAdjacentTrees = 0;
                int numAdjacentLumbers = 0;
                int numAdjacentOpens = 0;
                for (int neighbourX = x - 1; neighbourX <= x + 1; neighbourX++)
                {
                    if (neighbourX < 0 || neighbourX >= width)
                        continue;
                    for (int neighbourY = y - 1; neighbourY <= y + 1; neighbourY++)
                    {
                        if (neighbourY < 0 || neighbourY >= height)
                            continue;
                        if (neighbourX == x && neighbourY == y)
                            continue;
                        const auto oldContent = area[neighbourX][neighbourY];
                        if (oldContent == '.')
                            numAdjacentOpens++;
                        else if (oldContent == '#')
                            numAdjacentLumbers++;
                        else if (oldContent == '|')
                            numAdjacentTrees++;
                        else
                            assert(false);
                    }
                }

                const auto oldContent = area[x][y];
                if (oldContent == '.')
                {
                    if (numAdjacentTrees >= 3)
                        areaNext[x][y] = '|';
                }
                else if (oldContent == '#')
                {
                    if (!(numAdjacentLumbers >= 1 && numAdjacentTrees >=1))
                        areaNext[x][y] = '.';
                }
                else if (oldContent == '|')
                {
                    if (numAdjacentLumbers >= 3)
                        areaNext[x][y] = '#';
                }
                else
                    assert(false);
            }
        }
        area = areaNext;
    }
    std::cout << "After 10 minutes" << std::endl;
    int numTrees = 0;
    int numLumbers = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            const auto content = area[x][y];
            cout << content;
            if (content == '#')
                numLumbers++;
            else if (content == '|')
                numTrees++;
        }
        std::cout << std::endl;
    }
    std::cout << "numLumbers: " << numLumbers << std::endl;
    std::cout << "numTrees: " << numTrees << std::endl;
    std::cout << "result: " << (numTrees * numLumbers) << std::endl;

    return 0;
}
