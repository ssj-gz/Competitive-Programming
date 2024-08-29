#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

struct PartNumber
{
    int y = -1;
    int numberBeginPos = -1;
    int numberEndPos = -1;
    int64_t value = -1;
};

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
    vector<PartNumber> partNumbers;
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
                    partNumbers.push_back({y, numberBeginPos, numberEndPos, partNumberValue});
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
    for (const auto& partNumber : partNumbers)
    {
        std::cout << " part number value: " << partNumber.value << " at y: " << partNumber.y << " from: " << partNumber.numberBeginPos << " to " << partNumber.numberEndPos << std::endl;
    }
    int64_t gearRatioSum = 0;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            if (schematic[x][y] == '*')
            {
                vector<PartNumber> adjacentPartNumbers;
                for (const auto& partNumber : partNumbers)
                {
                    if (partNumber.y < y - 1 || partNumber.y > y + 1)
                        continue;
                    if (partNumber.numberEndPos < x - 1 || partNumber.numberBeginPos > x + 1)
                        continue;
                    adjacentPartNumbers.push_back(partNumber);
                }
                std::cout << "Gear at : " << x << ", " << y << " has " << adjacentPartNumbers.size() << " adjacent part numbers" << std::endl;
                if (adjacentPartNumbers.size() == 2)
                    gearRatioSum += adjacentPartNumbers[0].value * adjacentPartNumbers[1].value;

            }
        }
    }
    std::cout << "gearRatioSum: " << gearRatioSum << std::endl;
}
