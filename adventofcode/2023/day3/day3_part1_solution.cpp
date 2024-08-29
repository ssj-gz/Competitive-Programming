#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int main()
{
    vector<string> schematicRaw;
    string schematicLine;
    while (getline(cin, schematicLine))
    {
        schematicRaw.push_back(schematicLine);
    }
    const int width = schematicRaw.front().size();
    const int height = schematicRaw.size();
    vector<vector<char>> schematic(width, vector<char>(height, ' '));
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            schematic[x][y] = schematicRaw[y][x];
        }
    }

    auto isDigit = [](const char character)
    {
        return character >= '0' && character <= '9';
    };

    int64_t partNumberSum = 0;
    for (int y = 0; y < height; y++)
    {
        int numberBeginPos = -1;
        for (int x = 0; x < width; x++)
        {
            if (numberBeginPos != -1)
            {
                if ((x == width - 1) ||  !isDigit(schematic[x][y]))
                {
                    // End of number.
                    const int numberEndPos = (!isDigit(schematic[x][y]) ? x - 1 : x);
                    int partNumberValue = 0;
                    std::cout << "Found number  at y: " << y << " from: " << numberBeginPos << " to " << numberEndPos << " (inclusive)" << std::endl;
                    for (int numberX = numberBeginPos; numberX <= numberEndPos; numberX++)
                    {
                        assert(isDigit(schematic[numberX][y]));
                        partNumberValue = partNumberValue * 10 + schematic[numberX][y] - '0';

                    }
                    std::cout << "Found number " << partNumberValue << " at y: " << y << " from: " << numberBeginPos << " to " << x << " (inclusive)" << std::endl;
                    bool hasAdjacentSymbol = false;
                    for (int numberX = numberBeginPos; numberX <= numberEndPos; numberX++)
                    {
                        for (int neighbourX = numberX - 1; neighbourX <= numberX + 1; neighbourX++)
                        {
                            for (int neighbourY = y - 1; neighbourY <= y + 1; neighbourY++)
                            {
                                if (neighbourX < 0 || neighbourX >= width || neighbourY < 0 || neighbourY >= height)
                                    continue;
                                const auto neighbourContent = schematic[neighbourX][neighbourY];
                                if (neighbourContent == '.')
                                    continue;
                                if (isDigit(neighbourContent))
                                    continue;
                                cout << "Found adjacent symbol: " << neighbourContent << std::endl;
                                hasAdjacentSymbol = true;
                            }
                        }
                    }
                    if (hasAdjacentSymbol)
                    {
                        std::cout << "Adding: " << partNumberValue << std::endl;
                        partNumberSum += partNumberValue;
                    }
                    numberBeginPos = -1;
                }
            }
            else
            {
                if (isDigit(schematic[x][y]))
                    numberBeginPos = x;
            }
        }
    }
    std::cout << partNumberSum << std::endl;
}
